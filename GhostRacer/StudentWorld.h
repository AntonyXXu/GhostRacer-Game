#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>

class GhostRacer;
class Actor;


class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	virtual ~StudentWorld();
	bool checkCollision(Actor* actorA, Actor* actorB);
	bool checkHolyWaterCollision(Actor* holyWaterSpray);
	int checkLaneCollisions(Actor* actor);
	void savedSoul();
	void addToInteractActorList(Actor* actor);
	void addToNoInteractActorList(Actor* actor);
	static const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	static const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
	GhostRacer* getGhostRacer() const;

private:
	//Member Variables
	GhostRacer* m_ghostRacer;
	std::list<Actor*> m_interactActorList;
	std::list<Actor*> m_noInteractActorList;
	std::vector<double> m_botCollisionActor;
	std::vector<double> m_topCollisionActor;
	const int kNumYellowLine = VIEW_HEIGHT / SPRITE_HEIGHT;
	const int kLeftWhiteLine = LEFT_EDGE + ROAD_WIDTH / 3;
	const int kRightWhiteLine = RIGHT_EDGE - ROAD_WIDTH / 3;
	const int kNumWhiteLines = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
	const int kNumLanes = 3;
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
	int moveActiveActors(std::list<Actor*>& actorList);
	void removeDeadActors(std::list<Actor*>& actorList);
	int getLane(double x_coord);
	void updateGameText();

};

#endif // STUDENTWORLD_H_
