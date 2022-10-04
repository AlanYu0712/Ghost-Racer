#include "Actor.h"
#include "StudentWorld.h"


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//int imageID, double startX, double startY, int dir = 0, double size = 1.0, unsigned int depth = 0

//////////////////////////
////  ACTOR
//////////////////////////


Actor::Actor(StudentWorld* SW, int imageID, double startX, double startY, int dir, double size, unsigned int depth,double vert_V = -4)
	:GraphObject(imageID,startX,startY, dir, size, depth), m_status(true), m_horiV(0)
 {
	m_SW = SW;
	m_vertV = vert_V;
	m_GR = this->getWorld()->getGR();
}

//DESTURCTOR
//Actor::~Actor() {
//	delete m_GR;
//	delete m_SW;
//}

//STATUS
bool Actor::getStatus() const{ return m_status; }
void Actor::setStatus(bool status) { m_status = status; }

//HORIZONTAL SPEED
int Actor::getHoriV() const{ return m_horiV; }
void Actor::setHoriV(double horiV) { m_horiV = horiV; }

//VERTICLE SPEED
double Actor::getVertV() const{ return m_vertV; }
void Actor::setVertV(double vertV) { m_vertV = vertV;}

//STUDENT WORLD	
StudentWorld* Actor::getWorld() const{	return m_SW;}

//GHOST RACER
GhostRacer* Actor::GR() const { return m_GR; }

//MOVE
bool Actor::moveRelativeToGhostRacerVerticalSpeed() {
	//setting movement
	double GR_speed = getWorld()->getGR()->getVertV();
	double vert_speed = this->m_vertV - GR_speed;
	int horiz_speed = this->m_horiV;
	double new_y = this->getY() + vert_speed;
	double new_x = this->getX() + horiz_speed;
	this->moveTo(new_x, new_y);

	//checking availability (Still in the frame?)
	if (this->getX() < 0 || this->getY() < 0 || this->getX() > VIEW_WIDTH || this->getY() > VIEW_HEIGHT) {
		this->setStatus(false);
		return false;
	}

	return true;
}


//////////////////////////
////  AGENT
//////////////////////////
Agent::Agent(int hitPoints, StudentWorld* SW, int imageID, double startX, double startY, int dir, double size, double vert_V = -4)
	:Actor(SW, imageID, startX, startY, dir, size, 0, vert_V) {
	m_hitPoints = hitPoints;
}

//HIT POINTS
int Agent::getHitPoints() const { return m_hitPoints; }
void Agent::decrementHitPoints(int HP) { m_hitPoints -= HP; }
void Agent::increaseHP(int HP) { m_hitPoints += HP; }

//isAvoidance Worthy
bool Agent::isCollisionAvoidanceWorthy() const {
	return true;
}

//DAMAGE
bool Agent::takeDamageAndPossiblyDie(int hp) {
	this->decrementHitPoints(hp);
	if (this->getHitPoints() <= 0) {
		this->setStatus(false);
		this->soundWhenDie();
		takeDamgedifferent();
		return true;
	}
	this->soundWhenHurt();
	return false;
}

void Agent::takeDamgedifferent(){}


//////////////////////////
////  GHOST RACER
//////////////////////////
GhostRacer::GhostRacer(StudentWorld* SW, double startX = 128, double startY = 32)
	: Agent(100, SW, IID_GHOST_RACER, startX, startY, 90, 4.0,0) {
	m_holyQuota = 10;
}

//SOUND
void GhostRacer::soundWhenHurt() const{}
void GhostRacer::soundWhenDie() const{ this->getWorld()->playSound(SOUND_PLAYER_DIE); }

//Holy Spray Quota
int GhostRacer::getQuota() const{ return m_holyQuota; }
void GhostRacer::incrementQuota(int q) { m_holyQuota += q; }

