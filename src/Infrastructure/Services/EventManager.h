#pragma once
#include "IEventManager.h"

namespace G4Worker::Infrastructure::Services
{
    class EventManager : Interfaces::IEventManager
    {
        public:
        Utils::Event<> onReset;

        Utils::Event<>& OnReset() override { return onReset; }
    };
}