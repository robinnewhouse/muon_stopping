//Four Bar Coincidence Check
//By: Matthew Stukel, Robin Newhouse
//2017-11-21
//The program will track if a muon travels through two blocks, 
//stops in lead between them, and emits an energetic electron through top or bottom

#include <fstream>
#include <iostream>
#include <cmath>
#include <ctime>

#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include <TRandom3.h>

using namespace std;

// MAIN PROGRAM
void lead_stopping()
{
  // //Time tag infile of the run
  // ifstream infile0T("results1T.txt");
  // ifstream infile1T("results3T.txt");
  // ifstream infile2T("results5T.txt");
  // ifstream infile3T("results7T.txt");
  // // //Event energy of the run
  // // ifstream infile0C("results1C.txt");
  // // ifstream infile1C("results3C.txt");
  // // ifstream infile2C("results5C.txt");
  // // ifstream infile3C("results7C.txt");


  // int const max_n = 1674451;//PMT 1 events
  // int const max_n1= 1654801;//PMT 3 events
  // int const max_n2= 1647941;//PMT 5 events
  // int const max_n3= 1683501;//PMT 7 events

  // int const minbin = 0;
  // int const maxbin = 10000;

  // int i=0,q=0,w=0,t=0,y=0,u=0,j=0,k=0,s=0;


  // double x;
  // double OC,OT,NC,NT,GC,GT,JC,JT;
  // long record_time =0;
  // Int_t record_pmt;

  vector<long> PMT0;
  vector<long> PMT1;
  vector<long> PMT2;
  vector<long> PMT3;
  vector<long> PMT4;
  vector<long> PMT5;
  vector<long> PMT6;
  vector<long> PMT7;
  // vector <long>  PMT1_time;
  // vector <long>  PMT3_time;
  // vector <long>  PMT5_time;
  // vector <long>  PMT7_time;
  vector<long> decay_time_difference;
  vector<bool> signal_in_upper;

  TTree *time_diff_tree = new TTree("time_diff_tree", "Time differences in muon in and electron out");
  time_diff_tree->Branch("decay_time_difference", &decay_time_difference);
  time_diff_tree->Branch("signal_in_upper", &signal_in_upper);


  //////////////////////Import the data//////////////////////////

  bool loadfile = false;
  if (loadfile){
    TFile *f = new TFile("raw_data.root", "RECREATE");
    f->cd();
    TTree *pmt_tree = new TTree("PMT_Tree", "Hits in PMT tubes");
    pmt_tree->ReadFile("run000047.txt","time/L:channel/b:charge/i",',');
    TBranch        *b_time;   //!
    TBranch        *b_channel;   //!
    TBranch        *b_charge;   //!
    Long64_t        time;
    UChar_t         channel;
    UInt_t          charge;
    pmt_tree->SetBranchAddress("time", &time, &b_time);
    pmt_tree->SetBranchAddress("channel", &channel, &b_channel);
    pmt_tree->SetBranchAddress("charge", &charge, &b_charge);
    // pmt_tree->Draw("time");
    pmt_tree->Fill();
    pmt_tree->Write();
    f->Close();


    TTree *split_tree = new TTree("split_tree", "Hits in PMT tubes separated by tube");
    split_tree->Branch("PMT0", &PMT0);
    split_tree->Branch("PMT1", &PMT1);
    split_tree->Branch("PMT2", &PMT2);
    split_tree->Branch("PMT3", &PMT3);
    split_tree->Branch("PMT4", &PMT4);
    split_tree->Branch("PMT5", &PMT5);
    split_tree->Branch("PMT6", &PMT6);
    split_tree->Branch("PMT7", &PMT7);
    



  }

  TFile *f = new TFile("raw_data.root");
  TTree *pmt_tree = new TTree("PMT_Tree", "Hits in PMT tubes");
  pmt_tree = (TTree*) f->Get("PMT_Tree;5");
  TBranch        *b_time;   //!
  TBranch        *b_channel;   //!
  TBranch        *b_charge;   //!
  Long64_t        time;
  UChar_t         channel;
  UInt_t          charge;
  pmt_tree->SetBranchAddress("time", &time);
  pmt_tree->SetBranchAddress("channel", &channel);
  pmt_tree->SetBranchAddress("charge", &charge);


  Long64_t nentries = pmt_tree->GetEntriesFast();
  cout << "nentries: " << nentries << endl;
  // nentries = 100000; //testing
  cout << "iterating over " << nentries << " entries"<<endl;

  for (Long64_t i=0; i<nentries;i++) {
    pmt_tree->GetEntry(i); // next line

    if (i%10000== -1) { // print out info
      pmt_tree->GetEntry(i);
      pmt_tree->Show();
      cout << "time: " << time << endl;
    }

    // store time in ns
    switch (channel) {
      case 0:
      PMT0.push_back(time);
      break;
      case 1:
      PMT1.push_back(time);
      break;
      case 2:
      PMT2.push_back(time);
      break;
      case 3:
      PMT3.push_back(time);
      break;
      case 4:
      PMT4.push_back(time);
      break;
      case 5:
      PMT5.push_back(time);
      break;
      case 6:
      PMT6.push_back(time);
      break;
      case 7:
      PMT7.push_back(time);
      break;
      default:
      cout << "channel value out of scope: " << channel;
    }    
  }

  int i_range = 500; //don't search through the whole array
  int coincidence_threshold = 20;
  int capture_threshold = 4000;

  int stopping_counter = 0;


  clock_t begin = clock();

  // actually look for coincides
  // PMT1 && PMT3 && ~PMT5 && ~PMT6
  for (int i = 0; i < PMT1.size(); ++i)
  {
    bool coinc_in_1 = false;
    bool coinc_in_3 = false;
    bool coinc_in_5 = false;
    bool coinc_in_7 = false;


    for (int j = -i_range; j < i_range; ++j)
    {
      // cout << "PMT1[i_1]: " << PMT1[i_1] << "   PMT3[j_3]: " << PMT3[j_3] << endl;  
      int k = max(i+j,0); // don't look below 0
      if (abs(PMT1[i] - PMT3[k]) < coincidence_threshold)
        coinc_in_3 = true;
      if (abs(PMT1[i] - PMT5[k]) < coincidence_threshold){
        coinc_in_5 = true;
        break;
      }
      if (abs(PMT1[i] - PMT7[k]) < coincidence_threshold){
        coinc_in_7 = true;
        break;
      }
    }

    // found a muon stopping event. now see if it decays into the top of bottom pmts
    if (coinc_in_3 && !coinc_in_5 && !coinc_in_5){
      stopping_counter++;
      long muon_in_time = PMT1[i];
      long electron_out_time_PMT1 = 666;
      long electron_out_time_PMT3 = 666;
      long electron_out_time_PMT5 = 666;
      long electron_out_time_PMT7 = 666;

      bool second_coinc_in_1 = false;
      bool second_coinc_in_3 = false;
      bool second_coinc_in_5 = false;
      bool second_coinc_in_7 = false;

      for (int jj = -i_range; jj < i_range; ++jj){
        int kk = max(i+jj,0); // don't look below 0
        // this would indicate an escaping electron after muon capture
        if (abs(PMT1[kk] - muon_in_time) < capture_threshold){ 
          // but don't count the initial coincidence 
          if (PMT1[kk] - muon_in_time > coincidence_threshold){
            second_coinc_in_1 = true;
            electron_out_time_PMT1 = PMT1[kk];
          }
        }

        if (abs(PMT3[kk] - muon_in_time) < capture_threshold){
          if (PMT3[kk] - muon_in_time > coincidence_threshold){ 
            second_coinc_in_3 = true;
            electron_out_time_PMT3 = PMT3[kk];
          }
        }
        if (abs(PMT5[kk] - muon_in_time) < capture_threshold){
          if (PMT5[kk] - muon_in_time > coincidence_threshold){ 
            second_coinc_in_5 = true;
            electron_out_time_PMT5 = PMT5[kk];
          }
        }
        if (abs(PMT7[kk] - muon_in_time) < capture_threshold){
          if (PMT7[kk] - muon_in_time > coincidence_threshold){ 
            second_coinc_in_7 = true;
            electron_out_time_PMT7 = PMT7[kk];
          }
        }
      }

      // do logical checks 
      // PMT1 && PMT3 && ~PMT5 && ~PMT7
      if (second_coinc_in_1 && second_coinc_in_3 && !second_coinc_in_5 && !second_coinc_in_7){
        // check if truly an escaping electron
        if (abs(electron_out_time_PMT1 - electron_out_time_PMT3) < coincidence_threshold){
          // store the average time difference
          decay_time_difference.push_back((electron_out_time_PMT1 + electron_out_time_PMT3)/2 - muon_in_time);
          signal_in_upper.push_back(true);
        }
      }
      // ~PMT1 && ~PMT3 && PMT5 && PMT7
      if (!second_coinc_in_1 && !second_coinc_in_3 && second_coinc_in_5 && second_coinc_in_7){
        // check if truly an escaping electron
        if (abs(electron_out_time_PMT5 - electron_out_time_PMT7) < coincidence_threshold){
          // store the average time difference
          decay_time_difference.push_back((electron_out_time_PMT5 + electron_out_time_PMT7)/2 - muon_in_time);
          signal_in_upper.push_back(false);
        }
      }

    }
  }

  time_diff_tree->Fill();


  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  for (int i = 0; i < decay_time_difference.size(); ++i){
    cout << decay_time_difference[i] << (signal_in_upper[i]?" upper":" lower") << endl;
  }
  cout << "stopping event counter: "<< stopping_counter << endl;
  cout << "signal event counter: "<< decay_time_difference.size() << endl;
  cout << "time elapsed for search: " << elapsed_secs << endl;

  TFile *file = new TFile("decay_time_difference.root", "RECREATE");
  file->cd();
  time_diff_tree->Write();
  file->Close();
  cout << "Wrote file" << endl;


  // TTree *split_tree = new TTree("Split_Tree", "Hits in PMT tubes separated by tube");
  // split_tree->Branch("PMT0", &PMT0);
  // split_tree->Branch("PMT1", &PMT1);
  // split_tree->Branch("PMT2", &PMT2);
  // split_tree->Branch("PMT3", &PMT3);
  // split_tree->Branch("PMT4", &PMT4);
  // split_tree->Branch("PMT5", &PMT5);
  // split_tree->Branch("PMT6", &PMT6);
  // split_tree->Branch("PMT7", &PMT7);

  // ifstream infile( "run000047_split/xaa" );

  // while (infile)
  // {
  //   string s;
  //   if (!getline( infile, s )) break;

  //   istringstream ss( s );
  //   vector <string> record;

  //   while (ss)
  //   {
  //     string s;
  //     if (!getline( ss, s, ',' )) break;
  //     record.push_back( s );
  //   }
  //   // record[0] >> record_time;
  //   // record[1] >> record_pmt;
  //   switch (1) {
  //     case 1:
  //       PMT1_time.push_back(record_time);
  //       break;
  //     case 3:
  //       PMT3_time.push_back(record_time);
  //       break;
  //     case 5:
  //       PMT5_time.push_back(record_time);
  //       break;
  //     case 7:
  //       PMT7_time.push_back(record_time);
  //       break;
  //     default:
  //       cout << "bad record";
  //       continue;
  //     }

    // data.push_back( record );
  // }
  // if (!infile.eof())
  // {
  //   cerr << "Fooey!\n";
  // }

  // for(std::vector<vector<string>>::size_type i = 0; i != PMT1_time.size(); i++) {
  //      std::cout << PMT1_time[i] << endl;
  // }
  return;

//    while(infile0T >> OT)
//     {
//       if(w>max_n) break;
//       xarray1T[w] = OT;
//       w++;
//     }
//    cout << "Infile 0T is finished loading" << endl;
//    w = 0;

//    while(infile1T >> NT)
//     {
//       if(t>max_n1) break;
//       xarray3T[t] = NT;
//       t++;
//     }
//   cout<< "Infile 1T is finished loading"<< endl;

//   while(infile2T >> GT)
//     {
//       if(u>max_n2)break;
//       xarray5T[u]=GT;
//       u++;
//     }
//   cout << "Infile 2T is finished loading"<< endl;
//   u=0;


//   while (infile3T >> JT)
//     {
//       if (k>max_n3)break;
//       xarray7T[k]=JT;
//       k++;
//     }
//   cout<< "Infile 3T is finished loading"<<endl;
//   k=0;


// ///////////////////////////////////////////////////////////////

//   double nandiff;
//   double eventcnt;
//   double eventadded;


//   ////////////////////////////////Coincidence Test////////////
//   double tdiff; // time in microseconds
//   double coincdiff;
//   double counter=1000;
//   double coincwtP3=0;
//   double coincwtP5=0;
//   double coincwtP7=0;
//   double allbarmuon=0;
//   double threebarmuon=0;
//   double twobarmuon=0;


//   Int_t xarray3T_size = sizeof(xarray3T)/sizeof(xarray3T[0]);

//   // iterate over first block hits and find coincidences
//   for (  int i_1T = 0; i_1T < counter; i_1T++)
//   {
//     // find index of first hit in 2nd block that might be coincident with current 1st block time
//     cout<< "Doing search " << endl;
//     Int_t low = TMath::BinarySearch(xarray3T_size, xarray3T, xarray3T[10]); 
//     // cout<< low << endl;

//   }

// return;

//    for (int g=0; g<max_n;g++){//max_n
//     tdiff = (xarray1T[g+1]-xarray1T[g]) * 4.0E-3; // measure difference and convert to microseconds
//      //Check if the event is a stopping muon
//       if (tdiff < 20 && tdiff > 0 ){
// 	counter +=1;
// 	cout<<"The counter is at "<<counter<<endl;
// 	cout << "Muon Stopping Event Found"<<endl;

// 	//Check for coincidence in bar 2
// 	for (int b=0; b<max_n1;b++){//max_n1
// 	  coincdiff = xarray1T[g]-xarray3T[b];
// 	  if (coincdiff < 20 && coincdiff > -20){
// 	    coincwtP3 = 1;
// 	    cout << "Found coincidence with bar 2"<<endl;
// 	   break;
// 	  }
// 	 }
// 	//Check for coincidence in bar 3
// 	if (coincwtP3 > 0){
// 	  for (int n=0;n<max_n2;n++){//max_n2
// 	    coincdiff = xarray1T[g]-xarray5T[n];
// 	    if (coincdiff < 20 && coincdiff>-20){
// 	      coincwtP5 = 1;
// 	      cout << "Found coincidence with bar 3"<<endl;

// 	      break;
// 	    }
// 	  }  
// 	}

// 	//Check for coincidence in bar 4
// 	if (coincwtP3 >0 && coincwtP5 >0){
// 	  for (int m=0;m<max_n3;m++){//max_n3
// 	    coincdiff = xarray1T[g]-xarray7T[m];
// 	    if (coincdiff < 20 && coincdiff>-20){
// 	      coincwtP7 = 1;
// 	      cout << "Found coincidence with bar 4"<<endl;
// 	      break;
// 	    }
// 	  }
// 	}

// 	if ( coincwtP3 > 0 && coincwtP5 >0 && coincwtP7 > 0){
// 	  allbarmuon += 1;
// 	  // t1->Fill(xarray1C[g]);// Energy of Muon Stopping in PMT 1 but travelling through all
// 	  // t2->Fill(xarray3C[b]);// Energy of Muon passing through PMT 2 and stopping in PMT 1
// 	  // t3->Fill(xarray5C[n]);// Energy of Muon passing through PMT 3 and stopping in PMT 1
// 	  // t4->Fill(xarray7C[m]);// Energy of Muon passing through PMT 4 and stopping in PMT 1
// 	}
// 	else if(coincwtP3 > 0 && coincwtP5 >0 && coincwtP7 == 0){
// 	  threebarmuon +=1;
// 	  // A1->Fill(xarray1C[g]);// Energy of Muon Stopping in PMT 1 but travelling through only PMT 1-3
// 	  // A2->Fill(xarray3C[b]);// Energy of Muon passing through PMT 2 and stopping in PMT 1
// 	  // A3->Fill(xarray5C[n]);// Energy of Muon passing through PMT 3 and stopping in PMT 1
// 	}
// 	else if (coincwtP3 > 0 && coincwtP5 == 0 && coincwtP7 == 0){
// 	  twobarmuon +=1;
// 	  // N1->Fill(xarray1C[g]);// Energy of Muon Stopping in PMT 1 but travelling through only PMT 1-2
// 	  // N2->Fill(xarray3C[b]);// Energy of Muon passing through PMT 2 and Stopping in PMT 1
// 	}
//       }

//     coincwtP3= 0.0;
//     coincwtP5= 0.0;
//     coincwtP7= 0.0;

//     cout << "Loop " << counter << " is done"<<endl;
//    }

// cout << " The total number of muons that stopped in the detector while going through all 4 bars is "<< allbarmuon <<endl;
// cout << " This corresponds to "<<(allbarmuon/counter)*100 << " percent of the total muons come directly from above" <<endl;

// cout << " The total number of muons that stopped in the detector while going through 3 bars is "<< threebarmuon <<endl;
// cout << " This corresponds to "<<(threebarmuon/counter)*100 << " percent of the total muons come directly from above" <<endl;

// cout << " The total number of muons that stopped in the detector while going through 2 bars is "<<twobarmuon <<endl;
// cout << " This corresponds to "<<(twobarmuon/counter)*100 << " percent of the total muons come directly from above" <<endl;

//  /////////////////////////////Draw the canvases////////////////////////


//  TCanvas *c1 = new TCanvas("c1","t1 canvas",1000,600);
//  TCanvas *c2 = new TCanvas("c2","t2 canvas",1000,600);
//  TCanvas *c3 = new TCanvas("c3","t3 canvas",1000,600);
//  TCanvas *c4 = new TCanvas("c4","t4 canvas",1000,600);

//  TCanvas *c5 = new TCanvas("c5","A1 canvas",1000,600);
//  TCanvas *c6 = new TCanvas("c6","A2 canvas",1000,600);
//  TCanvas *c7 = new TCanvas("c7","A3 canvas",1000,600);

//  TCanvas *c8 = new TCanvas("c8","N1 canvas",1000,600);
//  TCanvas *c9 = new TCanvas("c9","N2 canvas",1000,600);


   // c1->Update();
   // c1->cd();
   // t1->Draw();
   // 
   // c2->Update();
   // c2->cd();
   // t2->Draw();
// 
   // c3->Update();
   // c3->cd();
   // t3->Draw();
// 
   // c4->Update();
   // c4->cd();
   // t4->Draw();
// 
   // c5->Update();
   // c5->cd();
   // A1->Draw();
// 
   // c6->Update();
   // c6->cd();
   // A2->Draw();
// 
   // c7->Update();
   // c7->cd();
   // A3->Draw();
// 
   // c8->Update();
   // c8->cd();
   // N1->Draw();
   // 
   // c9->Update();
   // c9->cd();
   // N2->Draw();




}
