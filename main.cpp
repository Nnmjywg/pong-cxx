// main.cpp

#include <iostream> // For io
#include <fstream>  // For logging
#include <stack> //
#include <math.h> // abs()
#include <time.h> // Rand seeder
#include <stdint.h>


// For language implementation I will later include unistd and I can implement getopt.
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "variables.h"
//#include "enums.h"

// Game Entity data structure
struct GameObject
{
  SDL_Rect ScreenLocation;
  SDL_Rect BitmapLocation;

  int SpeedX;
  int SpeedY;

  int Direction;
};

// Global data //

// SDL stuff
SDL_Window* gWindow = NULL;
SDL_Renderer* gWindowRenderer;
SDL_Rect gScreen;
SDL_Event e;

// Game objects
GameObject gPlayer1; // Paddle 1
GameObject gPlayer2; // Paddle 2
GameObject gBall; // Ball

// Scores. I feel like it's better to leave them uninitialized.
int gPlayer1Score;
int gPlayer2Score;

// Global functions //
bool CheckWallCollision(GameObject& object);
bool BallCollision(GameObject& paddle, GameObject& paddle2, GameObject& Ball);

void HandleBall();
void MoveBall(GameObject& Ball);


void Player1Score();
void Player2Score();

//int DisplayText(std::string text, int x, int y, int w, int h);

void Render();

bool init();
void log();

bool GameQuit;

//int input(const Uint8* Keyboard);

void Quit();

////

SDL_Rect OUpRect;
SDL_Rect OLeftRect;
SDL_Rect ORightRect;
SDL_Rect ODownRect;

class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};

LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }
    return time;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
    return mPaused && mStarted;
}

int main(int argc, char *argv[])
{
  bool pause = false;
  std::cout << "\t.:Pong:.\n\n(LTimer class by lazyfoo)\n";
  std::cout << "Player 1 score: [" << gPlayer1Score << "]\n";
  std::cout << "Player 2 score: [" << gPlayer2Score << "]\n\n";
  //The frames per second timer
  LTimer fpsTimer;

  //The frames per second cap timer
  LTimer capTimer;
  //Start counting frames per second
  int countedFrames = 0;
  fpsTimer.start();

  SDL_Texture* texture1;
  const Uint8* keyStates = SDL_GetKeyboardState(NULL);
  if(init() == false) {
    std::cout << "Init failed. Bye bye!\n";
    return 1;
  }
  while(!GameQuit)
  {
    const Uint8* keyStates = SDL_GetKeyboardState(NULL);
    while(SDL_PollEvent(&e))
    {
      if(e.type == SDL_QUIT)
        GameQuit = true;
      if(e.type == SDL_KEYDOWN)
      {
        switch(e.key.keysym.scancode)
        {
          case SDL_SCANCODE_P:
            pause = !pause;
          break;

          case SDL_SCANCODE_ESCAPE:
            GameQuit = true;
          break;
        }
      }
    }

    if(!pause && gPlayer1Score != 10 && gPlayer2Score != 10)
    {
      MoveBall(gBall);
      BallCollision(gPlayer1, gPlayer2, gBall);
      CheckWallCollision(gBall);
    }
    if(keyStates[SDL_SCANCODE_UP] && !pause)
    {
      if(gPlayer2.ScreenLocation.y > 1)
        gPlayer2.ScreenLocation.y -= PLAYER_SPEED_EASY;
    }

    if(keyStates[SDL_SCANCODE_DOWN] && !pause)
    {
      if(gPlayer2.ScreenLocation.y < ((WINDOW_HEIGHT - PADDLE_HEIGHT) - 1))
        gPlayer2.ScreenLocation.y += PLAYER_SPEED_EASY;
    }

    if(keyStates[SDL_SCANCODE_W] && !pause)
    {
      if(gPlayer1.ScreenLocation.y > 1)
        gPlayer1.ScreenLocation.y -= PLAYER_SPEED_EASY;
    }

    if(keyStates[SDL_SCANCODE_S] && !pause)
    {
      if(gPlayer1.ScreenLocation.y < ((WINDOW_HEIGHT - PADDLE_HEIGHT) - 1))
        gPlayer1.ScreenLocation.y += PLAYER_SPEED_EASY;
    }

    if(keyStates[SDL_SCANCODE_R] && keyStates[SDL_SCANCODE_LSHIFT])
    {
      if(gPlayer1Score == 10 || gPlayer2Score == 10)
      {
        gPlayer1Score = 0;
        gPlayer2Score = 0;

        gPlayer1.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
        gPlayer1.ScreenLocation.x = PADDLE1_X;

        gPlayer2.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
        gPlayer2.ScreenLocation.x = PADDLE2_X;

        gBall.ScreenLocation.x = BALL_CENTER_X;
        gBall.ScreenLocation.y = BALL_CENTER_Y;

        gBall.Direction = rand() % 6 + 1;
      }
    }

    if(keyStates[SDL_SCANCODE_X])
    {
      gPlayer1.ScreenLocation.y = (((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2)) + 20);
      gPlayer1.ScreenLocation.x = PADDLE1_X;

      gPlayer2.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
      gPlayer2.ScreenLocation.x = PADDLE2_X;

      gBall.ScreenLocation.x = BALL_CENTER_X;
      gBall.ScreenLocation.y = BALL_CENTER_Y;

      gBall.Direction = LEFT;
    }

    if(keyStates[SDL_SCANCODE_Q] && keyStates[SDL_SCANCODE_LSHIFT])
    {
      std::cout << "Player 1 score: [" << gPlayer1Score << "]\n";
      std::cout << "Player 2 score: [" << gPlayer2Score << "]\n";
      GameQuit = true;
    }
    capTimer.start();
    //Calculate and correct fps
    float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
    if( avgFPS > 2000000 )
    {
        avgFPS = 0;
    }
    if(!pause)
    {
      Render();
    }
    ++countedFrames;
    int frameTicks = capTimer.getTicks();
    if( frameTicks < SCREEN_TICKS_PER_FRAME )
    {
        //Wait remaining time
        SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
    }
  }
  Quit();
  return 0;
}

