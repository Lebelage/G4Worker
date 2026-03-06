#include "GeantCore.h"
#include "ExperimentMessenger.h"
#include "DetectorManager.h"
#include "SourceGenerator.h"
#include "RunAction.h"
#include "SteppingAction.h"
#include "EventManager.h"

#include "GaN_AlGan_battery_exp.h"

// Geant4 physics
#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4StepLimiterPhysics.hh"

#pragma region Constructor/Destructor

G4Worker::GeantCore::GeantCore(int argc, char **argv, std::shared_ptr<ExperimentConfig> cfg) : config{cfg}
{
    events = App::Services().Resolve<Infrastructure::Services::Interfaces::IEventManager>();

    events->OnReset().Add([this]()
                          { this->OnUpdateGeometry(); });

    Initialize(argc, argv);
}

G4Worker::GeantCore::~GeantCore()
{
}
#pragma endregion

#pragma region Methods

void G4Worker::GeantCore::Initialize(int argc, char **argv)
{
    InitializeRunManager(argc, argv);
}

void G4Worker::GeantCore::InitializeRunManager(int argc, char **argv)
{
    const bool isInteractive = (argc == 1);

    if (isInteractive)
        ui = std::make_unique<G4UIExecutive>(argc, argv);

    runManager = std::make_unique<G4RunManager>();
    runManager->SetNumberOfThreads(10);

    expMessenger = std::make_unique<Messengers::ExperimentMessenger>(*config);

    detManager = std::make_unique<G4Worker::Detectors::DetectorManager>(*config);
    detManager->ApplyConfigChanges();

    auto *physics = InitializePhysics();

    runManager->SetUserInitialization(physics);
    runManager->SetUserInitialization(new G4Worker::Detectors::DetectorConstruction(*config));
    runManager->SetUserAction(new SourceGenerator(*config));
    runManager->SetUserAction(new RunAction());

    auto *det = static_cast<const G4Worker::Detectors::DetectorConstruction *>(runManager->GetUserDetectorConstruction());
    runManager->SetUserAction(new SteppingAction(det));

    runManager->Initialize();
}
FTFP_BERT *G4Worker::GeantCore::InitializePhysics()
{
    auto *physics = new FTFP_BERT();
    physics->ReplacePhysics(new G4EmStandardPhysics_option4());
    physics->RegisterPhysics(new G4StepLimiterPhysics());

    return physics;
}
void G4Worker::GeantCore::InitializeUI(int argc, char **argv)
{
    uiManager = G4UImanager::GetUIpointer();

    const auto isInteractive = (argc == 1);
    if (!isInteractive)
    {
        // Batch mode: запускаем переданный макрос
        G4String command = "/control/execute ";
        uiManager->ApplyCommand(command + G4String(argv[1]));
        return;
    }

    visManager = std::make_unique<G4VisExecutive>();
    visManager->Initialize();

    uiManager->ApplyCommand("/control/macroPath AppConfigs");
    uiManager->ApplyCommand("/control/execute init.mac");

    ui->SessionStart();
}
#pragma endregion

#pragma region Handlers

void G4Worker::GeantCore::OnUpdateGeometry()
{

    uiManager->ApplyCommand("/vis/disable");

    detManager->ApplyConfigChanges();

    runManager->BeamOn(0);

    uiManager->ApplyCommand("/vis/scene/clear");
    uiManager->ApplyCommand("/vis/scene/create");
    uiManager->ApplyCommand("/vis/scene/add/volume world");
    uiManager->ApplyCommand("/vis/sceneHandler/attach");

    uiManager->ApplyCommand("/vis/enable");

    uiManager->ApplyCommand("/vis/viewer/refresh");
}

#pragma endregion