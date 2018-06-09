/*******************************************************************************
 * File: WNSEvent.cc
 * Description: C++ source file of the  classes.
 * Comments:
 * Author: GMH(gumh@ihep.ac.cn)
 * Created: 2017/11/06
 *******************************************************************************/

#include <math.h>
#include <TMath.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TDecompSVD.h>
#include "Event.h"

ClassImp(WNSEvent)
WNSEvent::WNSEvent()
    : adcs(NULL)
{}

WNSEvent::~WNSEvent()  {
}

void WNSEvent::SetWave(UShort_t *ptr, int adcCount)
{
    for(int i=0; i<adcCount; i++) {
        UShort_t val = ptr[i];
        UShort_t transVal = (val>>8&0x00ff)|(val<<8&0xff00);
        (*adcs)[i] = transVal;
    }
}
