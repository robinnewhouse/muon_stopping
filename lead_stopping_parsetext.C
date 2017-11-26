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
void lead_stopping_parsetext()
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

  vector<vector<long>> vectorized_PMT0;
  vector<vector<long>> vectorized_PMT1;
  vector<vector<long>> vectorized_PMT2;
  vector<vector<long>> vectorized_PMT3;
  vector<vector<long>> vectorized_PMT4;
  vector<vector<long>> vectorized_PMT5;
  vector<vector<long>> vectorized_PMT6;
  vector<vector<long>> vectorized_PMT7;

  // vector <long>  PMT1_time;
  // vector <long>  PMT3_time;
  // vector <long>  PMT5_time;
  // vector <long>  PMT7_time;
  vector<double> decay_time_difference;
  vector<bool> signal_in_upper;

  TTree *time_diff_tree = new TTree("time_diff_tree", "Time differences in muon in and electron out");
  time_diff_tree->Branch("decay_time_difference", &decay_time_difference);
  time_diff_tree->Branch("signal_in_upper", &signal_in_upper);


  //////////////////////Import the data//////////////////////////

  bool loadfile = false;
  if (loadfile){
    TFile *f = new TFile("raw_data_run48.root", "RECREATE");
    f->cd();
    TTree *pmt_tree = new TTree("PMT_Tree", "Hits in PMT tubes");
    pmt_tree->ReadFile("run000048.txt","time/L:channel/b:charge/i",',');
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


    // TTree *split_tree = new TTree("split_tree", "Hits in PMT tubes separated by tube");
    // split_tree->Branch("PMT0", &PMT0);
    // split_tree->Branch("PMT1", &PMT1);
    // split_tree->Branch("PMT2", &PMT2);
    // split_tree->Branch("PMT3", &PMT3);
    // split_tree->Branch("PMT4", &PMT4);
    // split_tree->Branch("PMT5", &PMT5);
    // split_tree->Branch("PMT6", &PMT6);
    // split_tree->Branch("PMT7", &PMT7);
    



  }
// return;

  TFile *f = new TFile("raw_data_run48.root");
  TTree *pmt_tree = new TTree("PMT_Tree", "Hits in PMT tubes");
  pmt_tree = (TTree*) f->Get("PMT_Tree;8");
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

  cout << "finished splitting input into pmt vectors"<<endl;


  // this whole section is to chunk the data into individual time chunks that can relate to one another
  // without this the
  vector<long> * temp_store;
  int vectorized_index;
  long prev;

  temp_store = new vector<long>();
  vectorized_index = 0;
  prev = PMT1[0];
  for (int i = 1; i < PMT1.size(); ++i){
    long current = PMT1[i];
    if (prev > 0 && current <= 0){ // looped past max int value
      vectorized_index++; // move to next index
      vectorized_PMT1.push_back(*temp_store);
      temp_store = new vector<long>();
    }
    temp_store->push_back(current);
    prev = current;
  }

  temp_store = new vector<long>();
  vectorized_index = 0;
  prev = PMT3[0];
  for (int i = 1; i < PMT3.size(); ++i){
    long current = PMT3[i];
    if (prev > 0 && current <= 0){ // looped past max int value
      vectorized_index++; // move to next index
      vectorized_PMT3.push_back(*temp_store);
      temp_store = new vector<long>();
    }
    temp_store->push_back(current);
    prev = current;
  }

  temp_store = new vector<long>();
  vectorized_index = 0;
  prev = PMT5[0];
  for (int i = 1; i < PMT5.size(); ++i){
    long current = PMT5[i];
    if (prev > 0 && current <= 0){ // looped past max int value
      vectorized_index++; // move to next index
      vectorized_PMT5.push_back(*temp_store);
      temp_store = new vector<long>();
    }
    temp_store->push_back(current);
    prev = current;
  }

  temp_store = new vector<long>();
  vectorized_index = 0;
  prev = PMT7[0];
  for (int i = 1; i < PMT7.size(); ++i){
    long current = PMT7[i];
    if (prev > 0 && current <= 0){ // looped past max int value
      vectorized_index++; // move to next index
      vectorized_PMT7.push_back(*temp_store);
      temp_store = new vector<long>();
    }
    temp_store->push_back(current);
    prev = current;
  }  

