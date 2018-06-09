{
	ifstream list("list.txt");
	double Q;
	double T;
	int Rt;
	int Pw;
	int trigger;
        int board;
        int channel;
	
	TFile*fout=new TFile("ToTal.root","RECREATE");
	TTree*t=new TTree("tot","Total");
	t->Branch("Q",&Q,"Q/D");
	t->Branch("T",&T,"T/D");
	t->Branch("Rt",&Rt,"Rt/I");
	t->Branch("Pw",&Pw,"Pw/I");
        t->Branch("trigger",&trigger,"trigger/I");
        t->Branch("board",&board,"board/I");
        t->Branch("channel",&channel,"channel/I");
	while(list.good())
	{
		int k;	
		list>>k;
		char cmd[100];
		sprintf(cmd,".! ls %d/ana-*-NORM*>name.txt",k);
		std::vector<string> in_data;
		string tmp_name;
		gROOT->ProcessLine(cmd);
		ifstream f("name.txt");
		if(f.is_open())
		{
			while (!f.eof())
			{
				f>>tmp_name;
				in_data.push_back(tmp_name);
			}
		}
		f.close();
		int data_num=in_data.size()-1;
		cout<<"There are: "<<data_num<<" ROOT data files linked!"<<endl;
		std::vector<string>::iterator it = in_data.begin();
		char **data=new char*[data_num];
		for (int i=0;i<data_num; i++)
		{
			string tmp_str=*it;
			int len = tmp_str.length();
			data[i] = (char *)malloc((len+1)*sizeof(char));
			tmp_str.copy(data[i],len,0);
			data[i][len]='\0';
			cout<<data[i]<<"\t"<<sizeof(data[i])<<endl;
			it++;
		}
		gROOT->ProcessLine(".! rm name.txt");
		TChain *bank=new TChain("bank");
		for(int i=0;i<data_num;i++)bank->Add(data[i]);
		bank->SetBranchAddress("Q",&Q);
		bank->SetBranchAddress("T",&T);
		bank->SetBranchAddress("Rt",&Rt);
		bank->SetBranchAddress("Pw",&Pw);
	        bank->SetBranchAddress("trigger",&trigger);
	        bank->SetBranchAddress("board",&board);
        	bank->SetBranchAddress("channel",&channel);

	//bank->Draw("T");


		int loop=bank->GetEntries();
		for(int i=0;i< loop;i++)
		{
			bank->GetEntry(i);
			t->Fill();
		}
	}
	list.close();
	fout->cd();
	t->Write();
	fout->Close();
}
