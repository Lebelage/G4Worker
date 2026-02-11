#pragma once

/// Geant 4
#include "G4String.hh"

/// std
#include <unordered_map>
#include <string>
#include <optional>

class G4Material;
class G4NistManager;
struct ExperimentConfig;
struct MaterialBuildSpec;

namespace G4Worker
{
    class Materials
    {
    public:
        explicit Materials(const ExperimentConfig &cfg);
        std::optional<G4Material*> Get(const G4String &name);

    private:
        const ExperimentConfig &fCfg;
        G4NistManager *fNist = nullptr;
        std::unordered_map<std::string, G4Material *> fCache;

        G4Material *BuildFromSpec(const std::string &name, const MaterialBuildSpec &spec);
    };

}