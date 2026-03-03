#include "DetectorConstruction.h"
#include "GaN_AlGan_battery_exp.h"
#include "Materials.h"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Exception.hh"
#include "G4UserLimits.hh"
#include "G4NistManager.hh"

#include "EventManager.h"
#include "App.h"

#include <iostream>

void OnHandle();

G4Worker::DetectorConstruction::DetectorConstruction(ExperimentConfig &cfg) : fCfg{cfg}
{
    auto events = App::Services().Resolve<Infrastructure::Services::Interfaces::IEventManager>();
    events->OnReset().Add([this]()
                          { this->OnHandle(); });
}

G4VPhysicalVolume *G4Worker::DetectorConstruction::Construct()
{

    G4cout << ">>> Construct CALLED, thread = "
           << G4Threading::G4GetThreadId()
           << G4endl;

    return BuildWorld();
}

G4VPhysicalVolume *G4Worker::DetectorConstruction::BuildWorld() const
{
    if (fCfg.type == ExpType::Stack)
        return BuildStack();

    // fallback
    auto worldMat =
        G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");

    auto solidWorld = new G4Box("World", 1 * mm, 1 * mm, 1 * mm);
    auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");

    return new G4PVPlacement(
        nullptr, {}, logicWorld, "World", nullptr, false, 0);
}

G4VPhysicalVolume *G4Worker::DetectorConstruction::BuildStack() const
{
    Materials mats(fCfg);

    auto *worldMat = mats.Get(fCfg.worldMaterial).value();

    // World is a cube worldSize^3
    auto half = fCfg.worldSize / 2.0;
    auto *solidWorld = new G4Box("World", half, half, half);
    auto *logicWorld = new G4LogicalVolume(solidWorld, worldMat, "WorldLV");
    auto *physWorld = new G4PVPlacement(nullptr, {}, logicWorld, "WorldPV", nullptr, false, 0);

    if (fCfg.layers.empty())
    {
        G4Exception("DetectorConstruction", "NoLayers", FatalException, "No layers. Use /exp/layer/add.");
    }

    double totalZ = 0.0;
    for (auto &L : fCfg.layers)
        totalZ += L.thickness;

    fTotalZ = totalZ;
    fStackTopZ = fCfg.stackPos.z() + totalZ / 2.0;

    // Stack container (vacuum)
    auto *solidStack = new G4Box("StackSolid", fCfg.stackX / 2, fCfg.stackY / 2, totalZ / 2);
    auto *logicStack = new G4LogicalVolume(solidStack, worldMat, "StackLV");
    new G4PVPlacement(nullptr, fCfg.stackPos, logicStack, "StackPV", logicWorld, false, 0);

    // Place layers along +Z from top surface
    double zTop = totalZ / 2.0;
    double zCursor = zTop;

    int copyNo = 0;
    for (const auto &L : fCfg.layers)
    {
        auto *mat = mats.Get(L.material).value();

        auto *solidLayer = new G4Box("LayerSolid", fCfg.stackX / 2, fCfg.stackY / 2, L.thickness / 2);
        auto *logicLayer = new G4LogicalVolume(solidLayer, mat, "LayerLV");

        /// limits
        auto *limits = new G4UserLimits();
        limits->SetMaxAllowedStep(1 * nm);
        logicLayer->SetUserLimits(limits);

        zCursor -= L.thickness / 2.0;
        new G4PVPlacement(nullptr, {0, 0, zCursor}, logicLayer, "LayerPV", logicStack, false, copyNo);
        zCursor -= L.thickness / 2.0;

        copyNo++;
    }

    return physWorld;
}

void G4Worker::DetectorConstruction::OnHandle()
{
    std::cout << "OnHandle" << std::endl;
}