////

bool init()
{
  bool success = true;

  if(SDL_Init(SDL_INIT_VIDEO < 0))
  {
    success = false;
    return success;
  }

  if(TTF_Init() < 0)
  {
    success = false;
    return success;
  }
  srand(time(NULL)); // Seed rng

  // From SDL_Rect.h
  // 64 typedef struct SDL_Rect
  // 65 {
  // 66  int x, y;
  // 67  int w, h;
  // 68 } SDL_Rect;
  GameQuit = false;

  gPlayer1.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
  gPlayer1.ScreenLocation.x = PADDLE1_X;
  gPlayer1.ScreenLocation.w = PADDLE_WIDTH;
  gPlayer1.ScreenLocation.h = PADDLE_HEIGHT;

  gPlayer2.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
  gPlayer2.ScreenLocation.x = PADDLE2_X;
  gPlayer2.ScreenLocation.w = PADDLE_WIDTH;
  gPlayer2.ScreenLocation.h = PADDLE_HEIGHT;

  gBall.ScreenLocation.x = BALL_CENTER_X;
  gBall.ScreenLocation.y = BALL_CENTER_Y;
  gBall.ScreenLocation.w = BALL_DIAMETER;
  gBall.ScreenLocation.h = BALL_DIAMETER;

  gPlayer1.BitmapLocation.x = PADDLE_BITMAP_X;
  gPlayer1.BitmapLocation.y = PADDLE_BITMAP_Y;
  gPlayer1.BitmapLocation.w = PADDLE_WIDTH;
  gPlayer1.BitmapLocation.h = PADDLE_HEIGHT;

  gPlayer2.BitmapLocation.x = PADDLE_BITMAP_X;
  gPlayer2.BitmapLocation.y = PADDLE_BITMAP_Y;
  gPlayer2.BitmapLocation.w = PADDLE_WIDTH;
  gPlayer2.BitmapLocation.h = PADDLE_HEIGHT;

  gBall.BitmapLocation.x = BALL_BITMAP_X;
  gBall.BitmapLocation.y = BALL_BITMAP_Y;
  gBall.BitmapLocation.w = BALL_DIAMETER;
  gBall.BitmapLocation.h = BALL_DIAMETER;

  gPlayer1.SpeedY = PLAYER_SPEED_EASY;
  gPlayer1.SpeedX = 0;
  gPlayer2.SpeedY = PLAYER_SPEED_EASY;
  gPlayer2.SpeedX = 0;

  gPlayer1Score = 0;
  gPlayer2Score = 0;

  gScreen.x = 0;
  gScreen.y = 0;
  gScreen.w = WINDOW_WIDTH;
  gScreen.h = WINDOW_HEIGHT;

  OUpRect.x = 0;
  OUpRect.y = -10;
  OUpRect.w = WINDOW_WIDTH;
  OUpRect.h = 10;

  OLeftRect.x = -10;
  OLeftRect.y = 0;
  OLeftRect.w = 10;
  OLeftRect.h = WINDOW_HEIGHT;

  ODownRect.x = 0;
  ODownRect.y = WINDOW_HEIGHT;
  ODownRect.w = WINDOW_WIDTH;
  ODownRect.h = 10;

  ORightRect.x = WINDOW_WIDTH;
  ORightRect.y = 0;
  ORightRect.w = 10;
  ORightRect.h = WINDOW_HEIGHT;

  gBall.Direction = rand() % 6 + 1;

  gWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_MAXIMIZED);
  // Render SDL Window
  gWindowRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
  if(gWindowRenderer == nullptr)
  {
    success = false;
    return success;
  }
  SDL_RenderSetLogicalSize(gWindowRenderer, WINDOW_WIDTH, WINDOW_HEIGHT);
  SDL_SetRenderDrawColor(gWindowRenderer, 0x00, 0x00, 0x00, 0xFF);

  return success;
}

