
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int analyze()
{

  TFile *f = new TFile("../B1-build/Electrons.root");
  TTree *tree = (TTree*) f->Get("ScorerKin");

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
  TH2F *hcol1_1 = new TH2F("hcol1_1","AlpideSens1",1000,dist-0.04,dist+0.02,100,0,5);
  //TH2F *hcol1_1 = new TH2F("hcol1_1","AlpideSens1",1000,1.46,1.52,100,0,5);
    hcol1_1->GetYaxis()->SetTitle("Ekine [MeV]");
  //TH2F *hcol1_2 = new TH2F("hcol1_2","AlpideSens2",1000,-1.52,-1.46,100,0,5);
  TH2F *hcol1_2 = new TH2F("hcol1_2","AlpideSens2",1000,-(dist+0.02),-(dist-0.04),100,0,5);
    hcol1_2->GetYaxis()->SetTitle("Ekine [MeV]");


  Int_t neutrons = 10000;
  Int_t count1_tot = 0;
  Int_t count2_tot = 0;
  Int_t count_totICE = 0;
  TString serialID;
  TString previousSerialID = "0";
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++)
  {
      tree->GetEntry(i);
      //if (!(particleID == 1)) {cout<<particleID<<endl;continue;}// 1: electron

        serialID = MakeSerialID(trackID,parentID,eventID);
        if (serialID == previousSerialID) {continue;} // track an electron ONCE
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

        if (Z>0)
        {
          h_ekine1->Fill(ekine);
          hcol1_1->Fill(Z,ekine);
          count1_tot++;
        }
        if (Z<0)
        {
          h_ekine2->Fill(ekine);
          hcol1_2->Fill(Z,ekine);
          count2_tot++;
        }
        if (ekine < 0.2){count_totICE++;}
        previousSerialID = serialID;


    }

  TH1F *h1 = (TH1F*) h_ekine1->Clone();
    h1->SetNameTitle("h1","(+)");
    h1->SetAxisRange(0,0.5,"X");
    h1->SetAxisRange(0,50,"Y");
    h1->GetXaxis()->SetTitle("Ekine [MeV]");
    h1->GetYaxis()->SetTitle("count");
  TH1F *h2 = (TH1F*) h_ekine2->Clone();
    h2->SetNameTitle("h2","(+)");
    h2->SetAxisRange(0,0.5,"X");
    h2->SetAxisRange(0,50,"Y");
    h2->GetXaxis()->SetTitle("Ekine [MeV]");
    h2->GetYaxis()->SetTitle("count");

  TCanvas *c= new TCanvas("c","ekin:Z");
    c->SetCanvasSize(1100, 1200);
    c->SetWindowSize(900, 500);
  c->Divide(2,3);
  c->cd(2); 
  gStyle->SetPalette(kBird);
  hcol1_1->Draw("COLZ");
  c->cd(1);
  gStyle->SetPalette(kBird);
  hcol1_2->Draw("COLZ");

  //TCanvas *c21= new TCanvas("c21","1) ekin");
  c->cd(4); h_ekine1->Draw();
  c->cd(3); h_ekine2->Draw();
  c->cd(6); h1->Draw();
  c->cd(5); h2->Draw();
  c->SaveAs("dist_0.2mm.pdf");

  ofstream myfile;
  myfile.open ("dist_0.2mm.txt");
  myfile << "steps: " << n<<"\n";
  myfile << "1) count: "<< count1_tot<<"\n";
  myfile << "2) count: "<< count2_tot<<"\n";
  myfile << "Sum count: "<< count2_tot+count1_tot<<"\n";
  myfile << "ICE count: "<< count_totICE<<"\n";
  myfile << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons<<"\n";

  cout << "steps: " << n << endl;
  cout << "1) count: "<< count1_tot << endl;
  cout << "2) count: "<< count2_tot << endl;
  cout << "Sum count: "<< count2_tot+count1_tot << endl;
  cout << "- - - - - - - - - - -"<< endl;
  cout << "ICE count: "<< count_totICE << endl;
  cout << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << endl;

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
