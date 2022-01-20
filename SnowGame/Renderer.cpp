#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Renderer.h"
#include "TileManager.h"
#include <fstream>
#include <string>
#include "CharacterManager.h"
#include <stdlib.h>

using namespace std;

// learned rendering func from a tutorial by codergopher https://www.youtube.com/watch?v=pjLpipQRMIw
Renderer::Renderer() : window(NULL), renderer(NULL) {

	// init SDL_GetDesktopDisplayMode and check if there is a problem when init happens
	if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
		cout << "ERROR: SDL_GetCurrentDisplayMode wasn't initialised: SDL_ERROR: " << SDL_GetError() << endl;

	// create the window in the center of user's screen
	window = SDL_CreateWindow("Snow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1300, 640, SDL_WINDOW_SHOWN);
	// create renderer and assign it to the window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// if the window fails to init, an error message is displayed in the console to help with debugging
	// else SDL_GetWindowSize runs and assigns the height and width of the window to class variables
	if (window == NULL) 
		cout << "ERROR: Window isn't initialised. SDL_ERROR: " << SDL_GetError() << endl;
	else 
		SDL_GetWindowSize(window, &width, &height);

	// if renderer fails to init, an error message is displayed to help with debugging
	if (renderer == NULL) cout << "ERROR: Renderer has failed. SDL_ERROR: " << SDL_GetError() << endl;
}

void Renderer::destroyAndQuit() {
	// when the game stops running this function gets called
	// first window is destroyed
	// and then renderer is destroyed
	// after both destructions are complete, SDL_Quit() runs
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Renderer::clear() {
	SDL_SetRenderDrawColor(renderer, 0, 231, 255, 1); // light blue colour to simulate "sky"
	SDL_RenderClear(renderer);
}


void Renderer::handleCharacter(string path, CharacterManager* character, int numOfAnims) {
	character->animatePlayer(path, renderer, numOfAnims);
	SDL_Rect source = character->getSource();
	SDL_Rect destination = character->getDestination();
	// most important part of this function is to get character's current flip, this is used to display the character
	// looking towards the correct side
	// e.g.: if player's last action was moving towards the left, then the player will continue looking towards left
	SDL_RenderCopyEx(renderer, character->getTexture(), &source, &destination, 0, NULL, (SDL_RendererFlip)character->getCharacterFlip());
}

void Renderer::handleArrow(ArrowManager* arrow, string path, int numOfAnims) {
	arrow->animateArrow(path, renderer, numOfAnims);
	SDL_Rect src = arrow->getSrc();
	SDL_Rect dest = arrow->getDest();
	// in this case flip is directly hard coded since the enemy doesn't rotate and fires at the same direction
	SDL_RenderCopyEx(renderer, arrow->getTexture(), &src, &dest, 0 , NULL, SDL_RendererFlip::SDL_FLIP_HORIZONTAL);
}


void Renderer::displayTexture() {
	SDL_RenderPresent(renderer);
}

void Renderer::loadMap(string path) {
	int id, mapWidth, mapHeight, tileWidth = 16;
	ifstream read(path);

	// assigning width and height of the entire map
	// the file starts with 100 which is the width
	// 2nd number in the same line is the height which is 20
	read >> mapWidth >> mapHeight;
	// first the height is looped over to get each ID line by line
	for (int h = 0; h < mapHeight; h++) {
		for (int w = 0; w < mapWidth; w++) {
			read >> id;
			TileManager tile;
			tile.setTexture("assets/maps/newtileset.png", renderer);
			// i take away 1 from id because Tiled's IDs start from 1 rather than 0 so if I won't take away 1
			// it'd display the wrong texture
			// since height and width are the same for the tile i multiply w (for x) and h (for y) by tileWidth (16)
			tile.setSource((id - 1) * tileWidth, 0, tileWidth, tileWidth);
			tile.setDestination(w * tileWidth, (h * tileWidth) + (height / 2), tileWidth, tileWidth);
			// if id is bigger than 1, the tile's collidable status is set to true, else it's set to false
			id > 1 ? tile.setCollidableStatus(true) : tile.setCollidableStatus(false);
			map.push_back(tile); // and finally, the created tile is pushed into the map vector
		}
	}
}

void Renderer::initSnowballs() {
	// as long as there are less than 15 snowballs this function will run
	// initSnowballs gets called in Game::init
	while (snowballCount < 15) {
		// get random number from the size of the map
		int randomNum = rand() % map.size();
		TileManager currentTile = map[randomNum]; // get tile from map with the index of random number
		// if the tile is collidable, at least after x 100 and not outside of the screen
		// snowball is created and added onto the map
		if (currentTile.getCollidableStatus() &&
			currentTile.getDestination().x > 100 &&
			currentTile.getDestination().x < width)
		{
			TileManager snowball;
			int snowballSize = 16;
			snowball.setTexture("assets/maps/snowball.png", renderer);
			snowball.setSource(0, 0, snowballSize, snowballSize);
			snowball.setDestination(
				currentTile.getDestination().x,
				currentTile.getDestination().y - snowballSize,
				snowballSize,
				snowballSize
			);
			snowballs.push_back(snowball);
			snowballCount++; // finally, snowballCounter is increased by 1;
		}
	}
}

void Renderer::draw(vector<TileManager> items) {
	// this function is strictly to draw the map and snowballs
	for (TileManager& item : items) renderTexture(item);
}

void Renderer::renderTexture(TileManager tile) {
	SDL_Rect source = tile.getSource();
	SDL_Rect destination = tile.getDestination();
	SDL_RenderCopy(renderer, tile.getTexture(), &source, &destination);
}
