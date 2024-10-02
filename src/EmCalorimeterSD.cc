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
/// \file EmCalorimeterSD.cc
/// \brief Implementation of the EmCalorimeterSD class
//

#include "EmCalorimeterSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4ios.hh"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmCalorimeterSD::EmCalorimeterSD(const G4String& name)
 : G4VSensitiveDetector(name)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmCalorimeterSD::~EmCalorimeterSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmCalorimeterSD::Initialize(G4HCofThisEvent* /*hce*/)
{
  G4String hcName = SensitiveDetectorName + "HitsCollection";

  fHitsCollection
    = new EmCalorimeterHitsCollection(SensitiveDetectorName, hcName);

  // Create a hit for each calorimeter layer
  for (G4int i=1000; i<1100; ++i) {
    auto newHit = new EmCalorimeterHit();
    newHit->SetLayerNumber(i);
    fHitsCollection->insert(newHit);
  }
  for (G4int i=2000; i<2100; ++i) {
    auto newHit = new EmCalorimeterHit();
    newHit->SetLayerNumber(i);
    fHitsCollection->insert(newHit);
  }
  for (G4int i=3000; i<3100; ++i) {
    auto newHit = new EmCalorimeterHit();
    newHit->SetLayerNumber(i);
    fHitsCollection->insert(newHit);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool EmCalorimeterSD::ProcessHits(G4Step* step,
                                    G4TouchableHistory* /*history*/)
{
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();
  if ( edep == 0. ) return false;

  auto touchable = step->GetPreStepPoint()->GetTouchable();
  auto copyNumber = touchable->GetCopyNumber();
  G4int detectorNumber = (G4int)(copyNumber/1000);
  auto arrayLayerNumber = copyNumber - detectorNumber*1000 + (detectorNumber - 1)*100; // must be from 0 to 299

  // Get hit accounting data for this layer
  auto hit = (*fHitsCollection)[arrayLayerNumber];
  if ( hit == nullptr ) {
    G4cerr << "Cannot access hit " << arrayLayerNumber << G4endl;
    exit(1);
  }

  // Add values
  hit->AddEdep(edep);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmCalorimeterSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  //G4cout << "> " <<  fHitsCollection->GetName()
  //       << ": in this event: " << G4endl;

  G4int nofHits = fHitsCollection->entries();
  for ( G4int i=0; i<nofHits; i++ ) {
    (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
