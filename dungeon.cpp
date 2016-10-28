#include "dungeon.h"
#include "utilities.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Game Objects //////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char GameObject::getType() { // For Spawning and inventory reasons

	if (m_name == SHORT_SWORD || m_name == MACE || m_name == LONG_SWORD || m_name == MAGIC_AXE || m_name == MAGIC_FANGS_OF_SLEEP)
		return WEAPON;

	if (m_name == TELEPORTATION || m_name == ENHANCE_ARMOR || m_name == ENHANCE_DEXTERITY || m_name == STRENGTH || m_name == ENHANCE_HEALTH)
		return SCROLL;

	if (m_name == GOLDEN_IDOL_NAME)
		return GOLDEN_IDOL;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Weapons ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Weapon::setType(string name) { //

	if (name == SHORT_SWORD) {

		setDexBonus(0);
		setDamageAmt(2);

	}

	if (name == MACE) {

		setDexBonus(0);
		setDamageAmt(2);

	}

	if (name == LONG_SWORD) {

		setDexBonus(2);
		setDamageAmt(4);

	}

	if (name == MAGIC_AXE) {

		setDexBonus(5);
		setDamageAmt(5);

	}

	if (name == MAGIC_FANGS_OF_SLEEP) {

		setDexBonus(3);
		setDamageAmt(2);

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Random Generators /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Point* randomPoint(Level *currLevel) {

	if (currLevel->rooms.empty())
		return NULL;

	Room* randRoom = currLevel->rooms[randInt(currLevel->rooms.size())]; // Grabs a random room

	int x = (randRoom->getWestBoundary() + 1) + randInt(randRoom->getLength()); // Gets random coordinates within randRoom
	int y = (randRoom->getNorthBoundary() + 1) + randInt(randRoom->getWidth());

	if (currLevel->level[y][x] == ROOM) // If spot is not occupied
		return new Point(x, y);

	else // if (spot is occupied)
		return randomPoint(currLevel); // Recursive function that keeps calling itself until it returns a valid point

}

Monster* randomMonster(Level* currLevel) {

	int numLevel = currLevel->getDungeon()->getNumLevel(); 

	//if (numLevel == 0 || numLevel == 1)  Level 0-1
		int caseLimit = 1; // case 0 (only Snakewomen)
	
	if (numLevel == 2) // Level 2
		caseLimit = 2; // case 0 (Snakewomen), case 1 (Bogeymen)

	if (numLevel == 3) // Level 3
		caseLimit = 3; // case 0 (Snakewomen), case 1 (Bogeymen), case 2 (Dragons)

	switch (randInt(caseLimit)) { // Generates random monsters for a level according to what level it is
	
		case 0:
			return (new Snakewomen(currLevel));

		case 1:
			return (new Bogeymen(currLevel));

		case 2:
			return (new Dragon(currLevel));
	
	}

}

string randomScroll() {

	switch (randInt(5)) { // Generates random scrolls for dragon drops
	
	case 0:
		return TELEPORTATION;

	case 1:
		return ENHANCE_ARMOR;

	case 2:
		return ENHANCE_DEXTERITY;

	case 3:
		return ENHANCE_HEALTH;

	case 4:
		return STRENGTH;
	
	}

}

string randomObject() {

	switch (randInt(8)) { // Generates random SpawnedObjects

	case 0:
		return SHORT_SWORD;

	case 1:
		return MACE;

	case 2:
		return LONG_SWORD;

	case 3:
		return TELEPORTATION;

	case 4:
		return ENHANCE_ARMOR;

	case 5:
		return STRENGTH;

	case 6:
		return ENHANCE_HEALTH;

	case 7:
		return ENHANCE_DEXTERITY;

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Actors  ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Actor::Actor(Level* level, char actorType, int init_hp, int init_armor, int init_str, int init_dex, Weapon* init_weapon, Point* init_pos) {

	m_currLevel = level;
	type = actorType;

	if (init_hp > MAX_HP)
		m_hp = MAX_HP;

	else
		m_hp = init_hp;

	if (init_hp > MAX_HP)
		m_maxHp = MAX_HP;

	else
		m_maxHp = init_hp;

	if (init_armor > MAX_ARMOR)
		m_armor = MAX_ARMOR;

	else
		m_armor = init_armor;

	if (init_str > MAX_STR)
		m_str = MAX_STR;

	else
		m_str = init_str;

	if (init_dex > MAX_DEX)
		m_dex = MAX_DEX;

	else
		m_dex = init_dex;

	m_weapon = init_weapon;
	m_pos = init_pos;

	m_inventory.push_back(m_weapon);

}

void Actor::regainHp() {

	if (trueWithProbability(1.0 / 10.0))
		if (getHp() < getMaxHp())
			setHp(getHp() + 1);

}

void Actor::move(char input) {

	switch (input) {

	case ARROW_LEFT:
		if (canMove(ARROW_LEFT))
			getPos()->setX(getPos()->getX() - 1);

		break;

	case ARROW_RIGHT:
		if (canMove(ARROW_RIGHT))
			getPos()->setX(getPos()->getX() + 1);

		break;

	case ARROW_UP:
		if (canMove(ARROW_UP))
			getPos()->setY(getPos()->getY() - 1);

		break;

	case ARROW_DOWN:
		if (canMove(ARROW_DOWN))
			getPos()->setY(getPos()->getY() + 1);

		break;

	case MONSTER: // All monsters use move if pathToPlayer() <= smellDistance (Unless for the Dragon)

		int playerX = getCurrLevel()->player->getPos()->getX(); // Did not declare above because this function is recursive 
		int playerY = getCurrLevel()->player->getPos()->getY(); // and thus the Player isn't the only one calling move (Monsters are also) 

		int monsterX = getPos()->getX();
		int monsterY = getPos()->getY();

		if (monsterX < playerX) {
			
			move(ARROW_RIGHT); // Calls move to try to move closer to the player
			break;

		}

		if (monsterX > playerX) {

			move(ARROW_LEFT);
			break;

		}

		if (monsterY < playerY) {

			move(ARROW_DOWN);
			break;

		}

		if (monsterY > playerY) {
			
			move(ARROW_UP);
			
			break;
		}

	}

}

bool Actor::canMove(char dir) {

	int actorX = getPos()->getX(); // Monsters also call this function, so I did not use playerX and playerY
	int actorY = getPos()->getY();

	if (dir == ARROW_DOWN)
		if (actorY + 1 < getCurrLevel()->getCurrRoom(this)->getSouthBoundary()) { // Checks if next move will hit the boundary

			if (actorInWay(getCurrLevel(), actorX, actorY + 1)) { // If there is an Actor in the next spot

				attack(getCurrLevel()->getActor(actorX, actorY + 1)); // Attack it (Monsters will only attack the Player and the Player will only
				return false;                                         // attack Monsters)

			}

			else
				return true; // No boundary or actor in way, so move in that direction

		}

	if (dir == ARROW_UP)
		if (actorY - 1 > getCurrLevel()->getCurrRoom(this)->getNorthBoundary()) {

			if (actorInWay(getCurrLevel(), actorX, actorY - 1)) {

				attack(getCurrLevel()->getActor(actorX, actorY - 1));
				return false;

			}

			else
				return true;

		}

	if (dir == ARROW_LEFT)
		if (actorX - 1 > getCurrLevel()->getCurrRoom(this)->getWestBoundary()) {

			if (actorInWay(getCurrLevel(), actorX - 1, actorY)) {

				attack(getCurrLevel()->getActor(actorX - 1, actorY));
				return false;

			}

			else
				return true;

		}

	if (dir == ARROW_RIGHT)
		if (actorX + 1 < getCurrLevel()->getCurrRoom(this)->getEastBoundary()) {

			if (actorInWay(getCurrLevel(), actorX + 1, actorY)) {

				attack(getCurrLevel()->getActor(actorX + 1, actorY));
				return false;

			}

			else
				return true;

		}

	return false; // If a boundary is in the way don't move

}

bool Actor::actorInWay(Level* currLevel, int x, int y) {

	// Player attacks if a monster is in the way
	if (this == currLevel->player && (currLevel->level[y][x] == DRAGON || currLevel->level[y][x] == SNAKEWOMEN || currLevel->level[y][x] == BOGEYMEN))
		return true;

	// Monsters attack if player is in the way
	else if (currLevel->player->getPos()->getX() == x && currLevel->player->getPos()->getY() == y)
		return true;

	return false;

}

void Actor::attack(Actor* actor) {

	int damageDealt = randInt(getStr() + getWeapon()->getDamageAmt());         // Computed by given forumlas
	 
	int attackerPoints = getDex() + getWeapon()->getDexBonus();                // Computed by given formulas
	int defenderPoints = actor->getDex() + actor->getWeapon()->getDexBonus();


	if (randInt(attackerPoints) >= randInt(defenderPoints)) // Determines if Actor attacks actor
		actor->setHp(actor->getHp() - damageDealt); // If true, Actor deals damageDealt to actor

	if (actor->getActorType() == DRAGON) // If the actor defending is a dragon, it will attack back
		attack(this);                    // since an attack counts as a move and the dragon only attacks
	                                     // if the Player is immediately in its vincity
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Player ////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Player::doAction(char input) { // The Player's actions

	if (input == ARROW_LEFT || input == ARROW_RIGHT || input == ARROW_DOWN || input == ARROW_UP)
		move(input);

	if (input == INVENTORY || input == WIELD || input == READ_SCROLL)
		showInv(input);

	if (input == PICK_UP)
		pickUp();

	if (input == STAIRCASE)
		descend();

	if (input == CHEAT)
		cheat();

}

void Player::cheat() {

	setArmor(MAX_ARMOR);
	setDex(MAX_DEX);
	setMaxHp(MAX_HP);
	setHp(MAX_HP);
	setStr(MAX_STR);

}

void Player::descend() {

	// If player is on the staircase
	if (getCurrLevel()->staircase->getPos()->getX() == getPos()->getX() && getCurrLevel()->staircase->getPos()->getY() == getPos()->getY()) {
	
		Dungeon* dungeon = getCurrLevel()->getDungeon();
		
		dungeon->setNumLevel(dungeon->getNumLevel() + 1);                 // The next floor is now the current floor
		dungeon->setCurrFloor(dungeon->levels[dungeon->getNumLevel()]);   //

		dungeon->getCurrFloor()->addPlayer(this);      // Add the same Player to the next level
		setPos(randomPoint(dungeon->getCurrFloor()));  // Place the Player randomly on the next level

	}

}

void Player::pickUp() {

	for (size_t index = 0; index < getCurrLevel()->spawnedObjects.size(); index++) 
		if (getPos()->getX() == getCurrLevel()->spawnedObjects[index]->getPos()->getX() && getPos()->getY() == getCurrLevel()->spawnedObjects[index]->getPos()->getY()) {
		
			if (getCurrLevel()->spawnedObjects[index]->getType() == WEAPON)                                 // Checks if the Player is standing on one of the SpawnedObjects
				m_inventory.push_back(new Weapon(getCurrLevel()->spawnedObjects[index]->getName()));        // If so, add the SpawnedObject to the Player's inventory as
			                                                                                                // a GameObject
			if (getCurrLevel()->spawnedObjects[index]->getType() == SCROLL)
				m_inventory.push_back(new Scroll(getCurrLevel()->spawnedObjects[index]->getName()));

			if (getCurrLevel()->spawnedObjects[index]->getType() == GOLDEN_IDOL) {

				m_inventory.push_back(new GameObject(getCurrLevel()->spawnedObjects[index]->getName()));
				getCurrLevel()->endGame();

			}

			getCurrLevel()->spawnedObjects.erase(getCurrLevel()->spawnedObjects.begin() + index);  // Removes the SpawnedObject the Player picked up from the level
		
		}

}

void Player::showInv(char input) {

	clearScreen();

	cout << "Inventory:" << endl;
	
	for (size_t index = 0; index < m_inventory.size(); index++)                           // (char)(index + 97) == 'a'
		cout << (char)(index + 97) << ". " << m_inventory[index]->getName() << endl; 

	char select = getCharacter(); // Player's choice to select an item from the inventory (if 'w' or 'r')

	if (input == WIELD) // If the Player initially wanted to wield a weapon (opened the inventory by typing 'w')
		wield(select); 

	if (input == READ_SCROLL)  // If the Player initially wanted to wield a weapon (opened the inventory by typing 'r')
		read(select);

}

void Player::read(char selection) {

	for (size_t index = 0; index < m_inventory.size(); index++)
		if (selection == (char)(index + 97)) {

			Scroll* scroll = dynamic_cast<Scroll*>(m_inventory[index]);  // If the item that was selected is actually a scroll

			if (scroll != nullptr) { // If scroll, perform the scrolls ability
			
				if (scroll->getName() == TELEPORTATION)
					setPos(randomPoint(getCurrLevel()));
				
				if (scroll->getName() == ENHANCE_ARMOR)
					setArmor(getArmor() + 1 + randInt(3)); 

				if (scroll->getName() == ENHANCE_DEXTERITY)
					setDex(getDex() + 1);

				if (scroll->getName() == ENHANCE_HEALTH)
					setMaxHp(getMaxHp() + 3 + randInt(7));

				if (scroll->getName() == STRENGTH)
					setStr(getStr() + 1 + randInt(3));
			
				m_inventory.erase(m_inventory.begin() + index);

			}

		}

		// else don't read it

}

void Player::wield(char selection) {

	for (size_t index = 0; index < m_inventory.size(); index++)
		if (selection == (char)(index + 97)) {

			Weapon* weaponToWield =  dynamic_cast<Weapon*>(m_inventory[index]); // If item selected is actually a weapon, wield

			if (weaponToWield != nullptr)
				setWeapon(weaponToWield);

		}

		// else don't wield it

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Monsters //////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Monster::doAction(int smellDistance) {

	if (pathToPlayer() <= smellDistance) // If player is within the monster's smell distance
		move(MONSTER);                   // (Does not apply to Dragon since the dragon attacks if the player is in its immediate vincity)

	if (getActorType() == DRAGON) // Dragon has a chance of healing hp 
		regainHp();

}

void Monster::dropItem(double dropRate) {

	if (trueWithProbability(dropRate)) { // Monster drops depending on their drop rates

		string type; // Figures out what item to drop depending on the monster

		if (getActorType() == SNAKEWOMEN)
			type = getWeapon()->getName();

		if (getActorType() == BOGEYMEN)
			type = MAGIC_AXE;

		if (getActorType() == DRAGON)
			type = randomScroll();

		getCurrLevel()->spawnedObjects.push_back(new SpawnedObject(type, getPos())); // Adds the dropped item to the level in the Monster's pos where it died

	}
}

int Monster::pathToPlayer() {

	int stepsToPlayer = 0; 

	int monsterX = getPos()->getX();
	int monsterY = getPos()->getY();

	int playerX = getCurrLevel()->player->getPos()->getX();
	int playerY = getCurrLevel()->player->getPos()->getY();
	
	if (monsterX < playerX)
		stepsToPlayer = stepsToPlayer + (playerX - monsterX); // Adds the difference between the player's rows and the monster's rows and the player's 
	                                                          // columns and the monster's columns   
	else if (monsterX > playerX)
		stepsToPlayer = stepsToPlayer + (monsterX - playerX);

	if (monsterY < playerY)
		stepsToPlayer = stepsToPlayer + (playerY - monsterY);

	else if (monsterY > playerY)
		stepsToPlayer = stepsToPlayer + (monsterY - playerY);

	return stepsToPlayer;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Level /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Level::Level(Dungeon* dungeon) {

	m_dungeon = dungeon;

	for (int row = 0; row <= 17; row++) // Initializes a blank level
		for (int column = 0; column <= 69; column++)
			level[row][column] = NOT_FILLED;

	createRooms();                // Creates the level without the player
	//addCorridors();
	addSpawnedObjects();

	if (getDungeon()->getNumLevel() < 4)
		addStaircase();

	else
		addGoldenIdol();

	addMonsters();

}

bool Level::endGame() {

	if (player->m_inventory.back()->getType() == GOLDEN_IDOL)
		return true;

	return false;

}

Actor* Level::getActor(int x, int y) { // Gets actor at level[y][x]

	if (player->getPos()->getX() == x && player->getPos()->getY() == y)
		return player;

	for (size_t index = 0; index < monsters.size(); index++)
		if (monsters[index]->getPos()->getX() == x && monsters[index]->getPos()->getY() == y)
			return monsters[index];

}

void Level::addStaircase() {

	staircase = new Staircase(this);

}

void Level::addGoldenIdol() {

	spawnedObjects.push_back(new SpawnedObject(GOLDEN_IDOL_NAME, this));

}

void Level::addPlayer(Player* startPlayer) {

	player = startPlayer; // Uses the same Player for the whole game
	player->setCurrLevel(this); // Adds player to the current level

}

void Level::addMonsters() {

	for (int index = 0; index < 2 + randInt(5 * (0 + 1)); index++)
		monsters.push_back(randomMonster(this)); // Adds random monsters depending on the level number

}

void Level::addSpawnedObjects() {

	int numObjects = 2 + randInt(2); // 2 to 3 SpawnedObjects per level

	for (int index = numObjects; index > 0; index--)
		spawnedObjects.push_back(new SpawnedObject(randomObject(), this)); // Adds numObjects random SpawnedObjects

}


Room* Level::getCurrRoom(Actor* actor) { // Gets the room the actor is currently in

	int actorX = actor->getPos()->getX();
	int actorY = actor->getPos()->getY();

	for (size_t index = 0; index < rooms.size(); index++) // Traverses the container of rooms until it finds the one the actor is in
		if (actorY < rooms[index]->getSouthBoundary() && actorY > rooms[index]->getNorthBoundary() && actorX < rooms[index]->getEastBoundary() && actorX > rooms[index]->getWestBoundary())
			return rooms[index];

	return NULL; // Should never reach

}

bool Level::playerDead() {

	if (player->getHp() <= 0)
		return true;

	return false;

}

void Level::updateMonsters() { // Checks for dead monsters and removes them from the level

	for (size_t index = 0; index < monsters.size(); index++)
		if (monsters[index]->getHp() <= 0) {

			monsters[index]->dropItem(monsters[index]->getDropRate());
			monsters.erase(monsters.begin() + index);

		}

}

void Level::updateLevel() { // Before the level is printed, the level is updated

	updateMonsters();

	// Adds the rooms
	for (size_t index = 0; index < rooms.size(); index++)
		for (size_t coords = 0; coords < rooms[index]->roomCoords.size(); coords++)
			level[rooms[index]->roomCoords[coords]->getY()][rooms[index]->roomCoords[coords]->getX()] = ROOM;

	// Add spawned objects
	for (size_t index = 0; index < spawnedObjects.size(); index++) { 

		// Add Weapons
		if (spawnedObjects[index]->getType() == WEAPON)
			level[spawnedObjects[index]->getPos()->getY()][spawnedObjects[index]->getPos()->getX()] = WEAPON;

		// Add Scrolls
		if (spawnedObjects[index]->getType() == SCROLL)
			level[spawnedObjects[index]->getPos()->getY()][spawnedObjects[index]->getPos()->getX()] = SCROLL;

		if (spawnedObjects[index]->getType() == GOLDEN_IDOL)
			level[spawnedObjects[index]->getPos()->getY()][spawnedObjects[index]->getPos()->getX()] = GOLDEN_IDOL;

	}

	// Add the Staircase
	if (getDungeon()->getNumLevel() < 4)
		level[staircase->getPos()->getY()][staircase->getPos()->getX()] = STAIRCASE;

	// Add the Player
	level[player->getPos()->getY()][player->getPos()->getX()] = PLAYER;

	// Add Monsters
	for (size_t index = 0; index < monsters.size(); index++) { // Add monsters

		// Add Snakewomen
		if (monsters[index]->getActorType() == SNAKEWOMEN)
			level[monsters[index]->getPos()->getY()][monsters[index]->getPos()->getX()] = SNAKEWOMEN;

		// Add Bogeymen
		if (monsters[index]->getActorType() == BOGEYMEN)
			level[monsters[index]->getPos()->getY()][monsters[index]->getPos()->getX()] = BOGEYMEN;

		// Add Dragons
		if (monsters[index]->getActorType() == DRAGON)
			level[monsters[index]->getPos()->getY()][monsters[index]->getPos()->getX()] = DRAGON;

	}

	// Add Walls
	addWalls();

}

void Level::createRooms() {

	for (int currRow = 1; currRow <= AVAIL_ROWS; currRow++) {      // Traverse level
		for (int currCol = 1; currCol <= AVAIL_COLS; currCol++) {  //

		//	if (trueWithProbability(0.03) && rooms.empty()) { // To test with only one room in each level
			if (trueWithProbability(0.03)) {  // Random probability to produce a room (chosen arbitrarily)

				Room* room = new Room();

				int roomWidth = room->getWidth();
				int roomLength = room->getLength();

				if (roomFits(currRow, currCol, roomWidth, roomLength) && !AdjacentRoom(level, currRow, currCol, roomWidth, roomLength)) { // Create room if possible

					rooms.push_back(room); // Add room to list of rooms

					for (int roomRow = currRow; roomRow < (currRow + roomWidth); roomRow++)
						for (int roomCol = currCol; roomCol < (currCol + roomLength); roomCol++) {
							
							level[roomRow][roomCol] = ROOM;

							room->roomCoords.push_back(new Point(roomCol, roomRow)); // Room Coords

							// North Boundary
							if (roomRow == currRow)
								room->setNorthBoundary(roomRow - 1);

							// South Boundary
							if (roomRow == currRow + roomWidth - 1)
								room->setSouthBoundary(roomRow + 1);

							// West Boundary
							if (roomCol == currCol)
								room->setWestBoundary(roomCol - 1);

							// East Boundary
							if (roomCol == currCol + roomLength - 1)
								room->setEastBoundary(roomCol + 1);

						}

				}

			}

		}

	}

}

bool Level::roomFits(int currentRow, int currentCol, int roomWidth, int roomLength) {

	// if (there is space for the room in the level from the current pos)
	return (currentRow + roomWidth < AVAIL_ROWS) && (currentCol + roomLength < AVAIL_COLS);

}

bool Level::AdjacentRoom(char currentLevel[][70], int currentRow, int currentCol, int roomWidth, int roomLength) {

	for (int roomRow = currentRow; roomRow <= (currentRow + roomWidth); roomRow++)  // Traverse possible room space to see if there are any rooms surrounding
		for (int roomCol = currentCol; roomCol <= (currentCol + roomLength); roomCol++) {

			// North of space
			if (currentLevel[roomRow - 1][roomCol] == ROOM)
				return true;

			// South of space
			if (currentLevel[roomRow + 1][roomCol] == ROOM)
				return true;

			// East of space
			if (currentLevel[roomRow][roomCol + 1] == ROOM)
				return true;

			// West of space
			if (currentLevel[roomRow][roomCol - 1] == ROOM)
				return true;

		}
	
		return false;

}

void Level::printLevel(char selection) { // selection is the player's most recent typed character

	for (int row = 0; row <= 17; row++) {  // Prints contents of level
		
		for (int column = 0; column <= 69; column++)
			cout << level[row][column];

		cout << endl;

	}

	// Prints the stats of the player and his actions

	cout << "Dungeon Level: 0, " 
		 << "Hit points: " << player->getHp() << ", " 
		 << "Armor: " << player->getArmor() << ", " 
		 << "Strength: " << player->getStr() << ", " 
		 << "Dexterity: " << player->getDex() 
		 << endl << endl;

	if (playerDead()) {

		cout << "Press q to exit game." << endl;
		return;
	}

	if (endGame()) {

		cout << "Congratulations, you won!" << endl;
		cout << "Press q to exit game." << endl;
		return;
	}

	if (selection == WIELD)
		cout << "You are wielding " << player->getWeapon()->getName();

	if (selection == PICK_UP)
		cout << "You pick up " << player->m_inventory.back()->getName();

	//if (selection == READ_SCROLL);
	

}

void Level::addWalls() {

	for (int row = 0; row <= 17; row++) // row = 0 & row = 18 are reserved for walls
		for (int column = 0; column <= 69; column++) // column = 0 & column = 70 are reserved for walls
			if (level[row][column] == NOT_FILLED)
				level[row][column] = WALL;

}

/*void Level::addCorridors() {

	for (int numRooms = 0; numRooms < rooms.size() - 1; numRooms++) {

		int startRow = rooms[numRooms]->boundary[randInt(rooms[numRooms]->boundary.size())]->getY(); // Grabs a random point on the boundary of a room
		int startCol = rooms[numRooms]->boundary[randInt(rooms[numRooms]->boundary.size())]->getX();

		int endRow = rooms[numRooms + 1]->boundary[randInt(rooms[numRooms + 1]->boundary.size())]->getY();
		int endCol = rooms[numRooms + 1]->boundary[randInt(rooms[numRooms + 1]->boundary.size())]->getX();

		placePath(startRow, startCol, endRow, endCol);

	}

}

bool Level::placePath(int startRow, int startCol, int endRow, int endCol) {
	
	level[startRow][startCol] = ROOM;

	if (startRow == endRow && startCol == endCol)
		return true;

	// North
	if (startRow < endRow && level[startRow - 1][startCol] == NOT_FILLED && level[startRow - 1][startCol + 1] == NOT_FILLED && level[startRow - 1][startCol - 1] == NOT_FILLED && placePath(startRow - 1, startCol, endRow, endCol))
		return true;

	// South
	if (startRow > endRow && level[startRow + 1][startCol] == NOT_FILLED && level[startRow + 1][startCol + 1] == NOT_FILLED && level[startRow + 1][startCol - 1] == NOT_FILLED && placePath(startRow + 1, startCol, endRow, endCol))
		return true;

	// West
	if (startCol > endCol && level[startRow][startCol - 1] == NOT_FILLED && level[startRow + 1][startCol - 1] == NOT_FILLED && level[startRow - 1][startCol - 1] == NOT_FILLED && placePath(startRow, startCol - 1, endRow, endCol))
		return true;

	// East
	if (startRow < endRow && level[startRow][startCol + 1] == NOT_FILLED && level[startRow + 1][startCol + 1] == NOT_FILLED && level[startRow - 1][startCol + 1] == NOT_FILLED && placePath(startRow, startCol + 1, endRow, endCol))
		return true;

	return false;

}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Dungeon ///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Dungeon::Dungeon() {

	for (int index = 0; index < MAX_LEVELS; index++) { // Pre create all the levels

		setNumLevel(index);
		levels[index] = new Level(this);

	}

	setNumLevel(0);                        // Starting level
	setCurrFloor(levels[getNumLevel()]);   //

	startPlayer = new Player(getCurrFloor()); // Initialize the Player

	getCurrFloor()->addPlayer(startPlayer);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





