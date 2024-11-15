#include "PrimaryGeneratorAction.hh"

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"

#include "WriteToFile.hh"

//using namespace CLHEP;

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    particleGun = new G4ParticleGun(1); // 1 particle per event

    // Get particle definition
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");

    // Set particle properties
    particleGun->SetParticleDefinition(particle);
    particleGun->SetParticleEnergy(200. * keV); // Set the energy
        // Output for debug
    std::ofstream outputFile("output.txt");
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

void PrimaryGeneratorAction::GenerateLaueVertex(G4double distance, G4double rMin, G4double rMax, G4ThreeVector& vertex) {
    // define an uniform distribution on a disk
    //G4double r = std::sqrt(G4UniformRand() * (rMax*rMax - rMin*rMin) + rMin*rMin); //np.sqrt(np.random.uniform(r_min**2, r_max**2, n_points))
    G4double r = std::sqrt(G4RandFlat::shoot(rMin*rMin, rMax*rMax));
    G4double theta =  G4UniformRand() * 2.0 * CLHEP::pi; //np.random.uniform(0, 2*np.pi, n_points)
    vertex.setX(r * std::cos(theta));
    vertex.setY(r * std::sin(theta));
    vertex.setZ(distance); 
}

void PrimaryGeneratorAction::GenerateSquare(G4double number) {
    G4cout << "Hello " << number * number << G4endl;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    // Set source position
    //G4ThreeVector position(0.0, 0.0, -300.*cm); // Origin, modify as needed
    //particleGun->SetParticlePosition(position);

    // Generate direction within cone
    /*
    G4double coneAngle = 3.0 * deg; // Half-angle of the cone
    G4ThreeVector direction;
    GenerateDirectionInCone(coneAngle, direction);
    particleGun->SetParticleMomentumDirection(direction);
    */

    // Generate direction from the surface of the Laue lens
    G4double distance = -300. * cm;
    G4double rMin = 0.3 * m;
    G4double rMax = 1. * m;
    G4ThreeVector vertex;
    GenerateLaueVertex(distance, rMin, rMax, vertex);
    particleGun->SetParticlePosition(vertex);
    G4ThreeVector detectorPosition;
    detectorPosition.setX(0.);
    detectorPosition.setY(0.);
    detectorPosition.setZ(-20.*cm);
    G4ThreeVector direction;
    direction = (detectorPosition - vertex).unit(); 
    particleGun->SetParticleMomentumDirection(direction);
    
    // Output for debug
    /*
    std::ostringstream strValue;
    strValue << vertex.getX()/cm << " " << vertex.getY()/cm << " " << vertex.getZ()/cm;
    testOutput.print(strValue.str());
    */

    // Generate the particle
    particleGun->GeneratePrimaryVertex(anEvent);
}

