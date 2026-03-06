#pragma once

#include "DetectorConstruction.h"

#include "G4StateManager.hh"
#include "G4Threading.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
struct ExperimentConfig;

namespace G4Worker::Detectors
{
    class DetectorManager
    {
    public:
        DetectorManager(ExperimentConfig &cfg) : cfg(cfg) {}

        void ApplyConfigChanges()
        {
            if (!G4Threading::IsMasterThread())
            {
                G4Exception("DetectorManager", "GeomUpdate-NotMaster", JustWarning,
                            "Geometry update requested from worker thread. Ignore.");
                return;
            }

            auto state = G4StateManager::GetStateManager()->GetCurrentState();
            if (state != G4State_Idle)
            {
                G4Exception("DetectorManager", "GeomUpdate-RunActive", JustWarning,
                            "Run is active or geometry is closed. Stop run before updating geometry.");
                return;
            }

            auto *rm = G4RunManager::GetRunManager();

            auto *detBase = rm->GetUserDetectorConstruction();
            auto *det = const_cast<DetectorConstruction *>(static_cast<const DetectorConstruction *>(detBase));

            if (!det)
            {
                G4Exception("DetectorManager", "GeomUpdate-NoDet", FatalException,
                            "No DetectorConstruction registered.");
                return;
            }

            G4GeometryManager::GetInstance()->OpenGeometry();
            G4PhysicalVolumeStore::GetInstance()->Clean();
            G4LogicalVolumeStore::GetInstance()->Clean();
            G4SolidStore::GetInstance()->Clean();

            G4VPhysicalVolume *newWorld = det->BuildWorld();

            rm->DefineWorldVolume(newWorld, /*topologyIsChanged*/ true);

            rm->GeometryHasBeenModified();
        }

        ExperimentConfig &cfg;
    };

}