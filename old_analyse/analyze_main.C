
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);
void AnalyzeRootFile(char* path, char*filename);

int analyze_main(char* dirname)
{
  TFile *f;
  char path[500];
  char filename[500];

  if (!(strncmp(dirname, "dist",4)))
  {system("ls ../B1-build/dist >rootdirlist.txt");}
  if (!(strncmp(dirname, "thick",5))){system("ls ../B1-build/thick >rootdirlist.txt");}

  ifstream instream;
  instream.open("rootdirlist.txt");

  while(1)
  {
    instream >> filename;
    if (!instream.good()) break;
    sprintf(path,"../B1-build/%s/%s",dirname,filename);
    cout << path << endl;
    //f = new TFile (path);
    AnalyzeRootFile(path, filename);
  }

  return 0;
}





void AnalyzeRootFile(char* path, char*filename)
{
  TFile *f = TFile::Open(path);
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
  // TOT histograms
  TH1F *h_tot1 = new TH1F("h_tot1","1) Tot. Energy Spectrum ",1000,0,4);
  TH1F *h_tot2 = new TH1F("h_tot2","2) Tot. Energy Spectrum ",1000,0,4);

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
          h_tot1->Fill(ekine); // gamma and electrons
          if (particleID == 1){h_ekine1->Fill(ekine); count1_tot++;} //electron
        }
        if (Z<0)
        {
          h_tot2->Fill(ekine); // gamma and electrons
          if (particleID == 1){h_ekine2->Fill(ekine); count2_tot++;} //electron
        }
        if (ekine < 0.2){count_totICE++;}
        previousSerialID = serialID;
    }

  TH1F *h1 = (TH1F*) h_ekine1->Clone();
    h1->SetNameTitle("h1","1) electron spectrum");
    h1->SetAxisRange(0,0.5,"X");
    h1->SetAxisRange(0,50,"Y");
    h1->GetXaxis()->SetTitle("Ekine [MeV]");
    h1->GetYaxis()->SetTitle("count");
  TH1F *h2 = (TH1F*) h_ekine2->Clone();
    h2->SetNameTitle("h2","2) electron spectrum");
    h2->SetAxisRange(0,0.5,"X");
    h2->SetAxisRange(0,50,"Y");
    h2->GetXaxis()->SetTitle("Ekine [MeV]");
    h2->GetYaxis()->SetTitle("count");

  h_tot1->SetAxisRange(0,0.5,"X");
  h_tot2->SetAxisRange(0,0.5,"X");

  // subtrackt 1 from 2
  TH1F *hist = (TH1F*) h_tot2->Clone();
  hist->SetName("hist");
  hist->Add(h_tot1,-1);
  hist->GetXaxis()->SetTitle("Ekine [MeV]");
  hist->GetYaxis()->SetTitle("count");
  hist->SetAxisRange(0,0.5,"X");
  hist->SetAxisRange(0,100,"Y");

  TCanvas *c1= new TCanvas("c1"," ");
    c1->Divide(2,1);
    c1->cd(2); h_tot1->Draw();
    c1->cd(1); h_tot2->Draw();
  TCanvas *c2= new TCanvas("c2"," ");
    c2->Divide(2,1);
    c2->cd(2); h1->Draw();
    c2->cd(1); h2->Draw();

  TCanvas *c3= new TCanvas("c3"," ");
  hist->Draw();

  cout << "steps: " << n << endl;
  cout << "1) count: "<< count1_tot << endl;
  cout << "2) count: "<< count2_tot << endl;
  cout << "Sum count: "<< count2_tot+count1_tot << endl;
  cout << "- - - - - - - - - - -"<< endl;
  cout << "ICE count: "<< count_totICE << endl;
  cout << "Yield (e-/n): "<<(Float_t) count_totICE/neutrons << endl;


  char pdfname[500];
  strcat(pdfname,filename);
  strcat(pdfname,".pdf");

  cout <<"filename:"<< filename<<endl;
  cout <<"saving pdf as:"<< pdfname << "..."<<endl;
  c2->SaveAs(pdfname);

return;
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
