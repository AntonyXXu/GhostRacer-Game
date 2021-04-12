#define _USE_MATH_DEFINES

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

using namespace std;

//Actor
Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth,
	double xSpeed, double ySpeed, bool collidable, bool sprayable, StudentWorld* worldptr) :
	GraphObject(imageID, startX, startY, dir, size, depth)
{
	m_alive = true;
	m_collidable = collidable;
	m_sprayable = sprayable;
	m_xspeed = xSpeed;
	m_yspeed = ySpeed;
	m_worldPtr = worldptr;
}

bool Actor::getAlive() const { return m_alive; };

bool Actor::collidable() const { return m_collidable; };

bool Actor::sprayable() const { return m_sprayable; };

double Actor::getXSpeed() const { return m_xspeed; };

double Actor::getYSpeed() const { return m_yspeed; }

StudentWorld* Actor::getWorld() const { return m_worldPtr; };

bool Actor::offScreen() const
{
	if (getY() > VIEW_HEIGHT || getY() < 0 || getX() > VIEW_WIDTH || getX() < 0)
	{
		return true;
	}
	return false;
}
void Actor::kill() { m_alive = false; };

void Actor::setXSpeed(double XSpeed) {
	m_xspeed = XSpeed;
}
void Actor::setYSpeed(double YSpeed) {
	m_yspeed = YSpeed;
}

void Actor::moveRelative(Actor* other_actor)
{
	double newX = getX() + getXSpeed();
	double newY = getY() + getYSpeed() - other_actor->getYSpeed();
	moveTo(newX, newY);
	if (offScreen())
	{
		kill();
	}
}

//BorderLine
BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, 0, 2, 2, 0, -4, false, false, worldptr) {}

void BorderLine::doSomething() {
	//Set vertical movement relative to ghost racer speed
	moveRelative(getWorld()->getGhostRacer());
}

//Dynamic Actor
DynamicActor::DynamicActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool spray,
	double xSpeed, double ySpeed, int health, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, dir, size, depth, xSpeed, ySpeed, true, spray, worldptr)
{
	m_health = health;
};

int DynamicActor::getHealth() const { return m_health; };

void DynamicActor::changeHealth(int health) {
	m_health += health;
	m_health = std::min(100, m_health);
	if (m_health <= 0)
	{
		kill();
	}
};

//GhostRacer
GhostRacer::GhostRacer(StudentWorld* worldptr) :
	DynamicActor(IID_GHOST_RACER, 128, 32, 90, 4, 0, false, 0, 0, 100, worldptr)
{
	m_sprayNum = 10;
}

void GhostRacer::doSomething()
{
	if (!getAlive()) {
		return;
	};

	if (getX() <= StudentWorld::LEFT_EDGE && getDirection() > 90) {
		setDirection(82);
		changeHealth(-10);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() >= StudentWorld::RIGHT_EDGE && getDirection() < 90) {
		setDirection(98);
		changeHealth(-10);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else {
		int input;
		if (getWorld()->getKey(input))
		{
			switch (input)
			{
			case KEY_PRESS_SPACE:
				//create holy
				break;

			case KEY_PRESS_LEFT:
				if (getDirection() < 114) {
					setDirection(getDirection() + 8);
				};
				break;

			case KEY_PRESS_RIGHT:
				if (getDirection() > 66) {
					setDirection(getDirection() - 8);
				};
				break;

			case KEY_PRESS_UP:
				if (getYSpeed() < 5) {
					setYSpeed(getYSpeed() + 1);
				};
				break;

			case KEY_PRESS_DOWN:
				if (getYSpeed() > -1) {
					setYSpeed(getYSpeed() - 1);
				};
				break;

			default:
				break;
			}
		}
	}
	ghostRacerMove();
};
int GhostRacer::getSpray() {
	return m_sprayNum;
}
void GhostRacer::ghostRacerMove() {
	double maxTick = 4;
	double delta_x = cos(getDirection() * M_PI / 180) * maxTick;
	moveTo(getX() + delta_x, getY());
}

//Pedestrian
Pedestrian::Pedestrian(int imageID, double startX, double startY, double size, StudentWorld* worldptr) :
	DynamicActor(imageID, startX, startY, 0, size, 0, true, 0, -4, 2, worldptr) {
	m_moveplan = 0;
}

void Pedestrian::setMovePlan(bool sprayed)
{
	if (sprayed)
	{
		setXSpeed(-getXSpeed());
		getXSpeed() > 0 ? setDirection(180) : setDirection(0);
		getWorld()->playSound(SOUND_PED_HURT);
	}

	//Decrement moveplan if greater than 0
	if (m_moveplan > 1)
	{
		m_moveplan -= 1;
	}
	//Reset moveplan if 0
	else
	{
		//New Speed cannot be 0 as per specs
		int newXSpeed = randInt(0, 1);
		if (newXSpeed)
		{
			newXSpeed = randInt(1, 3);
			setDirection(0);
		}
		else
		{
			newXSpeed = randInt(-3, -1);
			setDirection(180);
		}
		setXSpeed(newXSpeed);
		m_moveplan = randInt(4, 32);
	}
};

//Human Pedestrian
HumanPedestrian::HumanPedestrian(double startX, double startY, StudentWorld* worldptr) :
	Pedestrian(IID_HUMAN_PED, startX, startY, 2, worldptr) {};

void HumanPedestrian::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	//Check if GhostRacer hit human pedestrian. Kill GhostRacer if true
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->kill();
		return;
	}
	//move
	moveRelative(getWorld()->getGhostRacer());
	setMovePlan(false);
	return;
}

ZombiePedestrian::ZombiePedestrian(double startX, double startY, StudentWorld* worldptr) :
	Pedestrian(IID_ZOMBIE_PED, startX, startY, 3, worldptr) {
	m_grunt = 0;
};
void ZombiePedestrian::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->changeHealth(-5);
		getWorld()->playSound(SOUND_PED_DIE);
		kill();
		return;
	}
	/*set direction towards GhostRacer if x within 30pixels*/
	double deltaX = getX() - getWorld()->getGhostRacer()->getX();
	if (abs(deltaX) < 30)
	{
		if (deltaX == 0)
		{
			setDirection(270);
			setXSpeed(0);
		}
		else if (deltaX < 0)
		{
			setXSpeed(1);
		}
		else
		{
			setXSpeed(-1);
		}
		m_grunt -= 1;
		if (m_grunt <= 0)
		{
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			m_grunt = 20;
		}
	}
	//move zombie
	moveRelative(getWorld()->getGhostRacer());
	setMovePlan(false);
	return;
}

ZombieCab::ZombieCab(double startX, double startY, double yspeed, StudentWorld* worldptr) :
	DynamicActor(IID_ZOMBIE_CAB, startX, startY, 90, 4, 0, true, 0, yspeed, 3, worldptr) 
{
	m_hit = false;
}

void ZombieCab::doSomething()
{
	if (!getAlive())
	{
		return;
	}
	//Check for collision and adjust direction if hit
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		while (!m_hit)
		{
			m_hit = true;
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			if (getX() <= getWorld()->getGhostRacer()->getX())
			{
				setXSpeed(-5);
				int newDirection = 120 + randInt(0, 20);
				setDirection(newDirection);
			}
			else
			{
				setXSpeed(5);
				int newDirection = 60 - randInt(0, 20);
				setDirection(newDirection);
			}
		}
	}
	//move zombie cab
	moveRelative(getWorld()->getGhostRacer());
}