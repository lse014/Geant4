
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int foil(char* filename)
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


  Double_t thick = 0.01;

  // Converterfoil HISTOGRAMS
  Int_t bin_nmbr = 40;
  TH1F *h_foil = new TH1F("h_foil","nCapture",bin_nmbr,-(thick/2+0.01),thick/2+0.01);
    h_foil->GetXaxis()->SetTitle("Z [mm]");
    h_foil->GetYaxis()->SetTitle("count");
  TH1F *h_foil_g = new TH1F("h_foil_g","gamma",20,0,20);

  // TOT histograms
  // TH1F *h_tot1 = new TH1F("h_tot1","1) Tot. Energy Spectrum ",1000,0,4);
  // TH1F *h_tot2 = new TH1F("h_tot2","2) Tot. Energy Spectrum ",1000,0,4);


  Int_t neutrons = 10000;
  Int_t count_foil_ICe = 0;
  Int_t count_foil_gamma = 1;
  Int_t count_gamma_per_capture = 0;
  TString serialID;
  TString previousSerialID = "0";
  Double_t previousEventID = 0;
  Double_t previousEventID_g = 0;
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++) // loop thorugh all steps
  {
      tree->GetEntry(i);

        serialID = MakeSerialID(trackID,parentID,eventID);
        if (serialID == previousSerialID) {continue;} // track a particle ONCE
        if (eventID == previousEventID) {continue;} // track a capture ONCE
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

        if (-0.05<Z && Z<0.05             // converter foil position (0,0,0), thickness: 5um-50um
            && processName_ == "nCapture" )
          {
            h_foil->Fill(Z);
            if (particleID == 1)
              {
                count_foil_ICe++;
              }
            if (particleID == 2) // something wrong here :)
              {
                count_foil_gamma++;
                if (previousEventID_g == eventID )
                  {
                    count_gamma_per_capture ++;
                  }
                else
                  {
                    //cout << count_gamma_per_capture << endl;
                    h_foil_g->Fill(count_gamma_per_capture,1);
                    count_gamma_per_capture = 0;
                  }
                previousEventID_g = eventID; // something wrong here :)
              }
          }

        previousSerialID = serialID;
        previousEventID = eventID;

    }

    // when have 60% of neutrons been absorbed?
    // check "thick" variable.
    Double_t integral = 0;
    Double_t z_60 = 0;
    for (Int_t i = 2; i <= bin_nmbr; i++)
    {
      integral = h_foil->Integral(1,i);
      //cout << "integral: "<< integral << endl;
      if (integral/neutrons >= 0.6)
      {
        z_60 = thick/2 + (h_foil->GetBinCenter(i));
        break;
      }
    }

// Draw converter foil histogram
  TCanvas *c5= new TCanvas("c5"," ");
  h_foil_g->Draw();

  TCanvas *c6= new TCanvas("c6"," ");
  h_foil->Draw();
  h_foil->SetFillColor(17);
  h_foil->SetLineColor(17);
  h_foil->SetFillStyle(3016);


  cout << "-  -  -  -  -  -  -  -  -  -  -  -  -"<< endl;
  cout << "FOIL\n"<< endl;
  cout << " ICE count: "<< count_foil_ICe << endl;
  cout << " gamma count: "<< count_foil_gamma << endl;
  cout << " Yield (e-/n): "<<(Float_t) count_foil_ICe/neutrons << "\n"<<endl;

  cout << " 60% neutrons absorbed after "<< z_60 << " mm" << endl;
  cout << "-  -  -  -  -  -  -  -  -  -  -  -  -"<< endl;
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
