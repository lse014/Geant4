
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int sens(char* filename)
{
  TFile *f = TFile::Open(filename);
  //TFile *f = new TFile("../B1-build/Electrons.root");

  TTree *tree = (TTree*) f->Get("ScorerKin");

  cout << "reading from "<< f->GetName()<<"..."<<endl;
  Double_t ekine;
  Double_t Z;
  Double_t trackID;
  Double_t parentID;
  Double_t eventID;
  Double_t productionVolume; TString volumeAtVertex_;
  Double_t creatorProcess; TString processName_;
  Double_t particleID;


  tree->SetBranchAddress("KineticEnergy",&ekine);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("trackID",&trackID);
  tree->SetBranchAddress("parentID",&parentID);
  tree->SetBranchAddress("EventID",&eventID);
  tree->SetBranchAddress("productionVolume",&productionVolume);
  tree->SetBranchAddress("creatorProcess",&creatorProcess);
  tree->SetBranchAddress("particleID",&particleID);

  // ELECTRON HISTOGRAMS
  Double_t range = 0.5;
  Int_t bins = range/0.001;
  TH1F *h_ekine1 = new TH1F("h_ekine1","Energy Spectrum ",bins,0,range);
    h_ekine1->SetAxisRange(0,50,"Y");
    h_ekine1->GetXaxis()->SetTitle("Ekine [MeV]");
    h_ekine1->GetYaxis()->SetTitle("count");
  TH1F *h_ekine2 = new TH1F("h_ekine2","Energy Spectrum ",bins,0,range);
    h_ekine2->SetAxisRange(0,50,"Y");
    h_ekine2->GetXaxis()->SetTitle("Ekine [MeV]");
    h_ekine2->GetYaxis()->SetTitle("count");

  // counters
  Int_t neutrons = 10000;
  Int_t count1_tot = 0;
  Int_t count2_tot = 0;
  Int_t count_totICE = 0;

  // loop thorugh all steps
  TString serialID;
  TString previousSerialID = "0";
  Double_t previousEventID = 0;
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++)
  {
      tree->GetEntry(i);
      if (!(particleID == 1)) {continue;}// 1 = electron

      switch ((Int_t)creatorProcess)
      {
        case 1: processName_= "compt"; break;
        case 2: processName_= "nCapture"; break;
        case 3: processName_= "phot"; break;
        case 4: processName_= "conv"; break;
      }
      switch((Int_t)productionVolume)
      {
        case 1: volumeAtVertex_= "gd"          ; break;
        case 2: volumeAtVertex_= "AlpideSens1" ; break;
        case 3: volumeAtVertex_= "al_1"        ; break;
        case 4: volumeAtVertex_= "AlpideSens2" ; break;
        case 5: volumeAtVertex_= "al_2"        ; break;
      }
      serialID = MakeSerialID(trackID,parentID,eventID);
      if (serialID == previousSerialID || volumeAtVertex_ != "gd" || !(processName_== "nCapture")){
        // Update IDs and go to next iteration
        previousEventID = eventID;
        previousSerialID = serialID;
        continue;
      }
      if (Z>0) {h_ekine1->Fill(ekine);} // ALPIDE1 position
      if (Z<0) {h_ekine2->Fill(ekine);} // ALPIDE2 position

      previousSerialID = serialID;
      previousEventID = eventID;
    }

  Double_t totICE_1 = h_ekine1->GetEntries();
  Double_t totICE_2 = h_ekine2->GetEntries();

  TH1F *h_eEkine_scaled_1 = (TH1F*) h_ekine1->Clone("h_eEkine_scaled_1");
    h_eEkine_scaled_1->Scale(100/totICE_1);
    h_eEkine_scaled_1->GetYaxis()->SetTitle("Conversion Electrons %");
  TH1F *h_eEkine_scaled_2 = (TH1F*) h_ekine2->Clone("h_eEkine_scaled_2");
    h_eEkine_scaled_2->Scale(100/totICE_2);
    h_eEkine_scaled_2->GetYaxis()->SetTitle("Conversion Electrons %");


  TCanvas *c1= new TCanvas("c1"," ");
    c1->Divide(2,1);
    c1->ToggleEventStatus();
    c1->cd(2); h_ekine1->Draw();
    c1->cd(1); h_ekine2->Draw();

  TCanvas *c2= new TCanvas("c2"," ");
    c2->Divide(2,1);
    c2->ToggleEventStatus();
    c2->cd(2); h_eEkine_scaled_1->Draw();
    c2->cd(1); h_eEkine_scaled_2->Draw();




  cout << "ALPIDE" << endl;
  cout << " steps: " << n << endl;
  cout << " 1) count: "<< totICE_1 << endl;
  cout << " 2) count: "<< totICE_2 << endl;
  cout << " Sum count: "<< totICE_1+totICE_2 << endl;
  //cout << " ICE count in ALPIDEs: "<< count_totICE << endl;
  cout << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << endl;

  char pdfname[500];
    strcpy(pdfname,filename);
    strcat(pdfname,".pdf");
    cout <<"filename:"<< filename<<endl;
    cout <<"saving pdf as:"<< pdfname << "..."<<endl;
    c2->SaveAs(pdfname);

  char txtname[500];
    strcpy(txtname,filename);
    strcat(txtname,".txt");

  ofstream myfile;
    myfile.open (txtname);
    myfile << "ALPIDE" <<  "\n";
    myfile << " steps: " << n << "\n";
    myfile << " 1) count: "<< count1_tot << "\n";
    myfile << " 2) count: "<< count2_tot << "\n";
    myfile << " Sum count: "<< count2_tot+count1_tot << "\n";
    //myfile << " ICE count in ALPIDEs: "<< count_totICE << "\n";
    myfile << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << "\n";
    cout <<"saving terminal output as:"<< txtname << " ..."<<endl;
  myfile.close();

  return 0;

}

TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID)
{

  TString trackID_ = to_string((Int_t)trackID);
  TString parentID_ = to_string((Int_t)parentID);
  TString eventID_ = to_string((Int_t)eventID);
  TString serialID_ = eventID_+parentID_+trackID_;
  //cout << serialID_ <<endl;
  return serialID_;
}
