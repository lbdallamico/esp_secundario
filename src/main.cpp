#include "../include/settings_ht.h"

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xD1, 0x9C, 0xD4};
uint8_t const Sequencia_Cabo[4] = {PIN_CABO_1, PIN_CABO_2, PIN_CABO_3, PIN_CABO_4};

FEEDBACK_TEST Read_Current_Test(ROUTINE_TEST routine_current);
FEEDBACK_TEST Converte_Feedback(bool isPass, ROUTINE_TEST routine_current);

DeviceBox caixa_2;

void setup()
{
  Serial.begin(115200);
  Serial.println();
  #if defined(CAIXA2)
    uint8_t i;
    for (i = 0; i < 4; i++)
    {
      pinMode(Sequencia_Cabo[i], INPUT_PULLDOWN);
    }
  #endif
  caixa_2.Initializer(broadcastAddress);
  Serial.println("INICIOU");
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
  delay(DELAY_LOOP);
}

FEEDBACK_TEST Read_Current_Test(ROUTINE_TEST routine_current)
{
  if (routine_current != INITIAL)
  {
    uint8_t index = 0;
    for (index = 0; index < 4; index++)
    {
      if (routine_current == Sequencia_Cabo[index])
      {
        if(digitalRead(Sequencia_Cabo[index]) == LOW)
        {
          return Converte_Feedback(false, routine_current);
        }
      }
      else
      {
        if(digitalRead(Sequencia_Cabo[index]) == HIGH)
        {
          return Converte_Feedback(false, routine_current);
        }
      }
    }
    return Converte_Feedback(true, routine_current);
  }
  return Converte_Feedback(false, routine_current);
}

FEEDBACK_TEST Converte_Feedback(bool isPass, ROUTINE_TEST routine_current)
{
  switch (routine_current)
  {
    // -----------------------
    case CONTINUIDADE_CABO_1:
      if (isPass)
        return TEST_1_PASS;
      else
        return TEST_1_FAIL;
      break;
    // -----------------------
    case CONTINUIDADE_CABO_2:
      if (isPass)
        return TEST_2_PASS;
      else
        return TEST_2_FAIL;
      break;
    // -----------------------
    case CONTINUIDADE_CABO_3:
      if (isPass)
        return TEST_3_PASS;
      else
        return TEST_3_FAIL;
      break;
    // -----------------------    
    case CONTINUIDADE_CABO_4:
      if (isPass)
        return TEST_4_PASS;
      else
        return TEST_4_FAIL;
      break;    
    default:
      break;
  }
  return ANY_TEST_RUNNING;
}