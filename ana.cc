{
	double length=55;//meter
	double Dt=-5.95E5;//ps
	double c=299792458;//m/s light speed
	double Mn=939.565413;// MeV neutron mass
	TFile*f=new TFile("ana.root");
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
	float energy[3];
	float TOF[3];
	float det[3];
	float dT[2];
	TFile * rootf= new TFile("re_ToTal.root","RECREATE" );
	TTree*ana=new TTree("ana","results of ana");
	ana->Branch("TOF",&TOF,"TOF[3]/F");
	ana->Branch("det",&det,"det[3]/F");
	ana->Branch("energy",&energy,"energy[3]/F");
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
			dT[ii]=0;
		}
		TOF[2]=0;
		det[2]=0;
		if(board!=3)continue;
		det[0]=Q;
		energy[0]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
		TOF[0]=T;
		if(channel==1)old_b=2;
		else old_b=14;
		old_tr=trigger;
		int tmp_count=1;
		while(i-tmp_count>-1)
		{
			bank->GetEntry(i-tmp_count);
			if(board==old_b&&trigger==old_tr)
			{
				if(det[channel]&&(TMath::Abs(dT[channel-1])>TMath::Abs(TOF[channel]-T)))
				{
					dT[channel-1]=TOF[channel]-T;
					det[channel]=Q;
					energy[channel]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[channel]=T;
				}
				else if(!det[channel])
				{
					dT[channel-1]=TOF[channel]-T;
					det[channel]=Q;
					energy[channel]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[channel]=T;
				}
			}
			else if(trigger!=old_tr) break;
			tmp_count++;
		}
		tmp_count=1;
		while(i+tmp_count<total)
		{
			bank->GetEntry(i+tmp_count);
			if(board==old_b&&trigger==old_tr)
			{
				if(det[channel]&&(TMath::Abs(dT[channel-1])>TMath::Abs(TOF[channel]-T)))
				{
					dT[channel-1]=TOF[channel]-T;
					det[channel]=Q;
					energy[channel]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[channel]=T;
				}
				else if(!det[channel])
				{
					dT[channel-1]=TOF[channel]-T;
					det[channel]=Q;
					energy[channel]=Mn*(1/TMath::Sqrt(1-TMath::Power(length/(c*(T-Dt)/1E12),2))-1);
					TOF[channel]=T;
				}
			}
			else if(trigger!=old_tr) break;	
			tmp_count++;
		}
		
		if(det[0]!=0){board=old_b;ana->Fill();
				for(int ii=0;ii<3;ii++)
		{
			cout<<TOF[ii]<<"\t"<<det[ii]<<"\t"<<board<<"\t"<<old_tr<<endl;
		}
		}
	}
	
	rootf->cd();
	//h->Write();
	ana->Write();
	rootf->Close();
	f->Close();
}
