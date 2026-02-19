#include "SteppingAction.h"

#include "DetectorConstruction.h"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction(const G4Worker::DetectorConstruction* det) : fDet(det) {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  const G4double edep = step->GetTotalEnergyDeposit();
  if (edep <= 0.) return;

  auto dx = step->GetStepLength() ;

  auto pre  = step->GetPreStepPoint();
  auto post = step->GetPostStepPoint();

  auto vol = pre->GetTouchableHandle()->GetVolume();
  if (!vol) return;

  // слои размещаются как "LayerPV" (new G4PVPlacement(..., "LayerPV", ...))
  if (vol->GetName() != "LayerPV") return;

  // координата шага
  const G4double zMid = 0.5*(pre->GetPosition().z() + post->GetPosition().z());

  // глубина внутрь стека (0 на верхней поверхности, дальше по лучу)
  const G4double depth = fDet->GetStackTopZ() - zMid;

  if (depth < 0.0 || depth > fDet->GetTotalThickness()) return;

  // IDs
  const auto* evt = G4RunManager::GetRunManager()->GetCurrentEvent();
  const G4int eventID = evt ? evt->GetEventID() : -1;

  const auto* tr = step->GetTrack();
  const G4int trackID = tr->GetTrackID();
  const G4int stepNo  = tr->GetCurrentStepNumber();

  const G4int copyNo = pre->GetTouchableHandle()->GetCopyNumber();

  auto* ana = G4AnalysisManager::Instance();

  // H1(0): E(depth)

  ana->FillH1(0, depth, edep);

  //if (dx > 0 && edep > 0) ana->FillH1(1, edep/dx);

  //if (edep > 0) ana->FillH1(1, edep);

  // ntuple: edep за шаг
  ana->FillNtupleIColumn(0, eventID);
  ana->FillNtupleIColumn(1, trackID);
  ana->FillNtupleIColumn(2, stepNo);
  ana->FillNtupleIColumn(3, copyNo);
  ana->FillNtupleDColumn(4, depth);
  ana->FillNtupleDColumn(5, edep);
  ana->AddNtupleRow();
}
