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
	m_ghostRacer = NULL;
	m_bonusPoints = 5000;
	m_soulsToSave = 2 * getLevel() + 5;
	m_previousBorderY = 0;
	for (int i = 0; i < kNumLanes; i++)
	{
		m_botCollisionActor.push_back(VIEW_HEIGHT);
		m_topCollisionActor.push_back(VIEW_HEIGHT);
	}
}

int StudentWorld::init()
{
	m_ghostRacer = new GhostRacer(this);
	addToInteractActorList(m_ghostRacer);
	m_bonusPoints = 5000;
	m_soulsToSave = 2 * getLevel() + 5;
	m_previousBorderY = 0;

	//init yellow lines. init i as double to multiply sprite
	for (int i = 0; i < kNumYellowLine; i++) {
		addBorders_helper(true, i * SPRITE_HEIGHT);
		addBorders_helper(true, i * SPRITE_HEIGHT);
	}
	//init white lines
	for (int i = 0; i < kNumWhiteLines; i++) {
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
	int returnval = moveActiveActors(m_interactActorList);
	if (returnval != -1) { return returnval; }
	returnval = moveActiveActors(m_noInteractActorList);
	if (returnval != -1) { return returnval; }

	//Remove dead actors
	removeDeadActors(m_interactActorList);
	removeDeadActors(m_noInteractActorList);

	//Add new actors 
	addActors();

	updateGameText();
	m_bonusPoints -= 1;
	return GWSTATUS_CONTINUE_GAME;
	//return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{
	while (!m_interactActorList.empty())
	{
		Actor* del = m_interactActorList.front();
		delete del;
		m_interactActorList.pop_front();
	}
	while (!m_noInteractActorList.empty())
	{
		Actor* del = m_noInteractActorList.front();
		delete del;
		m_noInteractActorList.pop_front();
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
}
bool StudentWorld::checkHolyWaterCollision(Actor* holyWaterSpray)
{
	list<Actor*>::iterator itr = m_interactActorList.begin();
	while (itr != m_interactActorList.end())
	{

		if ((*itr)->sprayable() &&
			checkCollision(holyWaterSpray, (*itr)))
		{
			(*itr)->sprayedHolyWater();
			return true;
		}
		itr++;
	}
	return false;
}


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

	list<Actor*>::iterator itr = m_interactActorList.begin();
	int lane = getLane(actor->getX());
	if (actor->getYSpeed() > getGhostRacer()->getYSpeed())
	{
		//Check if there is an actor above within 96 pixels
		while (itr != m_interactActorList.end())
		{
			if ((*itr)->collidable() &&
				(*itr) != actor &&
				(*itr)->getY() - actor->getY() < 96 &&
				(*itr)->getY() - actor->getY() > 0 &&
				getLane((*itr)->getX()) == lane)
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
		while (itr != m_interactActorList.end())
		{
			if ((*itr)->collidable() &&
				(*itr) != actor &&
				actor->getY() - (*itr)->getY() < 96 &&
				actor->getY() - (*itr)->getY() > 0 &&
				getLane((*itr)->getX()) == lane)
			{
				return 2;
			}
			itr++;
		}
		return 0;
	}
}

void StudentWorld::savedSoul()
{
	m_soulsToSave -= 1;
}
void StudentWorld::addToInteractActorList(Actor* actor)
{
	m_interactActorList.push_front(actor);
}
void StudentWorld::addToNoInteractActorList(Actor* actor)
{
	m_noInteractActorList.push_front(actor);
}
void StudentWorld::updateGameText()
{
	string score = to_string(getScore());
	string level = to_string(getLevel());
	string souls = to_string(m_soulsToSave);
	string lives = to_string(getLives());
	string health = to_string(m_ghostRacer->getHealth());
	string sprays = to_string(m_ghostRacer->getSpray());
	string bonus = to_string(m_bonusPoints);

	setGameStatText("Score: " + score + "  " + "Lvl: " + level + "  " + "SoulsToSave: " + souls + "  " +
		"Lives: " + lives + "  " + "Health: " + health + "  " + "Sprays: " + sprays + "  " + "Bonus: " + bonus);
}

void StudentWorld::addActors()
{
	addBorderLines();
	addHumanPedestrian();
	addZombiePedestrian();
	addZombieCab();
	addOilSlick();
	addSprayBottle();
	addLostSoul();
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
		addToInteractActorList(human);
	}

}
void StudentWorld::addZombiePedestrian()
{
	int ChanceZombiePed = max(100 - 10 * getLevel(), 20);
	if (!randInt(0, ChanceZombiePed))
	{
		ZombiePedestrian* human = new ZombiePedestrian(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
		addToInteractActorList(human);
	}
}

void StudentWorld::addZombieCab()
{
	int chanceVehicle = max(100 - getLevel() * 10, 20);
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
		addToInteractActorList(cab);
	}
}

void StudentWorld::addOilSlick()
{
	int  chanceOilSlick = max(150 - getLevel() * 10, 40);
	if (!randInt(0, chanceOilSlick))
	{
		double xCoord = randInt(LEFT_EDGE, RIGHT_EDGE);
		int size = randInt(2, 5);
		OilSlick* oil = new OilSlick(xCoord, VIEW_HEIGHT, size, this);
		addToNoInteractActorList(oil);
	}
}

void StudentWorld::addSprayBottle()
{
	int chanceSprayBottle = 100 + getLevel() * 10;
	if (!randInt(0, chanceSprayBottle))
	{
		double xCoord = randInt(LEFT_EDGE, RIGHT_EDGE);
		SprayBottle* bottle = new SprayBottle(xCoord, VIEW_HEIGHT, this);
		addToInteractActorList(bottle);
	}
}

void StudentWorld::addLostSoul()
{
	int chanceLostSoul = 100;
	if (!randInt(0, chanceLostSoul))
	{
		double xCoord = randInt(LEFT_EDGE, RIGHT_EDGE);
		LostSoul* soul = new LostSoul(xCoord, VIEW_HEIGHT, this);
		addToNoInteractActorList(soul);
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
		addToNoInteractActorList(left);
		addToNoInteractActorList(right);
		break;
	case false:
		left = new BorderLine(IID_WHITE_BORDER_LINE, kLeftWhiteLine, startY, this);
		right = new BorderLine(IID_WHITE_BORDER_LINE, kRightWhiteLine, startY, this);
		addToNoInteractActorList(left);
		addToNoInteractActorList(right);
		m_previousBorderY = startY;
		break;
	}
}

int StudentWorld::getLane(double x_coord)
{
	//Get Lane, with left lane = 1, middle = 2, right = 3
	if (LEFT_EDGE <= x_coord && x_coord < kLeftWhiteLine)
	{
		return 0;
	}
	else if (x_coord <= kRightWhiteLine)
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

int StudentWorld::moveActiveActors(std::list<Actor*>& actorList)
{
	list<Actor*>::iterator itr = actorList.begin();
	while (itr != actorList.end()) {
		(*itr)->doSomething();

		//Check if ghostRacer is dead
		if (!m_ghostRacer->getAlive())
		{
			playSound(SOUND_PLAYER_DIE);
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}

		//Check if the level is completed
		if (m_soulsToSave <= 0)
		{
			increaseScore(m_bonusPoints);
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}

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
	return -1;
}
void StudentWorld::removeDeadActors(std::list<Actor*>& actorList)
{
	list<Actor*>::iterator itr = actorList.begin();
	while (itr != actorList.end()) {
		if (!(*itr)->getAlive())
		{
			Actor* del = (*itr);
			itr = actorList.erase(itr);
			delete del;
		}
		else
		{
			itr++;
		}
	}
}