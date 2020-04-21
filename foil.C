TString MakeSerialID(Double_t trackID, Double_t parentID, Double_t eventID);


int foil(char* filename)
{
  TFile *f = TFile::Open(filename);
  //TFile *f = new TFile("../B1-build/Electrons.root");

  TTree *tree = (TTree*) f->Get("ScorerKin");

  cout << "reading from "<< f->GetName()<<"..."<<endl;
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

  //tree->Scan("particleID:productionVolume:creatorProcess:EventID:parentID:trackID:stepID:KineticEnergy:X:Y:Z");

  Double_t thick = 0.025; // 5um-25um --> 0.005mm-0.025mm

  /// HISTOGRAMS:
  // nCapture profile
  Int_t bin_nmbr = 100;
  TH1F *h_Z = new TH1F("h_Z","nCapture profile",bin_nmbr,-(thick/2+0.005),thick/2+0.005);
    h_Z->GetXaxis()->SetTitle("Z [mm]");
    h_Z->GetYaxis()->SetTitle("count");
  // Kinetic energy
  TH1F *h_eEkine = new TH1F("h_eEkine","eEkine",100,0,0.5); //bin_nmbr = 100
    h_eEkine->GetXaxis()->SetTitle("ekine [MeV]");
    h_eEkine->GetYaxis()->SetTitle("count");
    //h_eEkine->SetAxisRange(0,600,"Y");
    h_eEkine->SetFillColor(40);
    h_eEkine->SetLineColor(40);
    h_eEkine->SetFillStyle(3016);
  TH1F *h_gEkine = new TH1F("h_gEkine","gEkine",100,0,5);
    h_gEkine->GetXaxis()->SetTitle("ekine [MeV]");
    h_gEkine->GetYaxis()->SetTitle("count");

  Int_t count = 0;
  Int_t cout_dead = 0;
  Int_t count_gamma = 0;
  Int_t count_tot_gamma = 0;
  Int_t count_ICE = 0;
  Int_t count_nCapture = 0;
  TString serialID;
  TString previousSerialID = "0";
  Double_t previouseventID = 0;
  Double_t previousParticleID = 0;
  Double_t previoustrackID = 0;
  //cout << "eventID | parentID | trackID | stepID |particleID |  ekine | X,Y,Z | productionVolume | creatorProcess"<<endl;
  Int_t n = (Int_t) tree->GetEntries();
  for (Int_t i = 0; i < n ; i++) // loop thorugh all steps
  {
      tree->GetEntry(i);

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


      // track individual particles, not steps
      // serialID = MakeSerialID(trackID,parentID,eventID);
      // if (serialID == previousSerialID) {previousSerialID = serialID; continue;}
      // // must be created in gd
      // if (volumeAtVertex_ != "gd")      {previousSerialID = serialID; continue;}
      // // must be created by nCapture
      // if (!(processName_== "nCapture")) {previousSerialID = serialID; continue;}


      // Filters, particle must be produced in inside foil by nCapture.
      serialID = MakeSerialID(trackID,parentID,eventID);
      if (serialID == previousSerialID || volumeAtVertex_ != "gd" || !(processName_== "nCapture"))
          {
            // Update IDs and go to next iteration
            previouseventID = eventID;
            previousSerialID = serialID;
            previoustrackID = trackID;
            previousParticleID = particleID;
            continue;
          }

      // nCapture profile. Fill once per event.
      if (previouseventID != eventID)
      {
        h_Z->Fill(Z);
        // THESE STATISTICS ARE WRONG. one nuclei can undergo several gamma decays and internal conversions.
        if (particleID == 1) {count_ICE++;}
        if (particleID == 2) {count_gamma++;} //count nCapture resulting in gamma
      }
      //  ekine
      if (particleID == 1) {h_eEkine->Fill(ekine);} // electron
      if (particleID == 2) {h_gEkine->Fill(ekine);} // gamma


      //if (!(processName_== "nCapture")) {continue;}
      //if (!(particleID== 1)) {continue;}
      //if (!(volumeAtVertex_== "gd")) {continue;}

      // if (eventID != previouseventID) {cout<< "\n"<<endl;}
      // if (particleID==1 && trackID != previoustrackID && previousParticleID == 1) {cout<< "- - - - - - - - - - - - - - - - - - - - - - - - "<<endl;}
      // if (particleID==1)
      // {cout <<"e-: "<<  eventID <<" " <<parentID <<" " <<trackID <<" "<<stepID<<" "<<ekine<<" ("<<X<<","<<Y<<","<<Z<<") " << productionVolume <<" "<< creatorProcess<<endl;}
      // if (particleID==2)
      // {cout <<"g: "<<  eventID <<" " <<parentID <<" " <<trackID <<" "<<stepID<<" "<<ekine<<" ("<<X<<","<<Y<<","<<Z<<") " << productionVolume <<" "<< creatorProcess<<endl;}


      // Update IDs
      previouseventID = eventID;
      previousSerialID = serialID;
      //
      // previousEventID = eventID;
      previoustrackID = trackID;
      previousParticleID = particleID;
      count++;
    }
    Int_t neutrons =10000;
    Int_t nCaptures = h_Z->GetEntries();
    Int_t totICE = h_eEkine->GetEntries();
    Int_t totGamma = h_gEkine->GetEntries();

    Double_t integral = 0;
    Double_t z_60 = 0;
    for (Int_t i = 2; i <= bin_nmbr; i++) // iterate through and integrate histogram bins
    {
      integral = h_Z->Integral(1,i);
      if (integral/nCaptures >= 0.6)
      {
        z_60 = thick/2 + (h_Z->GetBinCenter(i));
        break;
      }
    }

    //h_gEkine->SetAxisRange(0,5000,"Y");
    h_gEkine->GetYaxis()->SetTitleOffset(1.4);
    h_gEkine->SetFillColor(46);
    h_gEkine->SetLineColor(46);
    h_gEkine->SetFillStyle(3016);

    //  	TPad (const char *name, const char *title, Double_t xlow, Double_t ylow, Double_t xup, Double_t yup, Color_t color=-1, Short_t bordersize=-1, Short_t bordermode=-2)


    Double_t w = 1300;
    Double_t h = 500;
    TCanvas *c1= new TCanvas("c1","eEkine & Z", w, h);
      c1->SetWindowSize(w + (w - c1->GetWw()), h + (h - c1->GetWh()));
      c1->Divide(3,1, 0.01,0.01);

    c1->cd(1); h_Z->Draw();
    c1->cd(2); h_eEkine->Draw();
    c1->cd(3); h_gEkine->Draw();


    cout << "count: " <<count << endl;
    cout << "Incident neutrons: "<<neutrons << endl;
    cout << "nCaptures: " <<nCaptures<< endl;
    cout << "Z_60% nCapture: " <<z_60<< "mm" <<endl;
    cout << "Total ICE: " <<totICE<< endl;
    cout << "Total gamma: " <<totGamma<< endl;


    // - - - - - - - - - - - - - - - - - - - - -
    // SAVE canvas and text output

    char pdfname[500];
    strcpy(pdfname,filename);
    strcat(pdfname,".pdf");
    cout <<"filename:"<< filename<<endl;
    cout <<"saving pdf as:"<< pdfname << "..."<<endl;
    c1->SaveAs(pdfname);

    char txtname[500];
    strcpy(txtname,filename);
    strcat(txtname,".txt");
    ofstream myfile;
    myfile.open (txtname);
      myfile << "count: " <<count << "\n";
      myfile << "Incident neutrons: "<<neutrons << "\n";
      myfile << "nCaptures: " <<nCaptures<< "\n";
      myfile << "Z_60% nCapture: " <<z_60<< "mm" << "\n";
      myfile << "Total ICE: " <<totICE<< "\n";
      myfile << "Total gamma: " <<totGamma << "\n";
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
