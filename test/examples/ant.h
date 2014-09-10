/*
 * File:   ant.cpp
 * Author: Joshua Rubini
 *
 * Created on October 25, 2011, 10:38 AM
 */

class ant {
public:
    ant();
    int GetX();
    int GetY();
    int GetEnergy();
    void ChangeEnergy(int);
    char Print();
    bool Left();
    bool Right();
    bool Forward();
private:
    int x;
    int y;
    char dir;
    int energy;
};
