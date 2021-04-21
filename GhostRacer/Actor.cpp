#define _USE_MATH_DEFINES

#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <cmath>

using namespace std;

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

bool Actor::getAlive() const { return m_alive; }

bool Actor::collidable() const { return m_collidable; }

bool Actor::sprayable() const { return m_sprayable; }

double Actor::getXSpeed() const { return m_xspeed; }

double Actor::getYSpeed() const { return m_yspeed; }

StudentWorld* Actor::getWorld() const { return m_worldPtr; }

bool Actor::offScreen() const
{
	if (getY() > VIEW_HEIGHT || getY() < 0 || getX() > VIEW_WIDTH || getX() < 0)
	{
		return true;
	}
	return false;
}
void Actor::kill() { m_alive = false; }

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

void Actor::sprayedHolyWater()
{
	return;
}

BorderLine::BorderLine(int imageID, double startX, double startY, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, 0, 2, 2, 0, -4, false, false, worldptr) {}

void BorderLine::doSomething() {
	//Set vertical movement relative to ghost racer speed
	moveRelative(getWorld()->getGhostRacer());
}

DynamicActor::DynamicActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool spray,
	double xSpeed, double ySpeed, int health, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, dir, size, depth, xSpeed, ySpeed, true, spray, worldptr)
{
	m_health = health;
	m_moveplan = 0;
}

int DynamicActor::getHealth() const { return m_health; }

void DynamicActor::changeHealth(int health) {
	m_health += health;
	m_health = std::min(100, m_health);
	if (m_health <= 0)
	{
		kill();
	}
}
int DynamicActor::getMovePlan() const
{
	return m_moveplan;
}
void DynamicActor::decrementMovePlan()
{
	m_moveplan -= 1;
}
void DynamicActor::setMovePlan(bool sprayed, bool pedestrian)
{
	if (sprayed)
	{
		setXSpeed(-getXSpeed());
		getXSpeed() > 0 ? setDirection(0) : setDirection(180);
		getWorld()->playSound(SOUND_PED_HURT);
		return;
	}

	//Decrement moveplan if greater than 0
	if (getMovePlan() > 1)
	{
		decrementMovePlan();
	}
	//Reset moveplan if 0
	else
	{
		//Update X or Y speed based on pedestrian or cab
		double newSpeed;
		if (pedestrian)
		{

			//New Speed cannot be 0 as per specs
			newSpeed = randInt(0, 1);
			if (newSpeed)
			{
				newSpeed = randInt(1, 3);
				setDirection(0);
			}
			else
			{
				newSpeed = randInt(-3, -1);
				setDirection(180);
			}
			setXSpeed(newSpeed);
			m_moveplan = randInt(4, 32);
		}
		else {
			//New Speed cannot be 0 as per specs
			newSpeed = getYSpeed() + randInt(-2, 2);
			setYSpeed(newSpeed);
			m_moveplan = randInt(4, 32);
		}
	}
}

GhostRacer::GhostRacer(StudentWorld* worldptr) :
	DynamicActor(IID_GHOST_RACER, 128, 32, 90, 4, 0, false, 0, 0, 100, worldptr)
{
	m_sprayNum = 10;
}

void GhostRacer::doSomething()
{
	if (!getAlive()) {
		return;
	}

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
			double xCoord, yCoord;
			HolyWater* sprayHolyWater;
			switch (input)
			{
			case KEY_PRESS_SPACE:
				//create holy water projectile
				if (m_sprayNum > 0)
				{
					xCoord = getX() + SPRITE_HEIGHT * cos(getDirection() * M_PI / 180);
					yCoord = getY() + SPRITE_HEIGHT * sin(getDirection() * M_PI / 180);
					sprayHolyWater = new HolyWater(xCoord, yCoord, getDirection(), getWorld());
					getWorld()->addToNoInteractActorList(sprayHolyWater);
					getWorld()->playSound(SOUND_PLAYER_SPRAY);
					m_sprayNum -= 1;
				}
				break;

			case KEY_PRESS_LEFT:
				if (getDirection() < 114) {
					setDirection(getDirection() + 8);
				}
				break;

			case KEY_PRESS_RIGHT:
				if (getDirection() > 66) {
					setDirection(getDirection() - 8);
				}
				break;

			case KEY_PRESS_UP:
				if (getYSpeed() < 5) {
					setYSpeed(getYSpeed() + 1);
				}
				break;

			case KEY_PRESS_DOWN:
				if (getYSpeed() > -1) {
					setYSpeed(getYSpeed() - 1);
				}
				break;

			default:
				break;
			}
		}
	}
	ghostRacerMove();
}

void GhostRacer::spin()
{
	//Randomly spin clockwise or counterclockwise by 5-20deg, but between 60 to 120deg
	int newDirection = randInt(5, 20);
	if (randInt(0, 1))
	{
		newDirection *= -1;
	}
	newDirection = getDirection() + newDirection;
	newDirection = min(120, max(60, newDirection));
	setDirection(newDirection);
}

int GhostRacer::getSpray() const {
	return m_sprayNum;
}
void GhostRacer::changeSpray(int change)
{
	m_sprayNum += change;
}
void GhostRacer::ghostRacerMove() {
	double maxTick = 4;
	double delta_x = cos(getDirection() * M_PI / 180) * maxTick;
	moveTo(getX() + delta_x, getY());
}

