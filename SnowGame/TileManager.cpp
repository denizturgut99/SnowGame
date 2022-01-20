#include "TileManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <string>

// setters in this class are a bit different because when loading the map
// i decided to add these values individually rather than creating a separate
// SDL_Rect like the other setters in CharacterManager or ArrowManager
// setters for CharacterManager and ArrowManager are also used in several places
// whereas setters for TileManager are only used in loadMap function in Renderer

void TileManager::setDestination(int x, int y, int w, int h) {
	destination.x = x;
	destination.y = y;
	destination.w = w;
	destination.h = h;
}

void TileManager::setSource(int x, int y, int w, int h) {
	source.x = x;
	source.y = y;
	source.w = w;
	source.h = h;
}

void TileManager::setTexture(std::string path, SDL_Renderer* renderer) {
	texture = IMG_LoadTexture(renderer, path.c_str());
}