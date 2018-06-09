/*******************************************************************************
 * File: decode_without_daq.cc
 * Description: Parse data raw data file and save to root.
 * Comments:
 * Author: GMH(gumh@ihep.ac.cn)
 * Created: 2017/11/06
 *******************************************************************************/

#include "Event.h"
#include "TFile.h"
#include "TTree.h"
#include "DataDef.h"
#include <TH1F.h>
#include <fstream>
#include <iomanip>

using namespace std;

void makeTimeString(char *buf, time_t sec, uint64_t ns) 
{
    time_t t = sec;
    strftime(buf, 40, "%Y-%m-%d %H:%M:%S.", localtime(&t));
    double subSec = ns/1e9;
    if(subSec > 1) {
        return;
    }
    sprintf(buf+strlen(buf), "%08lu", (uint64_t)(subSec*1e8));
}

int main(int argc, char **argv)
{
    TFile *f = TFile::Open(argv[2],"RECREATE");
    TTree *t1 = new TTree("wns", "wns event");
    ifstream *raw = new ifstream(argv[1]);
    WNSEvent *ev = new WNSEvent;
    t1->Branch("ev", "ev", ev);

    TH1F *h1 = new TH1F("h1", "h1", 400000, -40e6, 40e6);

    file_head_t fileHead;
    raw->read((char*)&fileHead, sizeof(fileHead));
    std::cout << "data version: 0x" << std::hex << fileHead.version << std::dec << std::endl;

    FullFragment ff(20*1024*1024);
    double lastTime = -1;
    double minTof = 1e100;
    while(raw->good()) {
        FullFragment::head_t *fh = ff.head();
        raw->read((char*)fh, sizeof(FullFragment::head_t));
        raw->read((char*)ff.firstFragment(), fh->size-sizeof(FullFragment::head_t));
        if(!raw->good()) {
            break;
        }

        int64_t dt = 0;
        {
            // 1. get TCM time
            uint8_t *ptr = ff.firstFragment();
            for(uint32_t i=0; i<ff.numberOfSubFragments(); i++) {
                FEECommonFragment feeF(ptr);
                if(feeF.packType() == Fragment::FDM) {
                }
                else if(feeF.packType() == Fragment::TCM) {
                    TCMFragment tcm(ptr);
                    dt = tcm.fixTof();
                }
                ptr += feeF.size();
            }
        }
        //std::cout <<  "find TCM, fixTOF: " << dt << std::endl;

        {
            // 2. fill FEM 
            uint8_t *ptr = ff.firstFragment();
            for(uint32_t i=0; i<ff.numberOfSubFragments(); i++) {
                FEECommonFragment feeF(ptr);
                if(feeF.packType() == Fragment::FDM) {
                    //std::cout << "FDM" << std::endl;
                    FDMFragment fdm(ptr);
                    ev->SetTofHRaw(fdm.tofH());
                    ev->SetTofLRaw(fdm.tofL());
                    ev->SetTof(fdm.tof() + dt);
                    if(ev->Tof() < minTof) {
                        minTof = ev->Tof();
                    }
                    //if(ev->Tof()/1000 < -10) {
                    //    std::cout << ev->Tof()/1000 << std::endl;
                    //}
                    h1->Fill(ev->Tof()/1000);
		    ev->SetEvN(fdm.t0id());
		    ev->SetAdcN(fdm.adcCount());
                    ev->SetBoard(fdm.boardId());
                    ev->SetChannel(fdm.channel());
                    ev->SetWave((uint16_t*)fdm.adcDataPtr(), fdm.adcCount());
                    t1->Fill();
                }
                else if(feeF.packType() == Fragment::TCM) {
                    //std::cout << "TCM" << std::endl;
                    TCMFragment tcm(ptr);
                    double t = (double)tcm.t0sec() + tcm.t0ns()/1e9;
                    if(lastTime > 0) {
                        double evdt = t - lastTime;
                        //std::cout << "DT: " << std::setprecision(10) << t << " " << evdt << std::endl;
                    }
                    lastTime = t;
                    char timeStr[256];
                    makeTimeString(timeStr, tcm.t0sec(), tcm.t0ns());
                    //std::cout << timeStr << std::endl;
                }
                ptr += feeF.size();
                //ev->SetAdcN(100);
            }
        }
    }

    std::cout << "min TOF is: " << minTof << std::endl;

    h1->Write();
    t1->Write();
    f->Write();
    f->Close();

    return 0;
}
