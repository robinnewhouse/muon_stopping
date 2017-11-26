// negative muon decay measurement in lead
//PHYS531 Lab Measurement 
// November 21 2017


//header file 
#include "nmuon_decay.h"

void nmuon_decay(){

importTfile();
fit_hist();
plot();

	
}
vector<double> * decay_time_difference;
vector<bool> * signal_in_upper;


void importTfile(){

TFile *myfile = TFile::Open("decay_time_difference_full.root");  // insert name of file here
TTree * time_diff_tree = (TTree*) myfile->Get("time_diff_tree;1");
time_diff_tree->SetBranchAddress("decay_time_difference", &decay_time_difference);  //insert variable names here 
time_diff_tree->SetBranchAddress("signal_in_upper", &signal_in_upper);
time_diff_tree->GetEvent(0);
}
TH1F *h_decay   = new TH1F("h_decay","Muon Decay Histogram ",20,0,20);

void fit_hist(){

h_decay->Sumw2();
for (int i = 0; i < decay_time_difference->size(); ++i)
{
	h_decay->Fill(decay_time_difference->at(i)/1000); // convert to microseconds
}

TF1 *expfit = new TF1("expfit","([0]*(TMath::Exp(-x/[1]))+ ([2]*(TMath::Exp(-x/[4])) + [3]))");  //, N1*exp(-t/t1) + N2*exp(-t/t2)
expfit->SetParameters(2000,0.09,2000,1,2.2); // sets inital guesses for parameters
expfit->SetParLimits(1,0,1);  // negative muon lifetime in lead should be 0.09 micros so this limit range should be good for p1
h_decay->Fit("expfit"); 

}

void plot(){

gStyle->SetOptFit(111);

TCanvas *c1 = new TCanvas("c1","c1",1200,800); // for exponential decay plot
h_decay->Draw();
h_decay->GetYaxis()->SetTitle("events/bin");
h_decay->GetXaxis()->SetTitle(" time [#mus]");

TCanvas *c2 = new TCanvas("c2","c2",1200,800); // for log decay plot
c2->SetLogy();
h_decay->Draw();
h_decay->GetYaxis()->SetTitle("events/bin");
h_decay->GetXaxis()->SetTitle("time [#mus]");

}






