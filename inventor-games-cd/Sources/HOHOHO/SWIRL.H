/*
    swirl.h - Defines for swirling fields.

    Tim Heidmann
    December 13, 1993
*/

#ifndef _SWIRL_H_
#define _SWIRL_H_

#include <Inventor/SbLinear.h>

#define MAXSWIRLARMS 5
class Swirl {
public:
    Swirl() : pivot(0.0,0.0,0.0), nArms(0) {};
    void setPivot(SbVec3f &o) {pivot = o;};
    void addArm(int theAxis, float theRadius,
	float theHi_speed, float theNorm_speed,
	float theHi_decay, float theNorm_decay,
	float theHold_time);
    void decay(float interval);
    void attack(float interval, float value);
    void getValues(SbVec3f &c, int &a, float &r, float &s) {
	c = center;
	a = arm[nArms-1].axis;
	r = arm[nArms-1].radius;
	s = arm[nArms-1].sign ? arm[nArms-1].speed : -arm[nArms-1].speed;
    };

private:
    enum armPhase {STILL, ATTACK, HOLD, DECAY};

    SbVec3f pivot, center;
    float play_time;
    class SwirlArm {
    public:
	enum armPhase phase;
	int axis, sign;
	float radius, theta;
	float speed, hi_speed, norm_speed, hi_decay, norm_decay, hold_time;
    } arm[MAXSWIRLARMS];
    int nArms;
};

#endif
