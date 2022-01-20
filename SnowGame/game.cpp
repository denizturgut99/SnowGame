#include "game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Renderer.h"
#include "TileManager.h"
#include <vector>
#include "ArrowManager.h"

using namespace std;

Game::Game() {}

Game::~Game() {}

void Game::Init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		cout << "Game Started." << endl;
		gameWindow = new Renderer(); // this will init the game screen
		gameWindow->loadMap("assets/maps/newtilemap"); // load the map from a file with png ids
		gameWindow->initSnowballs();
		snowballs = gameWindow->getSnowballs();
		player = new CharacterManager(); // creates player char
		addEnemy(); // function to and an enemy to the map
		arrow = new ArrowManager(); // create arrow
		loadedMap = gameWindow->getMap(); // get currently loaded map, the map is loaded in line 20
		bool isFirstCollidable = false; // local bool created to make sure player spawns in a specific firstly acquired collidable ground
		playerInitialDest = player->getDestination(); // player's initial location is saved to teleport the player back if they are hit by the arrow
		
		// for loop to get specific tile(s) to spawn the player
		for (TileManager &tile : loadedMap) {
			if (!isFirstCollidable && tile.getCollidableStatus() && tile.getDestination().x == 16) {
				isFirstCollidable = true; // value set to true so that the playerInitialDest doesn't get updated again
				// player's height is removed from tile.y to make sure player is on top of the tile and not inside it
				playerInitialDest.y = tile.getDestination().y - playerInitialDest.h;
				playerInitialDest.x = tile.getDestination().x;
				player->setDestination(playerInitialDest); // player's initial location is updated and saved
			}
		}

		isGameRunning = true; // after all of the initialising functions are finished, the game loop starts
	}
	else {
		// if there was an error with initialising SDL_INIT_EVERYTHING, the error will be logged into the console
		cout << "ERROR: SDL_INIT_EVERYTHING wasn't initialized. Info: " << SDL_GetError() << endl;
		isGameRunning = false;
	}
}

void Game::gameLoop() {
	timePrev = SDL_GetTicks(); // "previous" time is updated each frame
	timeCurr = (SDL_GetTicks() - timePrev); // current time is calculated by removing timePrev from SDL_GetTicks()
	SDL_Event event; // initialise events
	gameWindow->clear(); // clear renderer
	gameWindow->draw(gameWindow->getMap()); // draw map
	gameWindow->draw(gameWindow->getSnowballs()); // draw snowballs
	snowballs = gameWindow->getSnowballs(); // get the current snowballs vector

	// as long as there are events the while loop will run
	// if the event is SDL_QUIT, isGameRunning will be set to false and the game will be terminated
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			isGameRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_SPACE:
				// SPACE is handled here so that the player can slightly move forward/backward while falling
				// i wanted to avoid having too many else if conditions
				if (!isJumping) jump();
				break;
			};
			break;
		}
	}

	// fps is 60
	// if 1000 / fps is greater than timeCurr, SDL_Delay runs to cap the frames at 60
	// done this with a help from a tutorial: https://www.youtube.com/watch?v=d2P-7oWaAIo
	if ((1000 / fps) > timeCurr) SDL_Delay((1000 / fps) - timeCurr);

	handleGeneralMovement(); // all types of movements get handled here

	// if arrow's x and y values are 0, the location gets reset back to it's original place
	if (arrow->getDest().x == 0 && arrow->getDest().y == 0) {
		setInitialArrowLoc();
	}

	SDL_Rect playerDest = player->getDestination();
	SDL_Rect arrowDest = arrow->getDest();

	// if the player is hit by the arrow, player's location gets reset back to their spawning location
	if (player->isPlayerHit(&playerDest, &arrowDest)) {
		setInitialArrowLoc();
		player->setDestination(playerInitialDest);
	}

	for (int i = 0; i < snowballs.size(); i++) {
		SDL_Rect snowballDest = snowballs[i].getDestination();
		if (player->isSnowballCollected(&playerDest, &snowballDest)) {
			// if there is a collision with player and a snowball, the snowball will be removed from the snowballs vector
			// this will make it so that the specific snowball is no longer in the map
			snowballs.erase(snowballs.begin() + i);
			gameWindow->setSnowballs(snowballs);
		}
	}

	gameWindow->displayTexture(); // after all of the functions are done, the textures are displayed
}

void Game::destroy() {
	// when isGameRunning is set to false, main.cpp triggers this function which in return triggers a clean function in Rendere class
	gameWindow->destroyAndQuit();
}

