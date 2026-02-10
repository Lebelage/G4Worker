#include "App.h"
#include "GaN_AlGan_battery_exp.h"
#include "ExperimentMessenger.h"

#include <exception>

#include "FTFP_BERT.hh"
#include "G4EmStandardPhysics_option4.hh"
namespace G4Worker
{
    G4Worker::App::App(int argc, char **argv)
    {
        Inintialize(argc, argv);
    }

    void App::Inintialize(int argc, char **argv)
    {
        ExperimentConfig cfg;
        try
        {
            if (argc == 1)
                ui = std::make_unique<G4UIExecutive>(argc, argv);

            runManager = std::make_unique<G4RunManager>();
            visManager = std::make_unique<G4VisExecutive>();

            G4Worker::Messengers::ExperimentMessenger expMessenger(cfg);

            visManager->Initialize();

            auto *phys = new FTFP_BERT();
            phys->ReplacePhysics(new G4EmStandardPhysics_option4());
            runManager->SetUserInitialization(phys);

            uiManager.reset(G4UImanager::GetUIpointer());

            if (!ui)
            {
                // Режим командной строки (batch mode)
                G4String command = "/control/execute ";
                G4String fileName = argv[1];
                uiManager->ApplyCommand(command + fileName);
            }
            else
            {
                // Интерактивный режим
                uiManager->ApplyCommand("/control/execute init_vis.mac");
                ui->SessionStart();

                ui.release();
            }

            visManager.release();
            runManager.release();
        }
        catch (std::exception ex)
        {
        }
    }
}