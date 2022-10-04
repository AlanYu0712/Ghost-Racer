#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"



// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class StudentWorld;
class GhostRacer;

//BASE CLASS
class Actor: public GraphObject {
public:

	//CONSTRUCTOR
	Actor(StudentWorld* SW, int imageID, double startX, double startY, int dir, double size, unsigned int depth, double vert_V);

	//DESTUCTOR
	/*virtual ~Actor();*/

	//DOSOEMTHING
	virtual void doSomething()= 0;

	//GETTER
	//STATUS
	bool getStatus() const;
	void setStatus(bool status);
	//HORIZONTAL SPEED
	int getHoriV() const;
	void setHoriV(double horiV);
	//VERTICLE SPEED
	double getVertV() const;
	void setVertV(double vertV);
	//HELPER FUNCTIONS
	// If this actor is affected by holy water projectiles, then inflict that
	// affect on it and return true; otherwise, return false.
	virtual bool beSprayedIfAppropriate() = 0;

	// Does this object affect zombie cab placement and speed?
	virtual bool isCollisionAvoidanceWorthy() const = 0;

	

protected:
	//STUDENT WORLD	
	StudentWorld* getWorld() const;

	//GHOST RACER
	GhostRacer* GR() const;

	// Adjust the x coordinate by dx to move to a position with a y coordinate
	// determined by this actor's vertical speed relative to GhostRacser's
	// vertical speed.  Return true if the new position is within the view;
	// otherwise, return false, with the actor dead.
	//( was virtual but don't know why /ALSO THE PARAMETER THINGY)
	bool moveRelativeToGhostRacerVerticalSpeed();

private:
	bool m_status;
	int m_horiV;
	double m_vertV;
	StudentWorld* m_SW;
	GhostRacer* m_GR;

};


//Agent
class Agent :public Actor {
public:
	//CONSTRUCTOR
	Agent(int hitPoints, StudentWorld* SW, int imageID, double startX, double startY, int dir, double size, double vert_V);

	//HIT POINTS
	int getHitPoints() const;
	//HP
	
	void increaseHP(int HP);

	//DAMAGE
	// Do what the spec says happens when hp units of damage is inflicted.
	// Return true if this agent dies as a result, otherwise false.
	virtual bool takeDamageAndPossiblyDie(int hp);
	

	//VIRTUAL is avoidance-worthy (FROM ACTOR CLASS)
	virtual bool isCollisionAvoidanceWorthy() const;

protected:
	void decrementHitPoints(int HP);
	// What sound should play when this agent is damaged but does not die?
	virtual void soundWhenHurt() const = 0;

	// What sound should play when this agent is damaged and dies?
	virtual void soundWhenDie() const = 0;
	
private:
	//FOR ZOMBIES and GR
	virtual void takeDamgedifferent();

	int m_hitPoints;
};

//GHOST RACER
class GhostRacer : public Agent {
public:
	//CONSTRUCTOR
	GhostRacer(StudentWorld* SW, double startX, double startY);
	
	//Holy Spray Quota
	int getQuota() const;
	void incrementQuota(int q);

	//DOSOMETHING
	void doSomething();

	//Spin Algorithm
	void sbinalla(); //term used if you watch F1

	//VIRTUAL
	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();

protected:
	//sound
	virtual void soundWhenHurt() const;
	virtual void soundWhenDie() const;
	//Moving Algorithm
	void moveAlgo();

private:
	virtual void takeDamgedifferent();
	int m_holyQuota;
};

//PEDESTRIAN
class Pedestrian : public Agent
{
public:
	Pedestrian(StudentWorld* SW, int imageID, double x, double y, double size);
	
protected:
	//PLAY SOUND
	//(was virtual / was return type int dont know why)
	virtual void soundWhenHurt() const;
	virtual void soundWhenDie() const;
	//m_distance
	int getDistance() const;
	void setDistance(int newD);

	//Movement plan algorithm
	void movementPlan();

private:
	int m_distance;
};

