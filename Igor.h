#ifndef IGOR_H
#define IGOR_H

#include <Adafruit_SSD1306.h>
#include "RotaryEncoder.h"

class Igor
{
public:
    Igor();
    void setup();
    void loop();

private:
    enum State
    {
        MENU,
        COUNTING_UP,
        COUNTING_DOWN,
        SELECTING_DOWN_DURATION,
        IDLE
    };

    void initHardware();
    void initDisplay();
    void updateDisplay();
    bool buttonPressed();
    void handleButtonPresses();
    void startCountingUp();
    void startSelectingDownDuration();
    void confirmCountdownSelection();
    void stopCountingUp();
    void stopCountingDown();
    void resetFlowMinutes();
    void handleCounting();
    void successAnimation();
    int getRotation();
    void handleRotaryInput();
    void handleInactivity();

    Adafruit_SSD1306 display;
    RotaryEncoder encoder;
    State currentState;
    int menuIndex;
    String menuOptions[3];
    int flowMinutes;
    unsigned long lastActivityTime;
    const unsigned long inactivityLimit = 3 * 60000;
    int countdownValue;
    int initialCountdownValue;
    unsigned long previousMillis;
    bool isCounting;
    unsigned long buttonDebounceTime;
    const unsigned long buttonDebounceDelay = 800;
    unsigned long lastRotaryTime;
    const unsigned long rotaryDebounceDelay = 150;
    const unsigned long displayOffTimeLimit = 30 * 60000;
    unsigned long idleStartTime;
    bool displayOff;
    int elapsedMinutes;
};

#endif
