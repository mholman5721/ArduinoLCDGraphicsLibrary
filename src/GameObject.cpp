/*
   GameObject.cpp - Small game engine/ library for making games in 'portrait format' on an LCD display
   Created by Matthew Holman, 12-14-2020
*/

#include "GameObject.h"

GameObject::GameObject(int posX, int posY, int boundX, int boundY, int charPosInCGRAM, byte objectBitPattern[8], LiquidCrystal_I2C *lcd) {
  _posX = posX;
  _posY = posY;
  _boundX = boundX; // values are EXCLUSIVE
  _boundY = boundY; // values are EXCLUSIVE
  _charPosInCGRAM = charPosInCGRAM;
  _lcd = lcd;
  _lcd->createChar(charPosInCGRAM, objectBitPattern);
  _lcd->clear();
  _usingCustomArt = true;
  _currentTime = 0;
  _lastTime = 0;
  _waitTime = 0;
  _timerExpired = false;
}

GameObject::GameObject(int posX, int posY, int boundX, int boundY, char objectArt, LiquidCrystal_I2C *lcd) {
  _posX = posX;
  _posY = posY;
  _boundX = boundX; // values are EXCLUSIVE
  _boundY = boundY; // values are EXCLUSIVE
  _lcd = lcd;
  _objectArt = objectArt;
  _usingCustomArt = false;
  _currentTime = 0;
  _lastTime = 0;
  _waitTime = 0;
  _timerExpired = false;
}

GameObject::~GameObject() {
  _posX = 0;
  _posY = 0;
  _boundX = 0;
  _boundY = 0;
  _charPosInCGRAM = 0;
  _lcd = NULL;
  _objectArt = '\0';
  _usingCustomArt = false;
  _currentTime = 0;
  _lastTime = 0;
  _waitTime = 0;
  _timerExpired = false;
}

void GameObject::draw() {
  _lcd->setCursor(_posY, _posX);
  if (_usingCustomArt == true) {
    _lcd->write(_charPosInCGRAM);
  } else {
    _lcd->write(_objectArt);
  }
}

void GameObject::move(int direction) {
  switch (direction) {
    case 0: // Move Left
      if (_posX - 1 >= 0) {
        erase_position();
        _posX--;
      }
      break;
    case 1: // Move Right
      if (_posX + 1 < _boundX) {
        erase_position();
        _posX++;
      }
      break;
    case 2: // Move Down
      if (_posY - 1 >= 0) {
        erase_position();
        _posY--;
      }
      break;
    case 3: // Move Up
      if (_posY + 1 < _boundY) {
        erase_position();
        _posY++;
      }
      break;
    default:
      break;
  }
}

void GameObject::erase_position() {
  _lcd->setCursor(_posY, _posX);
  _lcd->write(' ');
}

int GameObject::get_posX() {
  return _posX;
}

void GameObject::set_posX(int value) {
  _posX = value;
}

int GameObject::get_posY() {
  return _posY;
}

void GameObject::set_posY(int value) {
  _posY = value;
}

void GameObject::set_timer_init(int value) {
  _waitTime = value;
}

void GameObject::update_timer(unsigned long time) {
  _currentTime = time;

  if (_currentTime - _lastTime > _waitTime) {
    _lastTime = _currentTime;

    _timerExpired = true;
  }
}

bool GameObject::get_timerExpired() {
  return _timerExpired;
}

void GameObject::reset_timer() {
  _timerExpired = false;
  _lastTime = _currentTime;
}

bool GameObject::check_collision(GameObject *obj) {
  if ((obj->get_posX() == _posX) && (obj->get_posY() == _posY)) {
    return true;
  }
  return false;
}

bool GameObject::check_collision(int posX, int posY) {
  if ((posX == _posX) && (posY == _posY)) {
    return true;
  }
  return false;
}