//DOSOMETHING
 void GhostRacer::doSomething() {
	 //1.
	 if (!this->getStatus())
		 return;
	 //2.
	 double LEFT = ROAD_CENTER - (ROAD_WIDTH / 2.0);
	 if (this->getX() <= LEFT) {
		 if (this->getDirection() > 90) {
			 this->takeDamageAndPossiblyDie(10);
		 }
		 this->setDirection(82);
		 this->getWorld()->playSound(SOUND_VEHICLE_CRASH);
	 }
	 //3.
	 double RIGHT = ROAD_CENTER + (ROAD_WIDTH / 2.0);
	 if (this->getX() >= RIGHT) {
		 if (this->getDirection() < 90) {
			 this->takeDamageAndPossiblyDie(10);
		 }
		 this->setDirection(98);
		 this->getWorld()->playSound(SOUND_VEHICLE_CRASH);
	 }
	 //4.
	 int ch;
	 int angle = this->getDirection();
	 double Velocity = this->getVertV();
	 if (this->getWorld()->getKey(ch)) {
		 switch (ch) {
		 case KEY_PRESS_SPACE:
			 if (m_holyQuota > 0) {
				 //i.
				 int theta = this->getDirection()-90;
				 double delta_x = sin(theta) * SPRITE_HEIGHT;
				 double delta_y = cos(theta) * SPRITE_HEIGHT;
				 double new_x = delta_x + this->getX();
				 double new_y = delta_y + this->getY();
				 Actor* spray = new Spray(this->getWorld(), new_x, new_y, this->getDirection());
				 this->getWorld()->addActor(spray);
				 //ii.
				 this->getWorld()->playSound(SOUND_PLAYER_SPRAY);
				 //iii.
				 m_holyQuota--;
			 }
			 
			 break; 
		 case KEY_PRESS_LEFT:
			 if (angle < 114)
				 this->setDirection(angle + 8);
			 break;
		 case KEY_PRESS_RIGHT:
			 if (angle > 66)
				 this->setDirection(angle - 8);
			 break;
		 case KEY_PRESS_UP:
			 if (Velocity < 5)
				 this->setVertV(++Velocity);
			 break;
		 case KEY_PRESS_DOWN:
			 if (Velocity > -1)
				 this->setVertV(--Velocity);
			 break;
		 }
	 }

	 //5.
	 moveAlgo();

}

//Moving Algorithm
void GhostRacer::moveAlgo() {
	double max_shift_per_tick = 4.0;
	double direction = this->getDirection();
	
	double delta_x = cos(direction*((3.1415)/180)) * max_shift_per_tick;
	double cur_x = this->getX();
	double cur_y = this->getY();
	this->moveTo(cur_x + delta_x, cur_y);
}

//SPIN
void GhostRacer::sbinalla() {
	int randDir = randInt(5, 20);
	int randPosNeg = randInt(0, 1);
	if (randPosNeg == 0) {
		if (this->getDirection() - randDir >= 60)
			this->setDirection(this->getDirection() - randDir);
	}
	else if (randPosNeg == 1) {
		if (this->getDirection() + randDir <= 120)
			this->setDirection(this->getDirection() + randDir);
	}
}

//VIRTUAL
//can it be SPRAYED
bool GhostRacer::beSprayedIfAppropriate() {
	return false;
}

void GhostRacer::takeDamgedifferent() {
	this->getWorld()->decLives();
}



//////////////////////////
////  PEDESTRIAN
//////////////////////////
Pedestrian::Pedestrian(StudentWorld* SW, int imageID, double x, double y, double size) 
	:Agent(2,SW,imageID,x,y,0,size)
{
	m_distance = 0;
}

//PLAYING SOUND
void Pedestrian::soundWhenHurt() const {	this->getWorld()->playSound(SOUND_PED_HURT);}
void Pedestrian::soundWhenDie() const {	this->getWorld()->playSound(SOUND_PED_DIE);}

//DISTANCE
int Pedestrian::getDistance() const { return m_distance;}
void Pedestrian::setDistance(int newD) { m_distance = newD; }

