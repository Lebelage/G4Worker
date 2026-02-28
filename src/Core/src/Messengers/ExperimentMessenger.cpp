#include "ExperimentMessenger.h"
#include "GaN_AlGan_battery_exp.h"

#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4Tokenizer.hh"
#include "G4Exception.hh"

namespace G4Worker::Messengers
{

    static ExpType ParseType(const G4String &s)
    {
        if (s == "stack")
            return ExpType::Stack;
        return ExpType::None;
    }

    // ------------------------------------------------------------

    ExperimentMessenger::ExperimentMessenger(ExperimentConfig &cfg)
        : fCfg(cfg)
    {
        G4UICommandBuilder builder(this);

        fDir = builder.Directory("/exp/", "Experiment configuration");

        fReset = builder.Command("/exp/reset");
        fType = builder.String("/exp/type");

        fWorldMat = builder.String("/exp/world/material");
        fWorldSize = builder.DoubleUnit("/exp/world/size", "Length");

        fStackXY = builder.Params("/exp/stack/xy",
                                  Param("x", 'd'),
                                  Param("xUnit", 's'),
                                  Param("y", 'd'),
                                  Param("yUnit", 's'));

        fLayersClear = builder.Command("/exp/layers/clear");

        fLayerAdd = builder.Params("/exp/layer/add",
                                   Param("mat", 's'),
                                   Param("th", 'd'),
                                   Param("unit", 's'));

        fMatCreate = builder.Params("/exp/material/create",
                                    Param("name", 's'),
                                    Param("density", 'd'),
                                    Param("densUnit", 's'));

        fMatAddMass = builder.Params("/exp/material/addMassFraction",
                                     Param("mat", 's'),
                                     Param("el", 's'),
                                     Param("fraction", 'd'));

        fMatFinalize = builder.Params("/exp/material/finalize",
                                      Param("mat", 's'));

        fSourceType = builder.Candidates("/exp/source/type", "gun decay");

        fGunParticle = builder.String("/exp/source/gun/particle");

        fGunEnergy = builder.DoubleUnit("/exp/source/gun/energy", "Energy");

        fGunPos = builder.Vec3Unit("/exp/source/gun/pos", "Length");

        fGunDir = builder.Params("/exp/source/gun/dir",
                                 Param("dx", 'd'),
                                 Param("dy", 'd'),
                                 Param("dz", 'd'));
    }

    // ------------------------------------------------------------

    ExperimentMessenger::~ExperimentMessenger()
    {
        delete fMatFinalize;
        delete fMatAddMass;
        delete fMatCreate;
        delete fLayerAdd;
        delete fLayersClear;
        delete fStackXY;
        delete fWorldSize;
        delete fWorldMat;
        delete fType;
        delete fReset;
        delete fDir;
    }

    // ------------------------------------------------------------

    void ExperimentMessenger::SetNewValue(G4UIcommand *cmd, G4String value)
    {
        auto* cm = new CommandManager();
        cm->ApplyCommand();
        // --------------------------------------------------------
        // RESET
        if (cmd == fReset)
        {
            fCfg = ExperimentConfig{};
            return;
        }

        // --------------------------------------------------------
        // TYPE
        if (cmd == fType)
        {
            fCfg.type = ParseType(value);
            return;
        }

        // --------------------------------------------------------
        // WORLD
        if (cmd == fWorldMat)
        {
            fCfg.worldMaterial = value;
            return;
        }

        if (cmd == fWorldSize)
        {
            fCfg.worldSize = fWorldSize->GetNewDoubleValue(value);
            return;
        }

        // --------------------------------------------------------
        // STACK SIZE
        if (cmd == fStackXY)
        {
            G4Tokenizer tok(value);
            auto xs = tok();
            auto xu = tok();
            auto ys = tok();
            auto yu = tok();

            fCfg.stackX = std::stod(xs) * G4UIcommand::ValueOf(xu);
            fCfg.stackY = std::stod(ys) * G4UIcommand::ValueOf(yu);

            return;
        }

        // --------------------------------------------------------
        // LAYERS
        if (cmd == fLayersClear)
        {
            fCfg.layers.clear();
            return;
        }

        if (cmd == fLayerAdd)
        {
            G4Tokenizer tok(value);
            auto mat = tok();
            auto th = tok();
            auto unit = tok();

            fCfg.layers.push_back(
                {mat, std::stod(th) * G4UIcommand::ValueOf(unit)});

            return;
        }

        // --------------------------------------------------------
        // MATERIALS
        if (cmd == fMatCreate)
        {
            G4Tokenizer tok(value);
            auto name = tok();
            auto dens = tok();
            auto unit = tok();

            MaterialBuildSpec spec;
            spec.density = std::stod(dens) * G4UIcommand::ValueOf(unit);
            spec.finalized = false;
            spec.useAtoms = true;

            fCfg.matBuild[name] = spec;

            return;
        }

        if (cmd == fMatAddMass)
        {
            G4Tokenizer tok(value);
            auto mat = tok();
            auto el = tok();
            auto fr = tok();

            fCfg.matBuild[mat].useAtoms = false;
            fCfg.matBuild[mat].mass.push_back({el, std::stod(fr)});

            return;
        }

        if (cmd == fMatFinalize)
        {
            G4Tokenizer tok(value);
            auto mat = tok();
            fCfg.matBuild[mat].finalized = true;

            return;
        }

        // --------------------------------------------------------
        // SOURCE (Physics)
        if (cmd == fSourceType)
        {
            fCfg.sourceType = (value == "gun")
                                  ? SourceType::Gun
                                  : SourceType::Decay;

            return;
        }

        if (cmd == fGunParticle)
        {
            fCfg.gun.particle = value;
            return;
        }

        if (cmd == fGunEnergy)
        {
            fCfg.gun.energy = fGunEnergy->GetNewDoubleValue(value);
            return;
        }

        if (cmd == fGunPos)
        {
            fCfg.gun.pos = fGunPos->GetNew3VectorValue(value);
            return;
        }

        if (cmd == fGunDir)
        {
            G4Tokenizer tok(value);
            auto dx = tok();
            auto dy = tok();
            auto dz = tok();

            fCfg.gun.dir = G4ThreeVector(
                               std::stod(dx),
                               std::stod(dy),
                               std::stod(dz))
                               .unit();

            return;
        }
    }

} // namespace