// temporary. read out stored vector
  cout << "vectorized_PMT1.size() = "<<vectorized_PMT1.size() << endl;
  cout << "vectorized_PMT3.size() = "<<vectorized_PMT3.size() << endl;
  cout << "vectorized_PMT5.size() = "<<vectorized_PMT5.size() << endl;
  cout << "vectorized_PMT7.size() = "<<vectorized_PMT7.size() << endl;

  // for (int i = 0; i < vectorized_PMT1.size(); i=i+100)
  // {
  //   cout << "vectorized_PMT1[" << i << "].size() = "<<vectorized_PMT1[i].size() << endl;
  //   cout << "vectorized_PMT1[" << i << "][0] = "<<vectorized_PMT1[i][0] << endl;
  //   cout << "vectorized_PMT1[" << i << "][1] = "<<vectorized_PMT1[i][1] << endl;
  //   cout << "vectorized_PMT1[" << i << "].end() = "<<vectorized_PMT1[i][vectorized_PMT1[i].size()-1] << endl;
  //   // cout << "vectorized_PMT1[" << i << "][-2] = "<<vectorized_PMT1[i][-2] << endl;
  //   cout << endl;
  // }
  // return;

  // int i_range = 500; //don't search through the whole array
  int coincidence_threshold = 2; // = 8 ns
  int capture_threshold = 5000; // = 20 microseconds

  int stopping_counter = 0;



  cout << "Beginning search " << endl;
  // actually look for coincides
  // PMT_A && PMT_B && ~PMT_C && ~PMT_D
  clock_t begin = clock();
  int m = 0;
  int n_chunks = vectorized_PMT1.size();
  // n_chunks = 100; // testing
  while (m < n_chunks ){
    if (m%10 == 0){
      cout << "m: " << m << "  --- signals: "  << decay_time_difference.size() << "  --- percent complete: "  << (m*100)/vectorized_PMT1.size() << endl;
    }
    // define the PMTs we're looking at. rearranging the physical orer is done here.
    vector<long> PMT_A = vectorized_PMT7[m];
    vector<long> PMT_B = vectorized_PMT5[m];
    vector<long> PMT_C = vectorized_PMT3[m];
    vector<long> PMT_D = vectorized_PMT1[m];

    for (int i = 0; i < PMT_A.size(); ++i)
    {
      bool coinc_in_A = true;
      bool coinc_in_B = false;
      bool coinc_in_C = false;
      bool coinc_in_D = false;



      for (int j = 0; j < PMT_B.size(); ++j)
      {
        if ((PMT_B[j] - PMT_A[i]) > coincidence_threshold) break; // don't look beyond where it might be
        if (abs(PMT_B[j] - PMT_A[i]) <= coincidence_threshold) // found coincidence
          coinc_in_B = true;
        if (abs(PMT_C[j] - PMT_A[i]) <= coincidence_threshold){
          coinc_in_C = true;
          break;
        }
        if (abs(PMT_D[j] - PMT_A[i]) <= coincidence_threshold){
          coinc_in_D = true;
          break;
        }
      }

      // found a muon stopping event. now see if it decays into the top of bottom pmts
      if (coinc_in_B && !coinc_in_C && !coinc_in_C){
        stopping_counter++;
        long muon_in_time = PMT_A[i];
        long electron_out_time_PMT_A = 666;
        long electron_out_time_PMT_B = 666;
        long electron_out_time_PMT_C = 666;
        long electron_out_time_PMT_D = 666;

        bool second_coinc_in_A = false;
        bool second_coinc_in_B = false;
        bool second_coinc_in_C = false;
        bool second_coinc_in_D = false;

        for (int k = 0; k < PMT_A.size(); ++k){
          // this would indicate an escaping electron after muon capture
          if (abs(PMT_A[k] - muon_in_time) <= capture_threshold){ 
            // but don't count the initial coincidence 
            if (PMT_A[k] - muon_in_time > coincidence_threshold){
              second_coinc_in_A = true;
              electron_out_time_PMT_A = PMT_A[k];
            }
          }

          if (abs(PMT_B[k] - muon_in_time) <= capture_threshold){
            if (PMT_B[k] - muon_in_time > coincidence_threshold){ 
              second_coinc_in_B = true;
              electron_out_time_PMT_B = PMT_B[k];
            }
          }
          if (abs(PMT_C[k] - muon_in_time) <= capture_threshold){
            if (PMT_C[k] - muon_in_time > coincidence_threshold){ 
              second_coinc_in_C = true;
              electron_out_time_PMT_C = PMT_C[k];
            }
          }
          if (abs(PMT_D[k] - muon_in_time) <= capture_threshold){
            if (PMT_D[k] - muon_in_time > coincidence_threshold){ 
              second_coinc_in_D = true;
              electron_out_time_PMT_D = PMT_D[k];
            }
          }
        }

        // do logical checks 
        // PMT_A && PMT_B && ~PMT_C && ~PMT_D
        if (second_coinc_in_A && second_coinc_in_B && !second_coinc_in_C && !second_coinc_in_D){
          // check if truly an escaping electron
          if (abs(electron_out_time_PMT_A - electron_out_time_PMT_B) < coincidence_threshold){
            // store the average time difference
            decay_time_difference.push_back(((electron_out_time_PMT_A + electron_out_time_PMT_B)/2.0 - muon_in_time)*4.0);
            signal_in_upper.push_back(true);
          }
        }
        // ~PMT_A && ~PMT_B && PMT_C && PMT_D
        if (!second_coinc_in_A && !second_coinc_in_B && second_coinc_in_C && second_coinc_in_D){
          // check if truly an escaping electron
          if (abs(electron_out_time_PMT_C - electron_out_time_PMT_D) < coincidence_threshold){
            // store the average time difference
            decay_time_difference.push_back(((electron_out_time_PMT_C + electron_out_time_PMT_D)/2.0 - muon_in_time)*4.0);
            signal_in_upper.push_back(false);
          }
        }

      }
    }

    m++;
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

  TFile *file = new TFile("decay_time_difference_run48_PMT_Tree8.root", "RECREATE");
  file->cd();
  time_diff_tree->Write();
  file->Close();
  cout << "Wrote file" << endl;


  // TTree *split_tree = new TTree("Split_Tree", "Hits in PMT tubes separated by tube");
  // split_tree->Branch("PMT0", &PMT0);
  // split_tree->Branch("PMT_A", &PMT1);
  // split_tree->Branch("PMT2", &PMT2);
  // split_tree->Branch("PMT_B", &PMT_B);
  // split_tree->Branch("PMT4", &PMT4);
  // split_tree->Branch("PMT_C", &PMT5);
  // split_tree->Branch("PMT6", &PMT6);
  // split_tree->Branch("PMT_D", &PMT7);

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
  //       PMT_B_time.push_back(record_time);
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
