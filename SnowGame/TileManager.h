#pragma once
#include <SDL.h>
#include <string>
#include <vector>

class TileManager {
public:
	TileManager() { 
		destination.x = 0;
		destination.y = 0;
		destination.w = 0;
		destination.h = 0;
		source.x = 0;
		source.y = 0;
		source.w = 0;
		source.h = 0;
	};
	SDL_Texture* getTexture() { return texture; };
	SDL_Rect getDestination() { return destination; };
	SDL_Rect getSource() { return source; };
	void setDestination(int x, int y, int w, int h);
	void setSource(int x, int y, int w, int h);
	void setTexture(std::string path, SDL_Renderer* renderer);
	bool getCollidableStatus() { return isCollidable; };
	void setCollidableStatus(bool status) { isCollidable = status; };
private:
	SDL_Texture* texture = NULL;
	SDL_Rect source;
	SDL_Rect destination;
	bool isCollidable = false;
};