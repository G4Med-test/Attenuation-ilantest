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
/// \file electromagnetic/TestEm0/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// $Id: RunAction.cc 68243 2013-03-19 18:03:11Z vnivanch $
// 

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4Run.hh"
#include "G4ProcessManager.hh"
#include "G4UnitsTable.hh"
#include "G4Electron.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "WriteOutputFile.hh"

RunAction::RunAction(DetectorConstruction* det, PrimaryGeneratorAction* kin)
:G4UserRunAction(),fDetector(det), fPrimary(kin)
{ 
gammaTransmitted = 0;
numberOfEvents = 0;
}

RunAction::~RunAction()
{ }

void RunAction::BeginOfRunAction(const G4Run*)
{
  //GetCuts();
  gammaTransmitted = 0;
  numberOfEvents = 0;
}

void RunAction::EndOfRunAction(const G4Run* aRun)
{ 
 G4cout << "End of Run" << G4endl;

 numberOfEvents = aRun->GetNumberOfEvent();

 G4double primaryParticleEnergy = fPrimary->GetInitialEnergy();

 G4double absorberMaterialDensity = fDetector->GetDensity();

 G4double targetThickness = fDetector -> GetSize();

 G4cout << "gamma transmitted: " << gammaTransmitted << G4endl;
 G4double gammaTransmittedFraction = (gammaTransmitted/numberOfEvents);

 G4double gammaAttenuationCoefficient = -(std::log(gammaTransmittedFraction))/(targetThickness*absorberMaterialDensity);
 
 WriteOutputFile* output = WriteOutputFile::GetInstance();
 output -> Fill(primaryParticleEnergy/MeV,  gammaAttenuationCoefficient/(cm*cm/g));
} 

void  RunAction::TransmittedGammaNumber()
{
  gammaTransmitted += 1;
 //G4cout << "gamma transmitted " << G4endl;
}



