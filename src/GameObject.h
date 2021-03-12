/*
   GameObject.h - Small game engine/ library for making games in 'portrait format' on an LCD display
   Created by Matthew Holman, 12-14-2020
*/

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <LiquidCrystal_I2C.h>
#include "Arduino.h"

class GameObject {
  private:
    // Variables
    int _posX;
    int _posY;
    int _boundX;
    int _boundY;
    int _charPosInCGRAM;
    bool _usingCustomArt;
    char _objectArt;
    LiquidCrystal_I2C *_lcd;
    unsigned long _currentTime;
    unsigned long _lastTime;
    unsigned long _waitTime;
    bool _timerExpired;
  public:
    GameObject(int posX, int posY, int boundX, int boundY, int charPosInCGRAM, byte objectBitPattern[8], LiquidCrystal_I2C *lcd);
    GameObject(int posX, int posY, int boundX, int boundY, char objectArt, LiquidCrystal_I2C *lcd);
    ~GameObject();
    void draw();
    void move(int direction);
    int get_posX();
    void set_posX(int value);
    int get_posY();
    void set_posY(int value);
    void set_timer_init(int value);
    void update_timer(unsigned long time);
    bool get_timerExpired();
    void reset_timer(); 
    bool check_collision(GameObject *obj);
    bool check_collision(int posX, int posY);
    void erase_position();
};

#endif
