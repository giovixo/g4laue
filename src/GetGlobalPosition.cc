#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "G4VSolid.hh"
#include "G4VisExtent.hh"

#include "GetGlobalPosition.hh"

G4ThreeVector GetGlobalPosition(G4VPhysicalVolume* volume) {
    G4Transform3D totalTransform = G4Transform3D();  // Start with the identity transform

    G4ThreeVector localTranslation = volume->GetObjectTranslation();
    G4RotationMatrix* localRotation = volume->GetObjectRotation();

    G4Transform3D localTransform = G4Transform3D(*localRotation, localTranslation);
    totalTransform = localTransform * totalTransform;

    return totalTransform.getTranslation();    
}




