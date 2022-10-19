#include "../include/settings_ht.h"

bool negador = 0;

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[6] = {0xC0, 0x49, 0xEF, 0xD1, 0x9C, 0xD4};

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
  caixa_2.Initializer(broadcastAddress);
  Serial.println("INICIOU");
}

void loop()
{
  Serial.println("BAGACA");
  while(caixa_2.getCurrentStatus() == ERROR_MODE)
  {
    Serial.println("ERROR MODE");
    delay(1000);
  }
  caixa_2.Process();
  delay(500);
  caixa_2.Debug_SeeVariables();
}