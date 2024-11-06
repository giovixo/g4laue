
#ifndef PrimaryGeneratorAction_H
#define PrimaryGeneratorAction_H 1

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ThreeVector.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"

// Mandatory user class that defines the properties of the
// particle gun and run initialization
// Derived from the G4VUserPrimaryGeneratorAction initialisation
// abstract base class.

class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();		// Constructor
    virtual ~PrimaryGeneratorAction();					// Destructor
    
public:
    // This method generates the primary particles
    void GeneratePrimaries(G4Event* anEvent);
    
    
private:
    G4ParticleGun* particleGun;
    void GenerateDirectionInCone(G4double coneAngle, G4ThreeVector& direction);
};

#endif


