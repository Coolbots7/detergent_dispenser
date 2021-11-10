#ifndef STEPPER_H
#define STEPPER_H

#include <AccelStepper.h>

#define MAX_SPEED_RPM 6
#define DEFAULT_SPEED_RPM 6

class Stepper {
  private:
    uint32_t steps_per_revolution;

    AccelStepper* stepper;

  public:
    Stepper(uint8_t enable_pin, uint8_t direction_pin, uint8_t step_pin, uint32_t steps_per_revolution) {
      this->steps_per_revolution = steps_per_revolution;

      this->stepper = new AccelStepper(AccelStepper::DRIVER, step_pin, direction_pin);
      this->stepper->setEnablePin(enable_pin);
      this->stepper->setPinsInverted(false, false, true);
      this->stepper->setMaxSpeed(steps_per_revolution * MAX_SPEED_RPM);
      this->stepper->setSpeed(steps_per_revolution * DEFAULT_SPEED_RPM);
      this->stepper->setAcceleration(steps_per_revolution * 10);

      this->stepper->enableOutputs();
    }

    void update() {
      this->stepper->run();
    }

    void moveSteps(uint32_t steps) {
      this->stepper->move(steps);
    }

    void moveRotations(float rotations) {
      this->moveSteps(floor(this->steps_per_revolution * rotations));
    }

    void enable() {
      this->stepper->enableOutputs();
    }

    void disable() {
      this->stepper->disableOutputs();
    }

    int32_t distanceToGo() {
      return this->stepper->distanceToGo();
    }
};

#endif
