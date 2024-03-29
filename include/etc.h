#pragma once
#include "Arduino.h"
#include "pin-api.h"
#include <MQUnifiedsensor.h>
#include <avr/eeprom.h>

void draw(int co2);
int getWarningIndex(int co2);

void log(const Printable& data) {
    if(Serial.available()) Serial.println(data);
}
void log(const char* data) {
    if(Serial.available()) Serial.println(data);
}

template<uint8_t led_pin>
class sys{
  public:
    void kernel_panic(const char* data) {
        while (1)
        {
            log(data);
            pin<led_pin>().digital().write(HIGH);
            delay(100);
            pin<led_pin>().digital().write(LOW);
            delay(100);
            pin<led_pin>().digital().write(HIGH);
            delay(100);
            pin<led_pin>().digital().write(LOW);
            delay(1000);
        }
    }
};

/*
    Gas    | a      | b
    H2     | 987.99 | -2.162
    LPG    | 574.25 | -2.222
    CO     | 36974  | -3.109
    Alcohol| 3616.1 | -2.675
    Propane| 658.71 | -2.168
*/
enum GasType {
    H2,
    LPG,
    CO,
    Alcohol,
    Propane
};
void getIndex(GasType type, float* a, float* b)
{
  switch (type)
  {
    case GasType::H2:
      *a = 987.99f;
      *b = -2.162f;
      break;
    case GasType::LPG:
      *a = 574.25f;
      *b = -2.222f;
      break;
    case GasType::CO:
      *a = 36974.0f;
      *b = -3.109;
      break;
    case GasType::Alcohol:
      *a = 3616.1f;
      *b = -2.675f;
      break;
    case GasType::Propane:
      *a = 658.71f;
      *b = -2.168f;
      break;
    default: break;
  }
}

template<uint8_t pin_id>
void reboot() {
  pin<pin_id>().analog().write(HIGH);
}
long getUptimeSecond()
{
  return millis()/1000;
}

//#define CALIBRATE_MODE 0
void calibrate(MQUnifiedsensor* mq9, float airRatio){

    float calcR0 = 0;
    #if defined(CALIBRATE_MODE)
    Serial.print("Calibrating please wait.");
    for (int i = 1; i <= 100; i++)
    {
        mq9->update();
        calcR0 += mq9->calibrate(airRatio);
        Serial.print(".");
    }
    mq9->setR0(calcR0 / 100);
    eeprom_write_float(0, calcR0);
    Serial.print(calcR0);
    Serial.println("  done!.");
    #else
    Serial.print("Set ");
    calcR0 = eeprom_read_float(0);
    Serial.print(calcR0);
    Serial.println(" R0");
    mq9->setR0(calcR0 / 100);
    #endif
   

    if (isinf(calcR0) || isnan(calcR0))
        sys<13>()
            .kernel_panic("R0 is infite (Open circuit detected) please check your wiring and supply");
    if (calcR0 == 0)
        sys<13>()
            .kernel_panic("R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply");
}