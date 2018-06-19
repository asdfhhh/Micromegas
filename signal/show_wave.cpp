#include "Event.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

void showWave(const char *file, int n, int old_t)
{
	TFile *tf = new TFile(file);
	TTree *tree = (TTree*)tf->Get("wns");
	WNSEvent *ev = new WNSEvent;
	tree->SetBranchAddress("ev", &ev);
	TFile *outf = new TFile("wave.root","RECREATE");
	int loop=tree->GetEntries();
	TGraph *gr;
	gr = new TGraph();
	int N_wave=0;
	for(int j=1;j<loop;j++)
	{
		tree->GetEntry(j);
		int trigger=ev->EvN();
		if(ev->Board()!=n)continue;
		int channel=ev->Channel();
		if(N_wave)
		{
			if(old_t!=trigger) break;
		}
		else if(old_t!=trigger)continue;
		int TOF=ev->Tof()/1000;
		for(int i=N_wave; i<TOF; i++)gr->SetPoint(i, i, 0);
		for(int i=TOF; i<(TOF+ev->Adcs()->GetNrows()); i++)
		{
			Double_t value = (*ev->Adcs())[i-TOF];
			gr->SetPoint(i, i, value);
		}
		N_wave=TOF+ev->Adcs()->GetNrows();
	}
	outf->cd();
	gr->Write();
	tf->Close();
	outf->Close();
}

int main(int argc, char **argv)
{
    showWave(argv[1], atoi(argv[2]), atoi(argv[3]));
}

