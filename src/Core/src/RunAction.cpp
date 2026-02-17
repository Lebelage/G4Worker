#include "RunAction.h"

///Geant4
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"

G4Worker::RunAction::RunAction()
{
    auto *ana = G4AnalysisManager::Instance();
    ana->SetDefaultFileType("csv");
    ana->SetNtupleMerging(true);

    // depth: 0..3.1 um
    const G4int nbins = 310; // 10 nm на бин
    const G4double dmin = 0.0;
    const G4double dmax = 3.1 * um;
    ana->CreateH1("Edep_vs_depth", "Edep vs depth; depth [um]; Edep [MeV]",
                  nbins, dmin, dmax);

    ana->CreateNtuple("steps", "Step edep in layers");
    ana->CreateNtupleIColumn("eventID"); // 0
    ana->CreateNtupleIColumn("trackID"); // 1
    ana->CreateNtupleIColumn("stepNo");  // 2
    ana->CreateNtupleIColumn("layer");   // 3 (copyNo)
    ana->CreateNtupleDColumn("depth");   // 4
    ana->CreateNtupleDColumn("edep");    // 5
    ana->FinishNtuple();
}

G4Worker::RunAction::~RunAction()
{
    delete G4AnalysisManager::Instance();
}

void G4Worker::RunAction::BeginOfRunAction(const G4Run *)
{
    auto *ana = G4AnalysisManager::Instance();
    ana->OpenFile("out"); // out.root
}

void G4Worker::RunAction::EndOfRunAction(const G4Run *)
{
    auto *ana = G4AnalysisManager::Instance();
    ana->Write();
    ana->CloseFile();
}
