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

            std::cout << "EventManager instance B = " << events.get() << std::endl;

            events->OnReset().Invoke();
            std::cout << "ApplyCommand invoked\n";
        }
    };
}