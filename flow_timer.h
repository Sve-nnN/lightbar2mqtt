#ifndef FLOW_TIMER_H
#define FLOW_TIMER_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "lightbar.h"
#include "settings.h"

class FlowTimer
{
public:
    FlowTimer(Lightbar *lightbar, Settings *settings);
    void setup();
    void loop();

private:
    // States
    enum State
    {
        MENU,
        COUNTING_UP,
        COUNTING_DOWN,
        SELECTING_DOWN_DURATION,
        IDLE,
        SETTINGS
    };
    State currentState = MENU;

    // Menu
    int menuIndex = 0;
    String menuOptions[7] = {"UP", "DOWN", "Reset", "LB On/Off", "LB Brighter", "LB Warmer", "Settings"};
    int settingsMenuIndex = 0;
    String settingsMenuOptions[2] = {"Brightness", "Temperature"};

    // Time
    int flowMinutes = 0;
    int countdownValue = 20;
    int initialCountdownValue = 20;
    unsigned long previousMillis = 0;
    int elapsedMinutes = 0;
    bool isCounting = false;

    // Inactivity
    unsigned long lastActivityTime = 0;
    const unsigned long inactivityLimit = 3 * 60000;
    const unsigned long displayOffTimeLimit = 30 * 60000;
    unsigned long idleStartTime = 0;
    bool displayOff = false;

    // Hardware
    Adafruit_SSD1306 display;
    Lightbar *lightbar;
    Settings *settings;

    // Debounce
    unsigned long buttonDebounceTime = 0;
    const unsigned long buttonDebounceDelay = 800;
    unsigned long lastRotaryTime = 0;
    const unsigned long rotaryDebounceDelay = 150;

    // Methods
    void initHardware();
    void initDisplay();
    void updateDisplay();
    void handleRotaryInput();
    void handleButtonPresses(unsigned long currentMillis);
    void handleCounting(unsigned long currentMillis);
    void handleInactivity(unsigned long currentMillis);
    bool buttonPressed();
    void startCountingUp();
    void startSelectingDownDuration();
    void confirmCountdownSelection();
    void stopCountingUp();
    void stopCountingDown();
    void resetFlowMinutes();
    void successAnimation();
    int getRotation();
};

#endif
