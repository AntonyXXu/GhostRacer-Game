#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
	m_bonusPoints = 5000;
	m_ghostRacer = NULL;
	m_previousBorderY = 0;
	m_soulsToSave = 0;

}

int StudentWorld::init()
{
	m_ghostRacer = new GhostRacer(this);

	//init yellow lines. init i as double to multiply sprite
	for (int i = 0; i < NUM_YELLOW_LINE; i++) {
		addBorders_helper(true, i * SPRITE_HEIGHT);
		addBorders_helper(true, i * SPRITE_HEIGHT);
		//BorderLine* left = new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, i * SPRITE_HEIGHT, this);
		//BorderLine* right = new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i * SPRITE_HEIGHT, this);
		//m_actorList.push_front(left);
		//m_actorList.push_front(right);
	}
	//init white lines
	for (int i = 0; i < NUM_WHITE_LINE; i++) {
		addBorders_helper(false, 4 * i * SPRITE_HEIGHT);
		addBorders_helper(false, 4 * i * SPRITE_HEIGHT);
		//BorderLine* left = new BorderLine(IID_WHITE_BORDER_LINE, LEFT_WHITE_LINE, 4 * i * SPRITE_HEIGHT, this);
		//BorderLine* right = new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_WHITE_LINE, 4 * i * SPRITE_HEIGHT, this);
		//m_actorList.push_back(left);
		//m_actorList.push_back(right);
	}

	return GWSTATUS_CONTINUE_GAME;

}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.

	m_ghostRacer->doSomething();

	//iterate through all actors to doSomething
	list<Actor*>::iterator itr = m_actorList.begin();
	while (itr != m_actorList.end()) {
		(*itr)->doSomething();
		itr++;
	}

	//Add new actors 
	addActors();


	return GWSTATUS_CONTINUE_GAME;
	//return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
	delete m_ghostRacer;
	m_ghostRacer = NULL;
	while (!m_actorList.empty())
	{
		Actor* del = m_actorList.front();
		delete del;
		m_actorList.pop_front();
	}
	/*list<Actor*>::iterator itr = m_actorList.begin();
	while (itr != m_actorList.end()) {
		Actor* del = (*itr);
		delete del;
		m_actorList.erase(itr);
	}*/

}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

void StudentWorld::addBorders_helper(bool yellowLine, double startY)
{
	//Inserts yellow or white borderline based on bool yellowLine
	BorderLine* left;
	BorderLine* right;
	switch (yellowLine)
	{
	case true:
		left = new BorderLine(IID_YELLOW_BORDER_LINE, LEFT_EDGE, startY, this);
		right = new BorderLine(IID_YELLOW_BORDER_LINE, RIGHT_EDGE, startY, this);
		m_actorList.push_front(left);
		m_actorList.push_front(right);
		break;
	case false:
		left = new BorderLine(IID_WHITE_BORDER_LINE, LEFT_WHITE_LINE, startY, this);
		right = new BorderLine(IID_WHITE_BORDER_LINE, RIGHT_WHITE_LINE, startY, this);
		m_actorList.push_back(left);
		m_actorList.push_back(right);
		m_previousBorderY = startY;
		break;
	}
}

void StudentWorld::addActors()
{
	addBorderLines();
}

void StudentWorld::addBorderLines()
{
	int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
	m_previousBorderY -= 4 - m_ghostRacer->getYSpeed();
	int delta_y = new_border_y - m_previousBorderY;
	if (delta_y >= SPRITE_HEIGHT)
	{
		addBorders_helper(true, new_border_y);
	}
	if (delta_y >= 4*SPRITE_HEIGHT)
	{
		addBorders_helper(false, new_border_y);
	}
}

GhostRacer* StudentWorld::getGhostRacer() const
{
	return m_ghostRacer;
}
