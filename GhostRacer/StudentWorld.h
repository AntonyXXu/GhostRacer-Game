#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <string>

class GhostRacer;


class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	virtual ~StudentWorld();
	static const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
	static const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
	static const int NUM_YELLOW_LINE = VIEW_HEIGHT / SPRITE_HEIGHT;
	static const int LEFT_WHITE_LINE = LEFT_EDGE + ROAD_WIDTH / 3;
	static const int RIGHT_WHITE_LINE = RIGHT_EDGE - ROAD_WIDTH / 3;
	static const int NUM_WHITE_LINE = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);

private:
	GhostRacer* m_ghostRacer;
	int m_remainingSouls;
	int m_bonusPoints;
	int m_previousBorderY;

};

#endif // STUDENTWORLD_H_
