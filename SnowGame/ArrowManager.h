#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>

// similar to custom struct in CharacterManager
struct ARROW_ANIM {
	std::string anim;
	int numOfAnims;
};

class ArrowManager {
public:
	ArrowManager() {
		// initial location for the arrow
		src.x = 100;
		src.y = 0;
		src.h = 7;
		src.w = 16;
		dest.x = 0;
		dest.y = 0;
		dest.h = 7;
		dest.w = 16;
	};
	void setDest(SDL_Rect newDest) { dest = newDest; };
	SDL_Rect getDest() { return dest; };
	SDL_Rect getSrc() { return src; };
	void handleFlight(int direction);
	void setTexture(SDL_Renderer* renderer, std::string path);
	SDL_Texture* getTexture() { return texture; };
	//std::string getArrowTexPath() { return arrowTexPath; };
	void animateArrow(std::string path, SDL_Renderer* renderer, int numOfAnims);
	ARROW_ANIM getArrowTex() { return arrowTex; };
private:
	SDL_Rect src;
	SDL_Rect dest;
	SDL_Texture* texture = nullptr;
	ARROW_ANIM arrowTex = { "assets/gfx/chars/player/archer/arrow.png", 1 };
};