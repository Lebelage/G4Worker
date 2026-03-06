#pragma once

/// Geant4
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <memory.h>

class FTFP_BERT;
class ExperimentConfig;

namespace G4Worker::Messengers {
    class ExperimentMessenger;
}

namespace G4Worker::Detectors
{
    class DetectorManager;
} // namespace G4Worker::Detectors

namespace G4Worker::Infrastructure::Services::Interfaces
{
    class IEventManager;
}

namespace G4Worker
{
    class GeantCore
    {
        public:
        GeantCore(int argc, char **argv, std::shared_ptr<ExperimentConfig> config);
        ~GeantCore();

        GeantCore(const GeantCore&) = delete;
        GeantCore& operator=(const GeantCore&) = delete;

        GeantCore(const GeantCore&&) = delete;
        GeantCore& operator=(const GeantCore&&) = delete;

        private:
        void Initialize(int argc, char **argv);
        void InitializeRunManager(int argc, char **argv);
        FTFP_BERT* InitializePhysics();

        public:
        void InitializeUI(int argc, char **argv);

        private:
        void OnUpdateGeometry();

        private:
        std::unique_ptr<G4UIExecutive> ui;
        std::unique_ptr<G4RunManager> runManager;
        std::unique_ptr<G4VisExecutive> visManager;
        G4UImanager *uiManager = nullptr;

        std::unique_ptr<G4Worker::Messengers::ExperimentMessenger> expMessenger;
        std::unique_ptr<G4Worker::Detectors::DetectorManager> detManager;

        std::shared_ptr<ExperimentConfig> config;

        std::shared_ptr<Infrastructure::Services::Interfaces::IEventManager> events;
    };
}