void Render()
{
  SDL_RenderClear(gWindowRenderer);
  SDL_SetRenderDrawColor(gWindowRenderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderFillRect(gWindowRenderer, &gScreen);

  SDL_SetRenderDrawColor(gWindowRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

  SDL_RenderFillRect(gWindowRenderer, &gPlayer1.ScreenLocation);
  SDL_RenderFillRect(gWindowRenderer, &gPlayer2.ScreenLocation);
  SDL_RenderFillRect(gWindowRenderer, &gBall.ScreenLocation);

  SDL_RenderPresent(gWindowRenderer);
}

void Quit()
{
  SDL_DestroyWindow(gWindow);
  SDL_Quit();
}

void MoveBall(GameObject& Ball)
{
    if(Ball.Direction == LEFT)
    {
      Ball.ScreenLocation.x -= BALL_SPEED;
    }

    else if(Ball.Direction == LEFTUP)
    {
      Ball.ScreenLocation.x -= BALL_SPEED;
      Ball.ScreenLocation.y += BALL_SPEED;
    }

    else if(Ball.Direction == RIGHTUP)
    {
      Ball.ScreenLocation.x += BALL_SPEED;
      Ball.ScreenLocation.y += BALL_SPEED;
    }

    else if(Ball.Direction == RIGHT)
    {
      Ball.ScreenLocation.x += BALL_SPEED;
    }

    else if(Ball.Direction == RIGHTDOWN)
    {
      Ball.ScreenLocation.x += BALL_SPEED;
      Ball.ScreenLocation.y -= BALL_SPEED;
    }

    else if(Ball.Direction == LEFTDOWN)
    {
      Ball.ScreenLocation.x -= BALL_SPEED;
      Ball.ScreenLocation.y -= BALL_SPEED;
    }
}

bool BallCollision(GameObject& paddle, GameObject& paddle2, GameObject& Ball)
{
  // Paddle 1 and ball
  if(SDL_HasIntersection(&paddle.ScreenLocation, &Ball.ScreenLocation))
  {
    if(Ball.ScreenLocation.x >= paddle.ScreenLocation.x && Ball.ScreenLocation.x <= (paddle.ScreenLocation.x + PADDLE_WIDTH))
    {
      // Middle of paddle
      if(Ball.ScreenLocation.y >= paddle.ScreenLocation.y + (PADDLE_HEIGHT/3) && Ball.ScreenLocation.y <= paddle.ScreenLocation.y + (PADDLE_HEIGHT*2/3))
      {
        switch(Ball.Direction)
        {
          case LEFT:
            Ball.Direction = RIGHT;
            break;
          case LEFTUP:
            Ball.Direction = RIGHTUP;
          case LEFTDOWN:
            Ball.Direction = RIGHTDOWN;
            break;
        }
        return true;
      }

      // Top of paddle
      if(Ball.ScreenLocation.y >= paddle.ScreenLocation.y && Ball.ScreenLocation.y < paddle.ScreenLocation.y + (PADDLE_HEIGHT/3))
      {
        switch(Ball.Direction)
        {
          case LEFT:
            Ball.Direction = RIGHTUP;
            break;
          case LEFTUP:
            Ball.Direction = RIGHT;
          case LEFTDOWN:
            Ball.Direction = RIGHTDOWN;
            break;
        }
        return true;
      }

      // Bottom of paddle
      if(Ball.ScreenLocation.y > paddle.ScreenLocation.y + (PADDLE_HEIGHT*2/3) && Ball.ScreenLocation.y <= paddle.ScreenLocation.y + (PADDLE_HEIGHT))
      {
        switch(Ball.Direction)
        {
          case LEFT:
            Ball.Direction = RIGHTDOWN;
            break;
          case LEFTUP:
            Ball.Direction = RIGHTUP;
          case LEFTDOWN:
            Ball.Direction = RIGHT;
            break;
        }
        return true;
      }
    }
  }

  else if(SDL_HasIntersection(&paddle2.ScreenLocation, &Ball.ScreenLocation))
  {
    if((Ball.ScreenLocation.x + BALL_DIAMETER) >= paddle2.ScreenLocation.x && (Ball.ScreenLocation.x + BALL_DIAMETER) <= (paddle2.ScreenLocation.x + PADDLE_WIDTH))
    {
      // Middle of paddle
      if((Ball.ScreenLocation.y + (BALL_DIAMETER/2)) >= paddle2.ScreenLocation.y + (PADDLE_HEIGHT/3) && (Ball.ScreenLocation.y + (BALL_DIAMETER/2)) <= paddle2.ScreenLocation.y + (PADDLE_HEIGHT*2/3))
      {
        switch(Ball.Direction)
        {
          case RIGHT:
            Ball.Direction = LEFT;
            break;
          case RIGHTUP:
            Ball.Direction = LEFTUP;
          case RIGHTDOWN:
            Ball.Direction = LEFTDOWN;
            break;
        }
        return true;
      }

      // Top of paddle
      if(Ball.ScreenLocation.y >= paddle2.ScreenLocation.y && Ball.ScreenLocation.y < paddle2.ScreenLocation.y + (PADDLE_HEIGHT/3))
      {
        switch(Ball.Direction)
        {
          case RIGHT:
            Ball.Direction = LEFTUP;
            break;
          case RIGHTUP:
            Ball.Direction = LEFT;
          case RIGHTDOWN:
            Ball.Direction = LEFTDOWN;
            break;
        }
        return true;
      }

      // Bottom of paddle
      if(Ball.ScreenLocation.y > paddle2.ScreenLocation.y + (PADDLE_HEIGHT*2/3) && Ball.ScreenLocation.y <= paddle2.ScreenLocation.y + (PADDLE_HEIGHT))
      {
        switch(Ball.Direction)
        {
          case RIGHT:
            Ball.Direction = LEFTDOWN;
            break;
          case RIGHTUP:
            Ball.Direction = LEFTUP;
          case RIGHTDOWN:
            Ball.Direction = LEFT;
            break;
        }
        return true;
      }
    }
  }
  else
  {
    return false;
  }
}

bool CheckWallCollision(GameObject& object)
{
  if(SDL_HasIntersection(&OUpRect, &object.ScreenLocation))
  {
    switch(object.Direction)
    {
      case LEFTUP:
        object.Direction = LEFTDOWN;
        break;
      case LEFTDOWN:
        object.Direction = LEFTUP;
        break;
      case RIGHTUP:
        object.Direction = RIGHTDOWN;
        break;
      case RIGHTDOWN:
        object.Direction = RIGHTUP;
        break;
    }
    return true;
  }

  if(SDL_HasIntersection(&ODownRect, &object.ScreenLocation))
  {
    switch(object.Direction)
    {
      case LEFTUP:
        object.Direction = LEFTDOWN;
        break;
      case LEFTDOWN:
        object.Direction = LEFTUP;
        break;
      case RIGHTUP:
        object.Direction = RIGHTDOWN;
        break;
      case RIGHTDOWN:
        object.Direction = RIGHTUP;
        break;
    }
    return true;
  }

  if(SDL_HasIntersection(&OLeftRect, &object.ScreenLocation))
  {
    gPlayer2Score++;
    std::cout << "Player 1 score: [" << gPlayer1Score << "]\n";
    std::cout << "Player 2 score: [" << gPlayer2Score << "]\n\n";

    SDL_Delay(2000);
    gPlayer1.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
    gPlayer1.ScreenLocation.x = PADDLE1_X;

    gPlayer2.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
    gPlayer2.ScreenLocation.x = PADDLE2_X;

    object.ScreenLocation.x = BALL_CENTER_X;
    object.ScreenLocation.y = BALL_CENTER_Y;

    object.Direction = rand() % 6 + 1;
    return true;
  }

  if(SDL_HasIntersection(&ORightRect, &object.ScreenLocation))
  {
    gPlayer1Score++;
    std::cout << "Player 1 score: [" << gPlayer1Score << "]\n";
    std::cout << "Player 2 score: [" << gPlayer2Score << "]\n\n";

    SDL_Delay(2000);
    gPlayer1.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
    gPlayer1.ScreenLocation.x = PADDLE1_X;

    gPlayer2.ScreenLocation.y = ((WINDOW_HEIGHT / 2) - (PADDLE_HEIGHT / 2));
    gPlayer2.ScreenLocation.x = PADDLE2_X;

    object.ScreenLocation.x = BALL_CENTER_X;
    object.ScreenLocation.y = BALL_CENTER_Y;

    object.Direction = rand() % 6 + 1;
    return true;
  }

  else {
    return false;
  }
}
