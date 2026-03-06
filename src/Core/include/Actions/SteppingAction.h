#pragma once
#include "G4UserSteppingAction.hh"

namespace G4Worker::Detectors { class DetectorConstruction; }

class SteppingAction : public G4UserSteppingAction {
public:
  explicit SteppingAction(const G4Worker::Detectors::DetectorConstruction* det);
  void UserSteppingAction(const G4Step* step) override;

private:
  const G4Worker::Detectors::DetectorConstruction* fDet = nullptr;
};