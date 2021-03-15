/* Matthew Holman          12-14-2020
   Small game demo
*/

// NOTE: To complete this project, I consulted the following tutorials
// CITATION: https://www.makerguides.com/character-i2c-lcd-arduino-tutorial/
// CITATION: https://arduinogetstarted.com/tutorials/arduino-keypad

/* keypad mappings to vanduino shield
  0 1 2 3 4 5 6 7

  1 3 5 7
  0 2 4 6

  r3 r4 c3 r1
  r2 c1 c2 c4
*/

#include <Arduino.h>
#include <Wire.h>              // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Keypad.h>
#include "PortraitIntegers.h"
#include "GameObject.h"

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,20,4) for 20x4 LCD.

// Keypad
const int ROW_NUM = 4;    //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pin_rows[ROW_NUM] = {7, 0, 1, 3};      //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {2, 4, 5, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

// "Art" assets
byte SHIP[] = {
    B11110,
    B11000,
    B01100,
    B11011,
    B11011,
    B01100,
    B11000,
    B11110};

byte ASTEROID1[] = {
    B01110,
    B10001,
    B10010,
    B01001,
    B10001,
    B10001,
    B10010,
    B01100};

byte ASTEROID2[] = {
    B00110,
    B01001,
    B10010,
    B10001,
    B01001,
    B10010,
    B10001,
    B01110};

byte ASTEROID3[] = {
    B00000,
    B01110,
    B10001,
    B10010,
    B01001,
    B10010,
    B01100,
    B00000};

// Game functions
void reset_asteroid(GameObject *asteroid);
void randomize_all_asteroids();
void reset_game(unsigned long &score_lastTime, unsigned long &start_lastTime, unsigned long &currentTime);

// Game variables
enum GAME_STATE
{
  STARTING,
  PLAYING
};
GAME_STATE game_state = STARTING;

int score_value = 0;
const int num_asteroids = 6;
const int num_rows = 19;
const int num_cols = 4;

// Game objects
PortraitInt *score_display;
GameObject *player;
GameObject *laser;
GameObject *asteroids[num_asteroids];

void setup()
{
  //Serial.begin(9600);

  // Randomize the random number generator
  randomSeed(analogRead(3));

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();

  // Set up score
  score_display = new PortraitInt(4, 19, 0, &lcd);

  // Set up player
  player = new GameObject(1, 0, num_cols, num_rows, &lcd);
  player->add_frame('\0', 4, SHIP);
  player->add_frame('*', -1, NULL);
  player->add_frame(' ', -1, NULL);
  player->add_frame('\0', 4, SHIP);
  player->add_frame('*', -1, NULL);
  player->add_frame(' ', -1, NULL);
  player->add_frame('\0', 4, SHIP);
  player->add_frame('*', -1, NULL);
  player->add_frame(' ', -1, NULL);
  player->set_animation_wait_time(150);
  player->set_animating(false);

  // Set up laser
  laser = new GameObject(1, 0, num_cols, num_rows, &lcd);
  laser->add_frame('-', -1, NULL);
  laser->set_drawing(false);
  laser->set_movement_wait_time(75);

  // Set up asteroids
  for (int i = 0; i < num_asteroids; i++)
  {
    asteroids[i] = new GameObject(0, 0, num_cols, num_rows, &lcd);
    if (i % 3 == 0)
    {
      asteroids[i]->add_frame('\0', 5, ASTEROID1);
      asteroids[i]->add_frame('*', -1, NULL);
      asteroids[i]->add_frame(' ', -1, NULL);
      asteroids[i]->add_frame('\0', 5, ASTEROID1);
      asteroids[i]->add_frame('*', -1, NULL);
      asteroids[i]->add_frame(' ', -1, NULL);
    }
    else if (i % 3 == 1)
    {
      asteroids[i]->add_frame('\0', 6, ASTEROID2);
      asteroids[i]->add_frame('*', -1, NULL);
      asteroids[i]->add_frame(' ', -1, NULL);
      asteroids[i]->add_frame('\0', 6, ASTEROID2);
      asteroids[i]->add_frame('*', -1, NULL);
      asteroids[i]->add_frame(' ', -1, NULL);
    }
    else
    {
      asteroids[i]->add_frame('\0', 7, ASTEROID3);
      asteroids[i]->add_frame('*', -1, NULL);
      asteroids[i]->add_frame(' ', -1, NULL);
      asteroids[i]->add_frame('\0', 7, ASTEROID3);
      asteroids[i]->add_frame('*', -1, NULL);
      asteroids[i]->add_frame(' ', -1, NULL);
    }
    asteroids[i]->set_animation_wait_time(100);
  }
  randomize_all_asteroids();
}

