/*
   PortraitIntegers.h - Library for displaying integer values in 'portrait format' on an LCD display
   Created by Matthew Holman, 12-14-2020
*/

#ifndef PORTRAITINTEGERS_H
#define PORTRAITINTEGERS_H

#include <LiquidCrystal_I2C.h>
#include "Arduino.h"

class PortraitInt {
  private:
    int _numCols;
    int _rowPos;
    int _value;
    int *_value_digits;
    void _clear_area(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY);
    int _findNumberOfDigits();
    LiquidCrystal_I2C *_lcd;
  public:
    PortraitInt(int numCols, int rowPos, int value, LiquidCrystal_I2C *lcd);
    ~PortraitInt();
    int get_value();
    void set_value(int value);
    void print_value();
};

#endif