//MOVEMENT PLAN
void Pedestrian::movementPlan() {

	//Human6. / Zombie 6.
	int new_HoriSpeed = randInt(1, 3);
	bool new_Direction = randInt(0, 1);
	m_distance = randInt(4, 32);

	//set speed and direction
	if (new_Direction) {
		this->setHoriV(-new_HoriSpeed);
		this->setDirection(180);
	}
	else {
		this->setHoriV(new_HoriSpeed);
		this->setDirection(0);
	}
}


//////////////////////////
////  HUMAN PED
//////////////////////////
HumanPedestrian::HumanPedestrian(StudentWorld* SW, double x, double y)
	:Pedestrian(SW, IID_HUMAN_PED, x, y, 2.0) {}

//DOSOMETHING
void HumanPedestrian::doSomething() {
	//1.
	if (!this->getStatus())
		return;
	//2.
	if (this->getWorld()->overlaps(this, this->GR())) {
		this->getWorld()->decLives();
		this->GR()->setStatus(false);
		//FOR "and the level ends", we will return GWSTATUS_PLAYER_DIED immediately after this is returned 
		return;
	}

	//3.
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	//4.
	int distance = this->getDistance() - 1;
	this->setDistance(distance);
	//5.
	if (distance > 0)
		return;
	//6.
	this->movementPlan();
}

//VIRTUAL
//can it be SPRAYED
bool HumanPedestrian::beSprayedIfAppropriate() {
	this->setHoriV((-1) * this->getHoriV());
	if (this->getDirection() == 0)
		this->setDirection(180);
	else
		this->setDirection(0);
	this->soundWhenHurt();
	
	return true;
}



//////////////////////////
////  ZOMBIE PED
//////////////////////////
ZombiePedestrian::ZombiePedestrian(StudentWorld* SW, double x, double y)
	:Pedestrian(SW, IID_ZOMBIE_PED, x, y, 3.0) {
	m_tick = 0;
}

//DOSOMETHING
void ZombiePedestrian::doSomething() {
	//1.
	if (!this->getStatus())
		return;
	//2.
	if (this->getWorld()->overlaps(this, this->GR())) {
		//a.
		this->GR()->takeDamageAndPossiblyDie(5);
		//b.
		this->takeDamageAndPossiblyDie(2);
		//c.
		return;
	}
	//3.
	double range_X = this->getX() - this->GR()->getX();
	if (abs(range_X) <= 30 && this->getY() > this->GR()->getY()) {
		//a.
		this->setDirection(270);
		//b.
		if (range_X < 0)
			this->setHoriV(1);
		else if (range_X > 0)
			this->setHoriV(-1);
		else
			this->setHoriV(0);
		//c.
		m_tick--;
		//d.
		if (m_tick <= 0) {
			//i.
			this->getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			//ii.
			m_tick = 20;
		}
	}
	//4.
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	//5.
	int distance = this->getDistance();
	if (distance > 0) {
		this->setDistance(distance - 1);
		return;
	}
	//6.
	this->movementPlan();
}

bool ZombiePedestrian::beSprayedIfAppropriate() {
	this->takeDamageAndPossiblyDie(1);
	return true;
}


void ZombiePedestrian::takeDamgedifferent() {
	//Drop Goodie
	if (!this->getWorld()->overlaps(this, this->GR())) {
		int chance = randInt(1, 5);
		if (chance == 1) {
			HealingGoodie* HG = new HealingGoodie(this->getWorld(), this->getX(), this->getY());
			this->getWorld()->addActor(HG);
		}
	}
	//ADD POINT
	this->getWorld()->increaseScore(150);
}

//////////////////////////
////  ZOMBIE CAB
//////////////////////////
ZombieCab::ZombieCab(StudentWorld* SW, double x, double y, double v)
	:Agent(3, SW, IID_ZOMBIE_CAB, x, y, 90, 4.0,v) {
	m_DamagedPlayer = false;
	m_distance = 0;
}

