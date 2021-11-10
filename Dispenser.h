#ifndef DISPENSER_H
#define DISPENSER_H

#include "Stepper.h"

class Dispenser {
  private:
    Stepper* pump;
    float rotations_per_oz;

  public:
    Dispenser(Stepper *pump, float rotations_per_oz) {
      this->pump = pump;
      this->rotations_per_oz = rotations_per_oz;
    }

    void dispenseOunces(float oz) {
      // Make sure pump is enabled
      this->pump->enable();
      this->pump->moveRotations(this->rotations_per_oz * oz);
    }

    bool dispensingComplete() {
      return !(abs(this->pump->distanceToGo()) > 0);
    }
};

#endif
