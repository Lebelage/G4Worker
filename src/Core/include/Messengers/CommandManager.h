#pragma once
#include "App.h"
#include "EventManager.h"

namespace G4Worker
{
    class CommandManager
    {
        public:
        void ApplyCommand()
        {   
            auto events = App::Services().Resolve<Infrastructure::Services::Interfaces::IEventManager>();
            events->OnReset();

        }

    };
}