//HUMAN PED
class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(StudentWorld* SW, double x, double y);
	virtual void doSomething();
	//VIRTUAL
	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();
};

//ZOMBIE PED
class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(StudentWorld* SW, double x, double y);
	virtual void doSomething();
	//VIRTUAL
	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();
	
private:
	int m_tick;
	//VIRTUAL taking DAMAGE DIFFERENT (FROM AGENT CLASS)
	virtual void takeDamgedifferent();
};

//ZOMBIE CAB
class ZombieCab : public Agent
{
public:
	ZombieCab(StudentWorld* SW, double x, double y, double v);

	virtual void doSomething();
	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();
	
protected:
	//sound
	virtual void soundWhenHurt() const;
	virtual void soundWhenDie() const;

private:
	bool m_DamagedPlayer;
	int m_distance;
	//VIRTUAL taking DAMAGE DIFFERENT (FROM AGENT CLASS)
	virtual void takeDamgedifferent();
};

//SPRAY
class Spray : public Actor
{
public:
	Spray(StudentWorld* SW, double x, double y, int dir);
	virtual void doSomething();
	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();
	//VIRTUAL is avoidance-worthy (FROM ACTOR CLASS)
	virtual bool isCollisionAvoidanceWorthy() const;

private:
	int m_pixels;
};

//GOODIES AND SLICKS
class GhostRacerActivatedObject : public Actor
{
public:
	GhostRacerActivatedObject(StudentWorld* SW, int imageID, double x, double y, double size, int dir);
	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();
	//VIRTUAL is avoidance-worthy (FROM ACTOR CLASS)
	virtual bool isCollisionAvoidanceWorthy() const;
protected:
	void doActivityCommon();
private:
	// Do the object's special activity (increase health, spin Ghostracer, etc.)
	virtual void doActivityDifferent(GhostRacer* gr) = 0;
	
	// Return the object's increase to the score when activated.
	virtual int getScoreIncrease() const = 0;

	// Return the sound to be played when the object is activated.
	virtual int getSound() const = 0;

	// Return whether the object dies after activation.
	virtual bool selfDestructs() const;

	virtual bool isSprayable() const = 0;
};

//OIL SLICK
class OilSlick : public GhostRacerActivatedObject
{
public:
	OilSlick(StudentWorld* SW, double x, double y);
	virtual void doSomething();
private:
	virtual void doActivityDifferent(GhostRacer* gr);
	virtual int getScoreIncrease() const;
	virtual int getSound() const;
	virtual bool selfDestructs() const;
	virtual bool isSprayable() const;
};


//HEALING GOODIE
class HealingGoodie : public GhostRacerActivatedObject
{
public:
	HealingGoodie(StudentWorld* SW, double x, double y);
	virtual void doSomething();
private:
	virtual void doActivityDifferent(GhostRacer* gr);
	virtual int getScoreIncrease() const;
	virtual int getSound() const;
	virtual bool isSprayable() const;
};

//HOLY GOODIE
class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
	HolyWaterGoodie(StudentWorld* SW, double x, double y);
	virtual void doSomething();
private:
	virtual void doActivityDifferent(GhostRacer* gr);
	virtual int getScoreIncrease() const;
	virtual int getSound() const;
	virtual bool isSprayable() const;
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
	SoulGoodie(StudentWorld* SW, double x, double y);
	virtual void doSomething();
private:
	virtual void doActivityDifferent(GhostRacer* gr);
	virtual int getScoreIncrease() const;
	virtual int getSound() const;
	virtual bool isSprayable() const;
};

//BORDER LINE
class BorderLine :public Actor {
public:
	//CONSTRUCTOR
	BorderLine(double startX, double startY, StudentWorld* SW, int imageID);

	virtual void doSomething();

	//VIRTUAL is avoidance-worthy (FROM ACTOR CLASS)
	virtual bool isCollisionAvoidanceWorthy() const;

	//VIRTUAL can it be SPRAYED (FROM ACTOR CLASS)
	virtual bool beSprayedIfAppropriate();

private:
};



#endif // ACTOR_H_
