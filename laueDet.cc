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
/// \file exampleED.cc
/// \brief Main program of the ED example

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4EmLivermorePolarizedPhysics.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << "USAGE" << G4endl;
    G4cerr << "Batch mode" << G4endl;
    G4cerr << "laueDet -m macro [-t nThreads]" << G4endl;
    G4cerr << "Interactive mode" << G4endl;
    G4cerr << "laueDet [-t nThreads]" << G4endl;
    G4cerr << "note: -t option is used only in multi-threaded mode." << G4endl;
    G4cerr << G4endl;
  }
}

int main(int argc,char** argv)
{
  G4String macro;
  G4String session;
  G4String physicsListName;
  G4String gdmlFileName;
  G4int nofThreads = 1;
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-t" ) {
      nofThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
    else {
      PrintUsage();
      return 1;
    }
  }

  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv);
  }

// Construct the run manager
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  runManager->SetNumberOfThreads(nofThreads);

// Get the pointer to the User Interface manager
  auto UImanager = G4UImanager::GetUIpointer();
  
  // Set mandatory initialization classes
  //
  // Detector construction

  runManager->SetUserInitialization(new ED::DetectorConstruction());
  UImanager->ApplyCommand("/control/execute detector.mac");

  // Physics list
  //auto physicsList = new FTFP_BERT;
  G4VModularPhysicsList* physicsList = new G4VModularPhysicsList();
  physicsList->RegisterPhysics(new G4EmLivermorePolarizedPhysics());
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new ED::ActionInitialization());

  // Initialize visualization
  //
  auto visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // auto visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  if ( ! ui ) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = macro; //argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
