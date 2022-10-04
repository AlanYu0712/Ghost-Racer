#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

#include <iostream>
#include <sstream>

using namespace std;

#include "Actor.h"



GameWorld* createStudentWorld(string assetPath)
{
    
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_bonus = 0;
    m_souls = 0;
    LastBorderLine = 0;
    m_GR = nullptr;

    
}

//DESTRUCTOR
StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{

    //INITIATING GHOST RIDAAAA
    m_GR = new GhostRacer(this,128,32);

    //LOCAL CONSTANTS
    double LEFT_EDGE = ROAD_CENTER - (ROAD_WIDTH / 2.0);
    double RIGHT_EDGE = ROAD_CENTER + (ROAD_WIDTH / 2.0);

    //YELLOW BORDER LINE
    int N = VIEW_HEIGHT / SPRITE_HEIGHT;
    for (int i = 0; i < N; ++i) {
        double Y = i * SPRITE_HEIGHT* 1.0;
        this->m_Actors.push_back(new BorderLine(LEFT_EDGE, Y, this, IID_YELLOW_BORDER_LINE));
        this->m_Actors.push_back(new BorderLine(RIGHT_EDGE, Y, this, IID_YELLOW_BORDER_LINE));

    }

    //WHITE BORDER LINE
    int M = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    for (int i = 0; i < M; ++i) {
        double Y = i * SPRITE_HEIGHT * 4.0;
        this->m_Actors.push_back(new BorderLine(LEFT_EDGE + (ROAD_WIDTH / 3), Y, this, IID_WHITE_BORDER_LINE));
        this->m_Actors.push_back(new BorderLine(RIGHT_EDGE - (ROAD_WIDTH / 3), Y, this, IID_WHITE_BORDER_LINE));
    }

    //data usage initialize
    LastBorderLine = 224;
    m_bonus = 5000;
    m_souls = 0;

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //bonus change
    --m_bonus;
    //data initalize
    int L = this->getLevel();
    int Souls2Save = (2 * L) + 5;
    //MOVING
    //GHOST RACER
    m_GR->doSomething();
    if (m_GR->getHitPoints() <= 0 || !m_GR->getStatus())
        return GWSTATUS_PLAYER_DIED;
    if (Souls2Save - m_souls <= 0) {
        this->increaseScore(m_bonus);
        return GWSTATUS_FINISHED_LEVEL;
    }

    //OTHER ACTORS
    for (int i = 0; i < m_Actors.size(); ++i) {

        if (m_Actors[i]->getStatus()) {
            m_Actors[i]->doSomething();
            if (m_GR->getHitPoints() <= 0||!m_GR->getStatus())
                return GWSTATUS_PLAYER_DIED;
            if (Souls2Save - m_souls <= 0) {
                this->increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    //REMOVE 
    for (int i = 0; i < m_Actors.size(); ++i) {
        if (!m_Actors[i]->getStatus()) {
            delete m_Actors[i];
            m_Actors.erase(m_Actors.begin() + i);
        }
    }

    LastBorderLine += (-4.0 - this->getGR()->getVertV());


    //ADDING    
    //BORDERLINE
    double new_border_y = VIEW_HEIGHT - (SPRITE_HEIGHT + 0.0);
    double delta_y = new_border_y - LastBorderLine;
    if (delta_y >= SPRITE_HEIGHT)
        NewYellowLines(new_border_y);
    if (delta_y >= 4.0 * SPRITE_HEIGHT)
        NewWhiteLines(new_border_y);

    //HUMAN PED
    int ChanceHumanPed = max(200 - L * 10, 30);
    if (randInt(0, ChanceHumanPed - 1) == 0) {
        int randX = randInt(0, VIEW_WIDTH);
        m_Actors.push_back(new HumanPedestrian(this, randX * 1.0, VIEW_HEIGHT*1.0));
    }

    //ZOMBIE PED
    int ChanceZombiePed = max(100 - L * 10, 20);
    if (randInt(0, ChanceZombiePed - 1) == 0) {
        int randX = randInt(0, VIEW_WIDTH);
        m_Actors.push_back(new ZombiePedestrian(this, randX * 1.0, VIEW_HEIGHT * 1.0));
    }

    //ZOMBIE CAB
    int ChanceVehicle = max(100 - L * 10, 20);
    if (randInt(0, ChanceVehicle - 1) == 0) {
        this->NewZombieCab();
    }

    //OIL SLICKS
    int ChanceOilSlick = max(150 - L * 10, 40);
    if (randInt(0, ChanceOilSlick - 1) == 0) {
        int randX = randInt(ROAD_CENTER - (ROAD_WIDTH / 2), ROAD_CENTER + (ROAD_WIDTH / 2));
        m_Actors.push_back(new OilSlick(this, randX * 1.0, VIEW_HEIGHT * 1.0));
    }

    //HOLY GOODIE
    int ChanceOfHolyWater = 100 + 10 * L;
    if (randInt(0, ChanceOfHolyWater - 1) == 0) {
        int randX = randInt(ROAD_CENTER - (ROAD_WIDTH / 2), ROAD_CENTER + (ROAD_WIDTH / 2));
        m_Actors.push_back(new HolyWaterGoodie(this, randX * 1.0, VIEW_HEIGHT * 1.0));
    }

    //SOULE
    int ChanceOfLostSoul = 100;
    if (randInt(0, ChanceOfLostSoul - 1) == 0) {
        int randX = randInt(ROAD_CENTER - (ROAD_WIDTH / 2), ROAD_CENTER + (ROAD_WIDTH / 2));
        m_Actors.push_back(new SoulGoodie(this, randX * 1.0, VIEW_HEIGHT * 1.0));
    }
    


    int Score = getScore();
    int Lives = getLives();
    int Health = getGR()->getHitPoints();
    int Sprays = getGR()->getQuota();
    int SoulsLeft = Souls2Save-m_souls;
    
    ostringstream oss;
    oss << "Score: " << Score << "  Lvl: " << L << "  Souls2Save: " << SoulsLeft << "  Lives: " << Lives << "  Health: " << Health << "  Sprays: " << Sprays << "  Bonus: " << m_bonus;
    this->setGameStatText(oss.str());
    /*return GWSTATUS_PLAYER_DIED;*/
    return GWSTATUS_CONTINUE_GAME;

    
}

void StudentWorld::cleanUp()
{
    delete m_GR;
    for (int i = 0; i < m_Actors.size(); ++i) {      
        delete m_Actors[i];
    }
    m_Actors.erase(m_Actors.begin(), m_Actors.end());

}

//HELPERS
bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const {
    //1.
    double delta_x = abs(a1->getX() - a2->getX());
    //2.
    double delta_y = abs(a1->getY() - a2->getY());
    //3.
    double radius_sum = a1->getRadius() + a2->getRadius();
    //4.
    if (delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6)
        return true;
    return false;

}

void StudentWorld::addActor(Actor* a) {
    m_Actors.push_back(a);
}

bool StudentWorld::sprayFirstAppropriateActor(Actor* a) {
    for (int i = 0; i < m_Actors.size(); ++i) {
        if (overlaps(a,m_Actors[i]) && m_Actors[i]->beSprayedIfAppropriate())
            return true;
    }

    return false;
}

void StudentWorld::recordSoulSaved() { m_souls++; }

bool StudentWorld::isInLane(int i, double x) const{
    int LEFT = ROAD_CENTER - (ROAD_WIDTH / 2);
    int LANE_WIDTH = (ROAD_WIDTH / 3);
    int LEFT_SIDE = LEFT + (LANE_WIDTH * (i - 1));
    int RIGHT_SIDE = LEFT + (LANE_WIDTH * i);
    if (x >= LEFT_SIDE && x < RIGHT_SIDE)
        return true;
    return false;
}

bool StudentWorld::needChangeSpeed(double x, double y, int i) {
    //DETERMINE WHICH LANE zombie cab is at
    int K = 0;
    for (int i = 1; i < 4; ++i) {
        if (isInLane(i, x))
            K = i;
    }
    //zombie cab is not in lane
    if (K == 0)
        return false;
    //check ghost racer
    if (isInLane(K, m_GR->getX()) && abs(m_GR->getY() - y) < 96) {
        if (i == -1 && m_GR->getY() < y)
            return true;
        if (i == 1 && m_GR->getY() > y)
            return true;
    }

    for (int i = 0; i < m_Actors.size(); ++i) {
        //check collision worthy
        if (m_Actors[i]->isCollisionAvoidanceWorthy()) {
            double Actor_X = m_Actors[i]->getX();
            double Actor_Y = m_Actors[i]->getY();
            if (isInLane(K, Actor_X) && abs(Actor_Y - y) < 96) {
                if (i == -1 && Actor_Y < y)
                    return true;
                if (i == 1 && Actor_Y > y)
                    return true;
            }
        }
    }

    return false;

}

//SELF
GhostRacer* StudentWorld::getGR() {
    return m_GR;
}

void StudentWorld::NewYellowLines(double Y) {
    double Xl = (ROAD_CENTER + 0.0) - (ROAD_WIDTH / 2.0);
    double Xr = ROAD_CENTER + (ROAD_WIDTH / 2.0);
    m_Actors.push_back(new BorderLine(Xr, Y, this, IID_YELLOW_BORDER_LINE));
    m_Actors.push_back(new BorderLine(Xl, Y, this, IID_YELLOW_BORDER_LINE));
}

void StudentWorld::NewWhiteLines(double Y) {
    double Xl = ROAD_CENTER - (ROAD_WIDTH / 2.0) + (ROAD_WIDTH / 3.0);
    double Xr = ROAD_CENTER + (ROAD_WIDTH / 2.0) - (ROAD_WIDTH / 3.0);
    m_Actors.push_back(new BorderLine(Xr, Y, this, IID_WHITE_BORDER_LINE));
    m_Actors.push_back(new BorderLine(Xl, Y, this, IID_WHITE_BORDER_LINE));
    LastBorderLine = 248;
}

void StudentWorld::NewZombieCab() {
    //1.
    double startY = 0;
    double startSpeed = 0;
    int chosen_lane = 0;
    int cur_lane = randInt(1, 3);
    //2.
    for (int k = 0; k < 4; ++k) {
        int index = -2;
        double toBottom = VIEW_HEIGHT*1.0;
        //I ADDED THIS for Ghost Racer
        if (isInLane(cur_lane%4, m_GR->getX())) {
            toBottom = m_GR->getY();
            index = -1;
        }
        //a.
        for (int i = 0; i < m_Actors.size(); ++i) {
            if (m_Actors[i]->isCollisionAvoidanceWorthy() && isInLane(cur_lane%4, m_Actors[i]->getX())) {
                if (m_Actors[i]->getY() < toBottom) {
                    toBottom = m_Actors[i]->getY();
                    index = i;
                }
            }
        }
        //b.
        if (index == -2 || toBottom > VIEW_HEIGHT / 3) {
            //i.
            chosen_lane = cur_lane%4;
            //ii.
            startY = SPRITE_HEIGHT / 2.0;
            //iii.
            startSpeed = randInt(2, 4) + getGR()->getVertV();
            break;
        }

        //c.
        index = -2;
        double toTop = 0.0;
        //I ADDED THIS for Ghost Racer
        if (isInLane(cur_lane % 4, m_GR->getX())) {
            toTop = m_GR->getY();
            index = -1;
        }
        for (int i = 0; i < m_Actors.size(); ++i) {
            if (m_Actors[i]->isCollisionAvoidanceWorthy() && isInLane(cur_lane % 4, m_Actors[i]->getX())) {
                if (m_Actors[i]->getY() > toTop) {
                    toTop = m_Actors[i]->getY();
                    index = i;
                }
            }
        }

        //d.
        if (index == -2 || toTop < (VIEW_HEIGHT * 2 / 3)) {
            //i.
            chosen_lane = cur_lane;
            //ii
            startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2.0;
            //iii.
            startSpeed =  getGR()->getVertV() - randInt(2, 4);
            break;
        }

        ++cur_lane;
    }
    

    //3.
    if (chosen_lane == 0)
        return;
    //4-5.
    //DEFAULT AT MIDDLE
    double startX = ROAD_CENTER;
    if (chosen_lane == 1) //LEFT LANE
        startX = ROAD_CENTER - (ROAD_WIDTH / 3.0);
    else if (chosen_lane == 3) //RIGHT LANE
        startX = ROAD_CENTER + (ROAD_CENTER / 3.0);

    m_Actors.push_back(new ZombieCab(this, startX, startY, startSpeed));
}