//SOUND
void ZombieCab::soundWhenDie() const { this->getWorld()->playSound(SOUND_VEHICLE_DIE); }
void ZombieCab::soundWhenHurt() const { this->getWorld()->playSound(SOUND_VEHICLE_CRASH); }

//DO SOMETHING
void ZombieCab::doSomething() {
	//1.
	if (!this->getStatus())
		return;
	//2. a.
	if (this->getWorld()->overlaps(this, this->GR()) && !m_DamagedPlayer) {
		//b.
		//c.
		this->getWorld()->playSound(SOUND_VEHICLE_CRASH);
		//d.
		this->GR()->takeDamageAndPossiblyDie(20);
		//e.
		if (this->getX() <= this->GR()->getX()) {
			//i.
			this->setHoriV(-5);
			//ii.
			int random = randInt(0, 19);
			this->setDirection(120 + random);
		}
		//f.
		if (this->getX() >= this->GR()->getX()) {
			//i.
			this->setHoriV(5);
			//ii.
			int random = (0, 19);
			this->setDirection(60 + random);
		}
		//g.
		m_DamagedPlayer = true;
	}
	//3.
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	//4.
	if (this->getVertV() > GR()->getVertV() && this->getWorld()->needChangeSpeed(getX(), getY(), 1)) {
		this->setVertV(this->getVertV() - 0.5);
		return;
	}
	//5.
	if (this->getVertV() < GR()->getVertV() && this->getWorld()->needChangeSpeed(getX(), getY(), -1)) {
		this->setVertV(this->getVertV() + 0.5);
		return;
	}
	//6.
	m_distance--;
	//7.
	if (m_distance > 0)
		return;
	//8.
	//a.
	m_distance = randInt(4, 32);
	//b.
	int randSpeed = randInt(-2, 2);
	this->setVertV(this->getVertV() + randSpeed);
}

//VIRTUAL
//can it be SPRAYED
bool ZombieCab::beSprayedIfAppropriate() {
	this->takeDamageAndPossiblyDie(1);
	return true;
}

//DAMAGE
void ZombieCab::takeDamgedifferent() {
	//Drop Goodie
		int chance = randInt(1, 5);
		if (chance == 1) {
			OilSlick* OS = new OilSlick(this->getWorld(), getX(), getY());
			this->getWorld()->addActor(OS);
		}
	//ADD POINT
	this->getWorld()->increaseScore(200);
}

//////////////////////////
////  HOLY WATER SPRAY
//////////////////////////
Spray::Spray(StudentWorld* SW, double x, double y, int dir)
	:Actor(SW, IID_HOLY_WATER_PROJECTILE, x, y, dir, 1.0, 1) {
	m_pixels = 0;
}

void Spray::doSomething() {
	//1.
	if (!this->getStatus())
		return;
	//2.
	if (this->getWorld()->sprayFirstAppropriateActor(this)) {
		this->setStatus(false);
		return;
	}
	//3.
	this->moveForward(SPRITE_HEIGHT);
	m_pixels += SPRITE_HEIGHT;
	//4.
	if (this->getX() < 0 || this->getY() < 0 || this->getX() > VIEW_WIDTH || this->getY() > VIEW_HEIGHT) {
		this->setStatus(false);
		return;
	}
	//5.
	if (m_pixels >= 160)
		this->setStatus(false);
}

bool Spray::beSprayedIfAppropriate() { return false; }
bool Spray::isCollisionAvoidanceWorthy() const { return false; }



//////////////////////////
////  Goodies & Slicks
//////////////////////////
GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* SW, int imageID, double x, double y, double size, int dir)
	:Actor(SW,imageID,x,y,dir,size,2){}

void GhostRacerActivatedObject::doActivityCommon() {
	//ALL 7.
	if (this->getWorld()->overlaps(this, this->GR())) {
		//all a.
		doActivityDifferent(GR());
		//exclude oil/ b.
		if (selfDestructs())
			this->setStatus(false);
		//oil b. / c.
		this->getWorld()->playSound(getSound());
		//exlude oil/ d.
		this->getWorld()->increaseScore(getScoreIncrease());
		
	}
}

