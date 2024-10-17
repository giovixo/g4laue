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

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4Event.hh"
#include "G4GenericMessenger.hh"

#include "G4SystemOfUnits.hh"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmCalorimeterSD::EmCalorimeterSD(const G4String& name)
 : G4VSensitiveDetector(name)
{
  // not working (for some reason the kew is read only once)
  fMessenger = new G4GenericMessenger(this, "/log/", "Log control");
  fMessenger->DeclareProperty("Nsteps", fNsteps, "Print the info with intervals Nsteps");
  // working
  std::ifstream inputFile("log.txt");
  if (inputFile.is_open()) {
    inputFile >> fNsteps;
    } else {
       fNsteps = 1; 
    }
  inputFile.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmCalorimeterSD::~EmCalorimeterSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmCalorimeterSD::Initialize(G4HCofThisEvent* /*hce*/)
{
  G4String hcName = SensitiveDetectorName + "HitsCollection";

  fHitsCollection
    = new EmCalorimeterHitsCollection(SensitiveDetectorName, hcName);
  //G4cout << "SensitiveDetectorName: " << SensitiveDetectorName << G4endl;

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

  // Add the value of energy depositi to the hit
  hit->AddEdep(edep);

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmCalorimeterSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  //G4cout << "> " <<  fHitsCollection->GetName()
  //       << ": in this event: " << G4endl;
  
  auto analysisManager = G4AnalysisManager::Instance();
  const G4Event* currentEvent = G4RunManager::GetRunManager()->GetCurrentEvent();
  G4int eventID = currentEvent->GetEventID()+1;
  G4int nofHits = fHitsCollection->entries();
  for ( G4int i=0; i<nofHits; i++ ) {
     //(*fHitsCollection)[i]->Print();
     // Add hits properties in the ntuple
     G4double energyDeposit = ((*fHitsCollection)[i]->GetEdep())/keV;
      if (energyDeposit > 0.) {
        G4int detectorNo = (*fHitsCollection)[i]->GetLayerNumber();
        if(!(eventID % fNsteps)) {
           G4cout << "Event ID " << eventID << " ---> ";
           G4cout << "Hit in the detector " << detectorNo
              << "  Edep = " << std::setw(7) << energyDeposit << " keV" << G4endl;
        }
        analysisManager->FillNtupleIColumn(0, 0, eventID);
        analysisManager->FillNtupleIColumn(0, 1, detectorNo);
        analysisManager->FillNtupleDColumn(0, 2, energyDeposit);
        analysisManager->AddNtupleRow(0);
        // Increment the row count
        fNtupleRowCount++;
      }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
