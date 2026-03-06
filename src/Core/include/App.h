#pragma once

#include "DIContainer.h"
#include "DetectorManager.h"
#include "GeantCore.h"

/// Geant4
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

/// std
#include <memory>

class ExperimentConfig;

namespace G4Worker::Messengers {
    class ExperimentMessenger;
}

namespace G4Worker::Infrastructure::Services::Interfaces
{
    class IEventManager;
}

namespace G4Worker
{

    class App
    {
    public:
        using Container = G4Worker::Infrastructure::Container;

        App(int argc, char **argv);
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        App(const App &&) = delete;
        App &operator=(const App &&) = delete;

        static Container& Services()
        {
            static Container services;
            return services;  
        }

    public: 
        void Initialize(int argc, char **argv);
        
        void InitializeLocalStorage();

        void Run();

        void ServiceRegistration();

    private:
        std::shared_ptr<ExperimentConfig> cfg;
        std::unique_ptr<GeantCore> geant;
        std::shared_ptr<Infrastructure::Services::Interfaces::IEventManager> events;
    
    private:
        void OnUpdateGeometry();
    };
}