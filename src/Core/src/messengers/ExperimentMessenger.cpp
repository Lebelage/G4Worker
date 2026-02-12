#include "ExperimentMessenger.h"
#include "GaN_AlGan_battery_exp.h"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4Tokenizer.hh"
#include "G4UIcommand.hh"
#include "G4Exception.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

namespace G4Worker::Messengers
{
    static ExpType ParseType(const G4String &s)
    {
        if (s == "stack")
            return ExpType::Stack;
        return ExpType::None;
    }

    G4Worker::Messengers::ExperimentMessenger::ExperimentMessenger(ExperimentConfig &cfg) : fCfg{cfg}
    {
        fDir = new G4UIdirectory("/exp/");
        fDir->SetGuidance("Experiment configuration");

        fReset = new G4UIcommand("/exp/reset", this);

        fType = new G4UIcmdWithAString("/exp/type", this);

        // world
        fWorldMat = new G4UIcmdWithAString("/exp/world/material", this);

        fWorldSize = new G4UIcmdWithADoubleAndUnit("/exp/world/size", this);
        fWorldSize->SetUnitCategory("Length");

        // stack xy: /exp/stack/xy 100 um 100 um
        fStackXY = new G4UIcommand("/exp/stack/xy", this);
        fStackXY->SetParameter(new G4UIparameter("x", 'd', false));
        fStackXY->SetParameter(new G4UIparameter("xUnit", 's', false));
        fStackXY->SetParameter(new G4UIparameter("y", 'd', false));
        fStackXY->SetParameter(new G4UIparameter("yUnit", 's', false));

        // layers
        fLayersClear = new G4UIcommand("/exp/layers/clear", this);

        // /exp/layer/add <mat> <th> <unit>
        fLayerAdd = new G4UIcommand("/exp/layer/add", this);
        fLayerAdd->SetParameter(new G4UIparameter("mat", 's', false));
        fLayerAdd->SetParameter(new G4UIparameter("th", 'd', false));
        fLayerAdd->SetParameter(new G4UIparameter("unit", 's', false));

        // --- materials ---
        // /exp/material/create <name> <density> <unit>
        fMatCreate = new G4UIcommand("/exp/material/create", this);
        fMatCreate->SetParameter(new G4UIparameter("name", 's', false));
        fMatCreate->SetParameter(new G4UIparameter("density", 'd', false));
        fMatCreate->SetParameter(new G4UIparameter("densUnit", 's', false));

        // /exp/material/addElementMass <mat> <el> <fraction>
        fMatAddMass = new G4UIcommand("/exp/material/addElementMass", this);
        fMatAddMass->SetParameter(new G4UIparameter("mat", 's', false));
        fMatAddMass->SetParameter(new G4UIparameter("el", 's', false));
        fMatAddMass->SetParameter(new G4UIparameter("fraction", 'd', false));

        // /exp/material/finalize <mat>
        fMatFinalize = new G4UIcommand("/exp/material/finalize", this);
        fMatFinalize->SetParameter(new G4UIparameter("mat", 's', false));

        // Source Type
        fSourceType = new G4UIcmdWithAString("/exp/source/type", this);
        fSourceType->SetCandidates("gun decay");

        // Gun 
        fGunParticle = new G4UIcmdWithAString("/exp/source/gun/particle", this);
        fGunEnergy = new G4UIcmdWithADoubleAndUnit("/exp/source/gun/energy", this);
        fGunEnergy->SetUnitCategory("Energy");
        fGunPos = new G4UIcmdWith3VectorAndUnit("/exp/source/gun/pos", this);
        fGunPos->SetUnitCategory("Length");

        fGunDir = new G4UIcommand("/exp/source/gun/dir", this);
        fGunDir->SetParameter(new G4UIparameter("dx", 'd', false));
        fGunDir->SetParameter(new G4UIparameter("dy", 'd', false));
        fGunDir->SetParameter(new G4UIparameter("dz", 'd', false));
    }

    G4Worker::Messengers::ExperimentMessenger::~ExperimentMessenger()
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

