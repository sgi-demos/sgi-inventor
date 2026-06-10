/*
    flakeset.h - Defines for Snowflake particle system for a Christmas card

    Tim Heidmann
    December 12, 1993
*/

#ifndef _FLAKESET_H_
#define _FLAKESET_H_

#include <Inventor/SbLinear.h>
#include <swirl.h>

class FlakeSet {
public:
    FlakeSet(int   iFlake = 100,
             float fallMin = 0.001,
	     float fallRange = 0.003);
    ~FlakeSet();
    void draw();
    void swirl(Swirl &s, float interval);
    void fall(float interval);
    void limit();
    void randomize(float f);
private:
    int nFlakes;
    SbVec3f *pos, *randpos, *drawpos;
    float *fallRate;
};

#endif
