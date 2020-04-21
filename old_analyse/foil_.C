
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int foil_(char* filename)
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
  Double_t stepID;


  tree->SetBranchAddress("KineticEnergy",&ekine);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("trackID",&trackID);
  tree->SetBranchAddress("parentID",&parentID);
  tree->SetBranchAddress("EventID",&eventID);
  tree->SetBranchAddress("productionVolume",&productionVolume);
  tree->SetBranchAddress("creatorProcess",&creatorProcess);
  tree->SetBranchAddress("particleID",&particleID);
  tree->SetBranchAddress("stepID",&stepID);


  Double_t thick = 0.025; // 5um-25um --> 0.005mm-0.025mm

  // Converterfoil HISTOGRAMS
  Int_t bin_nmbr =100;
  TH1F *h_foil = new TH1F("h_foil","nCapture",bin_nmbr,-(thick/2+0.005),thick/2+0.005);
  //TH1F *h_foil = new TH1F("h_foil","nCapture",bin_nmbr,-(thick/2+0.01),thick/2+0.01);
    h_foil->GetXaxis()->SetTitle("Z [mm]");
    h_foil->GetYaxis()->SetTitle("count");
  TH1F *hist_ICE = new TH1F("hist_ICE","ICE",1000,0,0.5);
    hist_ICE->GetXaxis()->SetTitle("ekine [MeV]");
    hist_ICE->GetYaxis()->SetTitle("count");

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
      cout << "p: "<<particleID<<", serialID: "<<serialID << ", stepID: "<<stepID<<endl;
      //if (serialID == previousSerialID) {continue;} // track a particle ONCE
      //if (eventID == previousEventID) {continue;} // track a capture ONCE
      //cout <<particleID<<", " << eventID <<", "<<parentID<<", " <<trackID << endl;
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
      if (!(processName_== "nCapture"))
      {
        previousEventID = eventID;
        continue;
      }
      if (eventID == previousEventID)
      {
        previousEventID = eventID;
        continue;
      }

      if (-0.05<Z && Z<0.05             // converter foil position (0,0,0), thickness: 5um-50um
          && processName_ == "nCapture" )
        {
          h_foil->Fill(Z);
          if (particleID == 1) // electron
            {
              count_foil_ICe++;
              hist_ICE->Fill(ekine);
            }
          if (particleID == 2) {count_foil_gamma++;} //gamma
        }

      //previousSerialID = serialID;
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
  TCanvas *c5= new TCanvas("c5","ICE ekine");
  hist_ICE->Draw();

  TCanvas *c6= new TCanvas("c6","nCapture vs. Z");
  h_foil->Draw();
  h_foil->SetFillColor(17);
  h_foil->SetLineColor(17);
  h_foil->SetFillStyle(3016);

  cout << "FOIL"<< endl;
  cout << " ICE count: "<< count_foil_ICe << endl;
  cout << " gamma count: "<< count_foil_gamma << endl;
  cout << " Yield (e-/n): "<<(Float_t) count_foil_ICe/neutrons << "\n"<<endl;

  cout << " 60% neutrons absorbed after "<< z_60 << " mm" << endl;

  char pdfname1[500];
    strcpy(pdfname1,filename);
    strcat(pdfname1,"ekine");
    strcat(pdfname1,".pdf");
    cout <<"filename:"<< filename<<endl;
    cout <<"saving pdf as:"<< pdfname1 << "..."<<endl;
    c5->SaveAs(pdfname1);

  char pdfname2[500];
    strcpy(pdfname2,filename);
    strcat(pdfname2,"Z");
    strcat(pdfname2,".pdf");
    cout <<"filename:"<< filename<<endl;
    cout <<"saving pdf as:"<< pdfname2 << "..."<<endl;
    c6->SaveAs(pdfname2);

  char txtname[500];
    strcpy(txtname,filename);
    strcat(txtname,".txt");

  ofstream myfile;
    myfile <<"FOIL"<< endl;
    myfile <<" ICE count: "<< count_foil_ICe << endl;
    myfile <<" gamma count: "<< count_foil_gamma << endl;
    myfile <<" Yield (e-/n): "<<(Float_t) count_foil_ICe/neutrons << "\n"<<endl;

    myfile <<" 60% neutrons absorbed after "<< z_60 << " mm" << endl;
    myfile.open (txtname);

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
