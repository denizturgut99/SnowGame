#pragma once
#include <SDL.h>
#include "Renderer.h"
#include "CharacterManager.h"
#include "TileManager.h"
#include "ArrowManager.h"

class Game {
public:
	Game();
	~Game();
	void gameLoop();
	void Init();
	bool isRunning() { return isGameRunning; }
	void destroy();
	bool isGoingToCollide(SDL_Rect *futureDest);
	void handleGeneralMovement();
	void movePlayerSide(int velocity);
	void jump();
	void addEnemy();
	void dropCharacter(CharacterManager* character, bool collision, bool isPlayer);
	bool isPlayerInVision();
	void enemyFireAnim();
	bool canEnemyFire();
	void arrowTravel();
	void setInitialArrowLoc();
private:
	bool isGameRunning = false;
	Renderer* gameWindow = nullptr;
	CharacterManager* player = nullptr;
	CharacterManager* enemy = nullptr;
	vector<TileManager> loadedMap;
	bool isJumping = false;
	int target;
	Uint32 timePrev = 0, timeCurr = 0;
	int sideMovementSpeed = 2;
	Uint32 fps = 60;
	int counter = 0;
	ArrowManager* arrow = nullptr;
	bool isArrowFlying = false;
	SDL_Rect playerInitialDest = {};
	int jumpCounter = 0;
	bool isSlideComplete = false;
	vector<TileManager> snowballs;
 };