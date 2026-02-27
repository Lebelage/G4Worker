#pragma once

#include <memory>

namespace G4Worker::Utils
{
    template <typename... Args>
    class Event
    {
    public:
        using Handler = std::function<void(Args...)>;
        using HandlerId = std::uint64_t;

        struct Entry
        {
            std::weak_ptr<void> owner;
            Handler handler;
        };

        template <typename T>
        HandlerId Add(std::shared_ptr<T> owner, Handler handler)
        {
            HandlerId id = nextId++;
            handlers[id] = Entry{
                owner,
                std::move(handler)};
            return id;
        }

        HandlerId Add(Handler handler)
        {
            HandlerId id = nextId++;
            handlers[id] = Entry{
                {},
                std::move(handler)};
            return id;
        }

        void Remove(HandlerId id)
        {
            handlers.erase(id);
        }

        void operator-=(HandlerId id)
        {
            Remove(id);
        }

        void operator()(Args... args)
        {
            std::vector<HandlerId> dead;

            for (auto &[id, entry] : handlers)
            {
                if (!entry.owner.expired())
                {
                    entry.handler(args...);
                }
                else
                {
                    dead.push_back(id);
                }
            }

            for (auto id : dead)
                handlers.erase(id);
        }

    private:
        std::unordered_map<HandlerId, Entry> handlers;
        HandlerId nextId = 1;
    };
}