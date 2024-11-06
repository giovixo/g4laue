#include "PrimaryGeneratorAction.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

//using namespace CLHEP;

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    particleGun = new G4ParticleGun(1); // 1 particle per event

    // Get particle definition
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");

    // Set particle properties
    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticleEnergy(200. * keV); // Set your desired energy
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particleGun;
}

void PrimaryGeneratorAction::GenerateDirectionInCone(G4double coneAngle, G4ThreeVector& direction) {
    // Generate random direction within cone
    G4double phi = G4UniformRand() * 2.0 * CLHEP::pi;
    //G4double cosTheta = 1.0 - G4UniformRand() * (1.0 - std::cos(coneAngle));
    G4double cosTheta = std::cos(coneAngle);
    //G4double cosTheta = std::cos(coneAngle) + 0.0005 * G4UniformRand(); 
    G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

    direction.setX(sinTheta * std::cos(phi));
    direction.setY(sinTheta * std::sin(phi));
    direction.setZ(cosTheta);   
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    // Set source position
    G4ThreeVector position(0.0, 0.0, -300.0*cm); // Origin, modify as needed
    particleGun->SetParticlePosition(position);

    // Generate direction within cone
    G4double coneAngle = 3.0 * deg; // Half-angle of the cone
    G4ThreeVector direction;
    GenerateDirectionInCone(coneAngle, direction);
    particleGun->SetParticleMomentumDirection(direction);

    // Generate the particle
    particleGun->GeneratePrimaryVertex(anEvent);
}

