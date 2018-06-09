#include "LowPassFilter.h"


LowPassFilter::LowPassFilter()
{
}


LowPassFilter::~LowPassFilter()
{
}


// initial the parameters
int LowPassFilter::Initial(lowp_t lowp)
{
	lowp->B = exp((-2.0 * TMath::Pi() * (lowp->cutoff /* effp->ininfo.rate*/)));
	lowp->A = 1 - lowp->B;
	lowp->outm1 = 0.0;
	return 1;
}


// processing data
double LowPassFilter::Processing(lowp_t lowp, double ibuf)
{
	return lowp->A * ibuf + lowp->B * lowp->outm1;
}


int LowPassFilter::Stop()
{
	return 0;
}
