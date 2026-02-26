#pragma once

/// Geant4
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"

/// std
#include <memory>

class G4Event;
struct ExperimentConfig;

namespace G4Worker
{
    class SourceGenerator : public G4VUserPrimaryGeneratorAction
    {
    public:
        explicit SourceGenerator(const ExperimentConfig &cfg);
        void GeneratePrimaries(G4Event *event) override;

    private:
        const ExperimentConfig &fCfg;
        std::unique_ptr<G4ParticleGun> gun;
    };
}