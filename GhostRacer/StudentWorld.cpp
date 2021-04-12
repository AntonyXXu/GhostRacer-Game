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
	m_soulsToSave = 2 * getLevel() + 5;
	for (int i = 0; i < NUM_LANES; i++)
	{
		m_botCollisionActor.push_back(VIEW_HEIGHT);
		m_topCollisionActor.push_back(VIEW_HEIGHT);
	}
}

int StudentWorld::init()
{
	m_ghostRacer = new GhostRacer(this);
	m_actorList.push_front(m_ghostRacer);


	//init yellow lines. init i as double to multiply sprite
	for (int i = 0; i < NUM_YELLOW_LINE; i++) {
		addBorders_helper(true, i * SPRITE_HEIGHT);
		addBorders_helper(true, i * SPRITE_HEIGHT);
	}
	//init white lines
	for (int i = 0; i < NUM_WHITE_LINE; i++) {
		addBorders_helper(false, 4 * i * SPRITE_HEIGHT);
		addBorders_helper(false, 4 * i * SPRITE_HEIGHT);
	}
	return GWSTATUS_CONTINUE_GAME;

}

int StudentWorld::move()
{
	//Reset the collision coordinates
	fill(m_botCollisionActor.begin(), m_botCollisionActor.end(), VIEW_HEIGHT);
	fill(m_topCollisionActor.begin(), m_topCollisionActor.end(), 0);



	//Iterate through all actors to doSomething
	//Also check collidable actors (avoid re-iterating to add cabs)
	list<Actor*>::iterator itr = m_actorList.begin();
	while (itr != m_actorList.end()) {
		(*itr)->doSomething();

		//Update the collision coordinates for ZombieCab Navigation
		if ((*itr)->collidable())
		{
			int actorLane = getLane((*itr)->getX());
			if (actorLane != -1)
			{
				double y_coord = (*itr)->getY();
				m_botCollisionActor[actorLane] = min(m_botCollisionActor[actorLane], y_coord);
				m_topCollisionActor[actorLane] = max(m_topCollisionActor[actorLane], y_coord);
			}
		}
		itr++;
	}
	//Check if ghostRacer is dead
	if (!m_ghostRacer->getAlive())
	{
		playSound(SOUND_PLAYER_DIE);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	//Remove dead actors
	itr = m_actorList.begin();
	while (itr != m_actorList.end()) {
		if (!(*itr)->getAlive())
		{
			Actor* del = (*itr);
			itr = m_actorList.erase(itr);
			delete del;
		}
		else
		{
			itr++;
		}
	}

	//Add new actors 
	addActors();

	update_game_text();
	m_bonusPoints -= 1;
	return GWSTATUS_CONTINUE_GAME;
	//return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
	while (!m_actorList.empty())
	{
		Actor* del = m_actorList.front();
		delete del;
		m_actorList.pop_front();
	}
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}
GhostRacer* StudentWorld::getGhostRacer() const
{
	return m_ghostRacer;
}
bool StudentWorld::checkCollision(Actor* actorA, Actor* actorB)
{
	//Checks collision based on object radius, and x/y coordinates	
	double deltaX = abs(actorA->getX() - actorB->getX());
	double deltaY = abs(actorA->getY() - actorB->getY());
	double radiusSum = actorA->getRadius() + actorB->getRadius();
	if (deltaX < radiusSum * 0.25 && deltaY < radiusSum * 0.6)
	{
		return true;
	}
	return false;
};

int StudentWorld::checkLaneCollisions(Actor* actor)
{
	//Check relative speed to GhostRacer
	//Returns 0 to signal no collision risks
	//Returns 1 to signal slow down due to actor above
	//Returns 2 to signal speed up due to actor below

	if (actor->getYSpeed() == getGhostRacer()->getYSpeed())
	{
		return 0;
	}

	list<Actor*>::iterator itr = m_actorList.begin();
	int lane = getLane(actor->getX());
	if (actor->getYSpeed() > getGhostRacer()->getYSpeed())
	{
		//Check if there is an actor above within 96 pixels
		while (itr != m_actorList.end())
		{
			if (
				(*itr)->collidable() &&
				(*itr) != actor &&
				(*itr)->getY() - actor->getY() < 96 &&
				(*itr)->getY() - actor->getY() > 0 &&
				getLane((*itr)->getX()) == lane
				)
			{
				return 1;
			}
			itr++;
		}

		return 0;
	}
	else
	{
		//Check if there is an actor below within 96 pixels
		while (itr != m_actorList.end())
		{
			if (
				(*itr)->collidable() &&
				(*itr) != actor &&
				actor->getY() - (*itr)->getY() < 96 &&
				actor->getY() - (*itr)->getY() > 0 &&
				getLane((*itr)->getX()) == lane
				)
			{
				return 2;
			}
			itr++;
		}
		return 0;
	}
}

void StudentWorld::update_game_text()
{
	string score = to_string(getScore());
	string level = to_string(getLevel());
	string lives = to_string(getLives());
	string health = to_string(m_ghostRacer->getHealth());
	string sprays = to_string(m_ghostRacer->getSpray());
	string bonus = to_string(m_bonusPoints);

	setGameStatText("Score: " + score + "  " + "Level: " + level + "  " +
		"Lives: " + lives + "  " + "Health: " + health + "  " +
		"Sprays: " + sprays + "  " + "Bonus Points: " + bonus);
}

void StudentWorld::addActors()
{
	addBorderLines();
	addHumanPedestrian();
	addZombiePedestrian();
	addZombieCab();
}

void StudentWorld::addBorderLines()
{
	int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
	m_previousBorderY -= 4 + m_ghostRacer->getYSpeed();
	double delta_y = new_border_y - m_previousBorderY;
	if (delta_y >= SPRITE_HEIGHT)
	{
		addBorders_helper(true, new_border_y);
	}
	if (delta_y >= 4 * SPRITE_HEIGHT)
	{
		addBorders_helper(false, new_border_y);
	}
}
void StudentWorld::addHumanPedestrian()
{
	int ChanceHumanPed = max(200 - getLevel(), 30);
	if (!randInt(0, ChanceHumanPed))
	{
		HumanPedestrian* human = new HumanPedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
		m_actorList.push_front(human);
	}

}
void StudentWorld::addZombiePedestrian()
{
	int ChanceZombiePed = max(100 - 10 * getLevel(), 20);
	if (!randInt(0, ChanceZombiePed))
	{
		ZombiePedestrian* human = new ZombiePedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
		m_actorList.push_front(human);
	}
}

void StudentWorld::addZombieCab()
{
	//int chanceVehicle = max(100 - getLevel() * 10, 20);
	int chanceVehicle = max(40 - getLevel() * 10, 20);
	if (!randInt(0, chanceVehicle))
	{
		//Create a 3-lane array, and randomize it. Initialize spawning ZombieCab variables
		int spawnLane = -1;
		double ySpawnCoord, ySpawnSpeed, xSpawnCoord;
		bool spawn = false;
		int spawnLanes[3] = { 0,1,2 };
		random_shuffle(begin(spawnLanes), end(spawnLanes));

		//iterate through the lanes, until either we find a valid lane or reach the end
		for (int i = 0; i < 3; i++)
		{
			int lane = spawnLanes[i];
			if (m_botCollisionActor[lane] > VIEW_HEIGHT / 3)
			{
				ySpawnCoord = SPRITE_HEIGHT / 2;
				ySpawnSpeed = getGhostRacer()->getYSpeed() + randInt(2, 4);
				spawn = true;
			}
			else if (m_topCollisionActor[lane] < VIEW_HEIGHT * 2 / 3)
			{
				ySpawnCoord = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
				ySpawnSpeed = getGhostRacer()->getYSpeed() - randInt(2, 4);
				spawn = true;
			}
			if (spawn)
			{
				spawnLane = lane;
				xSpawnCoord = ROAD_CENTER - ROAD_WIDTH / 3 + ROAD_WIDTH * lane / 3;
				break;
			}
		}
		if (!spawn)
		{
			return;
		}
		ZombieCab* cab = new ZombieCab(xSpawnCoord, ySpawnCoord, ySpawnSpeed, this);
		m_actorList.push_front(cab);
	}
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

int StudentWorld::getLane(double x_coord)
{
	//Get Lane, with left lane = 1, middle = 2, right = 3
	if (LEFT_EDGE <= x_coord && x_coord < LEFT_WHITE_LINE)
	{
		return 0;
	}
	else if (x_coord <= RIGHT_WHITE_LINE)
	{
		return 1;
	}
	else if (x_coord <= RIGHT_EDGE)
	{
		return 2;
	}
	else
	{
		return -1;
	}
}