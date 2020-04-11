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
/// \file B1EventAction.hh
/// \brief Definition of the B1EventAction class

#ifndef B1EventAction_h
#define B1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>


class B1RunAction;

/// Event action class
///

class B1EventAction : public G4UserEventAction
{
  public:
    B1EventAction(B1RunAction* runAction);
    virtual ~B1EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);

    void AddEdep(G4double edep) { fEdep += edep; }
    void StoreKineticEnergy(G4double ekin) { fEkin.push_back(ekin); }
    void StoreX(G4double x) { X.push_back(x); }
    void StoreY(G4double y) { Y.push_back(y); }
    void StoreZ(G4double z) { Z.push_back(z); }
    void StoreProductionVolume( G4double  volumeAtVertex) {productionVolume.push_back(volumeAtVertex);}
    void StoreCreatorProcess( G4double  processName) {creatorProcess.push_back(processName); }
    void StoreTrackID( G4double track_id) {trackID.push_back(track_id); }
    void StoreParentID( G4double parent_id) {parentID.push_back(parent_id); }
    void StoreParticleID( G4double particle_id) {particleID.push_back(particle_id); }


  private:
    B1RunAction* fRunAction;
    G4double     fEdep;

    std::vector<G4double> fEkin;
    std::vector<G4double> X;
    std::vector<G4double> Y;
    std::vector<G4double> Z;
    std::vector<G4double> productionVolume;
    std::vector<G4double> creatorProcess;
    std::vector<G4double> trackID;
    std::vector<G4double> parentID;
    std::vector<G4double> particleID;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
