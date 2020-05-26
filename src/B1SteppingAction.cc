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
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class

#include "B1SteppingAction.hh"
#include "B1EventAction.hh"
#include "B1DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::B1SteppingAction(G4String volume, B1EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume0(0),
  fScoringVolume1(0),
  fScoringVolume2(0),
  scoringVolume(volume)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1SteppingAction::~B1SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume0) {
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume0 = detectorConstruction->GetScoringVolume(0);
  }

  if (!fScoringVolume1) {
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume1 = detectorConstruction->GetScoringVolume(1);
  }

  if (!fScoringVolume2) {
    const B1DetectorConstruction* detectorConstruction
      = static_cast<const B1DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume2 = detectorConstruction->GetScoringVolume(2);
  }

  // get volume of the current step
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  if (scoringVolume == "foil")
  {
      if ((volume!=fScoringVolume0)) return; //track inside converter foil
  }
  if (scoringVolume == "sens")
  {
    if ((volume!=fScoringVolume1) && (volume!=fScoringVolume2) ) return; // track ALPIDE1 and ALPIDE2
  }


  G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  if(particleName== "e-" || particleName=="gamma"){
    G4double x;
    G4double y;
    G4double z;
    // Converter foil
    if (scoringVolume == "foil")
    {
     x = step->GetTrack()->GetVertexPosition().getX();
     y = step->GetTrack()->GetVertexPosition().getY();
     z = step->GetTrack()->GetVertexPosition().getZ();
    }
    // Sens
    if (scoringVolume == "sens")
    {
     x = step->GetTrack()->GetPosition().getX();
     y = step->GetTrack()->GetPosition().getY();
     z = step->GetTrack()->GetPosition().getZ();
    }

    //G4double eKin = step->GetTrack()->GetKineticEnergy(); // step energy
    G4double eKin = step->GetTrack()->GetVertexKineticEnergy(); // descrete spectrum
    G4double track_id = (G4double)step->GetTrack()->GetTrackID();
    G4double step_id = step->GetTrack()->GetCurrentStepNumber();
    //G4cout << step_id << G4endl;
    G4double parent_id = (G4double)step->GetTrack()->GetParentID();
    G4String volumeAtVertex_ = step->GetTrack()->GetLogicalVolumeAtVertex()->GetName();
    G4String processName_ = step->GetTrack()->GetCreatorProcess()->GetProcessName();

    G4double processName = 0.0;     // default
    if (processName_== "compt")     {processName = 1;}
    if (processName_== "nCapture")  {processName = 2;}
    if (processName_== "phot")      {processName = 3;}
    if (processName_== "conv")      {processName = 4;}

    G4double volumeAtVertex = 0.0;
    if (volumeAtVertex_== "gd")               {volumeAtVertex =  1;}
    if (volumeAtVertex_== "AlpideSens1")      {volumeAtVertex =  2;}
    if (volumeAtVertex_== "al_1")             {volumeAtVertex =  3;}
    if (volumeAtVertex_== "AlpideSens2")      {volumeAtVertex =  4;}
    if (volumeAtVertex_== "al_2")             {volumeAtVertex =  5;}

    G4double particle_id = 0.0; // default
    if (particleName == "neutron"){particle_id = 0;}
    if (particleName == "e-")     {particle_id = 1;}
    if (particleName == "gamma")  {particle_id = 2;}

    G4cout<< particleName <<"  "<<track_id<<" "<<step_id<<"  "<<eKin <<"  "<< processName_<<"  "<<volumeAtVertex_<< G4endl;
    // Store step information
    fEventAction->StoreKineticEnergy(eKin);
    fEventAction->StoreX(x);
    fEventAction->StoreY(y);
    fEventAction->StoreZ(z);
    fEventAction->StoreProductionVolume(volumeAtVertex);
    fEventAction->StoreCreatorProcess(processName);
    fEventAction->StoreTrackID(track_id);
    fEventAction->StoreStepID(step_id);
    fEventAction->StoreParentID(parent_id);
    fEventAction->StoreParticleID(particle_id);

  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
