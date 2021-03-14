/*
   GameObject.h - Small game engine/ library for making games in 'portrait format' on an LCD display
   Created by Matthew Holman, 12-14-2020
*/

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <LiquidCrystal_I2C.h>
#include "Arduino.h"

// Struct to hold art asset information
typedef struct art_frame
{
  bool usingCustomArt;
  int charPosInCGRAM;
  char objectArt;
  art_frame *next;
} art_frame_t;

class GameObject
{
private:
  // Position variables
  int _posX;
  int _posY;
  int _boundX;
  int _boundY;
  // Art asset linked list
  art_frame_t *_frames;
  art_frame_t *_display_frame;
  // Drawing varibales
  bool _animating;
  bool _drawing;
  int _total_frames;
  int _current_frame;
  // LCD screen access
  LiquidCrystal_I2C *_lcd;
  // Movement timer variables
  unsigned long _movement_current_time;
  unsigned long _movement_last_time;
  unsigned long _movement_wait_time;
  bool _movement_timer_Expired;
  // Animation timer variables
  unsigned long _animation_current_time;
  unsigned long _animation_last_time;
  unsigned long _animation_wait_time;

public:
  // Constructor/ Destructor functions
  GameObject(int posX, int posY, int boundX, int boundY, LiquidCrystal_I2C *lcd);
  ~GameObject();
  // Art asset functions
  void add_frame(char objectArt, int charPosInCGRAM, byte objectBitPattern[8]);
  // Drawing functions
  void draw();
  void erase_position();
  void set_current_frame(int value);
  int get_current_frame();
  int get_total_frames();
  void set_animating(bool value);
  bool get_animating();
  void set_drawing(bool value);
  bool get_drawing();
  // Movement functions
  void move(int direction);
  int get_posX();
  void set_posX(int value);
  int get_posY();
  void set_posY(int value);
  // Movement timer functions
  void set_movement_wait_time(int value);
  void update_movement_timer(unsigned long time);
  bool get_movement_timer_expired();
  void reset_movement_timer();
  // Animation timer functions
  void set_animation_wait_time(int value);
  void update_animation_timer(unsigned long time);
  void reset_animation_timer();
  // Collision functions
  bool check_collision(GameObject *obj);
  bool check_collision(int posX, int posY);
};

#endif
