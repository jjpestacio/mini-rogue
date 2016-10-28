// Game.cpp
#include "dungeon.h"
#include "Game.h"
#include "utilities.h"
#include <iostream>

using namespace std;

// Implement these and other Game member functions you may have added.


Game::Game(int goblinSmellDistance) {



}

void Game::play() {

	// Generates Dungeon
	Dungeon* game = new Dungeon();

	// Initial Input
	char input = 'a';

	// Loop to play the game while input != q
	do {

		Level* currLevel = game->getCurrFloor();

		clearScreen();
		currLevel->updateLevel();
		currLevel->printLevel(input); // input is for printing the player's actions

		if (currLevel->playerDead() || currLevel->endGame())
			while (input != 'q')
				input = getCharacter();

		else
			input = getCharacter();

		// Player and Monsters act
		currLevel->player->doAction(input);
		for (int index = 0; index < currLevel->monsters.size(); index++)
			currLevel->monsters[index]->doAction(currLevel->monsters[index]->getSmellDistance());

		// Chance for player to regain 1 HP
		currLevel->player->regainHp();

	} while (input != 'q');

	exit(1);
	
}

// You will presumably add to this project other .h/.cpp files for the
// various classes of objects you need to play the game:  player, monsters,
// weapons, etc.  You might have a separate .h/.cpp pair for each class
// (e.g., Player.h, Boegeyman.h, etc.), or you might put the class
// declarations for all actors in Actor.h, all game objects in GameObject.h,
// etc.
