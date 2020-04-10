
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int analyze(char* filename)
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

  Double_t dist = 0.2;
  Double_t thick = 0.05;

  // Converterfoil HISTOGRAMS
  Int_t bin_nmbr = 40;
  TH1F *h_foil = new TH1F("h_foil","nCapture",bin_nmbr,-(thick/2+0.01),thick/2+0.01);
    h_foil->GetXaxis()->SetTitle("Z [mm]");
    h_foil->GetYaxis()->SetTitle("count");
  TH1F *h_foil_g = new TH1F("h_foil_g","gamma",20,0,20);

  // TOT histograms
  // TH1F *h_tot1 = new TH1F("h_tot1","1) Tot. Energy Spectrum ",1000,0,4);
  // TH1F *h_tot2 = new TH1F("h_tot2","2) Tot. Energy Spectrum ",1000,0,4);

  // ELECTRON HISTOGRAMS
  //TH3D *h3 = new TH3D("h3", "h3", 20, -2, 2, 20, -2, 2, 20, 0, 4);
  TH1F *h_ekine1 = new TH1F("h_ekine1","Energy Spectrum ",1000,0,4);
    h_ekine1->SetAxisRange(0,50,"Y");
    h_ekine1->GetXaxis()->SetTitle("Ekine [MeV]");
    h_ekine1->GetYaxis()->SetTitle("count");
  TH1F *h_ekine2 = new TH1F("h_ekine2","Energy Spectrum ",1000,0,4);
    h_ekine2->SetAxisRange(0,50,"Y");
    h_ekine2->GetXaxis()->SetTitle("Ekine [MeV]");
    h_ekine2->GetYaxis()->SetTitle("count");


  Int_t neutrons = 10000;
  Int_t count_foil_ICe = 0;
  Int_t count_foil_gamma = 1;
  Int_t count_gamma_per_capture = 0;
  Int_t count1_tot = 0;
  Int_t count2_tot = 0;
  Int_t count_totICE = 0;
  TString serialID;
  TString previousSerialID = "0";
  Double_t previousEventID = 0;
  Double_t previousEventID_g = 0;
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++) // loop thorugh all steps
  {
      tree->GetEntry(i);
      //if (!(particleID == 1)) {cout<<particleID<<endl;continue;}// 1: electron

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

        // Converter foil - Start //
        //
        if (-0.05<Z && Z<0.05             // converter foil position (0,0,0), thickness: 5um-50um
            //&& particleID == 1              // electron
            && processName_ == "nCapture" ) // IC electron and gamma from nCapture
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
                    cout << count_gamma_per_capture << endl;
                    h_foil_g->Fill(count_gamma_per_capture,1);
                    count_gamma_per_capture = 0;
                  }
                previousEventID_g = eventID; // something wrong here :)
              }
          }
        // Converter foil - End  //

        if (Z>0.09) // ALPIDE1 position
          {
            // h_tot1->Fill(ekine); // gamma and electrons
            if (particleID == 1){h_ekine1->Fill(ekine); count1_tot++;} //electron
          }
        if (Z<-0.09) // ALPIDE2 position
          {
            // h_tot2->Fill(ekine); // gamma and electrons
            if (particleID == 1){h_ekine2->Fill(ekine); count2_tot++;} //electron
          }
        // if (ekine < 0.2 && particleID == 1){count_totICE++;}
        if (processName_ == "nCapture" && particleID == 1){count_totICE++;}
        previousSerialID = serialID;
        previousEventID = eventID;

    }

    // when have 60% of neutrons been absorbed?
    cout << "_____________________________"<< endl;
    Double_t integral = 0;
    for (Int_t i = 2; i <= bin_nmbr; i++)
    {
      integral = h_foil->Integral(1,i);
      //cout << "integral: "<< integral << endl;
      if (integral/neutrons >= 0.6)
      {
        Double_t z_60 = thick/2 + (h_foil->GetBinCenter(i));
        cout << "60% neutrons absorbed after "<< z_60 << " mm" << endl;
        break;
      }
    }
    cout << "_____________________________"<< endl;


  TH1F *h1 = (TH1F*) h_ekine1->Clone();
    h1->SetNameTitle("h1","1) electron spectrum");
    h1->SetAxisRange(0,0.5,"X");
    h1->SetAxisRange(0,50,"Y");
    h1->SetAxisRange(0,300,"Y");
    h1->GetXaxis()->SetTitle("Ekine [MeV]");
    h1->GetYaxis()->SetTitle("count");
  TH1F *h2 = (TH1F*) h_ekine2->Clone();
    h2->SetNameTitle("h2","2) electron spectrum");
    h2->SetAxisRange(0,0.5,"X");
    h2->SetAxisRange(0,50,"Y");
    h2->SetAxisRange(0,300,"Y");
    h2->GetXaxis()->SetTitle("Ekine [MeV]");
    h2->GetYaxis()->SetTitle("count");

  // h_tot1->SetAxisRange(0,0.5,"X");
  // h_tot2->SetAxisRange(0,0.5,"X");

  // subtrackt 1 from 2
  // TH1F *hist = (TH1F*) h_tot2->Clone();
  // hist->SetName("hist");
  // hist->Add(h_tot1,-1);
  // hist->GetXaxis()->SetTitle("Ekine [MeV]");
  // hist->GetYaxis()->SetTitle("count");
  // hist->SetAxisRange(0,0.5,"X");
  // hist->SetAxisRange(0,100,"Y");

  // TCanvas *c1= new TCanvas("c1"," ");
  //   c1->Divide(2,1);
  //   c1->cd(2); h_tot1->Draw();
  //   c1->cd(1); h_tot2->Draw();
  TCanvas *c2= new TCanvas("c2"," ");
    c2->Divide(2,1);
    c2->cd(2); h1->Draw();
    c2->cd(1); h2->Draw();

  //TCanvas *c3= new TCanvas("c3"," ");
  //hist->Draw();

// plot e hist and -g side by solidAlpide1
// TCanvas *c4= new TCanvas("c4"," ");
// h2->SetFillColor(kRed);
// h2->SetLineColor(kRed);
// h2->SetFillStyle(3016);
// //h2->SetFillStyle(4050);
// h2->Draw();
// hist->Draw("SAME");

// Draw converter foil histogram
  TCanvas *c5= new TCanvas("c5"," ");
  h_foil_g->Draw();

  TCanvas *c6= new TCanvas("c6"," ");
  h_foil->Draw();
  h_foil->SetFillColor(17);
  h_foil->SetLineColor(17);
  h_foil->SetFillStyle(3016);


  cout << "ALPIDE " << endl;
  cout << " steps: " << n << endl;
  cout << " 1) count: "<< count1_tot << endl;
  cout << " 2) count: "<< count2_tot << endl;
  cout << " Sum count: "<< count2_tot+count1_tot << endl;
  cout << " ICE count in ALPIDEs: "<< count_totICE << endl;
  cout << "- - - - - - - - - - -"<< endl;
  cout << "FOIL"<< endl;
  cout << " ICE count: "<< count_foil_ICe << endl;
  cout << " gamma count: "<< count_foil_gamma << endl;
  //cout << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << endl;
  cout << " Yield (e-/n): "<<(Float_t) count_foil_ICe/neutrons << endl;

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
