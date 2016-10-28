#ifndef DUNGEON_INCLUDED
#define DUNGEON_INCLUDED

#include "utilities.h"
#include <vector>
#include <string>

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Constants /////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

// ACTIONS //
const char INVENTORY = 'i';
const char WIELD = 'w';
const char READ_SCROLL = 'r';
const char PICK_UP = 'g';
const char CHEAT = 'c';

// ACTOR TYPES //
const char MONSTER = NULL;
const char PLAYER = '@';
const char SNAKEWOMEN = 'S';
const char BOGEYMEN = 'B';
const char DRAGON = 'D';
const char GOBLIN = 'G';

// MAXED STATS //
const int MAX_HP = 99;
const int MAX_ARMOR = 99;
const int MAX_STR = 99;
const int MAX_DEX = 99;

// SPAWNED OBJECTS //
const char WEAPON = ')';
const char SCROLL = '?';
const char GOLDEN_IDOL = '&';
const string GOLDEN_IDOL_NAME = "&";

// WEAPONS //
const string SHORT_SWORD = "short sword";
const string MACE = "mace";
const string LONG_SWORD = "long sword";
const string MAGIC_AXE = "magic axe";
const string MAGIC_FANGS_OF_SLEEP = "magic fangs of sleep";

// SCROLLS //
const string TELEPORTATION = "scroll of teleportation";
const string STRENGTH = "scroll of strength";
const string ENHANCE_ARMOR = "scroll of enhance armor";
const string ENHANCE_HEALTH = "scroll of enhance health";
const string ENHANCE_DEXTERITY = "scroll of enhance dexterity";

// LEVEL GENERATION //
const char WALL = '#';
const char ROOM = ' ';
const char STAIRCASE = '>';
const char NOT_FILLED = '.'; // Space in level has not been defined yet

const int MAX_LEVELS = 5;

const int TOTAL_ROWS = 18;
const int TOTAL_COLS = 70;

const int AVAIL_ROWS = 16; // First and last reserved for walls
const int AVAIL_COLS = 68; // First and last reserved for walls

class Dungeon;
class Level;
class Point;

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Random Generators /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

Point* randomPoint(Level *currLevel);
string randomObject();
string randomScroll();
string randomObject();

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// Point /////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class Point { // Used as coordinates for the level

public:

	Point(int x, int y) : X(x), Y(y) {}
	~Point() {}

	//bool operator==(const Point &b);

	int getX() const { return X; }
	void setX(int newX) { X = newX; }

	int getY() const { return Y; }
	void setY(int newY) { Y = newY; }
	

private:

	int X;
	int Y;

};

/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// GameObject ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class GameObject {

public:

	GameObject(string name) : m_name(name){}
	GameObject(string name, Point* pos) : m_name(name), m_pos(pos){}
	virtual ~GameObject() {}

	string getName() { return m_name; }
	char getType(); // Implemented in dungeon.cpp

private:
	
	string m_name;
	Point* m_pos;

};

class SpawnedObject : public GameObject { // SpawnedObjects are GameObjects the player can pick up

public:

	SpawnedObject(string name, Level* currLevel) : GameObject(name) { m_pos = randomPoint(currLevel); }
	SpawnedObject(string name, Point* pos) : GameObject(name) { m_pos = pos; }
	virtual ~SpawnedObject(){}

	Point* getPos() { return m_pos; }
	void setPos(Point* pos) { m_pos = pos; }

private:

	Point* m_pos;

};

class Weapon : public GameObject {

public:

	Weapon(string name) : GameObject(name) { setType(name); }
	virtual ~Weapon() {}

	virtual void setType(string name);

	int getDamageAmt() { return m_damageAmt; }
	void setDamageAmt(int damageAmt) { m_damageAmt = damageAmt; }

	int getDexBonus() { return m_dexBonus; }
	void setDexBonus(int dexBonus) { m_dexBonus = dexBonus; }

private:

	int m_dexBonus;
	int m_damageAmt;

};

class Scroll : public GameObject {

	public:
	
		Scroll(string name) : GameObject(name) {}
		virtual ~Scroll(){}

	private:

};


/////////////////////////////////////////////////////////////////////////////////
/////////////// Actor ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class Actor {

public:

	Actor(Level* level, char actorType, int init_hp, int init_armor, int init_str, int init_dex, Weapon* init_weapon, Point* init_pos);

	virtual ~Actor() {}

	char getActorType() { return type; } // Represents the character on the grid

	Level* getCurrLevel() { return m_currLevel; }
	void setCurrLevel(Level* currLevel) { m_currLevel = currLevel; }

	Point* getPos() { return m_pos; }
	void setPos(Point* pos) { m_pos = pos; }
	
	bool canMove(char dir);
	void move(char input);
	
	bool actorInWay(Level* level, int x, int y);
	void attack(Actor* actor);

	Weapon* getWeapon() { return m_weapon; }
	void setWeapon(Weapon* weapon) { m_weapon = weapon; }

	int getHp() { return m_hp; }
	void setHp(int hp) { m_hp = hp; }
	int getMaxHp(){ return m_maxHp; }
	void setMaxHp(int maxHp){ m_maxHp = maxHp; }
	void regainHp();

	int getArmor() { return m_armor; }
	void setArmor(int armor) { m_armor = armor; }

	int getStr() { return m_str; }
	void setStr(int str) { m_str = str; }

	int getDex() { return m_dex; }
	void setDex(int dex) { m_dex = dex; }

	int getSleepTime() { return m_sleepTime; }
	void setSleepTime(int sleepTime){ m_sleepTime = sleepTime; }

	std::vector<GameObject*> m_inventory;

protected:

	Level* m_currLevel;

	char type;

	int m_hp;
	int m_maxHp;
	int m_armor;
	int m_str;
	int m_dex;
	int m_sleepTime;

	Weapon* m_weapon;
	Point* m_pos;

};

class Player : public Actor {

public:

	Player(Level* level) : Actor(level, PLAYER, 20, 2, 2, 2, new Weapon(SHORT_SWORD), randomPoint(level)) {}
	virtual ~Player() {}

	void doAction(char input); // Did not make a pure virtual doAction function for Actor becasue all monsters move the same
		void pickUp(); 
		void descend();
		void cheat();
		void showInv(char input);
		void wield(char selection);
		void read(char selection);

private:
	
};

/////////////////////////////////////////////////////////////////////////////////
/////////////// Monsters ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class Monster : public Actor {

	public:
		
		Monster(Level* level, char actorType, int init_hp, int init_armor, int init_str, int init_dex, Weapon* init_weapon, Point* init_pos) : Actor(level, actorType, init_hp, init_armor, init_str, init_dex, init_weapon, init_pos) {}
		
	
		void doAction(int smellDistance); // Players and Monsters attack the same way
			int pathToPlayer(); // Used to find if Player is within smellDistance
			void dropItem(double dropRate);

		int getSmellDistance() { return m_smellDistance; }
		void setSmellDistance(int smellDistance) { m_smellDistance = smellDistance; }

		double getDropRate() { return m_dropRate; }
		void setDropRate(double dropRate) { m_dropRate = dropRate; }

	private:

		int m_smellDistance;
		double m_dropRate;

};

class Snakewomen : public Monster {

	public:
	
		Snakewomen(Level* level) : Monster(level, SNAKEWOMEN, 3 + randInt(4), 3, 2, 3, new Weapon(MAGIC_FANGS_OF_SLEEP), randomPoint(level)) {

			setSmellDistance(3);
			setDropRate(1.0/3.0);

		}

		virtual ~Snakewomen() {}

	private:

};

class Bogeymen : public Monster {

