#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"

/**********************************
GraphObject
		Actor
			DynamicActor
				GhostRacer
				ZombieCab
				Pedestrian
					ZombiePedestrain
					HumanPedestrian
			StaticActor
				Goodie
					OilSlick
					HolyWater
					LostSoul
			Spray
			Borderline
**********************************/

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool collidable, bool sprayable, StudentWorld* worldptr);
	virtual void doSomething() = 0;
	bool getAlive() const;
	bool collidable() const;
	bool sprayable() const;
	double getYSpeed();
	StudentWorld* getWorld() const;
	void kill();
	void setYSpeed(double YSpeed);

private:
	bool m_alive;
	bool m_collidable;
	bool m_sprayable;
	double m_yspeed;
	StudentWorld* m_worldPtr;
};

class DynamicActor : public Actor
{
public:
	DynamicActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth,
		double xspeed, double yspeed, int health, StudentWorld* worldptr);
	virtual void doSomething() = 0;
	int getHealth() const;
	void changeHealth(int health);
private:
	double m_xspeed;
	int m_health;
};

class GhostRacer : public DynamicActor
{
public:
	GhostRacer(StudentWorld* worldptr);
	virtual void doSomething();

private:
	int m_sprayNum;
	void ghostRacerMove();
};

class Pedestrian : public DynamicActor
{
public:
	Pedestrian(int imageID, double startX, double startY, double size, StudentWorld* worldptr) :
		DynamicActor(imageID, startX, startY, 0, size, 0, 0, -4, 2, worldptr)
	{}
private:
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(double startX, double startY, StudentWorld* worldptr) :
		Pedestrian(IID_ZOMBIE_PED, startX, startY, 3, worldptr)
	{}
private:
};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(double startX, double startY, StudentWorld* worldptr) :
		Pedestrian(IID_HUMAN_PED, startX, startY, 2, worldptr)
	{}
private:
};

#endif // ACTOR_H_
