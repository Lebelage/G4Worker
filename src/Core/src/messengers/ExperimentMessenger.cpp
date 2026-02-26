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

static ExpType ParseType(const G4String& s)
{
    if (s == "stack") return ExpType::Stack;
    return ExpType::None;
}

// ------------------------------------------------------------

ExperimentMessenger::ExperimentMessenger(ExperimentConfig& cfg)
    : fCfg(cfg)
{
    fDir = new G4UIdirectory("/exp/");
    fDir->SetGuidance("Experiment configuration");

    fReset = new G4UIcommand("/exp/reset", this);

    fType = new G4UIcmdWithAString("/exp/type", this);

    fWorldMat  = new G4UIcmdWithAString("/exp/world/material", this);
    fWorldSize = new G4UIcmdWithADoubleAndUnit("/exp/world/size", this);
    fWorldSize->SetUnitCategory("Length");

    fStackXY = new G4UIcommand("/exp/stack/xy", this);
    fStackXY->SetParameter(new G4UIparameter("x", 'd', false));
    fStackXY->SetParameter(new G4UIparameter("xUnit", 's', false));
    fStackXY->SetParameter(new G4UIparameter("y", 'd', false));
    fStackXY->SetParameter(new G4UIparameter("yUnit", 's', false));

    fLayersClear = new G4UIcommand("/exp/layers/clear", this);

    fLayerAdd = new G4UIcommand("/exp/layer/add", this);
    fLayerAdd->SetParameter(new G4UIparameter("mat", 's', false));
    fLayerAdd->SetParameter(new G4UIparameter("th", 'd', false));
    fLayerAdd->SetParameter(new G4UIparameter("unit", 's', false));

    fMatCreate = new G4UIcommand("/exp/material/create", this);
    fMatCreate->SetParameter(new G4UIparameter("name", 's', false));
    fMatCreate->SetParameter(new G4UIparameter("density", 'd', false));
    fMatCreate->SetParameter(new G4UIparameter("densUnit", 's', false));

    fMatAddMass = new G4UIcommand("/exp/material/addMassFraction", this);
    fMatAddMass->SetParameter(new G4UIparameter("mat", 's', false));
    fMatAddMass->SetParameter(new G4UIparameter("el", 's', false));
    fMatAddMass->SetParameter(new G4UIparameter("fraction", 'd', false));

    fMatFinalize = new G4UIcommand("/exp/material/finalize", this);
    fMatFinalize->SetParameter(new G4UIparameter("mat", 's', false));

    fSourceType = new G4UIcmdWithAString("/exp/source/type", this);
    fSourceType->SetCandidates("gun decay");

    fGunParticle = new G4UIcmdWithAString("/exp/source/gun/particle", this);
    fGunEnergy   = new G4UIcmdWithADoubleAndUnit("/exp/source/gun/energy", this);
    fGunEnergy->SetUnitCategory("Energy");

    fGunPos = new G4UIcmdWith3VectorAndUnit("/exp/source/gun/pos", this);
    fGunPos->SetUnitCategory("Length");

    fGunDir = new G4UIcommand("/exp/source/gun/dir", this);
    fGunDir->SetParameter(new G4UIparameter("dx", 'd', false));
    fGunDir->SetParameter(new G4UIparameter("dy", 'd', false));
    fGunDir->SetParameter(new G4UIparameter("dz", 'd', false));
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

void ExperimentMessenger::SetNewValue(G4UIcommand* cmd, G4String value)
{
    auto* rm = G4RunManager::GetRunManager();

    auto markGeom = [&]() {
        if (rm) rm->GeometryHasBeenModified();
    };

    auto markPhysics = [&]() {
        if (rm) rm->PhysicsHasBeenModified();
    };

    // --------------------------------------------------------
    // RESET
    if (cmd == fReset)
    {
        fCfg = ExperimentConfig{};
        markGeom();
        return;
    }

    // --------------------------------------------------------
    // TYPE
    if (cmd == fType)
    {
        fCfg.type = ParseType(value);
        markGeom();
        return;
    }

    // --------------------------------------------------------
    // WORLD
    if (cmd == fWorldMat)
    {
        fCfg.worldMaterial = value;
        markGeom();
        return;
    }

    if (cmd == fWorldSize)
    {
        fCfg.worldSize = fWorldSize->GetNewDoubleValue(value);
        markGeom();
        return;
    }

    // --------------------------------------------------------
    // STACK SIZE
    if (cmd == fStackXY)
    {
        G4Tokenizer tok(value);
        auto xs = tok(); auto xu = tok();
        auto ys = tok(); auto yu = tok();

        fCfg.stackX = std::stod(xs) * G4UIcommand::ValueOf(xu);
        fCfg.stackY = std::stod(ys) * G4UIcommand::ValueOf(yu);

        markGeom();
        return;
    }

    // --------------------------------------------------------
    // LAYERS
    if (cmd == fLayersClear)
    {
        fCfg.layers.clear();
        markGeom();
        return;
    }

    if (cmd == fLayerAdd)
    {
        G4Tokenizer tok(value);
        auto mat  = tok();
        auto th   = tok();
        auto unit = tok();

        fCfg.layers.push_back(
            {mat, std::stod(th) * G4UIcommand::ValueOf(unit)}
        );

        markGeom();
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

        markGeom();
        return;
    }

    if (cmd == fMatAddMass)
    {
        G4Tokenizer tok(value);
        auto mat = tok();
        auto el  = tok();
        auto fr  = tok();

        fCfg.matBuild[mat].useAtoms = false;
        fCfg.matBuild[mat].mass.push_back({el, std::stod(fr)});

        markGeom();
        return;
    }

    if (cmd == fMatFinalize)
    {
        G4Tokenizer tok(value);
        auto mat = tok();
        fCfg.matBuild[mat].finalized = true;

        markGeom();
        return;
    }

    // --------------------------------------------------------
    // SOURCE (Physics)
    if (cmd == fSourceType)
    {
        fCfg.sourceType = (value == "gun")
                          ? SourceType::Gun
                          : SourceType::Decay;

        markPhysics();
        return;
    }

    if (cmd == fGunParticle)
    {
        fCfg.gun.particle = value;
        markPhysics();
        return;
    }

    if (cmd == fGunEnergy)
    {
        fCfg.gun.energy = fGunEnergy->GetNewDoubleValue(value);
        markPhysics();
        return;
    }

    if (cmd == fGunPos)
    {
        fCfg.gun.pos = fGunPos->GetNew3VectorValue(value);
        markPhysics();
        return;
    }

    if (cmd == fGunDir)
    {
        G4Tokenizer tok(value);
        auto dx = tok(); auto dy = tok(); auto dz = tok();

        fCfg.gun.dir = G4ThreeVector(
            std::stod(dx),
            std::stod(dy),
            std::stod(dz)
        ).unit();

        markPhysics();
        return;
    }
}

} // namespace