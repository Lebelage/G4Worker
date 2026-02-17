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

    public:
        G4double GetTotalThickness() const { return fTotalZ; }
        G4double GetStackTopZ() const { return fStackTopZ; }

    private:
        G4VPhysicalVolume *BuildStack();

    private:
        const ExperimentConfig &fCfg;
        G4double fTotalZ = 0.0;
        G4double fStackTopZ = 0.0;
    };
}