void Game::handleGeneralMovement() {
	// increasing the SDL_Rect y number by 1 and 2 to check if there will be collision before the characters actually drop down
	SDL_Rect playerDestCopy = player->getDestination();
	playerDestCopy.y += 1;
	SDL_Rect enemyDestCopy = enemy->getDestination();
	enemyDestCopy.y += 2;

	// if the enemy is unable to fire and is still alive, idle animation will play
	// else if the enemy is hit by the player, the dead animation will play and the enemy will be laying on the ground
	if (!canEnemyFire() && !(enemy->getIsEnemyHit())) {
		gameWindow->handleCharacter(enemy->getEnemyIdleAnims().anim, enemy, enemy->getEnemyIdleAnims().numOfAnims);
	}
	else if (enemy->getIsEnemyHit()) {
		gameWindow->handleCharacter(enemy->getEnemyDeadAnims().anim, enemy, enemy->getEnemyDeadAnims().numOfAnims);
	}

	if (isPlayerInVision()) {
		counter > 150 ? counter = 0 : counter++;
		// canEnemyFire is being run inside isPlayerInVision because canEnemyFire 
		// has a counter which should start when the player is in archer's vision to delay archer's fire
		if (canEnemyFire() && !(enemy->getIsEnemyHit())) {
			enemyFireAnim();
			arrowTravel();
		}
		else setInitialArrowLoc();
	}
	else counter = 0; // counter is reset if the player leaves archer's vision

	dropCharacter(enemy, isGoingToCollide(&enemyDestCopy), false); // explained in the function itself
	dropCharacter(player, isGoingToCollide(&playerDestCopy), true); // explained in the function itself

	const Uint8* keystate = SDL_GetKeyboardState(NULL); // https://gamedev.stackexchange.com/a/19580 && https://wiki.libsdl.org/SDL_GetKeyboardState

	if (keystate[SDL_SCANCODE_A]) {
		// Left
		// when the player starts moving left, the sprite will be flipped horizontally.
		player->setCharacterFlip(rendererFlip::SDL_FLIP_HORIZONTAL);
		// move the enemy towards the left by making the velocity a negative number
		movePlayerSide(-sideMovementSpeed);
	}
	else if (keystate[SDL_SCANCODE_D]) {
		// Right
		// the sprite flip is reset when the player starts moving towards right
		player->setCharacterFlip(rendererFlip::SDL_FLIP_NONE);
		// positive numbers are passed into param to move the player towards right side
		movePlayerSide(sideMovementSpeed);
	}
	else if (keystate[SDL_SCANCODE_S]) {
		// slide
		// when player clicks S, the slide animation will start and isSlideComplete will be set to false
		isSlideComplete = false;
		SDL_Rect playerDest = player->getDestination(); // get player's current SDL_Rect location
		playerDest.h = 15; // height is set to 15 because the pngs for sliding animation have height of 15px
		player->setDestination(playerDest); // after setting the height to 15, player's height is set to 15 temporarily
		// handleCharacter is used to animated player and enemy sprite(s)
		gameWindow->handleCharacter(player->getPlayerSlideAnims().anim, player, player->getPlayerSlideAnims().numOfAnims);
		movePlayerSide(sideMovementSpeed * 2); // to simulate some speed, i multiply sideMovementSpeed by 2 
		isSlideComplete = true; // when all is done isSlideComplete is set to true (this value will be used later)
	}
	else if (keystate[SDL_SCANCODE_E]) {
		// attack anims
		gameWindow->handleCharacter(player->getPlayerAttackAnims().anim, player, player->getPlayerAttackAnims().numOfAnims);
		// get a copy of both player and enemy SDL_Rect values/locations
		SDL_Rect playerDest = player->getDestination();
		SDL_Rect enemyDest = enemy->getDestination();
		// if the enemy is hit, enemy's hit value is set to true, this value is used to show the enemy as dead above
		if (enemy->isEnemyHitCheck(&playerDest, &enemyDest, player->getCharacterFlip()) && !(enemy->getIsEnemyHit())) {
			gameWindow->handleCharacter(enemy->getEnemyDeathAnims().anim, enemy, enemy->getEnemyDeathAnims().numOfAnims);
			enemy->setIsEnemyHit(true);
		}
	}
	else {
		// when sliding is complete, player's height is reset to it's original and y value is updated according to the "new" height
		if (isSlideComplete) {
			SDL_Rect playerDest = player->getDestination();
			playerDest.h = playerInitialDest.h;
			playerDest.y -= playerDest.h;
			player->setDestination(playerDest);
			isSlideComplete = false; // isSlideComplete is set to false so that these lines of code don't run constantly
		}
		gameWindow->handleCharacter(player->getPlayerIdleAnims().anim, player, player->getPlayerIdleAnims().numOfAnims);
	}
}

