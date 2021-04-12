#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>

class GhostRacer;
class Actor;
struct collisionArray;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	virtual ~StudentWorld();
	bool checkCollision(Actor* actorA, Actor* actorB);
	int checkLaneCollisions(Actor* actor);
	void savedSoul();
	void addToActorList(Actor* actor);

	GhostRacer* getGhostRacer() const;
	static const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	static const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
	static const int NUM_YELLOW_LINE = VIEW_HEIGHT / SPRITE_HEIGHT;
	static const int LEFT_WHITE_LINE = LEFT_EDGE + ROAD_WIDTH / 3;
	static const int RIGHT_WHITE_LINE = RIGHT_EDGE - ROAD_WIDTH / 3;
	static const int NUM_WHITE_LINE = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
	static const int NUM_LANES = 3;

private:
	//Member Variables
	GhostRacer* m_ghostRacer;
	std::list<Actor*> m_actorList;
	std::vector<double> m_botCollisionActor; 
	std::vector<double> m_topCollisionActor;
	int m_soulsToSave;
	int m_bonusPoints;
	double m_previousBorderY;
	//Add Actors
	void addActors();
	void addBorderLines();
	void addHumanPedestrian();
	void addZombiePedestrian();
	void addZombieCab();
	void addOilSlick();
	void addSprayBottle();
	void addLostSoul();
	void addBorders_helper(bool yellowLine, double startY);
	//Helper Functions
	int getLane(double x_coord);
	void update_game_text();
};

struct collisionArray
{
public:
};

#endif // STUDENTWORLD_H_