    void G4Worker::Messengers::ExperimentMessenger::SetNewValue(G4UIcommand *cmd, G4String value)
    {
        if (cmd == fReset)
        {
            fCfg = ExperimentConfig{};
            fCfg.geomDirty = true;
            return;
        }

        // type
        if (cmd == fType)
        {
            fCfg.type = ParseType(value);
            fCfg.geomDirty = true;
            return;
        }

        // world
        if (cmd == fWorldMat)
        {
            fCfg.worldMaterial = value;
            fCfg.geomDirty = true;
            return;
        }
        if (cmd == fWorldSize)
        {
            fCfg.worldSize = fWorldSize->GetNewDoubleValue(value);
            fCfg.geomDirty = true;
            return;
        }

        // stack xy
        if (cmd == fStackXY)
        {
            G4Tokenizer tok(value);
            auto xs = tok();
            auto xu = tok();
            auto ys = tok();
            auto yu = tok();
            if (xs.empty() || xu.empty() || ys.empty() || yu.empty())
            {
                G4Exception("ExperimentMessenger", "BadStackXY", FatalException,
                            "Usage: /exp/stack/xy <x> <xUnit> <y> <yUnit>");
            }
            fCfg.stackX = std::stod(xs) * G4UIcommand::ValueOf(xu);
            fCfg.stackY = std::stod(ys) * G4UIcommand::ValueOf(yu);
            fCfg.geomDirty = true;
            return;
        }

        // layers
        if (cmd == fLayersClear)
        {
            fCfg.layers.clear();
            fCfg.geomDirty = true;
            return;
        }

        if (cmd == fLayerAdd)
        {
            G4Tokenizer tok(value);
            auto mat = tok();
            auto ths = tok();
            auto unit = tok();
            if (mat.empty() || ths.empty() || unit.empty())
            {
                G4Exception("ExperimentMessenger", "BadLayerAdd", FatalException,
                            "Usage: /exp/layer/add <material> <thickness> <unit>");
            }
            fCfg.layers.push_back({mat, std::stod(ths) * G4UIcommand::ValueOf(unit)});
            fCfg.geomDirty = true;
            return;
        }

        // --- materials ---
        if (cmd == fMatCreate)
        {
            G4Tokenizer tok(value);
            auto name = tok();
            auto dens = tok();
            auto unit = tok();
            if (name.empty() || dens.empty() || unit.empty())
            {
                G4Exception("ExperimentMessenger", "BadMatCreate", FatalException,
                            "Usage: /exp/material/create <name> <density> <unit>");
            }
            MaterialBuildSpec spec;
            spec.density = std::stod(dens) * G4UIcommand::ValueOf(unit);
            spec.finalized = false;
            spec.useAtoms = true;
            spec.mass.clear();
            fCfg.matBuild[std::string(name)] = spec;
            fCfg.geomDirty = true;
            return;
        }

        if (cmd == fMatAddMass)
        {
            G4Tokenizer tok(value);
            auto mat = tok();
            auto el = tok();
            auto fr = tok();
            if (mat.empty() || el.empty() || fr.empty())
            {
                G4Exception("ExperimentMessenger", "BadAddMass", FatalException,
                            "Usage: /exp/material/addElementMass <mat> <elementSymbol> <massFraction>");
            }
            auto it = fCfg.matBuild.find(std::string(mat));
            if (it == fCfg.matBuild.end())
            {
                G4Exception("ExperimentMessenger", "UnknownMat", FatalException,
                            "Material not created. Call /exp/material/create first.");
            }
            it->second.useAtoms = false;
            it->second.mass.push_back({el, std::stod(fr)});
            it->second.finalized = false;
            fCfg.geomDirty = true;
            return;
        }

        if (cmd == fMatFinalize)
        {
            G4Tokenizer tok(value);
            auto mat = tok();
            if (mat.empty())
            {
                G4Exception("ExperimentMessenger", "BadFinalize", FatalException,
                            "Usage: /exp/material/finalize <mat>");
            }
            auto it = fCfg.matBuild.find(std::string(mat));
            if (it == fCfg.matBuild.end())
            {
                G4Exception("ExperimentMessenger", "UnknownMat", FatalException,
                            "Material not created.");
            }

            auto &spec = it->second;
            if (spec.useAtoms)
            {
                // if (spec.atoms.empty())
                // {
                //     G4Exception("ExperimentMessenger", "NoAtoms", FatalException,
                //                 "No elements added (atoms mode).");
                // }
                // // небольшая проверка на нули
                // for (auto &a : spec.atoms)
                // {
                //     if (a.natoms <= 0)
                //     {
                //         G4Exception("ExperimentMessenger", "BadNatoms", FatalException,
                //                     "natoms must be > 0.");
                //     }
                // }
            }
            else
            {
                if (spec.mass.empty())
                {
                    G4Exception("ExperimentMessenger", "NoMass", FatalException,
                                "No elements added (mass mode).");
                }
                double s = 0.0;
                for (auto &m : spec.mass)
                {
                    if (m.massFraction <= 0)
                    {
                        G4Exception("ExperimentMessenger", "BadFraction", FatalException,
                                    "massFraction must be > 0.");
                    }
                    s += m.massFraction;
                }

                if (s <= 0)
                {
                    G4Exception("ExperimentMessenger", "BadSum", FatalException,
                                "Sum of mass fractions must be > 0.");
                }
                for (auto &m : spec.mass)
                    m.massFraction /= s;
            }

            spec.finalized = true;
            fCfg.geomDirty = true;
            return;
        }
    }
}
