#pragma once

#include "G4UserRunAction.hh"

namespace G4Worker
{
    class RunAction : public G4UserRunAction
    {
    public:
        RunAction();
        ~RunAction() override;

    public:
        void BeginOfRunAction(const G4Run *) override;
        void EndOfRunAction(const G4Run *) override;
    };
}