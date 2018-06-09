#pragma once
#include "TMath.h"
/* Private data for Highpass effect */
typedef struct highpstuff {
	float   cutoff;
	double  A0, A1, B1;
	double  inm1, outm1;
} *highp_t;

class HighPassFilter
{
public:
	HighPassFilter();
	~HighPassFilter();
	// Intial the parameters
	int Initial(highp_t highp);
	double Processing(highp_t highp,double ibuf);
	int Stop();
};

