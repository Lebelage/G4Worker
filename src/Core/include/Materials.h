#pragma once

/// Geant 4
#include "G4String.hh"

/// std
#include <unordered_map>
#include <string>

class G4Material;
class G4NistManager;
struct ExperimentConfig;

namespace G4Worker
{
    class Materials
    {
    public:
        explicit Materials(const ExperimentConfig &cfg);
        G4Material *Get(const G4String &name);

    private:
        const ExperimentConfig &fCfg;
        G4NistManager *fNist = nullptr;
        std::unordered_map<std::string, G4Material *> fCache;

        G4Material *BuildFromSpec(const std::string &name, const struct MaterialBuildSpec &spec);
    };

}