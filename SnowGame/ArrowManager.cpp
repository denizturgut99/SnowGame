#include "ArrowManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <string>

void ArrowManager::handleFlight(int direction) {
	dest.x += direction;
}

void ArrowManager::setTexture(SDL_Renderer* renderer, std::string path) {
	texture = IMG_LoadTexture(renderer, path.c_str());
}

void ArrowManager::animateArrow(std::string path, SDL_Renderer* renderer, int numOfAnims) {
	setTexture(renderer, path);
	// SDL_GetTicks() / 150 is to control the speed of the animation
	// and then we get the remainder of the results division by numOfAnims
	// this will delay each animation per frame
	// tutorial: https://www.youtube.com/watch?v=OrkpfmXHP_Q
	src.x = ((SDL_GetTicks() / 150) % numOfAnims) * src.w;
}