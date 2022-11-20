#include "../include/settings_ht.h"

bool negador = 0;

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xD1, 0x9C, 0xD4};

const uint64_t period_duct_cycle = (1/60)*1000000 /* ms */;
bool semaphro_dc = 0;
uint32_t Pulse_Conter = 0;
uint16_t Period_Read_Duct_Cycle = 0;
hw_timer_t *My_timer = NULL; /*Usar para interrup timer*/

FEEDBACK_TEST Read_Current_Test(ROUTINE_TEST routine_current);


void IRAM_ATTR Interrupt_GPIO()
{
  Pulse_Conter++;
}

void Timer__CalcDuctCycle(void)
{
  Period_Read_Duct_Cycle = (uint16_t)(period_duct_cycle / Pulse_Conter);
}

DeviceBox caixa_2;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  #if defined(CAIXA1)
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GRE, OUTPUT);
    pinMode(PIN_12V, OUTPUT);
    pinMode(PIN_5V , OUTPUT);
    pinMode(PIN_CAN, OUTPUT);
    pinMode(BUTTON_1, INPUT_PULLDOWN);
    pinMode(BUTTON_2, INPUT_PULLDOWN);
    pinMode(BUTTON_3, INPUT_PULLDOWN);
    uint8_t i;
    for (i = 0; i < sizeof(Pinout) - 1; i++)
    {
      digitalWrite(Pinout[i], LOW);
    }
  #elif defined(CAIXA2)
    pinMode(PIN_12V, INPUT_PULLDOWN);
    pinMode(PIN_5V , INPUT_PULLDOWN);
    pinMode(PIN_CAN, INPUT_PULLDOWN);
  #endif
  attachInterrupt(PIN_12V, Interrupt_GPIO, FALLING);
  caixa_2.Initializer(broadcastAddress);
  Serial.println("Setup");
}

void loop()
{
  Serial.println("Loop");
  while(caixa_2.getCurrentStatus() == ERROR_MODE)
  {
    Serial.println("ERROR MODE");
    delay(1000);
  }
  caixa_2.Debug_SeeVariables();
  caixa_2.setFeedbacTest(Read_Current_Test(caixa_2.getCurrentRoutine()));
  caixa_2.Process();
  delay(500);
}

FEEDBACK_TEST Read_Current_Test(ROUTINE_TEST routine_current)
{
  switch (routine_current)
  {
    case INITIAL:
      ;
      /* DO Nothing */
      break;
    case CONTINUITY:
      if (semaphro_dc)
      {
        timerAlarmDisable(My_timer);
        semaphro_dc = 0;
      }
      if ((analogRead(PIN_12V) > RANGE_HIGH_SIGNAL_LEVEL) &&
          (analogRead(PIN_5V) > RANGE_HIGH_SIGNAL_LEVEL) &&
          (analogRead(PIN_CAN) > RANGE_HIGH_SIGNAL_LEVEL))
        {
          return TEST_1_PASS;
        }
        else
        {
          return TEST_1_FAIL;
        }
      break;
    case DUCT_CYCLE:
      /* Rotina para a aquição do DUCT_CYCLE */
      if (!semaphro_dc)
      {
        My_timer = timerBegin(0, 80, true);
        timerAttachInterrupt(My_timer, &Timer__CalcDuctCycle, true);
        timerAlarmWrite(My_timer, DEFAULT_DUCT_CYCLE/2, true);
        timerAlarmEnable(My_timer); //Just Enable
        semaphro_dc = 1;
      }
      if ((int)abs((DEFAULT_DUCT_CYCLE-Period_Read_Duct_Cycle)/Period_Read_Duct_Cycle)*100 < 5 &&
          Period_Read_Duct_Cycle != 0)
      {
        return TEST_2_PASS;
      }
      else
      {
        return TEST_2_PASS;
      }
      break;
  default:
    break;
  }
  return ANY_TEST_RUNNING;
}