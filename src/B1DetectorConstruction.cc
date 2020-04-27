// Mine
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
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

// MY CODE - Start
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::B1DetectorConstruction(G4double distance, G4double thickness)
: G4VUserDetectorConstruction(),
  fScoringVolume1(0),
  fScoringVolume2(0)
{
  alpide_pos = distance*um;
  gd_z = thickness*um;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Colors
  //
  G4Colour red(1.0, 0.0, 0.0);
  G4Colour green(0.0, 1.0, 0.0);
  G4Colour white(1.0, 1.0, 1.0);

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  // Test paramteters

      /// Distance
      //
      //G4double alpide_pos = 100*um; //Test parameter
      // G4double alpide_pos = 200*um; //Test parameter
      // G4double alpide_pos = 500*um; //Test parameter

      /// Thickness
      //
      // G4double gd_z = 5*um; //Test parameter 1
      //G4double gd_z = 10*um; //Test parameter 2 *
      // G4double gd_z = 50*um; //Test parameter 3
      // G4double gd_z = 50*um; //Test parameter 4
      // G4double gd_z = 50*um; //Test parameter 5

  // Dimentions
  G4double world_sizeXYZ = 30 *cm;
  G4double alpide_x = 30.0*mm;
  G4double alpide_y = 15.0*mm;
  G4double alpide_z = 50.0*um;
  G4double al_z = 25*um;
  //G4double gd_z = 10*um; // Default*


  // Positions
  //G4double alpide_pos = 500*um; //Default?*
  G4double sensitive_pos = 1.5*um;
  G4double al_pos = 19.5*um;
  // Materials
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* Silicon = nist->FindOrBuildMaterial("G4_Si");
  G4Material* Aluminium = nist->FindOrBuildMaterial("G4_Al");
  //G4Material* converter_mat = nist->FindOrBuildMaterial("G4_Gd");
  // density calc. -----------------
  //G4double z, a_155, a_157;
  // G4double density_155,density_157;
  // density_155 = 6.98855*g/cm3; // Gd-155
  //  density_157 = 7.07873*g/cm3; // Gd-157
  // a_155 = 154.92*g/mole; // Gd-155
  // a_157 = 156.92*g/mole; // Gd-157
  // G4Material* Gd_155 = new G4Material("Gd_155", z=64., a_155, density_155);
  // G4Material* Gd_157 = new G4Material("Gd_157", z=64., a_157, density_157);

  //converter_mat = Gd_155;
  //converter_mat = Gd_157;

  // G4ISOTOPE -----------------
    G4int z, a, ncomp;
    G4double density_155,density_157, abundance, density, fractionmass;
    density_155 = 6.98855*g/cm3; //Gd-155
    density_157 = 7.07873*g/cm3; //Gd-157
    G4Element* GDisotope = new G4Element("Gdiso","GD",ncomp=1);
    //G4Isotope* gd155 = new G4Isotope("GD155", z=64, a=155., 154.92*g/mole);
    //GDisotope->AddIsotope(gd155, abundance=100.0*perCent); density = density_155;
    G4Isotope* gd157 = new G4Isotope("GD157", z=64, a=157., 156.92*g/mole);
    GDisotope->AddIsotope(gd157, abundance=100.0*perCent); density = density_157;
    G4Material* converter_mat = new G4Material("Gadolinium",density, 1, kStateSolid);
    converter_mat->AddElement(GDisotope, fractionmass = 1 );

  G4Box* solidWorld =
    new G4Box("World",                       //its name
       0.5*world_sizeXYZ, 0.5*world_sizeXYZ, 0.5*world_sizeXYZ);     //its size

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  //
  // Alpide 1
  //

   G4Box* solidAlpide1
      = new G4Box("alpide1",          //its name
                  0.5*alpide_x,       // its size
                  0.5*alpide_y,
                  0.5*alpide_z);
    G4LogicalVolume* logicAlpide1 =
      new G4LogicalVolume(solidAlpide1,         //its solid
                          Silicon,          //its material
                                    "alpide1");           //its name

      new G4PVPlacement(0,                       //no rotation
                        G4ThreeVector(0,0,alpide_pos),                    //at position
                        logicAlpide1,             //its logical volume
                        "alpide1",                //its name
                        logicWorld,                //its mother  volume
                        false,                   //no boolean operation
                        0,                       //copy number
                        checkOverlaps);          //overlaps checking

      G4VisAttributes* alpide1VisAttributes= new G4VisAttributes(red);
      logicAlpide1->SetVisAttributes(alpide1VisAttributes);

      // Alpide sensitive layer 1
      //
      G4Box* solidAlpideSens1 =
        new G4Box("AlpideSens1",                    //its name
            0.5*alpide_x, 0.5*alpide_y, 0.5*0.5*alpide_z); //its size half size of ALPIDE

      G4LogicalVolume* logicAlpideSens1 =
        new G4LogicalVolume(solidAlpideSens1,            //its solid
                            Silicon,             //its material
                            "AlpideSens1");         //its name

      new G4PVPlacement(0,                       //no rotation
                        G4ThreeVector(0.,0.,-sensitive_pos),
                        logicAlpideSens1,                //its logical volume
                        "AlpideSens1",              //its name
                        logicAlpide1,              //its mother  volume
                        false,                   //no boolean operation
                        0,                       //copy number
                        checkOverlaps);          //overlaps checking

      // Al layer 1
      //
       G4Box* solidAl_1
          = new G4Box("al_1",        //its name
                      0.5*alpide_x,       // its size
                      0.5*alpide_y,
                      0.5*al_z);
        G4LogicalVolume* logicAl_1 =
          new G4LogicalVolume(solidAl_1,           //its solid
                              Aluminium,            //its material
                              "al_1");             //its name

          new G4PVPlacement(0,                     //no rotation
                            G4ThreeVector(0,0,-al_pos),               //at position
                            logicAl_1,             //its logical volume
                            "al_1",                //its name
                            logicAlpide1,          //its mother  volume
                            false,                 //no boolean operation
                            0,                     //copy number
                            checkOverlaps);        //overlaps checking

          G4VisAttributes* al_1VisAttributes= new G4VisAttributes(green);
          logicAl_1->SetVisAttributes(al_1VisAttributes);

      //
      // Alpide 2
      //
       G4Box* solidAlpide2
          = new G4Box("alpide2",          //its name
                      0.5*alpide_x,       // its size
                      0.5*alpide_y,
                      0.5*alpide_z);
        G4LogicalVolume* logicAlpide2 =
          new G4LogicalVolume(solidAlpide2,         //its solid
                              Silicon,          //its material
                                        "alpide2");           //its name

          new G4PVPlacement(0,                       //no rotation
                            G4ThreeVector(0,0,-alpide_pos),                    //at position
                            logicAlpide2,             //its logical volume
                            "alpide2",                //its name
                            logicWorld,                //its mother  volume
                            false,                   //no boolean operation
                            0,                       //copy number
                            checkOverlaps);          //overlaps checking

          G4VisAttributes* alpide2VisAttributes= new G4VisAttributes(red);
          logicAlpide2->SetVisAttributes(alpide2VisAttributes);

          // Alpide sensitive layer 2
          //
          G4Box* solidAlpideSens2 =
            new G4Box("AlpideSens2",                    //its name
                0.5*alpide_x, 0.5*alpide_y, 0.5*0.5*alpide_z); //its size half size of ALPIDE

          G4LogicalVolume* logicAlpideSens2 =
            new G4LogicalVolume(solidAlpideSens2,            //its solid
                                Silicon,             //its material
                                "AlpideSens2");         //its name

          new G4PVPlacement(0,                       //no rotation
                            G4ThreeVector(0.,0.,sensitive_pos),
                            logicAlpideSens2,                //its logical volume
                            "AlpideSens2",              //its name
                            logicAlpide2,              //its mother  volume
                            false,                   //no boolean operation
                            0,                       //copy number
                            checkOverlaps);          //overlaps checking

          // Al layer 2
          //
           G4Box* solidAl_2
              = new G4Box("al_2",        //its name
                    0.5*alpide_x,       // its size
                    0.5*alpide_y,
                    0.5*al_z);
            G4LogicalVolume* logicAl_2 =
              new G4LogicalVolume(solidAl_2,           //its solid
                                  Aluminium,            //its material
                                  "al_2");             //its name

              new G4PVPlacement(0,                     //no rotation
                                G4ThreeVector(0,0,al_pos),               //at position
                                logicAl_2,             //its logical volume
                                "al_2",                //its name
                                logicAlpide2,          //its mother  volume
                                false,                 //no boolean operation
                                0,                     //copy number
                                checkOverlaps);        //overlaps checking

              G4VisAttributes* al_2VisAttributes= new G4VisAttributes(green);
              logicAl_2->SetVisAttributes(al_2VisAttributes);

  //
  // Gd foil
  //
   G4Box* solidGd
      = new G4Box("gd",               //its name
                  0.5*alpide_x,       // its size
                  0.5*alpide_y,
                  0.5*gd_z);
    G4LogicalVolume* logicGd =
      new G4LogicalVolume(solidGd,               //its solid
                          converter_mat,         //its material
                                    "gd");       //its name

      new G4PVPlacement(0,                       //no rotation
                        G4ThreeVector(0, 0, 0),  //at position
                        logicGd,                 //its logical volume
                        "gd",                    //its name
                        logicWorld,              //its mother  volume
                        false,                   //no boolean operation
                        0,                       //copy number
                        checkOverlaps);          //overlaps checking


      G4VisAttributes* gdVisAttributes= new G4VisAttributes(white);
      logicGd->SetVisAttributes(gdVisAttributes);


    // Set scoring volume
    fScoringVolume2 = logicAlpideSens2;
    fScoringVolume1 = logicAlpideSens1;
    fScoringVolume0 = logicGd;



  //
  //always return the physical World
  //
  return physWorld;
}

G4LogicalVolume* B1DetectorConstruction::GetScoringVolume(G4int i) const {
  if (i == 0) return fScoringVolume0;
  if (i == 1) return fScoringVolume1;
  if (i == 2) return fScoringVolume2;
  else;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
