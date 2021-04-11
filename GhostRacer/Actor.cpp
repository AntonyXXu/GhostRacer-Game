#define _USE_MATH_DEFINES

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

using namespace std;

//Actor
Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, double ySpeed, bool collidable, bool sprayable, StudentWorld* worldptr) :
	GraphObject(imageID, startX, startY, dir, size, depth)
{
	m_alive = true;
	m_collidable = collidable;
	m_sprayable = sprayable;
	m_yspeed = ySpeed;
	m_worldPtr = worldptr;
}
bool Actor::getAlive() const { return m_alive; };
bool Actor::collidable() const {
	return m_collidable;
};
bool Actor::sprayable() const {
	return m_sprayable;
};
double Actor::getYSpeed() {
	return m_yspeed;
}
StudentWorld* Actor::getWorld() const { return m_worldPtr; };
void Actor::kill() { m_alive = false; };
void Actor::setYSpeed(double YSpeed) {
	m_yspeed = YSpeed;
}

//BorderLine
BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, 0, 2, 2, -4, false, false, worldptr) {}
void BorderLine::doSomething() {
	//Set vertical speed to ghost racer speed
	double newY = getY() + getYSpeed() - getWorld()->getGhostRacer()->getYSpeed();
	if (newY > VIEW_HEIGHT) {
		kill();
	}
	moveTo(getX(), newY);
}

//Dynamic Actor
DynamicActor::DynamicActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool spray,
	double xspeed, double ySpeed, int health, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, dir, size, depth, ySpeed, true, spray, worldptr)
{
	m_xspeed = xspeed;
	m_health = health;
};
double DynamicActor::getXSpeed() const { return m_xspeed; };
int DynamicActor::getHealth() const { return m_health; };

void DynamicActor::setXSpeed(double XSpeed) {
	m_xspeed = XSpeed;
}
void DynamicActor::changeHealth(int health) {
	m_health += health;
	m_health = std::min(100, m_health);
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
void GhostRacer::ghostRacerMove() {
	double maxTick = 4;
	double delta_x = cos(getDirection() * M_PI / 180) * maxTick;
	moveTo(getX() + delta_x, getY());
}

