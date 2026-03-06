#include "App.h"
#include "ExperimentMessenger.h"
#include "DetectorConstruction.h"
#include "SourceGenerator.h"
#include "RunAction.h"
#include "SteppingAction.h"
#include "Events.h"
#include "ExperimentMessenger.h"
#include "FileProvider.h"

#include "AppPaths.h"


#include "GaN_AlGan_battery_exp.h"
#include "EventManager.h"

#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4StepLimiterPhysics.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

namespace G4Worker
{
    App::App(int argc, char **argv)
    {
        InitializeLocalStorage();
        ServiceRegistration();
        Initialize(argc, argv);
    }

    App::~App() = default;

    void App::Initialize(int argc, char **argv)
    {
        events = App::Services().Resolve<Infrastructure::Services::Interfaces::IEventManager>();

        cfg = std::make_shared<ExperimentConfig>();

        geant = std::make_unique<GeantCore>(argc, argv, cfg);

        geant->InitializeUI(argc, argv);
    }

    void App::InitializeLocalStorage()
    {
        G4Worker::Utils::FileProvider::CreateDirectory(Utils::Constants::AppPaths::__EXPERIMENTS_DIR_NAME);
        //G4Worker::Utils::FileProvider::CreateDirectory(Utils::Constants::AppPaths::__APPCONFIGS_DIR_NAME);
        G4Worker::Utils::FileProvider::CreateDirectory(Utils::Constants::AppPaths::__ANALYSIS_DIR_NAME);
        G4Worker::Utils::FileProvider::CreateFile(Utils::Constants::AppPaths::__GAN_INGAN_EXP_FILE_NAME);
    }

    void App::ServiceRegistration()
    {
        auto &services = Services();

        services.RegisterSingleton<
            G4Worker::Infrastructure::Services::Interfaces::IEventManager,
            G4Worker::Infrastructure::Services::EventManager>();
    }

}