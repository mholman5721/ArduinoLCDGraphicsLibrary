/*
   GameObject.cpp - Small game engine/ library for making games in 'portrait format' on an LCD display
   Created by Matthew Holman, 12-14-2020
*/

#include "GameObject.h"

GameObject::GameObject(int posX, int posY, int boundX, int boundY, LiquidCrystal_I2C *lcd)
{
  // Set position variables
  _posX = posX;
  _posY = posY;
  _boundX = boundX; // values are EXCLUSIVE
  _boundY = boundY; // values are EXCLUSIVE
  // Set animation frame variables
  _frames = NULL;
  _display_frame = NULL;
  _animating = false;
  _total_frames = 0;
  _current_frame = 0;
  // Set LCD reference
  _lcd = lcd;
  // Set timer variables
  _movement_current_time = 0;
  _movement_last_time = 0;
  _movement_wait_time = 0;
  _movement_timer_Expired = false;
  _animation_current_time = 0;
  _animation_last_time = 0;
  _animation_wait_time = 0;
}

GameObject::~GameObject()
{
  art_frame_t *ptr1 = NULL;
  art_frame_t *ptr2 = NULL;

  // Set position variables to zero
  _posX = 0;
  _posY = 0;
  _boundX = 0;
  _boundY = 0;
  // Free animation frames linked list
  ptr1 = _frames;
  ptr2 = _frames;
  while (ptr1 != NULL)
  {
    ptr2 = ptr1;
    ptr1 = ptr1->next;
    free(ptr2);
  }
  _frames = NULL;
  _display_frame = NULL;
  _animating = false;
  _total_frames = 0;
  _current_frame = 0;
  // Set LCD reference to NULL
  _lcd = NULL;
  // Set timer variables to zero/ false
  _movement_current_time = 0;
  _movement_last_time = 0;
  _movement_wait_time = 0;
  _movement_timer_Expired = false;
  _animation_current_time = 0;
  _animation_last_time = 0;
  _animation_wait_time = 0;
}

void GameObject::add_frame(char objectArt = '\0', int charPosInCGRAM = -1, byte objectBitPattern[8] = NULL)
{
  art_frame_t *newFrame = NULL;
  art_frame_t *ptr1 = NULL;
  art_frame_t *ptr2 = NULL;

  // Create newFrame in memory, and assign values to it
  newFrame = (art_frame_t *)malloc(sizeof(art_frame_t) * 1);
  newFrame->charPosInCGRAM = charPosInCGRAM;
  newFrame->usingCustomArt = false;
  newFrame->objectArt = objectArt;
  newFrame->next = NULL;

  // Create the custom art in the LCD screen's buffer if necessary
  if (objectArt == '\0' && charPosInCGRAM != -1)
  {
    newFrame->usingCustomArt = true;
    _lcd->createChar(charPosInCGRAM, objectBitPattern);
    _lcd->clear();
  }

  // There are no frames, so make newFrame the first
  if (_frames == NULL)
  {
    _frames = newFrame;
  }
  else
  { // Thare ARE frames, so find the last one and append to it
    ptr1 = _frames;
    ptr2 = _frames;

    // Once ptr1 reaches NULL, ptr2 will be on the last node in the list
    while (ptr1 != NULL)
    {
      ptr2 = ptr1;
      ptr1 = ptr1->next;
    }

    // Assign the newFrame to the end of the list
    ptr2->next = newFrame;
  }

  // Increment the total number of frames
  _total_frames++;

  // Set the current frame to be the first one
  _current_frame = 0;
  _display_frame = _frames;
}

void GameObject::draw()
{
  _lcd->setCursor(_posY, _posX);
  if (_display_frame->usingCustomArt == true)
  {
    _lcd->write(_display_frame->charPosInCGRAM);
  }
  else
  {
    _lcd->write(_display_frame->objectArt);
  }
}

void GameObject::erase_position()
{
  _lcd->setCursor(_posY, _posX);
  _lcd->write(' ');
}

void GameObject::set_current_frame(int value)
{
  if (value >= _total_frames)
  {
    _current_frame = _total_frames - 1;
  }
  else if (value < 0)
  {
    _current_frame = 0;
  }
  else
  {
    _current_frame = value;
  }

  // Point to the first frame in the list
  _display_frame = _frames;

  // Procede through the list to the correct frame
  for (int i = 0; i < _current_frame; i++)
  {
    _display_frame = _display_frame->next;
  }
}

int GameObject::get_current_frame()
{
  return _current_frame;
}

int GameObject::get_total_frames()
{
  return _total_frames;
}

void GameObject::set_animating(bool value)
{
  _animating = value;
}

bool GameObject::get_animating()
{
  return _animating;
}

void GameObject::move(int direction)
{
  switch (direction)
  {
  case 0: // Move Left
    if (_posX - 1 >= 0)
    {
      erase_position();
      _posX--;
    }
    break;
  case 1: // Move Right
    if (_posX + 1 < _boundX)
    {
      erase_position();
      _posX++;
    }
    break;
  case 2: // Move Down
    if (_posY - 1 >= 0)
    {
      erase_position();
      _posY--;
    }
    break;
  case 3: // Move Up
    if (_posY + 1 < _boundY)
    {
      erase_position();
      _posY++;
    }
    break;
  default:
    break;
  }
}

int GameObject::get_posX()
{
  return _posX;
}

void GameObject::set_posX(int value)
{
  _posX = value;
}

int GameObject::get_posY()
{
  return _posY;
}

void GameObject::set_posY(int value)
{
  _posY = value;
}

// Movement timer functions
void GameObject::set_movement_wait_time(int value)
{
  _movement_wait_time = value;
}

void GameObject::update_movement_timer(unsigned long time)
{
  _movement_current_time = time;

  if (_movement_current_time - _movement_last_time > _movement_wait_time)
  {
    _movement_last_time = _movement_current_time;

    _movement_timer_Expired = true;
  }
}

bool GameObject::get_movement_timer_expired()
{
  return _movement_timer_Expired;
}

void GameObject::reset_movement_timer()
{
  _movement_timer_Expired = false;
  _movement_last_time = _movement_current_time;
}

// Animation timer functions
void GameObject::set_animation_wait_time(int value)
{
  _animation_wait_time = value;
}

void GameObject::update_animation_timer(unsigned long time)
{
  _animation_current_time = time;

  if (_animating == true && _animation_current_time - _animation_last_time > _animation_wait_time)
  {
    _animation_last_time = _animation_current_time;

    // Move the animation forward to the next frame
    if (_current_frame < _total_frames - 1)
    {
      _display_frame = _display_frame->next;
      _current_frame++;
    }
    else
    { // Loop back to the first frame
      _display_frame = _frames;
      _current_frame = 0;
    }
  }
}

void GameObject::reset_animation_timer()
{
  _animation_last_time = _animation_current_time;
  _display_frame = _frames;
  _current_frame = 0;
}

// Collision detection functions
bool GameObject::check_collision(GameObject *obj)
{
  if ((obj->get_posX() == _posX) && (obj->get_posY() == _posY))
  {
    return true;
  }
  return false;
}

bool GameObject::check_collision(int posX, int posY)
{
  if ((posX == _posX) && (posY == _posY))
  {
    return true;
  }
  return false;
}