Pedestrian::Pedestrian(int imageID, double startX, double startY, double size, StudentWorld* worldptr) :
	DynamicActor(imageID, startX, startY, 0, size, 0, true, 0, -4, 2, worldptr) {
}

HumanPedestrian::HumanPedestrian(double startX, double startY, StudentWorld* worldptr) :
	Pedestrian(IID_HUMAN_PED, startX, startY, 2, worldptr) {}

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
	setMovePlan(false, true);
	return;
}

void HumanPedestrian::sprayedHolyWater()
{
	setMovePlan(true, true);
	getWorld()->playSound(SOUND_PED_HURT);
}

ZombiePedestrian::ZombiePedestrian(double startX, double startY, StudentWorld* worldptr) :
	Pedestrian(IID_ZOMBIE_PED, startX, startY, 3, worldptr) {
	m_grunt = 0;
}

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
		getWorld()->increaseScore(150);
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
	setMovePlan(false, true);
	return;
}

void ZombiePedestrian::sprayedHolyWater()
{
	setMovePlan(true, true);
	if (getHealth() > 1)
	{
		changeHealth(-1);
		getWorld()->playSound(SOUND_PED_HURT);
	}
	else
	{
		getWorld()->playSound(SOUND_PED_DIE);
		if (!randInt(0, 4))
		{
			HealingGoodie* heal = new HealingGoodie(getX(), getY(), getWorld());
			getWorld()->addToInteractActorList(heal);
		}
		kill();
	}
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
			getWorld()->getGhostRacer()->changeHealth(-20);
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
	//Test collisions. 0 is not near collisions, 1 is to require slowing down, 2 is to require speeding up.
	switch (getWorld()->checkLaneCollisions(this))
	{
	case 0:
		//No collisions, act on its movement plan
		decrementMovePlan();
		setMovePlan(false, false);
		break;
	case 1:
		//Actor ahead, slow down by half a pixel
		setYSpeed(getYSpeed() - 0.5);
	case 2:
		//Actor behind, speed up by half a pixel
		setYSpeed(getYSpeed() + 0.5);
	}
}

void ZombieCab::sprayedHolyWater()
{

	if (getHealth() > 1)
	{
		getWorld()->playSound(SOUND_VEHICLE_HURT);
		changeHealth(-1);

	}
	else
	{
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		getWorld()->increaseScore(200);
		if (!randInt(0, 4))
		{
			OilSlick* oil = new OilSlick(getX(), getY(), randInt(2, 5), getWorld());
			getWorld()->addToNoInteractActorList(oil);
		}

		kill();

	}

}

StaticActor::StaticActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth,
	bool sprayable, StudentWorld* worldptr) :
	Actor(imageID, startX, startY, dir, size, depth, 0, -4, false, sprayable, worldptr) {}

OilSlick::OilSlick(double startX, double startY, double size, StudentWorld* worldptr) :
	StaticActor(IID_OIL_SLICK, startX, startY, 0, size, 2, false, worldptr) {}

void OilSlick::doSomething()
{
	moveRelative(getWorld()->getGhostRacer());
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->spin();
		getWorld()->playSound(SOUND_OIL_SLICK);
		return;
	}
}

HealingGoodie::HealingGoodie(double startX, double startY, StudentWorld* worldptr) :
	StaticActor(IID_HEAL_GOODIE, startX, startY, 0, 1, 2, true, worldptr) {}
void HealingGoodie::doSomething()
{
	moveRelative(getWorld()->getGhostRacer());
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->changeHealth(10);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(250);
		kill();
		return;
	}
}
void HealingGoodie::sprayedHolyWater()
{
	kill();
}

SprayBottle::SprayBottle(double startX, double startY, StudentWorld* worldptr) :
	StaticActor(IID_HOLY_WATER_GOODIE, startX, startY, 90, 2, 2, true, worldptr) {}

void SprayBottle::doSomething()
{
	moveRelative(getWorld()->getGhostRacer());
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		getWorld()->getGhostRacer()->changeSpray(10);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
		kill();
		return;
	}
}
void SprayBottle::sprayedHolyWater()
{
	kill();
}

LostSoul::LostSoul(double startX, double startY, StudentWorld* worldptr) :
	StaticActor(IID_SOUL_GOODIE, startX, startY, 0, 4, 2, false, worldptr) {}

void LostSoul::doSomething()
{
	moveRelative(getWorld()->getGhostRacer());
	if (getWorld()->checkCollision(this, getWorld()->getGhostRacer()))
	{
		getWorld()->savedSoul();
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->increaseScore(100);
		kill();
		return;
	}
	setDirection(getDirection() + 10);
}

HolyWater::HolyWater(double startX, double startY, int dir, StudentWorld* worldptr) :
	Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1, 1, 0, 0, false, false, worldptr)
{
	m_travel = 160;
}

void HolyWater::doSomething()
{
	if (!getAlive())
	{
	return;
	}

	//check for collision with another actor
	if (getWorld()->checkHolyWaterCollision(this))
	{
		kill();
		return;
	}

	//Has not collided, move forward up to max distance
	moveForward(SPRITE_HEIGHT);
	m_travel -= SPRITE_HEIGHT;
	if (offScreen() || m_travel <= 0)
	{
		kill();
	}
}