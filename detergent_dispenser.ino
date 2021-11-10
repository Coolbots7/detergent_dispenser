#include "Dispenser.h"
#include "Operator.h"
#include "StateMachine.h"
#include "Stepper.h"

#define RATE                        10

// ====== Pump Motors ======
#define PUMP_1_ENABLE               33
#define PUMP_1_DIRECTION            19
#define PUMP_1_STEP                 23

#define PUMP_2_ENABLE               16
#define PUMP_2_DIRECTION            26
#define PUMP_2_STEP                 18

#define PUMP_3_ENABLE               17
#define PUMP_3_DIRECTION            21
#define PUMP_3_STEP                 16

// Decouple steps from oz because it can change with microstepping / motors
#define STEPS_PER_ROTATION          200
#define ROTATIONS_PER_OZ            26.3157895


// ====== Buttons and Indicators ======
#define BLEACH_ENABLE_SWITCH_PIN    34
#define BLEACH_IDTICATOR_PIN        2

#define SMALL_LOAD_SWITCH_PIN       22
#define SMALL_LOAD_INDICATOR_PIN    27

#define MEDIUM_LOAD_SWITCH_PIN      32
#define MEDIUM_LOAD_INDICATOR_PIN   25

#define LARGE_LOAD_SWITCH_PIN       35
#define LARGE_LOAD_INDICATOR_PIN    4

#define INDICATOR_BLINK_RATE        2


// ====== Dispensing config ======
#define LARGE_LOAD_DETERGENT_OZ     4.1
#define LARGE_LOAD_SOFTENER_OZ      2.6
#define LARGE_LOAD_BLEACH_OZ        0

#define MEDIUM_LOAD_DETERGENT_OZ    2.6
#define MEDIUM_LOAD_SOFTENER_OZ     1.4
#define MEDIUM_LOAD_BLEACH_OZ       0

#define SMALL_LOAD_DETERGENT_OZ     1.4
#define SMALL_LOAD_SOFTENER_OZ      0.7
#define SMALL_LOAD_BLEACH_OZ        0


// ====== Define globals ======
StateMachine* stateMachine;

Stepper* pump1;
Stepper* pump2;
Stepper* pump3;

Dispenser* detergent;
Dispenser* softener;
Dispenser* bleach;

Operator* smallLoadOperator;
Operator* mediumLoadOperator;
Operator* largeLoadOperator;
Operator* bleachOperator;

uint32_t lastLoopUpdateTime = 0;

bool allDispensersComplete = true;

void setup() {
  Serial.begin(115200);
  Serial.println("====== Detergent Dispenser ======");
  Serial.println("Initializing ...");


  // ====== Initialize state machine ======
  Serial.print("1 - Initializing state machine...");
  stateMachine = new StateMachine();
  Serial.println("OK");


  // ====== Initialize operators ======
  Serial.print("2 - Initializing operators...");
  smallLoadOperator = new Operator(SMALL_LOAD_SWITCH_PIN, SMALL_LOAD_INDICATOR_PIN, true);
  mediumLoadOperator = new Operator(MEDIUM_LOAD_SWITCH_PIN, MEDIUM_LOAD_INDICATOR_PIN, true);
  largeLoadOperator = new Operator(LARGE_LOAD_SWITCH_PIN, LARGE_LOAD_INDICATOR_PIN, true);
  bleachOperator = new Operator(BLEACH_ENABLE_SWITCH_PIN, BLEACH_IDTICATOR_PIN, true);
  Serial.println("OK");


  // ====== Initialize Pumps ======
  Serial.print("3 - Initializing pumps...");
  pump1 = new Stepper(PUMP_1_ENABLE, PUMP_1_DIRECTION, PUMP_1_STEP, STEPS_PER_ROTATION);
  pump2 = new Stepper(PUMP_2_ENABLE, PUMP_2_DIRECTION, PUMP_2_STEP, STEPS_PER_ROTATION);
  pump3 = new Stepper(PUMP_3_ENABLE, PUMP_3_DIRECTION, PUMP_3_STEP, STEPS_PER_ROTATION);
  Serial.println("OK");


  // ====== Initialize Dispensers ======
  Serial.print("4 - Initializing dispensers...");
  detergent = new Dispenser(pump1, ROTATIONS_PER_OZ);
  softener = new Dispenser(pump2, ROTATIONS_PER_OZ);
  bleach = new Dispenser(pump3, ROTATIONS_PER_OZ);
  Serial.println("OK");

  Serial.println("Initialization Complete!\n");
}