void loop()
{
  // Timing variables
  unsigned long currentTime;

  static unsigned long lastTime = 0;
  static const unsigned long wait_time = 5;

  static unsigned long start_lastTime;
  static const unsigned long start_wait_time = 2000;

  static unsigned long score_lastTime = 0;
  static const unsigned long score_wait_time = 500;

  // Collect keypad input
  char key = keypad.getKey();

  // Get current time for processing
  currentTime = millis();

  // Control the 'frame rate' of the game
  if (currentTime - lastTime > wait_time)
  {
    lastTime = currentTime;

    // Run the game, depending on its current state
    switch (game_state)
    {
    case STARTING:
      if (currentTime - start_lastTime > start_wait_time)
      {
        start_lastTime = currentTime;
        game_state = PLAYING;
      }
      break;
    case PLAYING:
      // Handle player input/ movement
      if (key == '4')
      { // Move left
        player->move(0);
      }
      else if (key == '6')
      { // Move right
        player->move(1);
      }
      else if (key == '8')
      { // Move down
        player->move(2);
      }
      else if (key == '2')
      { // Move up
        player->move(3);
      }
      if (key == '*' && laser->get_drawing() == false)
      { // Fire laser
        laser->set_posX(player->get_posX());
        laser->set_posY(player->get_posY() + 1);
        laser->set_drawing(true);
      }

      // Handle laser movement
      laser->update_movement_timer(currentTime);

      // If the timer has expired, move the laser up the screen
      if (laser->get_movement_timer_expired() == true)
      {
        laser->move(3);
        laser->reset_movement_timer();
      }

      // If the laser reaches the top of the screen stop drawing it
      if (laser->get_posY() == num_rows - 1)
      {
        laser->set_drawing(false);
      }

      // Handle asteroid movement/ collisions
      for (int i = 0; i < num_asteroids; i++)
      {
        // Update the asteroids' timers
        asteroids[i]->update_movement_timer(currentTime);

        // If the timer has expired, move the asteroid down the screen
        if (asteroids[i]->get_movement_timer_expired() == true && asteroids[i]->get_animating() == false)
        {
          asteroids[i]->move(2);
          asteroids[i]->reset_movement_timer();
        }

        // If an asteroid has collided with the player, reset the score time and set score to zero before starting the game over
        if (asteroids[i]->check_collision(player) == true)
        {
          reset_asteroid(asteroids[i]);
          player->set_animating(true);
        }

        // If the laser hits an asteroid, start its animation
        if (asteroids[i]->check_collision(laser) == true && asteroids[i]->get_animating() == false && laser->get_drawing() == true)
        {
          laser->set_drawing(false);
          asteroids[i]->set_animating(true);
          // Apply a penalty for using the laser
          score_value -= 10;
        }

        // If the asteroid is animating, and it has reached its final frame, reset it
        if (asteroids[i]->get_animating() == true && asteroids[i]->get_current_frame() == asteroids[i]->get_total_frames() - 1)
        {
          reset_asteroid(asteroids[i]);
        }

        // If an asteroid has reached the 'bottom' of the screen, make it reappear at the top
        if (asteroids[i]->get_posY() == 0)
        {
          reset_asteroid(asteroids[i]);
        }
      }

      // If the player is animating because it was struck by an asteroid, and it has reached its final frame, reset the game
      if (player->get_animating() == true && player->get_current_frame() == player->get_total_frames() - 1)
      {
        reset_game(score_lastTime, start_lastTime, currentTime);
      }

      // Increment score values every 'x' milliseconds
      if (currentTime - score_lastTime > score_wait_time)
      {
        score_lastTime = currentTime;

        score_value++;
      }

      // Keep Score in range
      if (score_value > 9999)
      {
        score_value = 0;
      }
      else if (score_value < 0)
      {
        score_value = 0;
      }

      // Set the final score value
      score_display->set_value(score_value);

      break;
    default:
      break;
    }

    // Display the score on the screen
    score_display->print_value();

    // Update and draw asteroids
    for (int i = 0; i < num_asteroids; i++)
    {
      asteroids[i]->update_animation_timer(currentTime);
      asteroids[i]->draw();
    }

    // Update and draw laser
    laser->draw();

    // Update and draw player
    player->update_animation_timer(currentTime);
    player->draw();
  }
}

void reset_asteroid(GameObject *asteroid)
{ // Reset all of an asteroid's values, so that it seems 'fresh' somewhere else
  asteroid->erase_position();
  asteroid->set_posY(num_rows - 1);
  asteroid->set_posX(random(0, num_cols));
  asteroid->set_current_frame(0);
  asteroid->set_animating(false);
}

void randomize_all_asteroids()
{ // Randomize the starting positions and speeds of all the asteroids
  for (int i = 0; i < num_asteroids; i++)
  {
    asteroids[i]->erase_position();
    asteroids[i]->set_posX(random(0, num_cols));
    asteroids[i]->set_posY(random(3, num_rows));
    asteroids[i]->set_movement_wait_time(random(200, 500));
  }
}

void reset_game(unsigned long &score_lastTime, unsigned long &start_lastTime, unsigned long &currentTime)
{ // Reset the player, laser, asteroid, and score variables, then reset the game

  player->set_animating(false);
  player->set_current_frame(0);
  player->erase_position();
  player->set_posX(1);
  player->set_posY(0);
  laser->set_drawing(false);
  randomize_all_asteroids();
  score_lastTime = currentTime;
  score_value = 0;
  start_lastTime = currentTime;
  game_state = STARTING;
}