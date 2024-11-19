#include "Game.h"
#include <algorithm>

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
	: mWindow(nullptr)
	, mRenderer(nullptr)
	, mTicksCount(0)
	, mIsRunning(true)
	, mLeftPaddleDir(0)
	, mBallCount(0)
{

}

bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL : %s", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow(
		"GameProgramming in C++(Chpater1)"
		, 100
		, 100
		, 1024
		, 768
		, 0);

	if (!mWindow)
	{
		SDL_Log("Failed to Create Window : %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow
		, -1
		, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer : %s ", SDL_GetError());
		return false;
	}

	mLeftPaddlePos.x = 10.0f;
	mLeftPaddlePos.y = 768.0f / 2.0f;

	mRightPaddlePos.x = 1024.0f - 20.0f;
	mRightPaddlePos.y = 768.0f / 2.0f;


	for (int i = 0; i < 10; i++)
	{
		mBallPos.x = 1024.0f / 2.0f + i *3;
		mBallPos.y = 768.0f / 2.0f + i * 4;
		mBallVel.x = -200.0f + i*4;
		mBallVel.y = 235.0f + i * 3;
		mBallVector.push_back(Ball{ mBallPos,mBallVel });
		mBallCount++;
	}
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mLeftPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mLeftPaddleDir -= 1;
	}

	if (state[SDL_SCANCODE_S])
	{
		mLeftPaddleDir += 1;
	}

	mRightPaddleDir = 0;
	if (state[SDL_SCANCODE_I])
	{
		mRightPaddleDir -= 1;
	}

	if (state[SDL_SCANCODE_K])
	{
		mRightPaddleDir += 1;
	}

}
void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();

	// Update paddle position based on direction
	if (mLeftPaddleDir != 0)
	{
		mLeftPaddlePos.y += mLeftPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mLeftPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mLeftPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mLeftPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mLeftPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	if (mRightPaddleDir != 0)
	{
		mRightPaddlePos.y += mRightPaddleDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mRightPaddlePos.y < (paddleH / 2.0f + thickness))
		{
			mRightPaddlePos.y = paddleH / 2.0f + thickness;
		}
		else if (mRightPaddlePos.y > (768.0f - paddleH / 2.0f - thickness))
		{
			mRightPaddlePos.y = 768.0f - paddleH / 2.0f - thickness;
		}
	}

	for (int i = 0; i < mBallCount; i++)
	{// Update ball position based on ball velocity
		mBallPos = mBallVector[i].ballPos;
		mBallVel = mBallVector[i].ballVel;
		
		mBallPos.x += mBallVel.x * deltaTime;
		mBallPos.y += mBallVel.y * deltaTime;

		// Bounce if needed
		// Did we intersect with the paddle?
		float leftDiff = mLeftPaddlePos.y - mBallPos.y;
		float rightDiff = mRightPaddlePos.y - mBallPos.y;
		// Take absolute value of difference
		leftDiff = (leftDiff > 0.0f) ? leftDiff : -leftDiff;
		rightDiff = (rightDiff > 0.0f) ? rightDiff : -rightDiff;
		if (
			// Our y-difference is small enough
			leftDiff <= paddleH / 2.0f &&
			// We are in the correct x-position
			mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
			// The ball is moving to the left
			mBallVel.x < 0.0f)
		{
			mBallVel.x *= -1.0f;
		}
		else if (
			rightDiff <= paddleH / 2.0f &&
			// We are in the correct x-position
			mBallPos.x <= 1012.0f && mBallPos.x >= 995.0f &&
			// The ball is moving to the left
			mBallVel.x > 0.0f)
		{
			mBallVel.x *= -1.0f;
		}
		
		//// Did the ball collide with the right wall?
		//else if (mBallPos.x >= (1024.0f - thickness) && mBallVel.x > 0.0f)
		//{
		//	mBallVel.x *= -1.0f;
		//}

		// Did the ball collide with the top wall?
		if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
		{
			mBallVel.y *= -1;
		}
		// Did the ball collide with the bottom wall?
		else if (mBallPos.y >= (768 - thickness) &&
			mBallVel.y > 0.0f)
		{
			mBallVel.y *= -1;
		}
		mBallVector[i].ballPos = mBallPos;
		mBallVector[i].ballVel = mBallVel;
		// Did the ball go off the screen? (if so, end game)
		if (mBallPos.x <= 0.0f || mBallPos.x >= 1024.0f)
		{
			if (i != mBallCount)
				std::swap(mBallVector[i], mBallVector[mBallCount - 1]);
			mBallVector.pop_back();
			--mBallCount;
			if (mBallCount <= 0)
				mIsRunning = false;
		}
	}
}
void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(
		mRenderer
		, 0
		, 0
		, 255
		, 255
	);

	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

	SDL_Rect wall{ 0
		,0
		,1024
		,thickness
	};
	SDL_RenderFillRect(mRenderer, &wall);

	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);


	
	SDL_Rect leftPaddle{
		 static_cast<int>(mLeftPaddlePos.x)
		,static_cast<int>(mLeftPaddlePos.y - paddleH / 2)
		,thickness
		,static_cast<int>(paddleH)
	};
	
	SDL_RenderFillRect(mRenderer, &leftPaddle);

	SDL_Rect rightPaddle{
		 static_cast<int>(mRightPaddlePos.x)
		,static_cast<int>(mRightPaddlePos.y - paddleH / 2)
		,thickness
		,static_cast<int>(paddleH)
	};

	SDL_RenderFillRect(mRenderer, &rightPaddle);

	for (int i = 0; i < mBallCount; i++)
	{
		mBallPos = mBallVector[i].ballPos;
		mBallVel = mBallVector[i].ballVel;
		SDL_Rect ball{
			 static_cast<int>(mBallPos.x - thickness / 2)
			,static_cast<int>(mBallPos.y - thickness / 2)
			,thickness
			,thickness
		};

		SDL_RenderFillRect(mRenderer, &ball);
	}
	SDL_RenderPresent(mRenderer);

}