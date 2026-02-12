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
/// \file electromagnetic/TestEm0/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

DetectorConstruction::DetectorConstruction()
:G4VUserDetectorConstruction(),
 fworld(nullptr), sBox(nullptr), lBox(nullptr), fBox(nullptr),fMaterial(nullptr),fDetectorMessenger(nullptr), fVacuum(nullptr), fWater(nullptr)
{
  // The thickness of the slab along the X direction is 1. mm by default
  DefineMaterials();
  fMaterial= fWater;
  fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{ delete fDetectorMessenger;}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructVolumes();
}

void DetectorConstruction::DefineMaterials()
{
  G4NistManager* manager = G4NistManager::Instance();
  //
  // define Elements
  //
  G4double z,a;
  
  G4Element* N  = new G4Element("Nitrogen" ,"N" , z= 7., a=  14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"   ,"O" , z= 8., a=  16.00*g/mole);
  //
  // define materials
  //
  G4double density;
  G4int ncomponents;
  G4double fractionmass;  
   
  // Vacuum
  density = universe_mean_density;
  fVacuum = new G4Material("Vacuum", z=1., a=1.01*g/mole,
				      density);  
  G4Material* Air = 
  new G4Material("Air", density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=70.*perCent);
  Air->AddElement(O, fractionmass=30.*perCent);

  fWater = manager->FindOrBuildMaterial("G4_WATER");
}


G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{

// The world is a box filled of vacuum with size 20 km  
  G4Box* sworld = new G4Box("World", 10000.* m,  10000.*m, 10000.*m);
  
  G4LogicalVolume*
    lworld = new G4LogicalVolume(sworld,          //its shape
                                 fVacuum,          //its material
                                 "World", 0, 0, 0);//its name


  fworld = new G4PVPlacement(nullptr,                                //no rotation
                             G4ThreeVector(),                //at (0,0,0)              
                           	 "World", lworld,       		//its name
                           	 0,                                //its mother  volume
                                 false,                        //no boolean operation
                                 0);                                //copy number

  // The world is invisible
  lworld -> SetVisAttributes (G4VisAttributes::GetInvisible());

  // slab 
  sBox = new G4Box("Target",                //its name
                     1.*mm,10000.*m,10000.*m);        //its dimensions // 20 km
                   
  
  lBox = new G4LogicalVolume(sBox,                        //its shape
                               fMaterial,                //its material
                               fMaterial->GetName());        //its name

  fBox = new G4PVPlacement(nullptr,                                //no rotation
                           G4ThreeVector(),                //at (0,0,0)
                           lBox,                        //its logical volume
                           fMaterial->GetName(),        //its name
                           lworld,                                //its mother  volume
                           false,                        //no boolean operation
                           0);                                //copy number
    

 // Visualisation attributes
 // Visualization attributes of the phantom 
  // Define the light blue color
 //G4Colour  lblue   (0.0, 0.0, .75);
 // G4VisAttributes* simpleBoxVisAtt = new G4VisAttributes(lblue);
 // simpleBoxVisAtt -> SetVisibility(true);
 // simpleBoxVisAtt -> SetForceWireframe(true);
 // lBox -> SetVisAttributes(simpleBoxVisAtt);
                      
 //always return the root volume
 //
  PrintParameters();
  return fworld;
}

void DetectorConstruction::PrintParameters()
{
  G4cout << "\n The Box is " << (sBox->GetXHalfLength())*2./mm
         << "mm thick and is made of " << lBox-> GetMaterial()->GetName() << G4endl;
}

#include "G4RunManager.hh"

void DetectorConstruction::SetMaterial(G4String materialChoice)
{
  // search the material by its name, or build it from nist data base
  G4Material* pttoMaterial = 
     G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (pttoMaterial) {
    fMaterial = pttoMaterial;
    if (lBox) lBox -> SetMaterial(pttoMaterial);
  } else {
    G4cout << "\n--> warning from DetectorConstruction::SetMaterial : "
           << materialChoice << " not found" << G4endl;  
  }  

 G4cout << " Target Material: " << lBox-> GetMaterial()->GetName() << G4endl; 
 PrintParameters();
}

void DetectorConstruction::SetThickness(G4double thickness)
{
if (sBox) sBox-> SetXHalfLength(thickness/2.);

if (sBox) G4cout << " The Target thickness is (mm): " << ((sBox -> GetXHalfLength())*2.)/mm << G4endl; 

PrintParameters();
 // G4RunManager::GetRunManager() -> GeometryHasBeenModified();
  //  G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
}

G4double DetectorConstruction::GetDensity()
{

G4double density = lBox-> GetMaterial() -> GetDensity();

return density;
}
G4double DetectorConstruction::GetSize()
{ 
 G4double size = sBox -> GetXHalfLength();
 G4double thickness = 2.* size;
 return thickness;
}      

