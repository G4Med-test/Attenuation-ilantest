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
// $Id: PhysicsList.cc 66241 2012-12-13 18:34:42Z gunter $
//

#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"
 
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() 
: G4VModularPhysicsList(),fCutForGamma(0),fCutForElectron(0),fCutForPositron(0),
  fCurrentDefaultCut(0),fEmPhysicsList(nullptr),fEmName("default"),fMessenger(nullptr)
{    
  G4LossTableManager::Instance();
  
  fCurrentDefaultCut   = 1.0*micrometer;
  fCutForGamma         = fCurrentDefaultCut;
  fCutForElectron      = fCurrentDefaultCut;
  fCutForPositron      = fCurrentDefaultCut;

  fMessenger = new PhysicsListMessenger(this);

  SetVerboseLevel(1);

  // EM physics
  //  opt0 as default
  fEmPhysicsList = new G4EmStandardPhysics();
  //AddPhysicsList("emstandard_opt0");
 
 // fix lower limit for cut
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(10*eV, 1*GeV);
  SetDefaultCutValue(1*mm);
}

PhysicsList::~PhysicsList()
{
  delete fMessenger;
}

void PhysicsList::ConstructParticle()
{
 fEmPhysicsList -> ConstructParticle();
}

//#include "G4EmProcessOptions.hh"

void PhysicsList::ConstructProcess()
{
// Roll-out
G4cout << "ConstructProcess: "<<fEmName<< G4endl;
if (fEmName=="default") 
{
   G4ExceptionDescription msg;
    msg << "Physics List has not been changed in UI macro !!!" << G4endl;
    msg << "DO IT!!!!" << G4endl;
    G4Exception("PhysicsList::ConstructProcess()",
    "MyCode0001", FatalException, msg);
}

  // Transportation
  //
 AddTransportation();

 fEmPhysicsList->ConstructProcess();
}

void PhysicsList::AddPhysicsList(const G4String& name)
{
  if (verboseLevel>0) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }
  
  if (name == fEmName) return;

  if (name == "emstandard_opt0"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics();

  } else if (name == "emstandard_opt1"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option1();

  } else if (name == "emstandard_opt2"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option2();
    
  } else if (name == "emstandard_opt3"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option3();
    
  } else if (name == "emstandard_opt4"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmStandardPhysics_option4();
    
  } else if (name == "empenelope"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmPenelopePhysics();
    
  } else if (name == "emlivermore"){
    fEmName = name;
    delete fEmPhysicsList;
    fEmPhysicsList = new G4EmLivermorePhysics();
    
  } else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
  }
  G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is activated"
           << G4endl;
           
G4EmParameters::Instance()->SetGeneralProcessActive(false);
}

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

void PhysicsList::SetCuts()
{ 
  // fixe lower limit for cut
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100*eV, 1*GeV);
  
  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(fCutForGamma, "gamma");
  SetCutValue(fCutForElectron, "e-");
  SetCutValue(fCutForPositron, "e+");
  DumpCutValuesTable();
}

void PhysicsList::SetCutForGamma(G4double cut)
{
  fCutForGamma = cut;
  SetParticleCuts(fCutForGamma, G4Gamma::Gamma());
  DumpCutValuesTable();
}

void PhysicsList::SetCutForElectron(G4double cut)
{
  fCutForElectron = cut;
  SetParticleCuts(fCutForElectron, G4Electron::Electron());
  DumpCutValuesTable();
}

void PhysicsList::SetCutForPositron(G4double cut)
{
  fCutForPositron = cut;
  SetParticleCuts(fCutForPositron, G4Positron::Positron());
  DumpCutValuesTable();
}

