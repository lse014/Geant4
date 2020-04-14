TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int gamma()
{

  TFile *f = new TFile("../B1-build/Electrons.root"); // in old_analyze
  TTree *tree = (TTree*) f->Get("ScorerKin");

  Double_t ekine;
  Double_t Z;
  Double_t trackID; Double_t parentID; Double_t eventID;
  Double_t particleID;

  tree->SetBranchAddress("KineticEnergy",&ekine);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("trackID",&trackID);
  tree->SetBranchAddress("parentID",&parentID);
  tree->SetBranchAddress("EventID",&eventID);
  tree->SetBranchAddress("particleID",&particleID);

  // hits for both alpides
  TH1F *hist = new TH1F("hist","Energy Spectrum ",1000,0,6);
    hist->GetXaxis()->SetTitle("Ekine [MeV]");
    hist->GetYaxis()->SetTitle("count");
  TH1F *h_gamma = new TH1F("h_gamma","Gamma Spectrum ",1000,0,6);
    h_gamma->GetXaxis()->SetTitle("Ekine [MeV]");
    h_gamma->GetYaxis()->SetTitle("count");
  // TH1F *h_electron = new TH1F("h_electron","Electron Spectrum ",10000,0,9);
  //   h_electron->GetXaxis()->SetTitle("Ekine [MeV]");
  //   h_electron->GetYaxis()->SetTitle("count");


  TString serialID;
  TString previousSerialID = "0";
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++)
  {
      tree->GetEntry(i);
      serialID = MakeSerialID(trackID,parentID,eventID);
      if (serialID == previousSerialID) {continue;} // track an particle ONCE

      hist->Fill(ekine);
      if (particleID == 2){h_gamma->Fill(ekine);}

      previousSerialID = serialID;
  }
  // subtrack two histograms??
  TH1F * h_electron = (TH1F*) hist->Clone();
  h_electron->SetName("h_electron");
  h_electron->Add(h_gamma,-1);

  TCanvas *c1 = new TCanvas("c1");
  hist->Draw();
  TCanvas *c2 = new TCanvas("c2");
  h_gamma->Draw();
  TCanvas *c3 = new TCanvas("c3");
  h_electron->Draw();

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
