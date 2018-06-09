#pragma once
#include "TMath.h"
/* Private data for Lowpass effect */
typedef struct lowpstuff {
	float   cutoff;
	double  A, B;
	double  outm1;
} *lowp_t;

class LowPassFilter
{
public:
	LowPassFilter();
	~LowPassFilter();


	// initial the parameters
	int Initial(lowp_t lowp);
	// processing data
	double Processing(lowp_t lowp, double ibuf);
	int Stop();
};

