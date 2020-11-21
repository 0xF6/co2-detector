#include "Arduino.h"
#include "TM74.h"
#include "etc.h"
#include "pin-api.h"
#include <MQUnifiedsensor.h>
#include <TimerOne.h>

#define DIO 5
#define RCLK 6
#define SCLK 7

#define Voltage_Resolution (5)
#define ADC_Bit_Resolution (10)
#define RatioMQ9CleanAir (4.4)

#define MQ9_AO A5
#define ON_BOARD_LED 13
#define ALARM 10


TM74 display(SCLK, RCLK, DIO);
MQUnifiedsensor MQ9("Arduino UNO", Voltage_Resolution, ADC_Bit_Resolution, MQ9_AO, "MQ-9");

int sensorValue = 0;

void thread()
{
  draw(sensorValue);
  int level = getWarningIndex(sensorValue);
  if(level != -1 && millis() % level == 0)
  {
    pin<ON_BOARD_LED>().digital().write(HIGH);
    pin<ALARM>().analog().write(125);
  }
  else 
  {
    pin<ON_BOARD_LED>().digital().write(LOW);
    pin<ALARM>().analog().write(LOW);
  }
}





void setup()
{
  Serial.begin(9600);
  pin<ON_BOARD_LED>().mode(OUTPUT);
  pin<ALARM>().mode(OUTPUT);
  MQ9.setRegressionMethod(1);

  float a = 0.0;
  float b = 0.0;
  getIndex(GasType::CO, &a, &b);
  
  MQ9.setA(a);
  MQ9.setB(b);

  MQ9.init();

  calibrate(&MQ9, RatioMQ9CleanAir);
  
  Timer1.initialize(2000);
  Timer1.attachInterrupt(thread);
}
void loop()
{
  MQ9.update();
  auto value = (int)MQ9.readSensor();
  
  noInterrupts();
  sensorValue = value;
  interrupts();
  if (getUptimeSecond() > 100)
    reboot<A1>();
}


String ValueToString(int value)
{
  if (value > 9999)
    return "DEAD";
  if (value > 999)
    return String(value);
  if (value > 99)
    return String("0") + String(value);
  if (value > 9)
    return String("00") + String(value);
  return String("000") + String(value);
}

int getWarningIndex(int co2)
{
  if (co2 > 2500) // critical   level
    return 2;
  if (co2 > 1000) // warning    level
    return 5;
  if (co2 > 900) // alert      level
    return 7;
  if (co2 > 800) // top-middle level
    return 50;
  if (co2 > 600) // normal     level
    return 1300;
  return -1;
}

void draw(int CO_value)
{
  String result = ValueToString(CO_value);
  Segment segs[] = {
      Segment(result[3]),
      Segment(result[2]),
      Segment(result[1]),
      Segment(result[0])
  };
  display.writeSegments(segs, sizeof(segs) / sizeof(Segment));
}