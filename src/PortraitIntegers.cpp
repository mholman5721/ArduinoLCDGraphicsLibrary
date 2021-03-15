/*
   PortraitIntegers.cpp - A library for displaying integer values in 'portrait format' on an LCD display
   Created by Matthew Holman, 12-14-2020
*/

#include "PortraitIntegers.h"

byte ZERO[] = {
  B00000,
  B00000,
  B11111,
  B10001,
  B10001,
  B11111,
  B00000,
  B00000
};

byte ONE[] = {
  B00000,
  B00000,
  B00000,
  B10010,
  B11111,
  B10000,
  B00000,
  B00000
};

byte TWO[] = {
  B00000,
  B00000,
  B11101,
  B10101,
  B10101,
  B10111,
  B00000,
  B00000
};

byte THREE[] = {
  B00000,
  B00000,
  B10001,
  B10101,
  B10101,
  B11111,
  B00000,
  B00000
};

byte FOUR[] = {
  B00000,
  B00000,
  B00111,
  B00100,
  B00100,
  B11111,
  B00000,
  B00000
};

byte FIVE[] = {
  B00000,
  B00000,
  B10111,
  B10101,
  B10101,
  B11101,
  B00000,
  B00000
};

byte SIX[] = {
  B00000,
  B00000,
  B11111,
  B10101,
  B10101,
  B11101,
  B00000,
  B00000
};

byte SEVEN[] = {
  B00000,
  B00000,
  B00001,
  B00001,
  B00001,
  B11111,
  B00000,
  B00000
};

byte EIGHT[] = {
  B00000,
  B00000,
  B11111,
  B10101,
  B10101,
  B11111,
  B00000,
  B00000
};

byte NINE[] = {
  B00000,
  B00000,
  B10111,
  B10101,
  B10101,
  B11111,
  B00000,
  B00000
};

PortraitInt::PortraitInt(int numCols, int rowPos, int value, LiquidCrystal_I2C *lcd) {
  _numCols = numCols;
  _rowPos = rowPos;
  _value = value;
  _value_digits = (int*)calloc(_numCols, sizeof(int));
  _lcd = lcd;
  _lcd->createChar(0, ZERO);
  _lcd->createChar(1, ZERO);
  _lcd->createChar(2, ZERO);
  _lcd->createChar(3, ZERO);
  _lcd->clear();
}

PortraitInt::~PortraitInt(){
  delete[] _value_digits;
  _value_digits = NULL;
  _lcd = NULL;
  _value = 0;
  _rowPos = 0;
  _numCols = 0;
}

void PortraitInt::print_value() {
  int numDigits = _findNumberOfDigits();
  int valCurrDigit = 0;
  int tempValue = _value;

  // Draw score depending on score value
  for (int i = 0; i < numDigits; i++) {
    // CITATION: Author: Martin B
    // CITATION: https://stackoverflow.com/questions/3118490/getting-each-individual-digit-from-a-whole-integer
    // CITATION: Accessed: 12-12-2020
    valCurrDigit = tempValue % 10;
    if (_value_digits[i] != valCurrDigit) {
      switch (valCurrDigit) {
        case 0:
          _value_digits[i] = 0;
          _lcd->createChar(i, ZERO);
          break;
        case 1:
          _value_digits[i] = 1;
          _lcd->createChar(i, ONE);
          break;
        case 2:
          _value_digits[i] = 2;
          _lcd->createChar(i, TWO);
          break;
        case 3:
          _value_digits[i] = 3;
          _lcd->createChar(i, THREE);
          break;
        case 4:
          _value_digits[i] = 4;
          _lcd->createChar(i, FOUR);
          break;
        case 5:
          _value_digits[i] = 5;
          _lcd->createChar(i, FIVE);
          break;
        case 6:
          _value_digits[i] = 6;
          _lcd->createChar(i, SIX);
          break;
        case 7:
          _value_digits[i] = 7;
          _lcd->createChar(i, SEVEN);
          break;
        case 8:
          _value_digits[i] = 8;
          _lcd->createChar(i, EIGHT);
          break;
        case 9:
          _value_digits[i] = 9;
          _lcd->createChar(i, NINE);
          break;
        default:
          break;
      }
    }
    tempValue /= 10;

    _lcd->setCursor(_rowPos, 3 - i);
    _lcd->write(i);
  }

  // Clear unneeded score spaces
  if (numDigits != 4) { // If there are 4 digits, we don't need to clear anything
    _clear_area(_rowPos, 0, _rowPos, 2 - (numDigits - 1)); // Only clear between index zero and two - meaning that index 3 should never be cleared
  }
}

int PortraitInt::get_value() {
  return _value;
}

void PortraitInt::set_value(int value) {
  _value = value;
}

void PortraitInt::_clear_area(unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY) {
  for (unsigned int j = startY; j <= endY; j++) {
    for (unsigned int i = startX; i <= endX; i++) {
      _lcd->setCursor(i, j);
      _lcd->write(' ');
    }
  }
}

// CITATION: Author: Karthikeya Boyini
// CITATION: https://www.tutorialspoint.com/determining-how-many-digits-there-are-in-an-integer-in-cplusplus
// CITATION: Accessed: 12-12-2020
int PortraitInt::_findNumberOfDigits() {
  int tempValue = _value;
  int digitCount = 0;

  // make sure val == 0 has one digit
  if (tempValue == 0) {
    digitCount = 1;
  }

  // find how many digits there are in total
  while (tempValue != 0) {
    tempValue /= 10;
    digitCount++;
  }

  return digitCount;
}