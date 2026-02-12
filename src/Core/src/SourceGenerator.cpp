#include "SourceGenerator.h"
#include "GaN_AlGan_battery_exp.h"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

G4Worker::SourceGenerator::SourceGenerator(const ExperimentConfig &cfg) : fCfg{cfg}
{
    gun = std::make_unique<G4ParticleGun>(1);
}

void G4Worker::SourceGenerator::GeneratePrimaries(G4Event *event)
{
    if (fCfg.sourceType == SourceType::Gun) {
        auto* def = G4ParticleTable::GetParticleTable()->FindParticle(fCfg.gun.particle);
        gun->SetParticleDefinition(def);
        gun->SetParticleEnergy(fCfg.gun.energy);
        gun->SetParticlePosition(fCfg.gun.pos);
        gun->SetParticleMomentumDirection(fCfg.gun.dir.unit());
        gun->GeneratePrimaryVertex(event);
    }
    else if(fCfg.sourceType == SourceType::Decay)
    {
        /// TODO
        return;
    }
}