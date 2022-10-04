#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
#include<vector>


class Actor;
class GhostRacer;
class BorderLine;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getGR();
    void addActor(Actor* a);
    bool sprayFirstAppropriateActor(Actor* a);
    void recordSoulSaved();
    //check if an objects, center is in lane (i be 1-3)
    bool isInLane(int i, double x)const;
    //check whether zombie cab need to change speed (i be +-1 to determine front or behind)
    bool needChangeSpeed(double x, double y, int i);


    // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor* a1, const Actor* a2) const;

private:
    //FUNC
    void NewYellowLines(double Y);
    void NewWhiteLines(double Y);
    void NewZombieCab();

    //DATA
    GhostRacer* m_GR;
    std::vector<Actor*> m_Actors;
    double LastBorderLine;
    int m_souls;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
