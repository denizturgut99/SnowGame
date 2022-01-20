#include "CharacterManager.h"
#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

//PlayerManager::PlayerManager() {}

void CharacterManager::setTexture(string path, SDL_Renderer* renderer) {
	// load texture from a file path that was received
	texture = IMG_LoadTexture(renderer, path.c_str());
}

void CharacterManager::animatePlayer(string path, SDL_Renderer* renderer, int numOfAnims) {
	setTexture(path, renderer);
	// SDL_GetTicks() / 150 is to control the speed of the animation
	// and then we get the remainder of the results division by numOfAnims
	// this will delay each animation per frame
	// tutorial: https://www.youtube.com/watch?v=OrkpfmXHP_Q
	source.x = ((SDL_GetTicks() / 150) % numOfAnims) * source.w;
}

bool CharacterManager::isPlayerHit(SDL_Rect* playerDest, SDL_Rect* arrowDest) {
	// if the arrow is flying, i check if there are any collisions between the player and arrow
	// else false is returned
	if (isArrowFlying) {
		return SDL_HasIntersection(playerDest, arrowDest);
	}
	else return false;
}

bool CharacterManager::isEnemyHitCheck(SDL_Rect* playerDest, SDL_Rect* enemyDest, rendererFlip flip) {
	// if the player is horizontally flipped, i take away 10 from x so that when player swings there is a sense of some "destination"
	// else i add the width to target the furthest x point of the sprite to be able to hit/miss the enemy
	flip == rendererFlip::SDL_FLIP_HORIZONTAL ? playerDest->x -= 10 : playerDest->x += playerDest->w;
	return SDL_HasIntersection(playerDest, enemyDest);
}

bool CharacterManager::isSnowballCollected(SDL_Rect* playerDest, SDL_Rect* snowballDest) {
	return SDL_HasIntersection(playerDest, snowballDest);
}
