#pragma once
#include "Events.h"

namespace G4Worker::Infrastructure::Services::Interfaces
{
    struct IEventManager
    {
        virtual ~IEventManager() = default;    
    };
}