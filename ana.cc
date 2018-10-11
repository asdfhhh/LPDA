{
	double length=55;//meter
	double Dt=-5.95E5;//ps
	double c=299792458;//m/s light speed
	double Mn=939.565413;// MeV neutron mass
	TFile*f=new TFile("ToTal.root");
	double Q;
	double T;
	int Rt;
	int Pw;
	int trigger;	
	int board;
	int channel;
	TTree*bank=(TTree*)f->Get("tot");
	bank->SetBranchAddress("Q",&Q);
	bank->SetBranchAddress("T",&T);
	bank->SetBranchAddress("Rt",&Rt);
	bank->SetBranchAddress("Pw",&Pw);
	bank->SetBranchAddress("board",&board);
	bank->SetBranchAddress("channel",&channel);
	bank->SetBranchAddress("trigger",&trigger);
	int total=bank->GetEntries();
	float energy[2];
	float TOF[2];
	float det[2];
	float dT;
	TFile * rootf= new TFile("re_ToTal.root","RECREATE" );
	TTree*ana=new TTree("ana","results of ana");
	ana->Branch("TOF",&TOF,"TOF[2]/F");
	ana->Branch("det",&det,"det[2]/F");
	ana->Branch("energy",&energy,"energy[2]/F");
	ana->Branch("dT",&dT,"dT/F");
	ana->Branch("board",&board,"board/I");
	int old_b=0;
	int old_tr=0;
	/*int nbin=10;//NBINS per decade
   	Double_t Lower=1E-7;
   	Double_t Higher=10;
   	Int_t NBINS = TMath::Log10(Higher/Lower)*nbin;
   	Double_t *edges=new Double_t[NBINS + 1];
	Double_t base=TMath::Power(10,double(1)/nbin);
	for(int i=0;i<NBINS + 1;i++)edges[i]=Lower*TMath::Power(base,i);
	TH1* h = new TH1D("spectrum","spectrum of neutron Energy",NBINS,edges
	);*/

	for(int i=0;i<total;i++)
	{
		bank->GetEntry(i);
		for(int ii=0;ii<2;ii++)
		{
			TOF[ii]=0;
			det[ii]=0;
			dT=0;
		}
		if(channel!=2)continue;
		det[0]=Q;
		energy[0]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
		TOF[0]=T;
		old_b=board;
		old_tr=trigger;
		int tmp_count=1;
		while(i-tmp_count>-1)
		{
			bank->GetEntry(i-tmp_count);
			if(board==old_b&&trigger==old_tr&&channel==1)
			{
				if(det[1]&&(TMath::Abs(dT)>TMath::Abs(TOF[0]-T)))
				{
					dT=TOF[0]-T;
					det[1]=Q;
					energy[1]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[1]=T;
				}
				else if(!det[1])
				{
					dT=TOF[0]-T;
					det[1]=Q;
					energy[1]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[1]=T;
				}
			}
			else if(board!=old_b||trigger!=old_tr) break;
			tmp_count++;
		}
		tmp_count=1;
		while(i+tmp_count<total)
		{
			bank->GetEntry(i+tmp_count);
			if(board==old_b&&trigger==old_tr&&channel==1)
			{
				if(det[1]&&(TMath::Abs(dT)>TMath::Abs(TOF[0]-T)))
				{
					dT=TOF[0]-T;
					det[1]=Q;
					energy[1]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[1]=T;
				}
				else if(!det[1])
				{
					dT=TOF[0]-T;
					det[1]=Q;
					energy[1]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[1]=T;
				}
			}
			else if(board!=old_b||trigger!=old_tr) break;	
			tmp_count++;
		}
		
		if(det[1]!=0){board=old_b;ana->Fill();}
	}
	
	rootf->cd();
	//h->Write();
	ana->Write();
	rootf->Close();
	f->Close();
}
