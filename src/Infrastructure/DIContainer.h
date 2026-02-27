#pragma once
#include <memory>
#include <unordered_map>
#include <functional>
#include <typeinfo>
#include <stdexcept>

namespace G4Worker::Infrastructure
{
    class Container
    {
    public:
        // TRANSIENT (новый объект каждый раз)
        template <typename Interface, typename Impl, typename... Args>
        void Register(Args&&... args)
        {
            factories[typeid(Interface).hash_code()] =
                [=]()
            {
                return std::static_pointer_cast<void>(
                    std::make_shared<Impl>(args...)
                );
            };
        }

        // SINGLETON (один объект)
        template <typename Interface, typename Impl, typename... Args>
        void RegisterSingleton(Args&&... args)
        {
            singletons[typeid(Interface).hash_code()] =
                std::static_pointer_cast<void>(
                    std::make_shared<Impl>(args...)
                );
        }

        // RESOLVE
        template <typename Interface>
        std::shared_ptr<Interface> Resolve()
        {
            size_t key = typeid(Interface).hash_code();

            auto itS = singletons.find(key);
            if (itS != singletons.end())
                return std::static_pointer_cast<Interface>(itS->second);

            auto itF = factories.find(key);
            if (itF == factories.end())
                throw std::runtime_error("Type not registered!");

            return std::static_pointer_cast<Interface>(itF->second());
        }

    private:
        std::unordered_map<size_t, std::function<std::shared_ptr<void>()>> factories;
        std::unordered_map<size_t, std::shared_ptr<void>> singletons;
    };
}