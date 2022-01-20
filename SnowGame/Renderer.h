#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include "TileManager.h"
#include "CharacterManager.h"
#include "ArrowManager.h"

using namespace std;

class Renderer {
public:
	Renderer();
	void destroyAndQuit();
	void clear();
	void renderTexture(TileManager tile);
	void displayTexture();
	void handleCharacter(string path, CharacterManager* character, int numOfAnims);
	float getWindowWidth() { return static_cast<float>(width); };
	float getWindowHeight() { return static_cast<float>(height); };
	void loadMap(string path);
	vector<TileManager> getMap() { return map; };
	void handleArrow(ArrowManager* arrow, string path, int numOfAnims);
	void initSnowballs();
	vector<TileManager> getSnowballs() { return snowballs; };
	void setSnowballs(vector<TileManager> newVector) { snowballs = newVector; };
	void draw(vector<TileManager> items);
private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_DisplayMode displayMode;
	int width = 0, height = 0;
	vector<TileManager> map;
	vector<TileManager> snowballs;
	int snowballCount = 0;
};