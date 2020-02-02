#pragma once
#ifndef __PIN_API__
#define __PIN_API__
#include "Arduino.h"
template<uint8_t id>
class _DIGITAL{
    public:
        inline void write(uint8_t value){
            digitalWrite(id, value);
        }
        inline int read(){
            return digitalRead(id);
        }
};
template<uint8_t id>
class _ANALOG{
    public:
        inline int read(){
            return analogRead(id);
        }
        inline void write(int value){
            analogWrite(id, value);
        }
};
template<uint8_t id>
class pin{
  public:
    inline void mode(uint8_t flag){
      pinMode(id, flag);
    }
    inline _DIGITAL<id> digital() {
        return _DIGITAL<id>();
    }
    inline _ANALOG<id> analog(){
        return _ANALOG<id>();
    }
};
#endif