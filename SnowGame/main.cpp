#include "game.h"

Game* game = nullptr;

int main(int argc, char* argv[]) {
	game = new Game(); // create game
	game->Init(); // initialise the game

	// as long as game->isRunning() returns true the game will continue running
	while (game->isRunning()) {
		game->gameLoop();
	}

	game->destroy(); // when game->isRunning() returns false destroy function runs

	return 0;
}
