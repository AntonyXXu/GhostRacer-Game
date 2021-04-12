#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"

/**********************************
GraphObject
		Actor
			Borderline
			DynamicActor
				GhostRacer
				Pedestrian
					ZombiePedestrain
					HumanPedestrian
				ZombieCab
			StaticActor
				Goodie
					OilSlick
					HolyWater
					LostSoul
			Spray
**********************************/

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth,
		double xSpeed, double ySpeed, bool collidable, bool sprayable, StudentWorld* worldptr);
	virtual void doSomething() = 0;
	bool getAlive() const;
	bool collidable() const;
	bool sprayable() const;
	double getXSpeed() const;
	double getYSpeed() const;
	StudentWorld* getWorld() const;
	bool offScreen() const;
	void kill();
	void setXSpeed(double XSpeed);
	void setYSpeed(double YSpeed);
	void moveRelative(Actor* otherActor);

private:
	bool m_alive;
	bool m_collidable;
	bool m_sprayable;
	double m_xspeed;
	double m_yspeed;
	StudentWorld* m_worldPtr;
};

class BorderLine : public Actor
{
public:
	BorderLine(int imageID, double startX, double startY, StudentWorld* worldptr);
	virtual void doSomething();
};

class DynamicActor : public Actor
{
public:
	DynamicActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, bool spray,
		double xSpeed, double yspeed, int health, StudentWorld* worldptr);
	virtual void doSomething() = 0;
	int getHealth() const;
	void changeHealth(int health);
	int getMovePlan() const; 
	void decrementMovePlan();
	void setMovePlan(bool sprayed, bool pedestrian);
	
private:
	int m_health;
	int m_moveplan;
};

class GhostRacer : public DynamicActor
{
public:
	GhostRacer(StudentWorld* worldptr);
	virtual void doSomething();
	int getSpray();
private:
	int m_sprayNum;
	void ghostRacerMove();
};

class Pedestrian : public DynamicActor
{
public:
	Pedestrian(int imageID, double startX, double startY, double size, StudentWorld* worldptr);
	virtual void doSomething() = 0;
private:

};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(double startX, double startY, StudentWorld* worldptr);
	virtual void doSomething();
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(double startX, double startY, StudentWorld* worldptr);
	virtual void doSomething();
private:
	int m_grunt;
};

class ZombieCab : public DynamicActor
{
public:
	ZombieCab(double startX, double startY, double yspeed, StudentWorld* worldptr);
	virtual void doSomething();
private:
	bool m_hit;
};

#endif // ACTOR_H_
