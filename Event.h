/*******************************************************************************
 * File: WNSEvent.h
 * Description: header file for the event.
 * Comments:
 * Author: GMH(gumh@ihep.ac.cn)
 * Created: 2017/11/06
 *******************************************************************************/

#ifndef __WNSEvent
#define __WNSEvent 1

#include "TObject.h"
#include "TClonesArray.h"
#include "TVectorD.h"
#include <iostream>

#define _IS_DECODE 1

class WNSEvent : public TObject {

    private:
        Int_t ev_n; 
        Int_t adc_n;
        Int_t tof_h_raw;
        Int_t tof_l_raw;
        Double_t tof;
        Int_t channel;
        Int_t board;
        TVectorD *adcs;

    public:
        WNSEvent();
        virtual ~WNSEvent();

        // Set Methods
        void     SetEvN(Int_t s_ev_n)        { ev_n  = s_ev_n  ; }
        void     SetBoard(Int_t s_board)     { board   = s_board; }
        void     SetChannel(Int_t s_channel) { channel = s_channel; }
        void     SetAdcN(Int_t s_adc_n)      {
            adc_n = s_adc_n ; 
            if(adcs != NULL) {
                delete adcs;
                //std::cout << "new wave: " << adc_n << std::endl;
            }
            adcs = new TVectorD(adc_n);
            //adcs->ResizeTo(adc_n);
        }

        void     SetTofHRaw(Int_t v )  { tof_h_raw = v ; }
        void     SetTofLRaw(Int_t v )  { tof_l_raw = v ; }
        void     SetTof(Double_t s_tof)         { tof = s_tof ; }

        // Get Methods
        Int_t    EvN()     const { return ev_n ; } 
        Int_t    Channel() const { return channel; }
        Int_t    Board()   const { return board; }
        TVectorD*Adcs()    const { return adcs ; }
        Int_t    TofHRaw() const { return tof_h_raw  ; }
        Int_t    TofLRaw() const { return tof_l_raw  ; }
        Double_t Tof()     const { return tof  ; }

        void SetWave(UShort_t *ptr, int adcCount);

        ClassDef(WNSEvent,1) 
};

#endif
