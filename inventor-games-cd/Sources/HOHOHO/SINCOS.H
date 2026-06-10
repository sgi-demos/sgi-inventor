/*
    sincos.h - Defines for Quick sin/cosine

    Tim Heidmann
    December 15, 1993
*/

#ifndef _SINCOS_H_
#define _SINCOS_H_

class SinCos {
public:
    SinCos(int ii = 100);
    ~SinCos();
    void eval(float, float &, float &);
private:
    float *s;
    const int n;
};

extern SinCos sincos;

// Quick approximation to Euclidian distance
#define FASTDIST(d,a,b,c) {                   \
    float x=fabs((float)(a)),                 \
	  y=fabs((float)(b)),                 \
	  z=fabs((float)(c));                 \
    if ((x) > (y))                            \
	if ((x) > (z))                        \
	    (d) = (x) + 0.25 * ((y) + (z));   \
	else                                  \
	    (d) = (z) + 0.25 * ((x) + (y));   \
    else                                      \
	if ((y) > (z))                        \
	    (d) = (y) + 0.25 * ((x) + (z));   \
	else                                  \
	    (d) = (z) + 0.25 * ((x) + (y));   \
}

#endif
