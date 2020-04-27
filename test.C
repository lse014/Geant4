
TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);
// void getDistance(char* filename);
// void getProcessString(Double_t processDouble,TString processString);
// void getVolumeString(Double_t volumeDouble, TString volumeString);

int test(char* filename)
{
  TFile *f = TFile::Open(filename);

  //TFile *f = new TFile("../B1-build/Electrons.root");

  TTree *tree = (TTree*) f->Get("ScorerKin");

  cout << "reading from "<< f->GetName()<<"..."<<endl;
  Double_t ekine;
  Double_t X;
  Double_t Z;
  Double_t trackID;
  Double_t parentID;
  Double_t eventID;
  Double_t productionVolume; TString volumeAtVertex_;
  Double_t creatorProcess; TString processName_;
  Double_t particleID;


  tree->SetBranchAddress("KineticEnergy",&ekine);
  tree->SetBranchAddress("X",&X);
  tree->SetBranchAddress("Z",&Z);
  tree->SetBranchAddress("trackID",&trackID);
  tree->SetBranchAddress("parentID",&parentID);
  tree->SetBranchAddress("EventID",&eventID);
  tree->SetBranchAddress("productionVolume",&productionVolume);
  tree->SetBranchAddress("creatorProcess",&creatorProcess);
  tree->SetBranchAddress("particleID",&particleID);

  /// HISTOGRAMS,
  // kinetic energy
  TH1D *h_eEkine1 = new TH1D("h_eEkine1","eEkine1",125,0,0.5);
    h_eEkine1->GetXaxis()->SetTitle("ekine [MeV]");
    h_eEkine1->GetYaxis()->SetTitle("count");
    h_eEkine1->SetAxisRange(0,30,"Y");
  TH1D *h_eEkine2 = new TH1D("h_eEkine2","eEkine2",125,0,0.5);
    h_eEkine2->GetXaxis()->SetTitle("ekine [MeV]");
    h_eEkine2->GetYaxis()->SetTitle("count");
    h_eEkine2->SetAxisRange(0,30,"Y");
  TH1D *h_gEkine1 = new TH1D("h_gEkine1","gEkine1",100,0,8);
    h_gEkine1->GetXaxis()->SetTitle("ekine [MeV]");
    h_gEkine1->GetYaxis()->SetTitle("count");
    h_gEkine1->SetAxisRange(0,7000,"Y");
  TH1D *h_gEkine2 = new TH1D("h_gEkine2","gEkine2",100,0,8);
    h_gEkine2->GetXaxis()->SetTitle("ekine [MeV]");
    h_gEkine2->GetYaxis()->SetTitle("count");
    h_gEkine2->SetAxisRange(0,7000,"Y");

  // position
  Double_t alpide_x = 0.030; //mm
  Double_t alpide_y = 0.015; //mm
  Double_t alpide_z = 0.050; //mm
  Double_t gd_z = 0.030; //mm, max(?)
  Double_t alpide_pos = 0.0100; // get with a function?

  Int_t nbinsz = 100;
  Double_t zlow = -(alpide_pos+alpide_z/2);
  Double_t zup = (alpide_pos+alpide_z/2);
  Int_t nbinsx = 100;
  Double_t xlow = -alpide_x/2;
  Double_t xup = alpide_x/2;

// NOT IMPORTANT
  //TH2D *h_ZX = new TH2D("h_ZX","nCapture profile",100,0.111-0.0005,0.111+0.0005,100,xlow,xup);
  TH2D *h_ZX = new TH2D("h_ZX","nCapture profile",nbinsz,zlow,zup,nbinsx,xlow,xup);
    h_ZX->GetXaxis()->SetTitle("Z [mm]");
    h_ZX->GetYaxis()->SetTitle("X [mm]");

  TString serialID;
  TString previousSerialID = "0";
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++) // loop thorugh all steps
  {
    tree->GetEntry(i);
    serialID = MakeSerialID(trackID,parentID,eventID); // Distinguish between particles
    switch ((Int_t)creatorProcess){
      case 1: processName_= "compt"; break;
      case 2: processName_= "nCapture"; break;
      case 3: processName_= "phot"; break;
      case 4: processName_= "conv"; break;
    }
    switch((Int_t)productionVolume){
      case 1: volumeAtVertex_= "gd"          ; break;
      case 2: volumeAtVertex_= "AlpideSens1" ; break;
      case 3: volumeAtVertex_= "al_1"        ; break;
      case 4: volumeAtVertex_= "AlpideSens2" ; break;
      case 5: volumeAtVertex_= "al_2"        ; break;
    }
    // Filters, particle must be produced in inside foil by nCapture.
    if (serialID == previousSerialID || volumeAtVertex_ != "gd" || !(processName_== "nCapture")){
          // Update IDs and go to next iteration
          previousSerialID = serialID;
          //previouseventID = eventID;
          //previoustrackID = trackID;
          //previousParticleID = particleID;
          continue;
        }
    // store hit coordinate
    h_ZX->Fill(Z,X);
    // Distriguish between sens1 and sens2
    if (Z>0){ // ALPIDE1 position
        // store ekine
        if (particleID == 1) {h_eEkine1->Fill(ekine);}
        if (particleID == 2) {h_gEkine1->Fill(ekine);}
      }
    if (Z<0){ // ALPIDE2 position
        if (particleID == 1) {h_eEkine2->Fill(ekine);}
        if (particleID == 2) {h_gEkine2->Fill(ekine);}
      }
    // store electron position ...

    // Update IDs and go to next iteration
    previousSerialID = serialID;
  }
  Int_t neutrons = 10000;
  Int_t totICE_1 = h_eEkine1->GetEntries();
  Int_t totICE_2 = h_eEkine2->GetEntries();
  Int_t totgamma_1 = h_gEkine1->GetEntries();
  Int_t totgamma_2 = h_gEkine2->GetEntries();

  Double_t w = 1300;
  Double_t h = 700;
  TCanvas *c= new TCanvas("c","Z vs. X", w, h);
  gStyle->SetPalette(kBird);
  h_ZX->Draw("COLZ");

  TCanvas *c1= new TCanvas("c1","eEkine & Z", w, h);
    c1->SetWindowSize(w + (w - c1->GetWw()), h + (h - c1->GetWh()));
    c1->Divide(2,2, 0.01,0.01);

    c1->cd(1); h_eEkine2->Draw();
    c1->cd(2); h_eEkine1->Draw();
    c1->cd(3); h_gEkine2->Draw();
    c1->cd(4); h_gEkine1->Draw();

  cout << "-----------------------" << endl;
  cout << "       Electrons       " << endl;
  cout << "-----------------------" << endl;
  cout << " 1) count: "<< totICE_1 << endl;
  cout << " 2) count: "<< totICE_2 << endl;
  cout << " Sum: "<< totICE_1+totICE_2 << endl;
  cout << " Yield (e-/n): "<<(Float_t) (totICE_1+totICE_2)/neutrons << endl;
  cout << "-----------------------" << endl;
  cout << "       Gammas       " << endl;
  cout << "-----------------------" << endl;
  cout << " 1) count: "<< totgamma_1 << endl;
  cout << " 2) count: "<< totgamma_2 << endl;
  cout << " Sum: "<< totgamma_1+totgamma_2 << endl;
  cout << " Yield (g/n): "<<(Float_t) (totgamma_1+totgamma_2)/neutrons << endl;

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


