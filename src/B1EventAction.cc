//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "B1EventAction.hh"
#include "B1RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::B1EventAction(B1RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.),
  fEkin(0.),
  X(0.),
  Y(0.),
  Z(0.),
  productionVolume(0.),
  creatorProcess(0.),
  trackID(0.),
  parentID(0.),
  particleID(0.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fEkin.clear();
  X.clear();
  Y.clear();
  Z.clear();
  productionVolume.clear();
  creatorProcess.clear();
  trackID.clear();
  parentID.clear();
  particleID.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event*evt)
{
  // accumulate statistics in run action
  fRunAction->AddEdep(fEdep);
  G4int EventID = evt->GetEventID();
  G4AnalysisManager*	man	=	G4AnalysisManager::Instance();

  G4int HitPerEvent = fEkin.size();
  for (int i = 0; i < HitPerEvent; i++) {
    man->FillNtupleDColumn(1,0,EventID);
    man->FillNtupleDColumn(1,1,fEkin[i]);
    man->FillNtupleDColumn(1,2,X[i]);
    man->FillNtupleDColumn(1,3,Y[i]);
    man->FillNtupleDColumn(1,4,Z[i]);
    man->FillNtupleDColumn(1,5,productionVolume[i]);
    man->FillNtupleDColumn(1,6,creatorProcess[i]);
    man->FillNtupleDColumn(1,7,trackID[i]);
    man->FillNtupleDColumn(1,8,parentID[i]);
    man->FillNtupleDColumn(1,9,particleID[i]);

    man->AddNtupleRow(1);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
