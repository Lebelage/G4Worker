#pragma once
#include "G4VUserDetectorConstruction.hh"

struct ExperimentConfig;

namespace G4Worker
{
    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        explicit DetectorConstruction(ExperimentConfig &cfg);
        ~DetectorConstruction() override = default;

        G4VPhysicalVolume *Construct() override;

    public:
        G4double GetTotalThickness() const { return fTotalZ; }
        G4double GetStackTopZ() const { return fStackTopZ; }

        void OnHandle();

    private:
        G4VPhysicalVolume *BuildStack();

    private:
        ExperimentConfig &fCfg;
        G4double fTotalZ = 0.0;
        G4double fStackTopZ = 0.0;
    };
}