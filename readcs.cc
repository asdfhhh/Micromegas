{
	////////////////////////////////////////////////////////////
	//data reading
	////////////////////////////////////////////////////////////
	struct CROSSSECTION
	{
		string element;
		double Nenergy;
		double sig;
	};
	ifstream B10("B10.txt");
	string buffer;
	char head[100];
	vector<CROSSSECTION> ENFdata;
	CROSSSECTION tmpdata;
	for(int i=0;i<6;i++)B10.getline(head,100);
	while(1)
	{

		B10>>buffer;
		//cout<<buffer<<"\t";
		if(buffer=="#END")break;
		tmpdata.Nenergy=std::stof(buffer);
		//cout<<tmpdata.Nenergy<<"\t";
		B10>>buffer;
		//cout<<buffer<<"\t";
		tmpdata.sig=std::stof(buffer);
		//cout<<tmpdata.sig<<"\t";
		tmpdata.element="B10";
		ENFdata.push_back(tmpdata);
		B10>>buffer;
		//cout<<endl;				
	}
	int B10_num=ENFdata.size();
	cout<<"collection B10 contains "<<B10_num<<" datas.\n";
	B10.close();
	ifstream B11("B11.txt");
	for(int i=0;i<6;i++)B11.getline(head,100);
	while(1)
	{

		B11>>buffer;
		//cout<<buffer<<"\t";
		if(buffer=="#END")break;
		tmpdata.Nenergy=std::stof(buffer);
		//cout<<tmpdata.Nenergy<<"\t";
		B11>>buffer;
		//cout<<buffer<<"\t";
		tmpdata.sig=std::stof(buffer);
		//cout<<tmpdata.sig<<"\t";
		tmpdata.element="B11";
		ENFdata.push_back(tmpdata);
		B11>>buffer;
		//cout<<endl;				
	}
	int B11_num=ENFdata.size()-B10_num;
	cout<<"collection B10 contains "<<B11_num<<" datas.\n";
	B11.close();
	//////////////////////////////////////////////////////////////////
	//data filling
	//////////////////////////////////////////////////////////////////
	int nbin=10;//NBINS per decade
   	Double_t Lower=1E-5;
   	Double_t Higher=2E7;
   	Int_t NBINS = TMath::Log10(Higher/Lower)*nbin;
   	Double_t *edges=new Double_t[NBINS + 1];
	Double_t base=TMath::Power(10,double(1)/nbin);
	for(int i=0;i<NBINS + 1;i++)edges[i]=Lower*TMath::Power(base,i);
	TH1* hB10 = new TH1D(
	/* name */ "B10",
	/* title */ "CrossSection of B10",
	/* number of bins */ NBINS,
	/* edge array */ edges
	);
	TH1* hB11 = new TH1D(
	/* name */ "B11",
	/* title */ "CrossSection of B11",
	/* number of bins */ NBINS,
	/* edge array */ edges
	);
	TH1* Total = new TH1D(
	/* name */ "Total",
	/* title */ "CrossSection of B10 and B11",
	/* number of bins */ NBINS,
	/* edge array */ edges
	);
	std::vector<CROSSSECTION>::iterator it=ENFdata.begin();
	for(int i=0;i<ENFdata.size();i++)
	{
		if(CROSSSECTION(*it).element=="B10")
		{
			hB10->Fill(CROSSSECTION(*it).Nenergy,CROSSSECTION(*it).sig);
			Total->Fill(CROSSSECTION(*it).Nenergy,CROSSSECTION(*it).sig*0.2);
		}
		else if(CROSSSECTION(*it).element=="B11")
		{
			hB11->Fill(CROSSSECTION(*it).Nenergy,CROSSSECTION(*it).sig);
			Total->Fill(CROSSSECTION(*it).Nenergy,CROSSSECTION(*it).sig*0.8);
		}
		else continue;
		it++;
	}

	//data processing
	double length=55;//meter
	double Dt=0;//ps
	double c=299792458;//m/s light speed
	double Mn=939.565413;// MeV neutron mass
	TFile*f=new TFile("ToTal.root");
	double Q;
	double T;
	TTree*bank=(TTree*)f->Get("tot");
	bank->SetBranchAddress("Q",&Q);
	bank->SetBranchAddress("T",&T);
	int total=bank->GetEntries();
	double energy;
	double det;
	TFile * rootf= new TFile("results.root","RECREATE" );
	TTree*ana=new TTree("ana","results of ana");
	ana->Branch("energy",&energy,"energy/F");
	ana->Branch("det",&det,"det/F");

	TH1* h = new TH1D(
	/* name */ "data",
	/* title */ "data of neutron Energy",
	/* number of bins */ NBINS,
	/* edge array */ edges
	);
	for(int i=0;i<total;i++)
	{
		bank->GetEntry(i);
		energy=(Mn*(1/TMath::Sqrt(1-TMath::Sq(length/(c*(T-Dt)/1E12)))-1))*1E6;
		det=Q;
		h->Fill(energy);
		ana->Fill();
	}
	f->Close();
	//Drawing
	TCanvas*c1=new TCanvas();
	c1->SetLogx();
	c1->SetLogy();
	Total->Draw("HIST");
	hB10->SetLineColor(2);
	hB10->Draw("same");
	hB11->SetLineColor(3);
	hB11->Draw("same");
	h->SetLineColor(1);
	h->Draw("HIST same");

	//compare with spectrum U5
	ifstream U5("FluxData.txt");	
	TH1* hU5 = new TH1D(
	/* name */ "U5",
	/* title */ "data of neutron Energy from U5",
	/* number of bins */ NBINS,
	/* edge array */ edges
	);
	double U5energy;
	double U5flux;
	while(U5.good())
	{
		U5>>U5energy;
		U5>>U5flux;
		hU5->Fill(U5energy,U5flux/757.378);
	}
	
	h->Divide(Total);
	/*double unibin=hU5->GetBinContent(h->GetMaximumBin())/h->GetBinContent(h->GetMaximumBin());
	cout<<unibin<<endl;*/
	
	h->Draw("HIST");
	hU5->SetLineColor(2);
	hU5->Draw("HIST same");
	
	rootf->cd();
	hB10->Write();
	hB11->Write();
	Total->Write();
	h->Write();
	hU5->Write();
	ana->Write();
	rootf->Close();

}
