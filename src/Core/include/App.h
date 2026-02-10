#pragma once

/// Geant4
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

/// std
#include <memory>

namespace G4Worker
{
    class App
    {
    public:
        App(int argc, char **argv);
        ~App() = default;

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        App(const App &&) = delete;
        App &operator=(const App &&) = delete;

    public:
        void Inintialize(int argc, char **argv);
        void Run();

    private:
        std::unique_ptr<G4UIExecutive> ui;
        std::unique_ptr<G4RunManager> runManager;
        std::unique_ptr<G4VisManager> visManager;
        std::shared_ptr<G4UImanager> uiManager;
    };
}