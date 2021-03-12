/* Matthew Holman          12-14-2020
   Small game demo
*/

// VIDEO: https://youtu.be/2Ip64RKjMUo

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
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Keypad.h>
#include "PortraitIntegers.h"
#include "GameObject.h"

// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4); // Change to (0x27,20,4) for 20x4 LCD.

// Keypad
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {7, 0, 1, 3}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {2, 4, 5, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// "Art" assets
byte SHIP[] = {
  B11110,
  B11000,
  B01100,
  B11011,
  B11011,
  B01100,
  B11000,
  B11110
};

// Game functions
void reset_asteroid(GameObject *asteroid);
void randomize_all_asteroids();

// Game variables
enum GAME_STATE {STARTING, PLAYING};
GAME_STATE game_state = STARTING;

int score_value = 0;
const int num_asteroids = 6;
const int num_rows = 19;
const int num_cols = 4;

// Game objects
PortraitInt *score_display;
GameObject *player;
GameObject *asteroids[num_asteroids];

void setup() {
  //Serial.begin(9600);

  // Randomize the random number generator
  randomSeed(analogRead(3));

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();

  // Set up score
  score_display = new PortraitInt(4, 19, 0, &lcd);

  // Set up player
  player = new GameObject(1, 0, num_cols, num_rows, 4, SHIP, &lcd);

  // Set up asteroids
  for (int i = 0; i < num_asteroids; i++) {
    asteroids[i] = new GameObject(0, 0, num_cols, num_rows, '*', &lcd);
  }
  randomize_all_asteroids();
}

void loop() {
  // timing variables
  unsigned long currentTime;

  static unsigned long lastTime = 0;
  static const unsigned long wait_time = 5;

  static unsigned long start_lastTime;
  static const unsigned long start_wait_time = 2000;

  static unsigned long score_lastTime = 0;
  static const unsigned long score_wait_time = 500;

  char key = keypad.getKey();

  currentTime = millis();
  if (currentTime - lastTime > wait_time) {
    lastTime = currentTime;

    switch (game_state) {
      case STARTING:
        if (currentTime - start_lastTime > start_wait_time) {
          start_lastTime = currentTime;
          game_state = PLAYING;
        }
        break;
      case PLAYING:
        // Handle player input
        if (key == '4') { // Move left
          player->move(0);
        } else if (key == '6') { // Move right
          player->move(1);
        } else if (key == '8') { // Move down
          player->move(2);
        } else if (key == '2') { // Move up
          player->move(3);
        }

        // Handle asteroid movement
        for (int i = 0; i < num_asteroids; i++) {
          // Update the asteroids' timers
          asteroids[i]->update_timer(currentTime);

          // If the timer has expired, move the asteroid down the screen
          if (asteroids[i]->get_timerExpired() == true) {
            asteroids[i]->move(2);
            asteroids[i]->reset_timer();
          }

          // If an asteroid has collided with the player, reset the score time and set score to zero before starting the game over
          if (asteroids[i]->check_collision(player) == true) {
            randomize_all_asteroids();
            player->erase_position();
            player->set_posX(1);
            player->set_posY(0);
            score_lastTime = currentTime;
            score_value = 0;
            start_lastTime = currentTime;
            game_state = STARTING;
          }

          // If an asteroid has reached the 'bottom' of the screen, make it reappear at the top
          if (asteroids[i]->get_posY() == 0) {
            reset_asteroid(asteroids[i]);
          }
        }

        // Increment score values every 'x' milliseconds
        if (currentTime - score_lastTime > score_wait_time) {
          score_lastTime = currentTime;

          score_value++;
        }

        // Keep Score in range
        if (score_value > 9999) {
          score_value = 0;
        } else if (score_value < 0) {
          score_value = 9999;
        }

        score_display->set_value(score_value);

        break;
      default:
        break;
    }
    score_display->print_value();
    player->draw();
    for (int i = 0; i < num_asteroids; i++) {
      asteroids[i]->draw();
    }
  }
}

void reset_asteroid(GameObject *asteroid) {
  asteroid->erase_position();
  asteroid->set_posY(num_rows - 1);
  asteroid->set_posX(random(0, num_cols));
}

void randomize_all_asteroids() {
  for (int i = 0; i < num_asteroids; i++) {
    asteroids[i]->erase_position();
    asteroids[i]->set_posX(random(0, num_cols));
    asteroids[i]->set_posY(random(3, num_rows));
    asteroids[i]->set_timer_init(random(300, 500));
  }
}
