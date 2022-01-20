#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>

using namespace std;

// custom struct to store the animation and number of animations the animation file has
struct CHARACTER_ANIM {
	string anim;
	int numOfAnims;
};

// "custom" enum class to avoid warnings, later on the values from here get turned into SDL_RendererFlip
enum class rendererFlip { SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL };

class CharacterManager {
public:
	CharacterManager() {
		// initial location values for all character sprites
		destination.x = 50;
		destination.y = 50;
		destination.w = 50;
		destination.h = 36;
		source.x = 0;
		source.y = 0;
		source.w = 50;
		source.h = 36;
	};
	SDL_Texture* getTexture() { return texture; };
	SDL_Rect getSource() { return source; };
	SDL_Rect getDestination() { return destination; };
	void animatePlayer(string path, SDL_Renderer* renderer, int numOfAnims);
	void setTexture(string path, SDL_Renderer* renderer);
	void setDestination(SDL_Rect dest) { destination = dest; };
	void setSource(SDL_Rect src) { source = src; };
	void movePlayerX(int velocity) { destination.x += velocity; };
	void movePlayerY(int posY, int target) { destination.y += posY; };
	void setCharacterFlip(rendererFlip flip) { currentFlip = flip; };
	bool isPlayerHit(SDL_Rect* playerDest, SDL_Rect* arrowDest);
	rendererFlip getCharacterFlip() { return currentFlip; };
	bool getArrowStatus() { return isArrowFlying; };
	void setArrowStatus(bool isFlying) { isArrowFlying = isFlying; };
	bool isEnemyHitCheck(SDL_Rect* playerDest, SDL_Rect* enemyDest, rendererFlip flip);
	bool getIsEnemyHit() { return isEnemyHit; };
	void setIsEnemyHit(bool isHit) { isEnemyHit = isHit; };
	bool isSnowballCollected(SDL_Rect* playerDest, SDL_Rect* snowballDest);
	CHARACTER_ANIM getPlayerIdleAnims() { return playerIdle; };
	CHARACTER_ANIM getPlayerRunningAnims() { return playerRunning; };
	CHARACTER_ANIM getPlayerSlideAnims() { return playerSlide; };
	CHARACTER_ANIM getPlayerAttackAnims() { return playerAttack; };
	CHARACTER_ANIM getEnemyIdleAnims() { return enemyIdle; };
	CHARACTER_ANIM getEnemyAttackAnims() { return enemyAttack; };
	CHARACTER_ANIM getEnemyDeathAnims() { return enemyDying; };
	CHARACTER_ANIM getEnemyDeadAnims() { return enemyDead; };

// private vars
private:
	SDL_Texture* texture = nullptr;
	SDL_Rect source;
	SDL_Rect destination;
	rendererFlip currentFlip = rendererFlip::SDL_FLIP_NONE;
	bool isArrowFlying = false;
	bool isEnemyHit = false;
	// file path for anim and num of sprites they include
	CHARACTER_ANIM playerIdle = { "assets/gfx/chars/player/adventurer/idle2.png", 4 };
	CHARACTER_ANIM playerRunning = { "assets/gfx/chars/player/adventurer/running2.png", 6 };
	CHARACTER_ANIM playerSlide = { "assets/gfx/chars/player/adventurer/slide.png", 3 };
	CHARACTER_ANIM playerAttack = { "assets/gfx/chars/player/adventurer/attack.png", 4 };
	CHARACTER_ANIM enemyIdle = { "assets/gfx/chars/player/archer/idle.png", 2 };
	CHARACTER_ANIM enemyAttack = { "assets/gfx/chars/player/archer/attack.png", 6 };
	CHARACTER_ANIM enemyDying = { "assets/gfx/chars/player/archer/death.png", 7 };
	CHARACTER_ANIM enemyDead = { "assets/gfx/chars/player/archer/dead.png", 1 };
};