#include "HighPassFilter.h"


HighPassFilter::HighPassFilter()
{
}


HighPassFilter::~HighPassFilter()
{
}


// Intial the parameters
int HighPassFilter::Initial(highp_t highp)
{
	highp->B1 = exp((-2.0 * TMath::Pi() * (highp->cutoff /* effp->ininfo.rate*/)));
	highp->A0 = (1 + highp->B1) / 2;
	highp->A1 = (-1 * (1 + highp->B1)) / 2;
	highp->inm1 = 0.0;
	highp->outm1 = 0.0;
	return 1;
}


double HighPassFilter::Processing(highp_t highp,double ibuf)
{
	return highp->A0 * ibuf + highp->A1 * highp->inm1 + highp->B1 * highp->outm1;
}


int HighPassFilter::Stop()
{
	return 0;
}
