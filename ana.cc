{
	double length=55;//meter
	double Dt=0;//ps
	double c=299792458;//m/s light speed
	double Mn=939.565413;// MeV neutron mass
	TFile*f=new TFile("ana-10517-NORM-19.root");
	double Q;
	double T;
	TTree*bank=(TTree*)f->Get("bank");
	bank->SetBranchAddress("Q",&Q);
	bank->SetBranchAddress("T",&T);
	int total=bank->GetEntries();
	float energy;
	float det;
	TFile * rootf= new TFile("results.root","RECREATE" );
	TTree*ana=new TTree("ana","results of ana");
	ana->Branch("energy",&energy,"energy/F");
	ana->Branch("det",&det,"det/F");
	int nbin=10;//NBINS per decade
   	Double_t Lower=1E-7;
   	Double_t Higher=10;
   	Int_t NBINS = TMath::Log10(Higher/Lower)*nbin;
   	Double_t *edges=new Double_t[NBINS + 1];
	Double_t base=TMath::Power(10,double(1)/nbin);
	for(int i=0;i<NBINS + 1;i++)edges[i]=Lower*TMath::Power(base,i);
	TH1* h = new TH1D(
	/* name */ "spectrum",
	/* title */ "spectrum of neutron Energy",
	/* number of bins */ NBINS,
	/* edge array */ edges
	);
	for(int i=0;i<total;i++)
	{
		bank->GetEntry(i);
		energy=Mn*(1/TMath::Sqrt(1-TMath::Sq(length/(c*(T-Dt)/1E12)))-1);
		det=Q;
		h->Fill(energy);
		ana->Fill();
	}
	
	rootf->cd();
	h->Write();
	ana->Write();
	rootf->Close();
	f->Close();
}
