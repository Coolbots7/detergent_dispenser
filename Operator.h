#ifndef OPERATOR_H
#define OPERATOR_H

#include "Arduino.h"

#define FADE_STEP 0.1

class Operator {
  private:
    bool pushbuttonInverted;
    uint8_t pushbuttonPin;
    uint8_t indicatorPin;

//    float brightness;

    bool blinking;
    uint32_t lastBlinkUpdateTime;
    float blinkTime;
//    uint32_t fadeDelay;
//    bool fadeUp;

  public:
    Operator(uint8_t pushbutton_pin, uint8_t indicator_pin, bool pushbutton_inverted = false) {
      this->pushbuttonPin = pushbutton_pin;
      this->indicatorPin = indicator_pin;
      this->pushbuttonInverted = pushbutton_inverted;

      pinMode(this->pushbuttonPin, INPUT_PULLUP);
      pinMode(this->indicatorPin, OUTPUT);

//      this->brightness = 0;

      this->blinking = false;
      this->lastBlinkUpdateTime = 0;
      this->blinkTime = 0;
//      this->fadeDelay = 0;
//      this->fadeUp = true;
    }

    void updateIndicator() {
      //      if ((lastBlinkUpdateTime + this->fadeDelay) >= millis()) {
      //        lastBlinkUpdateTime = millis();
      //
      //        if (this->brightness >= 1.0) {
      //          fadeUp = false;
      //        }
      //        else if (this->brightness <= 0.0f) {
      //          fadeUp = true;
      //        }
      //
      //        this->setIndicatorBrightness(this->brightness + (this->fadeUp ? FADE_STEP : -FADE_STEP));
      //      }
      if (this->blinking && (lastBlinkUpdateTime + this->blinkTime) <= millis()) {
        lastBlinkUpdateTime = millis();

        this->toggleIndicator();
      }
    }

    bool getPushbuttonState() {
      bool state = digitalRead(this->pushbuttonPin);

      if(this->pushbuttonInverted) {
        state = !state;
      }

      return state;
    }

    void setIndicator(bool state) {
      digitalWrite(this->indicatorPin, state);
    }

    bool getIndicator() {
      return digitalRead(this->indicatorPin);
    }

    void toggleIndicator() {      
      this->setIndicator(!this->getIndicator());
    }

//    void setIndicatorBrightness(float brightness) {
//      if (brightness > 1.0f) {
//        brightness = 1.0f;
//      }
//
//      if (brightness < 0.0f) {
//        brightness = 0.0f;
//      }
//
//      this->brightness = brightness;
//
//      //      analogWrite(this->indicatorPin, round(255 * this->brightness));
//    }

    void blinkIndicator(float rate) {
      if (rate <= 0.0f) {
        this->blinking = false;
        this->setIndicator(LOW);
        return;
      }

      this->blinkTime = 1000.0f * (1.0f / rate) / 2;
//      this->fadeDelay = blinkTime / 255;
      this->blinking = true;
      this->setIndicator(HIGH);
    }

};

#endif