	public:
		Bogeymen(Level* level) : Monster(level, BOGEYMEN, 5 + randInt(6), 2, 2 + randInt(2), 2 + randInt(2), new Weapon(SHORT_SWORD), randomPoint(level)) {
		
			setSmellDistance(5);
			setDropRate(1.0 / 10.0);
		
		}

		virtual ~Bogeymen(){}

	private:

};

class Dragon : public Monster {

public:
	Dragon(Level* level) : Monster(level,DRAGON, 20 + randInt(6), 4, 4, 4, new Weapon(LONG_SWORD), randomPoint(level)) {

		setSmellDistance(0);
		setDropRate(1);

	}

	virtual ~Dragon(){}

private:

};

/////////////////////////////////////////////////////////////////////////////////
/////////////// Level ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class Staircase {

public:

	Staircase(Level* currLevel) { m_pos = randomPoint(currLevel); }
	~Staircase();

	Point* getPos(){ return m_pos; }

private:

	Point* m_pos;

};

class Room {

public:

	Room() : m_width(4 + randInt(5)), m_length(7 + randInt(5)) {} // (5 + randInt(5)) produces a random integer between 7 and 10
	~Room() {}
	
	int getWidth() const { return m_width; }                                                        
	int getLength() const { return m_length; }

	int getNorthBoundary() { return m_northBoundary; } // Used boundaries to enclose Actors and SpawnedObjects
	void setNorthBoundary(int northBoundary) { m_northBoundary = northBoundary; }
	
	int getSouthBoundary() { return m_southBoundary; }
	void setSouthBoundary(int southBoundary) { m_southBoundary = southBoundary; }

	int getWestBoundary() { return m_westBoundary; }
	void setWestBoundary(int westBoundary) { m_westBoundary = westBoundary; }

	int getEastBoundary() { return m_eastBoundary; }
	void setEastBoundary(int eastBoundary){ m_eastBoundary = eastBoundary; }

	std::vector<Point*> roomCoords;

private:

	int m_width;
	int m_length;

	int m_northBoundary;
	int m_southBoundary;
	int m_westBoundary;
	int m_eastBoundary;

};

class Level {

public:

	Level(Dungeon* dungeon);
	~Level() {}

	void createRooms();
	//void addCorridors();
	void addWalls();
	void addSpawnedObjects();
	void addMonsters();
	void addPlayer(Player* startPlayer);
	void addStaircase();
	void addGoldenIdol();

	void updateMonsters();
	bool playerDead();
	void updateLevel();

	bool endGame();

	void printLevel(char selection);

	Dungeon* getDungeon(){ return m_dungeon; }
	Room* getCurrRoom(Actor* actor);
	Actor* getActor(int x, int y);

	Player* player;
	Staircase* staircase;

	std::vector<Monster*> monsters;
	std::vector<Room*> rooms;
	std::vector<SpawnedObject*> spawnedObjects;

	char level[TOTAL_ROWS][TOTAL_COLS];

private:

	// Helper Functions for creating rooms in a level
	bool roomFits(int currentRow, int currentCol, int roomWidth, int roomLength);
	bool AdjacentRoom(char currentLevel[][70], int currentRow, int currentCol, int roomWidth, int roomLength);
	//bool placePath(int startRow, int startCol, int endRow, int endCol);

	Dungeon* m_dungeon;
		
};

//class Corridor {};

/////////////////////////////////////////////////////////////////////////////////
/////////////// Dungeon /////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class Dungeon {

public:

	Dungeon();
	~Dungeon();

	void setCurrFloor(Level* currFloor) { m_currFloor = currFloor; }
	Level* getCurrFloor() { return m_currFloor; }

	int getNumLevel() { return m_numLevel; }
	void setNumLevel(int numLevel) { m_numLevel = numLevel; }

	Player* startPlayer;

	Level* levels[MAX_LEVELS];

private:

	Level* m_currFloor;
	int m_numLevel;


};

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

#endif
