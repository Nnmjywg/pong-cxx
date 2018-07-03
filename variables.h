// variables.h
// (C) 2018 nnmjywg

#ifndef VARIABLES_H
#define VARIABLES_H

const bool debug = true;

// FPS cap stuff
const int   SCREEN_FPS = 60;
const int   SCREEN_TICKS_PER_FRAME = 1000/SCREEN_FPS;

// Basic stuff needed for SDL2
const int   WINDOW_WIDTH = 800;
const int   WINDOW_HEIGHT = 600;

const int   WINDOW_CENTER_X = WINDOW_WIDTH / 2;
const int   WINDOW_CENTER_Y = WINDOW_HEIGHT / 2;

const char* WINDOW_TITLE = "Pong";

// Define the location of the paddles in the bitmap
const int   PADDLE_BITMAP_X = 0;
const int   PADDLE_BITMAP_Y = 0;

const int   BALL_BITMAP_X = 100;
const int   BALL_BITMAP_Y = 0;

// Positions of player 1 and player 2
const int   PADDLE1_X = 50;
const int   PADDLE2_X = WINDOW_WIDTH - 75;

// Paddle width and height
const int   PADDLE_WIDTH = 20;
const int   PADDLE_HEIGHT = 100;

// Speed of both players
const int   PLAYER_SPEED_EASY = 0x10;
const int   PLAYER_SPEED_HARD = 0x05;

// Hexadecimal is fun!
// Ball Speed -- this will not be constant
// The Ball Modifier will modify the speed based on the place it lands on the paddle
// Ball speed will never go under 10, and will accelerate over time
// Ball diameter
const int   BALL_SPEED = 0x07;
const int   BALL_MODIFIER = 5;
const int   BALL_DIAMETER = 10;

const int   BALL_CENTER_X = WINDOW_CENTER_X - (BALL_DIAMETER/2);
const int   BALL_CENTER_Y = WINDOW_CENTER_Y + (BALL_DIAMETER/2);

const int LANG = 3;
const int LANG_ENGLISH = 3;
const int LANG_GERMAN = 4;

const int   LEFT = 1;
const int   LEFTUP = 2;
const int   RIGHTUP = 3;
const int   RIGHT = 4;
const int   RIGHTDOWN = 5;
const int   LEFTDOWN = 6;

#endif