bool GhostRacerActivatedObject::selfDestructs() const{ return true; }

//VIRTUAL
bool GhostRacerActivatedObject::beSprayedIfAppropriate() {
	if (isSprayable()) {
		this->setStatus(false);
		return true;
	}
	else
		return false;
}

bool GhostRacerActivatedObject::isCollisionAvoidanceWorthy() const { return false; }



//////////////////////////
////  OIL SLICK
//////////////////////////
OilSlick::OilSlick(StudentWorld* SW, double x, double y)
	:GhostRacerActivatedObject(SW, IID_OIL_SLICK,x,y,randInt(2,5)*1.0,0){}

void OilSlick::doSomething() {
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	this->doActivityCommon();
}

void OilSlick::doActivityDifferent(GhostRacer* gr) {
	gr->sbinalla();
}

int OilSlick::getScoreIncrease() const{ return 0; }
int OilSlick::getSound() const { return SOUND_OIL_SLICK; }
bool OilSlick::selfDestructs() const{ return false; }
bool OilSlick::isSprayable() const { return false; }


//////////////////////////
////  HEALING GOODIE
//////////////////////////
HealingGoodie::HealingGoodie(StudentWorld* SW, double x, double y)
	:GhostRacerActivatedObject(SW, IID_HEAL_GOODIE,x,y,1.0,0){}

void HealingGoodie::doSomething() {
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	this->doActivityCommon();
}

void HealingGoodie::doActivityDifferent(GhostRacer* gr) {
	if (gr->getHitPoints() + 10 <= 100)
		gr->increaseHP(10);
}

int HealingGoodie::getScoreIncrease() const { return 250;}
int HealingGoodie::getSound() const { return SOUND_GOT_GOODIE; }
bool HealingGoodie::isSprayable() const{ return true; }


//////////////////////////
////  HOLY GOODIE
//////////////////////////
HolyWaterGoodie::HolyWaterGoodie(StudentWorld* SW, double x, double y)
	:GhostRacerActivatedObject(SW,IID_HOLY_WATER_GOODIE,x,y,2.0,90){}

void HolyWaterGoodie::doSomething() {
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	this->doActivityCommon();
}

void HolyWaterGoodie::doActivityDifferent(GhostRacer* gr) {
	gr->incrementQuota(10);
}

int HolyWaterGoodie::getScoreIncrease() const { return 50; }
int HolyWaterGoodie::getSound() const { return SOUND_GOT_GOODIE;}

bool HolyWaterGoodie::isSprayable() const { return true; }


//////////////////////////
////  SOUL GOODIE
//////////////////////////
SoulGoodie::SoulGoodie(StudentWorld* SW, double x, double y)
	:GhostRacerActivatedObject(SW, IID_SOUL_GOODIE, x, y, 4.0, 0){}

void SoulGoodie::doSomething() {
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
	this->doActivityCommon();
	this->setDirection(this->getDirection() - 10);
}

void SoulGoodie::doActivityDifferent(GhostRacer* gr) {
	this->getWorld()->recordSoulSaved();
}

int SoulGoodie::getScoreIncrease() const { return 100; }
int SoulGoodie::getSound() const { return SOUND_GOT_SOUL; }
bool SoulGoodie::isSprayable() const{ return false; }




//////////////////////////
////  BORDER LINE
//////////////////////////
BorderLine::BorderLine(double startX, double startY, StudentWorld* SW, int imageID)
	:Actor(SW,imageID, startX, startY, 0,2.0,2) {}

//DOSOMETHING
void BorderLine::doSomething(){
	if (!this->moveRelativeToGhostRacerVerticalSpeed())
		return;
}

//Is it avoidance-worthy
bool BorderLine::isCollisionAvoidanceWorthy() const {
	return false;
}

//can it be SPRAYED
bool BorderLine::beSprayedIfAppropriate() {
	return false;
}