{
	double length=55;//meter
	double Dt=0;//ps
	double c=299792458;//m/s light speed
	double Mn=939.565413;// MeV neutron mass
	TFile*f=new TFile("ana-11033-NORM-07.root");
	double Q;
	double T;
	int Rt;
	int Pw;
	int trigger;	
	int board;
	int channel;
	TTree*bank=(TTree*)f->Get("bank");
	bank->SetBranchAddress("Q",&Q);
	bank->SetBranchAddress("T",&T);
        bank->SetBranchAddress("Rt",&Rt);
	bank->SetBranchAddress("Pw",&Pw);
	bank->SetBranchAddress("board",&board);
	bank->SetBranchAddress("channel",&channel);
	bank->SetBranchAddress("trigger",&trigger);
	int total=bank->GetEntries();
	float energy[2];
	float det[2];
	TFile * rootf= new TFile("results.root","RECREATE" );
	TTree*ana=new TTree("ana","results of ana");
	ana->Branch("energy",&energy,"energy[2]/F");
	ana->Branch("det",&det,"det[2]/F");
	ana->Branch("board",&board,"board[2]/F");
	int miniloop=0;
	int old_ch=0;
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
		if(old_ch==channel||(!miniloop))
		{
			if(i)ana->Fill();
			for(int ii=0;ii<2;ii++)
			{
				energy[ii]=0;
				det[ii]=0;
			}

			miniloop=2;
		}
		energy[channel-1]=Mn*(1/TMath::Sqrt(1-TMath::Sq(length/(c*(T-Dt)/1E12)))-1);
		det[channel-1]=Q;
		old_ch=channel;
		miniloop--;
	}
	
	rootf->cd();
	//h->Write();
	ana->Write();
	rootf->Close();
	f->Close();
}
