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
// $Id$
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "EmCalorimeterSD.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"

#include "G4GDMLParser.hh"

#include "GetGlobalPosition.hh"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{ 
  fMessenger = new G4GenericMessenger(this, "/detector/", "Detector contruction");
  fMessenger->DeclareProperty("detAsizeZ", detAsizeZ, "Thickness of the detector A");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // --- MATERIALS DEFINITION ---
  // Get nist material manager
  auto nistManager = G4NistManager::Instance();

  G4double a;     // Atomic mass
  G4double z;     // Atomic number
  G4double density; // Density
  G4int nel;      // Number of elements in a compound
  //G4int natoms;       // Number of atoms in a compound
  //G4double fractionmass;
  //G4int ncomponents;

  // Vacuum
  G4Element*  H  = new G4Element("Hydrogen"  , "H" , z = 1. , a =  1.008*g/mole);
  G4Material* vacuum = new G4Material("Vacuum", density = 1.e-25*g/cm3, nel = 1);
  vacuum -> AddElement(H, 100*perCent);

  //  Silicon
  auto silicon = nistManager->FindOrBuildMaterial("G4_Si");

  // CZT 
  G4Element* elCd = nistManager->FindOrBuildElement("Cd");
  G4Element* elZn = nistManager->FindOrBuildElement("Zn");
  G4Element* elTe = nistManager->FindOrBuildElement("Te");
  // Typical density of CZT
  G4Material* CZT = new G4Material("CZT", density = 5.78 * g/cm3, 3);
  CZT->AddElement(elCd, 9);  // 9 Cd atoms
  CZT->AddElement(elZn, 1);  // 1 Zn atom
  CZT->AddElement(elTe, 10); // 10 Te atoms (equivalent to 1 Te atom per Cd+Zn unit)

  // Print all materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  // Define the output file for the lookup table
  G4String filename = "lookup_table.txt";
  std::ofstream lookupTable(filename);
  // Check if the file was successfully opened
  if (!lookupTable.is_open()) {
        G4cerr << "Error: Could not open the file for writing!" << G4endl;
  }
  lookupTable << "# Look up table (det,x,y,z) x,y,z in cm" << G4endl;
  lookupTable << "# detID  x(cm) y(cm) z(cm)" << G4endl;

  // --- VOLUMES DEFINITIONS ---
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double hx = 2.*m;
  G4double hy = 2.*m;
  G4double hz = 4.*m;

  // world volume
  auto worldS = new G4Box("World", hx, hy, hz);

  auto worldLV = new G4LogicalVolume(worldS, vacuum, "World");

  auto worldPV = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      worldLV,               //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking


  // detector A
  G4double detAx = 0.5*m; // half size
  G4double detAy = 0.5*m;
  G4double detAz = detAsizeZ/2.*cm; //0.5*cm;
  auto detectorAS = new G4Box("detectorAS", detAx, detAy, detAz);
  auto detectorALV = new G4LogicalVolume(detectorAS, silicon, "detectorA");

  auto detectorAPV = new G4PVPlacement(0,
                    G4ThreeVector(0, 0, -20.*cm),
                    detectorALV,          //its logical volume
                    "detectorA",            //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking
  G4ThreeVector detectorPositionLevel1 = GetGlobalPosition(detectorAPV);
  G4cout << "Detector position level 1: " << detectorPositionLevel1/cm << " cm" << G4endl;

  hx = detAx/10.;
  hy = detAy/10.;
  hz = detAz;
  auto detectorUnitAS = new G4Box("detectorUnitAS", hx, hy, hz);
  auto detectorUnitALV = new G4LogicalVolume(detectorUnitAS, silicon, "detectorUnitA");

  G4double xPos, yPos, zPos;
  for (G4int i=0; i<10; ++i) {
    for (G4int j=0; j<10; ++j) {
      xPos = -0.9*detAx + 2*i*hx;
      yPos = -0.9*detAy + 2*j*hy;
      zPos = 0.;
      auto detectorUnitAPV = new G4PVPlacement(0,
                    G4ThreeVector(xPos, yPos, zPos),
                    //G4ThreeVector(0, 0, 0),
                    detectorUnitALV,          //its logical volume
                    "detectorUnitA",            //its name
                    detectorALV,               //its mother  volume
                    false,                 //no boolean operation
                    1000 + i*10+j,                     //copy number
                    checkOverlaps);        //overlaps checking
      G4ThreeVector detectorPositionLevel2 = GetGlobalPosition(detectorUnitAPV);
      G4ThreeVector detectorPos = detectorPositionLevel1 + detectorPositionLevel2;
      // Debug
      G4cout << "Detector position (level 1+2): " << detectorPos/cm << " cm" << G4endl;
      // Print out the detector number and the position relative to worldLV
      G4double detectorPosX = detectorPos[0];
      G4double detectorPosY = detectorPos[1];
      G4double detectorPosZ = detectorPos[2];
      lookupTable << 1000 + i*10+j << "," << detectorPosX/cm << "," << detectorPosY/cm << "," << detectorPosZ/cm << G4endl;
    }
  }

  // detector B
  G4double detBx = 0.5*m; // half size
  G4double detBy = 0.5*m;
  G4double detBz = 1.*cm;
  auto detectorBS = new G4Box("detectorBS", detBx, detBy, detBz);
  auto detectorBLV = new G4LogicalVolume(detectorBS, CZT, "detectorB");

  auto detectorBPV =new G4PVPlacement(0,
                    G4ThreeVector(0, 0, +20.*cm),
                    detectorBLV,          //its logical volume
                    "detectorB",            //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking
  detectorPositionLevel1 = GetGlobalPosition(detectorBPV);
  G4cout << "Detector position level 1: " << detectorPositionLevel1/cm << " cm" << G4endl;

  hx = detBx/10.;
  hy = detBy/10.;
  hz = detBz;
  auto detectorUnitBS = new G4Box("detectorUnitBS", hx, hy, hz);
  auto detectorUnitBLV = new G4LogicalVolume(detectorUnitBS, CZT, "detectorUnitB");

  for (G4int i=0; i<10; ++i) {
    for (G4int j=0; j<10; ++j) {
      xPos = -0.9*detBx + 2*i*hx;
      yPos = -0.9*detBy + 2*j*hy;
      zPos = 0.;
      auto detectorUnitBPV = new G4PVPlacement(0,
                    G4ThreeVector(xPos, yPos, zPos),
                    //G4ThreeVector(0, 0, 0),
                    detectorUnitBLV,          //its logical volume
                    "detectorUnitB",            //its name
                    detectorBLV,               //its mother  volume
                    false,                 //no boolean operation
                    2000 + i*10+j,                     //copy number
                    checkOverlaps);        //overlaps checking
      G4ThreeVector detectorPositionLevel2 = GetGlobalPosition(detectorUnitBPV);
      G4ThreeVector detectorPos = detectorPositionLevel1 + detectorPositionLevel2;
      // Debug
      G4cout << "Detector position (level 1+2): " << detectorPos/cm << " cm" << G4endl;
      // Print out the detector number and the position relative to worldLV
      G4double detectorPosX = detectorPos[0];
      G4double detectorPosY = detectorPos[1];
      G4double detectorPosZ = detectorPos[2];
      lookupTable << 2000 + i*10+j << "," << detectorPosX/cm << "," << detectorPosY/cm << "," << detectorPosZ/cm << G4endl;
    }

  }


  // Detector C
  G4double rmin = 95.*cm;
  G4double rmax = 100.*cm;
  hz = 30.*cm;
  G4double phimin = 0.;
  G4double dphi = 360.*deg;
  auto detectorCS = new G4Tubs("detectorC", rmin, rmax, hz, phimin, dphi);
  auto detectorCLV = new G4LogicalVolume(detectorCS, CZT, "detectorC");

  new G4PVPlacement(0,
                    G4ThreeVector(),       //at (0,0,0)
                    detectorCLV,                //its logical volume
                    "detectorC",                //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

  rmin = 95.*cm;
  rmax = 100.*cm;
  hz = 30.*cm;
  phimin = 0.*deg;
  dphi = 3.6*deg;
  auto detectorUnitCS = new G4Tubs("detectorUnitC", rmin, rmax, hz, phimin, dphi);
  auto detectorUnitCLV = new G4LogicalVolume(detectorUnitCS, CZT, "detectorUnitC");

  for (G4int i=0; i<100; ++i) { 
    G4ThreeVector detectorPos(100.0*cm, 0.0, 0.0);
    G4RotationMatrix* rotationMatrix = new G4RotationMatrix;
    rotationMatrix->rotateZ(i*dphi);
    detectorPos[0] = 100.*cm * std::cos(dphi/2. + i*dphi);
    detectorPos[1] = 100.*cm * std::sin(dphi/2. + i*dphi);
    new G4PVPlacement(rotationMatrix,
                    G4ThreeVector(),       //at (0,0,0)
                    detectorUnitCLV,                //its logical volume
                    "detectorUnitC",                //its name
                    detectorCLV,               //its mother  volume
                    false,                 //no boolean operation
                    3000+i,                     //copy number
                    checkOverlaps);        //overlaps checking
    // Debug
    G4cout << "Detector position (level 1+2): " << detectorPos/cm << " cm" << G4endl;
    lookupTable << 3000 + i << "," << detectorPos[0]/cm << "," << detectorPos[1]/cm << "," << detectorPos[2]/cm << G4endl;
  }
  
  // close the lookup table file
  lookupTable.close();
  G4cout << ">>> Lookup table file " << filename << " written succesfully" << G4endl;

  //always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  //
  // Sensitive detectors
  ///
  auto detectorASD = new EmCalorimeterSD("detectorASD");
  G4SDManager::GetSDMpointer()->AddNewDetector(detectorASD);
  SetSensitiveDetector("detectorUnitA", detectorASD);

  auto detectorBSD = new EmCalorimeterSD("detectorBSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(detectorBSD);
  SetSensitiveDetector("detectorUnitB", detectorBSD);

  auto detectorCSD = new EmCalorimeterSD("detectorCSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(detectorCSD);
  SetSensitiveDetector("detectorUnitC", detectorCSD);
}

}
