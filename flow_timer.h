#ifndef FLOW_TIMER_H
#define FLOW_TIMER_H

#include <Adafruit_SSD1306.h>
#include "RotaryEncoder.h"
#include "lightbar.h"
#include "settings.h"

class FlowTimer
{
public:
    FlowTimer(Lightbar *lightbar, Settings *settings);
    void setup();
    void loop();

private:
    enum State
    {
        STATE_MENU,
        STATE_COUNT_UP,
        STATE_COUNT_DOWN_SETUP,
        STATE_COUNT_DOWN,
        STATE_IDLE
    };

    void setupOled();
    void setupRotaryEncoder();
    void handleRotaryEncoder();
    void handleClick();
    void handleRotate();
    void displayMenu();
    void displayCountUp();
    void displayCountDownSetup();
    void displayCountDown();
    void displayIdle();
    void displaySuccess();
    void reset();

    Lightbar *lightbar;
    Settings *settings;
    Adafruit_SSD1306 display;
    RotaryEncoder encoder;
    State currentState;
    int menuPosition;
    unsigned long totalFocusTime;
    unsigned long sessionStartTime;
    unsigned long sessionDuration;
    unsigned long lastActivityTime;
    int countDownTime;
};

#endif
