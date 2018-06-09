#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "Event.h"
#include "DataProcessing.h"
int main(int argc, char **argv)
{
	if(argc<2)return 0;
	
	TFile *tf = new TFile(argv[1]);
	if(!tf->IsOpen()) exit(0);
	TTree *tree = (TTree*)tf->Get("wns");
	WNSEvent *ev = new WNSEvent;
	tree->SetBranchAddress("ev", &ev);
//	int ch_E=atoi(argv[2]);
	int board=0;
	int channel=0;
	int loop=tree->GetEntries();
	double Q;
	double T;
	int Rt;
	int peak_num;
	int Pw;
	int trigger;
	int draw_flag=0;
	char*pch=strstr(argv[1],"daq");
        strncpy(pch,"ana",3);
	TFile*output= new TFile(argv[1],"RECREATE");
	TTree*t=new TTree("bank","dE-E data bank");
	t->Branch("Q",&Q,"Q/D");
	t->Branch("T",&T,"T/D");
        t->Branch("Rt",&Rt,"Rt/I");
	t->Branch("Pw",&Pw,"Pw/I");
	t->Branch("board",&board,"board/I");
	t->Branch("channel",&channel,"channel/I");
	t->Branch("trigger",&trigger,"trigger/I");

	DataProcessing*datapro=new DataProcessing();
	for(int i=0;i<loop;i++)
	{
		tree->GetEntry(i);
		board=ev->Board();
		channel=ev->Channel();
		trigger=ev->EvN();

		if ((board!=2)&&(board!=14)&&(board!=17))continue;
		//if(channel!=1)continue;
		//CR after Smooth
		if(channel==1)
		{
			datapro->SetSmoothTimes(30);
			datapro->SwithD(ev->Adcs(),ev->Tof());
		}
		if(channel==2)
		{
			datapro->SetSmoothTimes(30);
			datapro->SwithDI(ev->Adcs(),ev->Tof());
		}
		//CR Method
		//datapro->CRFilter(ev->Adcs(),ev->Tof());
		//Smooth Method
		//datapro->SetSmoothTimes(30);
		//datapro->DoSmooth(ev->Adcs(),ev->Tof());
		//Nothing to do 
		//datapro->DoNothing(ev->Adcs(),ev->Tof());
		peak_num=datapro->GetPeakNum();
		if(draw_flag)
		{
			datapro->DrawWave();
			draw_flag--;
		}
		//else break;
		for(int ii=0;ii<peak_num;ii++)
		{
			Q=datapro->GetQ(ii);
			T=datapro->GetT(ii);
			Rt=datapro->GetRt(ii);
			Pw=datapro->GetPw(ii);
			t->Fill();
		}
	}
	output->cd();
	t->Write();
	output->Close();
	tf->Close();
	delete datapro;
	return 1;
}
