#include "Arduino.h"
#include "TM74.h"
#include "pin-api.h"
#define DIO 5
#define RCLK 6
#define SCLK 7

#define MQ9_AO A5
#define ON_BOARD_LED 13
#define ALARM 10

void draw(int co2);
int getWarningIndex(int co2);

TM74 display(SCLK, RCLK, DIO);

int raw_adc = 0;
float sensorValue = 0;


                     
void setup() {
  pin<ON_BOARD_LED>().mode(OUTPUT);
  pin<ALARM>().mode(OUTPUT);
}
  
void loop() {

  draw(sensorValue);

  int level = getWarningIndex(sensorValue);
  if(level != -1 && millis() % level == 0)
  {
    pin<ON_BOARD_LED>().digital().write(HIGH);
    pin<ALARM>().digital().write(255);
  }
  else 
  {
    pin<ON_BOARD_LED>().digital().write(LOW);
    pin<ALARM>().digital().write(LOW);
  }

  if(millis() % 200 != 0)
    return;
  raw_adc     = pin<MQ9_AO>().analog().read();
  sensorValue = ((10000.0 / 4096.0) * raw_adc) + 200;
}

String ValueToString(int value){
  if(value > 9999)
    return "DEAD";
  if(value > 999)
    return String(value);
  if(value > 99)
    return String("0") + String(value);
  if(value > 9)
    return String("00") + String(value);
  return String("000") + String(value);
}

int getWarningIndex(int co2)
{
  if(co2 > 2500) // critical   level
    return 2;
  if(co2 > 1000) // warning    level
    return 5;
  if(co2 > 900)  // alert      level
    return 7;
  if(co2 > 800)  // top-middle level
    return 50;
  if(co2 > 600)  // normal     level
    return 1300;
  return -1;
}

void draw(int CO_value){
  String result = ValueToString(CO_value);
  Segment segs[] = {
    Segment(result[3]),
    Segment(result[2]),
    Segment(result[1]),
    Segment(result[0])
  };
  display.writeSegments(segs, sizeof(segs) / sizeof(Segment));
}