/// NOT FUNCTIONAL ... yet?
// void getProcessString(Double_t processDouble,TString processString){
//   switch ((Int_t)processDouble){
//     case 1: processString= "compt"; break;
//     case 2: processString= "nCapture"; break;
//     case 3: processString= "phot"; break;
//     case 4: processString= "conv"; break;
//   }
//   cout << "yes" << endl;
//   return;
// }
//
// void getVolumeString(Double_t volumeDouble, TString volumeString){
//   switch((Int_t)volumeDouble){
//     case 1: volumeString= "gd"          ; break;
//     case 2: volumeString= "AlpideSens1" ; break;
//     case 3: volumeString= "al_1"        ; break;
//     case 4: volumeString= "AlpideSens2" ; break;
//     case 5: volumeString= "al_2"        ; break;
//   }
//   return;
// }


//Double_t getDistance(char* filename)
// void getDistance(char* filename)
// {
//   Int_t idist;
//   TString sdist("dist");
//   //Int_t n_char = filename.Sizeof();
//   Int_t n_char = strlen(filename);
//   Int_t i= 0;
//   Int_t j= 0;
//   //const char temp;
//   for (i; i<n_char; i++)
//   {
//     cout << typeid(filename[i]).name() << endl;
//     char str1[0] = filename[i];
//     char str2[] = "_" ;
//     int result = strcmp(str1,str2);
//     if (result == 0){cout << "YES"<<endl;break;} // begining of distance e.g..._100um...
//     //if (strcmp(filename[i], "_") == 0){break;} // begining of distance e.g..._100um...
//     //if (filename[i]== "_"){break;} // begining of distance e.g..._100um...
//   }
//   // for (j=i; j<n_char; j++){
//   //   cout << filename[j] << endl;
//   //   //if (filename[j]== "u"){break;} // end of distance
//   //   if (strcmp(filename[j], "u") == 0){break;} // end of distance
//   // }
//
//   TString sfilename(filename);
//   TString temp = sfilename(i+sdist.Sizeof(),j-1);
//   //const char* cdist[4] = temp.Data();
//
//   //sscanf (cdist,"%d",&idist);
//   //printf ("%d\n",i);
//
//   //ddist = (Double_t)dist_string;
//   cout << temp.Data() << endl;
//   cout << temp << endl;
//   cout << "i: "<<i<<" j: "<<j << endl;
//   cout << "3: "<<sfilename << endl;
//   //cout << idist << endl;
//   return;
// }
