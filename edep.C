
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);

int edep(char* filename){

  TFile *f = TFile::Open(filename);
  cout << "reading from "<< f->GetName()<<"..."<<endl;
  TTree *tree = (TTree*) f->Get("ScorerKin");
  tree->Print();

  Double_t ekine;
  Double_t X;
  Double_t Y;
  Double_t Z;
  Double_t trackID;
  Double_t parentID;
  Double_t eventID;
  Double_t productionVolume; TString volumeAtVertex_;
  Double_t creatorProcess; TString processName_;
  Double_t particleID;
  Double_t stepID;
  Double_t edepstep;

  tree->SetBranchAddress("KineticEnergy",&ekine);
  tree->SetBranchAddress("X",&X);
  tree->SetBranchAddress("Y",&Y);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("trackID",&trackID);
  tree->SetBranchAddress("parentID",&parentID);
  tree->SetBranchAddress("EventID",&eventID);
  tree->SetBranchAddress("productionVolume",&productionVolume);
  tree->SetBranchAddress("creatorProcess",&creatorProcess);
  tree->SetBranchAddress("particleID",&particleID);
  tree->SetBranchAddress("stepID",&stepID);
  tree->SetBranchAddress("edepStep",&edepstep);

  Double_t range = 0.5;
  Int_t bins = range/0.001;
  TH1F *h_eEdep = new TH1F("h_eEdep","Energy deposit per electron",bins,0,range);
  //bin_nmbr = 100
    h_eEdep->GetXaxis()->SetTitle("Energy (per electron)[MeV]");
    h_eEdep->GetYaxis()->SetTitle("count");
    //h_eEdep->SetAxisRange(0,600,"Y");
    h_eEdep->SetFillColor(40);
    h_eEdep->SetLineColor(40);
    h_eEdep->SetFillStyle(3016);

  //Int_t dataPoints = 2000;
  //Double_t eEdep[dataPoints], eEkine[dataPoints];

  Int_t count = 0;
  Int_t electron_count = 0;
  TString serialID;
  TString previousSerialID = "0";
  Double_t previousParticleID;
  Double_t edep=0;
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++) // loop thorugh all steps
  {
    tree->GetEntry(i);

    serialID = MakeSerialID(trackID,parentID,eventID);

    if (!(serialID == previousSerialID)){
      // store Energy
      if (previousParticleID==1) { //previous
        h_eEdep->Fill(edep);
        // eEdep[electron_count] = edep;
        // eEkine[electron_count] = ekine;
        // cout << eEdep[electron_count]<<" | ekine:" << eEkine[electron_count] <<endl;
        //electron_count++;
      }
      edep = 0; //reset to zero
      edep += edepstep;
    }
    else{
      edep += edepstep;
    }
    cout <<particleID<<"|"<< creatorProcess<<"|"<<serialID<<" | " <<edepstep<<"|"<<edep<<endl;

    previousSerialID = serialID;
    previousParticleID = particleID;
  }
  cout <<"Entries: "<<n <<endl;
  //cout <<"Entries (check): "<<count <<endl;

  //TGraph *gr1 = new TGraph (dataPoints,eEkine, eEdep);
  TCanvas *c1= new TCanvas("c1"," ",200,10,600,400);
    h_eEdep->Draw("HIST");
    //gr1->GetXaxis()->SetTitle("X-Axis");
    //gr1->GetYaxis()->SetTitle("Y-Axis");
    //gr1->Draw("*");


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
