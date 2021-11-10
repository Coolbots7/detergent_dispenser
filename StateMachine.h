#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

class StateMachine {
  private:
    uint8_t previousState;
    uint8_t currentState;

  public:
    enum State {
      IDLE = 1,
      DISPENSING_SMALL = 2,
      DISPENSING_SMALL_W_BLEACH = 3,
      DISPENSING_MEDIUM = 4,
      DISPENSING_MEDIUM_W_BLEACH = 5,
      DISPENSING_LARGE = 6,
      DISPENSING_LARGE_W_BLEACH = 7
    };

    StateMachine() {
      this->currentState = IDLE;
      this->previousState = 0;
    }

    uint8_t evaluate(bool smallLoadButton, bool mediumLoadButton, bool largeLoadButton, bool bleachSwitch, bool dispensingComplete) {
      uint8_t nextState = 0;

      if (this->currentState == IDLE) {
        if (smallLoadButton == HIGH) {
          if (bleachSwitch == HIGH) {
            nextState = DISPENSING_SMALL_W_BLEACH;
          }
          else {
            nextState = DISPENSING_SMALL;
          }
        }
        else if (mediumLoadButton == HIGH) {
          if (bleachSwitch == HIGH) {
            nextState = DISPENSING_MEDIUM_W_BLEACH;
          }
          else {
            nextState = DISPENSING_MEDIUM;
          }
        }
        else if (largeLoadButton == HIGH) {
          if (bleachSwitch == HIGH) {
            nextState = DISPENSING_LARGE_W_BLEACH;
          }
          else {
            nextState = DISPENSING_LARGE;
          }
        }
      }
      else if (this->currentState != IDLE ) {
        // Do nothing while in dispensing state

        if (dispensingComplete == true) {
          // return to IDLE state after dispensing is complete
          nextState = IDLE;
        }
      }

      // Propogate states
      this->previousState = this->currentState;
      if (nextState != 0) {
        this->currentState = nextState;
      }

      return this->currentState;
    }

    uint8_t getCurrentState() {
      return this->currentState;
    }

    uint8_t getPreviousState() {
      return this->previousState;
    }
};

#endif
