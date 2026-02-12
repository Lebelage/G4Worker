#pragma once

#include "G4UImessenger.hh"
#include "G4String.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcommand;

struct ExperimentConfig;

namespace G4Worker::Messengers
{
    class ExperimentMessenger : G4UImessenger
    {
    public:
        explicit ExperimentMessenger(ExperimentConfig &cfg);
        ~ExperimentMessenger() override;

        void SetNewValue(G4UIcommand *cmd, G4String value) override;

    private:
        ExperimentConfig &fCfg;
        G4UIdirectory *fDir = nullptr;

        // common
        G4UIcommand *fReset = nullptr;
        G4UIcmdWithAString *fType = nullptr;

        // world
        G4UIcmdWithAString *fWorldMat = nullptr;
        G4UIcmdWithADoubleAndUnit *fWorldSize = nullptr;

        // stack
        G4UIcommand *fStackXY = nullptr;
        G4UIcommand *fLayersClear = nullptr;
        G4UIcommand *fLayerAdd = nullptr;

        // materials
        G4UIcommand *fMatCreate = nullptr;
        G4UIcommand *fMatAddMass = nullptr;
        G4UIcommand *fMatFinalize = nullptr;

        // Source
        G4UIcmdWithAString *fSourceType = nullptr;

        // Gun
        G4UIcmdWithAString *fGunParticle = nullptr;
        G4UIcmdWithADoubleAndUnit *fGunEnergy = nullptr;
        G4UIcmdWith3VectorAndUnit *fGunPos = nullptr;
        G4UIcommand *fGunDir = nullptr;
    };
}