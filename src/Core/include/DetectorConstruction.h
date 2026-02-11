#pragma once
#include "G4VUserDetectorConstruction.hh"

struct ExperimentConfig;

namespace G4Worker
{
    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        explicit DetectorConstruction(const ExperimentConfig &cfg);
        ~DetectorConstruction() override = default;

        G4VPhysicalVolume *Construct() override;

    private:
        const ExperimentConfig &fCfg;

        G4VPhysicalVolume *BuildStack();
    };
}