void Game::dropCharacter(CharacterManager* character, bool collision, bool isPlayer) {
	// if there won't be a collision, then sprite will be moving down
	// isPlayer is used in else if to set the isJumping value to false, 
	// this is to make sure that the player can't fly away by holding space
	if (!collision) character->movePlayerY(2, 0);
	else if (collision && isPlayer) isJumping = false;
}

// function to move the player in the x axis
void Game::movePlayerSide(int velocity) {
	SDL_Rect playerDestCopy = player->getDestination();
	playerDestCopy.x += velocity; // velocity is added onto player's current x value to check if there will be collision in the next step

	// if there won't be collision in the next location update, player is moved by the amount of the velocity
	// and running animation plays
	// else the player can't move and the animation is set to idle
	if (!isGoingToCollide(&playerDestCopy)) {
		player->movePlayerX(velocity);
		gameWindow->handleCharacter(player->getPlayerRunningAnims().anim, player, player->getPlayerRunningAnims().numOfAnims);
	}
	else gameWindow->handleCharacter(player->getPlayerIdleAnims().anim, player, player->getPlayerIdleAnims().numOfAnims);
}

// function to make the player jump
void Game::jump() {
	// when space is clicked, this function starts running the isJumping is set to true,
	// this value is used to drop the player when they are in air
	isJumping = true;
	// target is how high the player can jump, which is calculated by player's height removed from player's y location
	target = player->getDestination().y - player->getDestination().h;
	// as long as player hasn't reached the jumping target, the sprite will be moved upwards
	// current implementation is pretty bad
	while (player->getDestination().y > target) {
		player->movePlayerY(-2, target);
	}
}

// function that returns a boolean depending on if there is going to be collision or not
bool Game::isGoingToCollide(SDL_Rect* futureDest) {
	futureDest->w -= 10; // removing 10 from width because the asset's width is too much
	bool isColliding = false; // initial value is set to false

	for (TileManager& tile : loadedMap) {
		SDL_Rect tileDest = tile.getDestination();
		// player location and each tile's location are checked for collision
		// SDL's helper function that helps with checking rectangular collision https://wiki.libsdl.org/SDL_HasIntersection
		if (SDL_HasIntersection(futureDest, &tileDest) && tile.getCollidableStatus()) {
			isColliding = true; // if there is a collision isColliding is set to true
		}
	}

	return isColliding;
}

bool Game::isPlayerInVision() {
	SDL_Rect playerDest = player->getDestination();
	SDL_Rect enemyDest = enemy->getDestination();
	// enemy's x value is reduced by 200 and width value increased by 200 to simulate the distance in which the enemy can see
	enemyDest.x -= 200;
	enemyDest.w += 200;
	return SDL_HasIntersection(&playerDest, &enemyDest);
}

void Game::enemyFireAnim() {
	// first the firing animation starts, and then arrow flying is set to true
	gameWindow->handleCharacter(enemy->getEnemyAttackAnims().anim, enemy, enemy->getEnemyAttackAnims().numOfAnims);
}

void Game::arrowTravel() {
	player->setArrowStatus(true); // isArrowFlying is set to true
	gameWindow->handleArrow(arrow, arrow->getArrowTex().anim, arrow->getArrowTex().numOfAnims);
	arrow->handleFlight(-5); // arrow starts flying towards left
}

bool Game::canEnemyFire() {
	// as soon as counter is above 100 function returns true
	// the value is incremented in a function above,
	// as long as the player is in the vision of the enemy, the counter is incremented by one
	// when it reaches a certain amount it gets reset back to 0
	return counter > 100;
}

void Game::addEnemy() {
	enemy = new CharacterManager(); // creates enemy
	// enemy sprite is flipped towards left
	enemy->setCharacterFlip(rendererFlip::SDL_FLIP_HORIZONTAL);
	SDL_Rect enemyDestCopy = enemy->getDestination();
	SDL_Rect enemySrcCopy = enemy->getSource();
	// all of the values are hard coded and mostly dont't change since the enemy isn't meant to move
	// but shoot only
	enemyDestCopy.w = 64;
	enemyDestCopy.h = 42;
	enemyDestCopy.x = 690;
	enemyDestCopy.y = 400;
	enemySrcCopy.w = 64;
	enemySrcCopy.h = 42;
	enemy->setDestination(enemyDestCopy);
	enemy->setSource(enemySrcCopy);
}

void Game::setInitialArrowLoc() {
	player->setArrowStatus(false); // isArrowFlying set to false
	// setting the starting point for the arrow
	SDL_Rect arrowDest = arrow->getDest();
	// arrow's x location is set the enemy's x location
	// and the y location is set to slightly below enemy's middle height
	arrowDest.x = enemy->getDestination().x;
	arrowDest.y = enemy->getDestination().y + (enemy->getDestination().h / 3);
	arrow->setDest(arrowDest);
}
