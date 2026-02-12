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
/// \file electromagnetic/TestEm0/TestEm0.cc
/// \brief Main program of the electromagnetic/TestEm0 example
//
//
// $Id: TestEm0.cc 66241 2012-12-13 18:34:42Z gunter $
//  

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
// ASCII file contains the output of the simulation
#include "WriteOutputFile.hh"
#include <unistd.h> 
 
int main(int argc,char** argv) {
    
  // Construct the  run manager
  auto* runManager = G4RunManagerFactory::CreateRunManager("Serial");

  //CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
 // runManager -> SetRandomNumberStore(true);
   time_t start_time = time(NULL);
                pid_t pid = getpid();
                G4int seed = start_time * (pid << 16);
                //seed =-965476352;
                G4Random::setTheSeed(seed);

  // set mandatory initialization classes
  DetectorConstruction* det;
  PrimaryGeneratorAction* prim;
  runManager->SetUserInitialization(det = new DetectorConstruction);
  runManager->SetUserInitialization(new PhysicsList);
  runManager->SetUserAction(prim = new PrimaryGeneratorAction());
  
  // set user action classes

  WriteOutputFile* output = WriteOutputFile::GetInstance();
   
  RunAction* run = new RunAction(det, prim);
  runManager->SetUserAction(run);

  runManager->SetUserAction(new SteppingAction(prim,run,det));

  // Visualization manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
 
  if (argc!=1)   // batch mode   
    {
     G4String command = "/control/execute ";
     G4String fileName = argv[1];
     G4UImanager::GetUIpointer()->ApplyCommand(command+fileName); 
    }
    
  else           // define UI terminal for interactive mode 
    { 
      G4UIExecutive * ui = new G4UIExecutive(argc,argv);   
      G4UImanager::GetUIpointer()-> ApplyCommand("/control/execute vis.mac");    
      ui->SessionStart();
      delete ui;
    }

  delete visManager;

  output ->  Save();
 
  delete output;
  delete runManager;

  return 0;
}


