#pragma once
#include "SDL\SDL.h"
#include <vector>

struct Vector2
{
	float x;
	float y;
};

struct Ball
{
	Vector2 ballPos;
	Vector2 ballVel;
};

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

private:

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	Uint32 mTicksCount;
	bool mIsRunning;

	int mLeftPaddleDir;
	int mRightPaddleDir;
	Vector2 mLeftPaddlePos;
	Vector2 mRightPaddlePos;
	Vector2 mBallPos;
	Vector2 mBallVel;
	std::vector<Ball> mBallVector;
	Uint32 mBallCount;
};