void loop() {
  // Update operator indicators
  smallLoadOperator->updateIndicator();
  mediumLoadOperator->updateIndicator();
  largeLoadOperator->updateIndicator();


  // Update stepper motors
  pump1->update();
  pump2->update();
  pump3->update();


  if ((lastLoopUpdateTime + (1000.0 * (1.0 / RATE))) <= millis()) {
    lastLoopUpdateTime = millis();

    // Update inputs
    bool smallButton = smallLoadOperator->getPushbuttonState();
    bool mediumButton = mediumLoadOperator->getPushbuttonState();
    bool largeButton = largeLoadOperator->getPushbuttonState();
    bool bleachSwitch = bleachOperator->getPushbuttonState();
    Serial.print("Inputs: ");
    Serial.print(smallButton ? "1" : "0");
    Serial.print(", ");
    Serial.print(mediumButton ? "1" : "0");
    Serial.print(", ");
    Serial.print(largeButton ? "1" : "0");
    Serial.print(", ");
    Serial.print(bleachSwitch ? "1" : "0");
    Serial.println();


    // Evaluate state
    stateMachine->evaluate(smallButton, mediumButton, largeButton, bleachSwitch, allDispensersComplete);
    Serial.print("State: ");
    Serial.print(stateMachine->getPreviousState());
    Serial.print(", ");
    Serial.print(stateMachine->getCurrentState());
    Serial.println();


    // Only set dispensers on rising edge
    if (stateMachine->getPreviousState() == StateMachine::IDLE && stateMachine->getPreviousState() != stateMachine->getCurrentState()) {
      allDispensersComplete = false;

      Serial.println("Setting dispensers for selected load");
      // Update outputs
      if (stateMachine->getCurrentState() == StateMachine::DISPENSING_SMALL || stateMachine->getCurrentState() == StateMachine::DISPENSING_SMALL_W_BLEACH) {
        Serial.println("Setting load 'small'");
        smallLoadOperator->blinkIndicator(INDICATOR_BLINK_RATE);

        detergent->dispenseOunces(SMALL_LOAD_DETERGENT_OZ);
        softener->dispenseOunces(SMALL_LOAD_SOFTENER_OZ);

        if (stateMachine->getCurrentState() == StateMachine::DISPENSING_SMALL_W_BLEACH) {
          Serial.println("with bleach!");
          bleachOperator->blinkIndicator(INDICATOR_BLINK_RATE);

          bleach->dispenseOunces(SMALL_LOAD_BLEACH_OZ);
        }
      }
      else if (stateMachine->getCurrentState() == StateMachine::DISPENSING_MEDIUM || stateMachine->getCurrentState() == StateMachine::DISPENSING_MEDIUM_W_BLEACH) {
        Serial.println("Setting load 'medium'");
        mediumLoadOperator->blinkIndicator(INDICATOR_BLINK_RATE);

        detergent->dispenseOunces(MEDIUM_LOAD_DETERGENT_OZ);
        softener->dispenseOunces(MEDIUM_LOAD_SOFTENER_OZ);

        if (stateMachine->getCurrentState() == StateMachine::DISPENSING_MEDIUM_W_BLEACH) {
          Serial.println("with bleach!");
          bleachOperator->blinkIndicator(INDICATOR_BLINK_RATE);

          bleach->dispenseOunces(MEDIUM_LOAD_BLEACH_OZ);
        }
      }
      else if (stateMachine->getCurrentState() == StateMachine::DISPENSING_LARGE || stateMachine->getCurrentState() == StateMachine::DISPENSING_LARGE_W_BLEACH) {
        Serial.println("Setting load 'large'");
        largeLoadOperator->blinkIndicator(INDICATOR_BLINK_RATE);

        detergent->dispenseOunces(LARGE_LOAD_DETERGENT_OZ);
        softener->dispenseOunces(LARGE_LOAD_SOFTENER_OZ);

        if (stateMachine->getCurrentState() == StateMachine::DISPENSING_LARGE_W_BLEACH) {
          Serial.println("with bleach!");
          bleachOperator->blinkIndicator(INDICATOR_BLINK_RATE);

          bleach->dispenseOunces(LARGE_LOAD_BLEACH_OZ);
        }
      }
    }
    // If we have been in a dispense state for at least on loop
    else if (stateMachine->getCurrentState() != StateMachine::IDLE) {
      // If we are in a dispensing state

      // Check if all dispensers are done
      if (detergent->dispensingComplete() && softener->dispensingComplete() && bleach->dispensingComplete()) {
        // Let the state machine know all dispensers are complete on the next evaluation
        allDispensersComplete = true;
      }
    }
    // Transition from a dispensing state to the idle state
    else if (stateMachine->getPreviousState() != StateMachine::IDLE && stateMachine->getCurrentState() == StateMachine::IDLE) {
      Serial.println("Setting dispensers and indicators to IDLE");
      // Disable motors
      pump1->disable();
      pump2->disable();
      pump3->disable();

      // Stop any blinking indicators
      smallLoadOperator->blinkIndicator(0);
      mediumLoadOperator->blinkIndicator(0);
      largeLoadOperator->blinkIndicator(0);
      bleachOperator->blinkIndicator(0);
    }
    // If we have been in the idle state for at least one loop
    else if (stateMachine->getCurrentState() == StateMachine::IDLE) {
      // Set bleach indicator on if pushbutton closed
      if (bleachOperator->getPushbuttonState() == HIGH) {
        bleachOperator->setIndicator(HIGH);
      }
      else {
        bleachOperator->setIndicator(LOW);
      }
    }
  }

}
