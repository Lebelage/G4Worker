#include "App.h"
#include "ExperimentMessenger.h"
#include "DetectorConstruction.h"
#include "SourceGenerator.h"
#include "RunAction.h"
#include "SteppingAction.h"
#include "Events.h"
#include "ExperimentMessenger.h"
#include "FileProvider.h"

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

        events->OnReset().Add([this]()
                              { this->OnUpdateGeometry(); });

        cfg = std::make_unique<ExperimentConfig>();

        // UI режим?
        const bool interactive = (argc == 1);

        if (interactive)
            ui = std::make_unique<G4UIExecutive>(argc, argv);

        runManager = std::make_unique<G4RunManager>();
        runManager->SetNumberOfThreads(8);

        // Messenger должен жить всё время работы приложения
        expMessenger = std::make_unique<Messengers::ExperimentMessenger>(*cfg);

        detManager = std::make_unique<DetectorManager>(*cfg);
        // detManager->ApplyConfigChanges();

        // Physics
        auto *phys = new FTFP_BERT();
        phys->ReplacePhysics(new G4EmStandardPhysics_option4());
        phys->RegisterPhysics(new G4StepLimiterPhysics());

        runManager->SetUserInitialization(phys);
        runManager->SetUserInitialization(new DetectorConstruction(*cfg));
        runManager->SetUserAction(new SourceGenerator(*cfg));
        runManager->SetUserAction(new RunAction());

        // SteppingAction зависит от детектора
        auto *det = static_cast<const DetectorConstruction *>(runManager->GetUserDetectorConstruction());
        runManager->SetUserAction(new SteppingAction(det));

        // ВАЖНО: Initialize() должен пройти всегда, но DetectorConstruction обязан уметь строить fallback world,
        // если /exp/type ещё не задан (иначе ты упадёшь при старте).
        runManager->Initialize();

        uiManager = G4UImanager::GetUIpointer();

        if (!interactive)
        {
            // Batch mode: запускаем переданный макрос
            G4String command = "/control/execute ";
            uiManager->ApplyCommand(command + G4String(argv[1]));
            return;
        }

        // Interactive mode: поднимаем визуализацию и стартовый init.mac
        visManager = std::make_unique<G4VisExecutive>();
        visManager->Initialize();

        // Если init.mac лежит в macros/, лучше так:
        // uiManager->ApplyCommand("/control/macroPath macros");
        // uiManager->ApplyCommand("/control/execute init.mac");

        uiManager->ApplyCommand("/control/execute init.mac");

        ui->SessionStart();
    }

    void App::InitializeLocalStorage()
    {
        G4Worker::Utils::FileProvider::CreateDirectory("Experiments");
        G4Worker::Utils::FileProvider::CreateFile("Experiments/AlGanExp.json");
    }

    void App::ServiceRegistration()
    {
        auto &services = Services();

        services.RegisterSingleton<
            G4Worker::Infrastructure::Services::Interfaces::IEventManager,
            G4Worker::Infrastructure::Services::EventManager>();
    }

    void App::OnUpdateGeometry()
    {
        // 1. Disable visualization — ок
        uiManager->ApplyCommand("/vis/disable");

        // 2. Apply geometry changes
        detManager->ApplyConfigChanges();

        // 3. Sync worker threads
        runManager->BeamOn(0);

        // 4. FULL scene rebuild instead of /vis/viewer/rebuild !!!
        uiManager->ApplyCommand("/vis/scene/clear");
        uiManager->ApplyCommand("/vis/scene/create");
        uiManager->ApplyCommand("/vis/scene/add/volume world");
        uiManager->ApplyCommand("/vis/sceneHandler/attach");

        // 5. Enable visualization
        uiManager->ApplyCommand("/vis/enable");

        // 6. Refresh viewer
        uiManager->ApplyCommand("/vis/viewer/refresh");
    }

}