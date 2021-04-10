#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

using namespace std;
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
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool collidable, bool sprayable, StudentWorld* worldptr) :
		GraphObject(imageID, startX, startY, dir, size, depth) 
	{
		m_alive = true;
		m_collidable = collidable;
		m_sprayable = sprayable;
		m_yspeed = 0;
	}
	virtual void doSomething() = 0;
	bool getAlive() const { return m_alive; };
	void kill() { m_alive = false; };
	bool collidable() const {
		return m_collidable;
	};
	bool sprayable() const {
		return m_sprayable;
	};
	double getYSpeed() {
		return m_yspeed;
	}
	StudentWorld* getWorld() const { return m_worldPtr; };
	
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
		double xspeed, double yspeed, int health) :
		Actor(imageID, startX, startY, dir, size, depth, true, true)
	{
		m_xspeed = xspeed;
		m_health = health;
	}
	virtual void doSomething();
	int getHealth() { return m_health; };
	void changeHealth(int health)
	{
		m_health += health;
		m_health = std::min(100, m_health);
	};
	

private:
	double m_xspeed;

	int m_health;
};

class GhostRacer : public DynamicActor
{
public:
	GhostRacer() :
		DynamicActor(IID_GHOST_RACER, 128, 32, 90, 4, 0, 0, 0, 100)
	{
		m_sprayNum = 10;
	}
	virtual void doSomething();

private:
	int m_sprayNum;
};

class Pedestrian : public DynamicActor
{
public:
	Pedestrian(int imageID, double startX, double startY, double size) :
		DynamicActor(imageID, startX, startY, 0, size, 0, 0, -4, 2)
	{}
private:
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(double startX, double startY) :
		Pedestrian(IID_ZOMBIE_PED, startX, startY, 3)
	{}
private:
};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(double startX, double startY) :
		Pedestrian(IID_HUMAN_PED, startX, startY, 2)
	{}
private:
};

#endif // ACTOR_H_
