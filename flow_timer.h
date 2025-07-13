/**
 * @file flow_timer.h
 * @brief Defines the FlowTimer class, which manages the pomodoro timer functionality.
 */

#ifndef FLOW_TIMER_H
#define FLOW_TIMER_H

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Encoder.h>
#include "lightbar.h"
#include "settings.h"

/**
 * @class FlowTimer
 * @brief Manages the pomodoro timer, including the display, rotary encoder, and state machine.
 */
class FlowTimer
{
public:
    /**
     * @brief Construct a new Flow Timer object.
     * @param lightbar A pointer to the Lightbar object.
     * @param settings A pointer to the Settings object.
     */
    FlowTimer(Lightbar *lightbar, Settings *settings);

    /**
     * @brief Sets up the FlowTimer.
     */
    void setup();

    /**
     * @brief The main loop for the FlowTimer.
     */
    void loop();

private:
    /**
     * @enum State
     * @brief Represents the different states of the FlowTimer.
     */
    enum State
    {
        MENU,                   /**< The main menu. */
        COUNTING_UP,            /**< Counting up state. */
        COUNTING_DOWN,          /**< Counting down state. */
        SELECTING_DOWN_DURATION,/**< Selecting the countdown duration. */
        IDLE,                   /**< Idle state. */
    };
    State currentState = MENU; /**< The current state of the FlowTimer. */

    // Menu
    int menuIndex = 0; /**< The current index of the main menu. */
    String menuOptions[3] = {"UP", "DOWN", "Reset"}; /**< The options for the main menu. */

    // Time
    int flowMinutes = 0; /**< The total number of minutes in flow state. */
    int countdownValue = 20; /**< The current value of the countdown timer. */
    int initialCountdownValue = 20; /**< The initial value of the countdown timer. */
    unsigned long previousMillis = 0; /**< The previous time in milliseconds. */
    int elapsedMinutes = 0; /**< The number of elapsed minutes. */
    bool isCounting = false; /**< Whether the timer is counting. */

    // Inactivity
    unsigned long lastActivityTime = 0; /**< The time of the last activity. */
    const unsigned long inactivityLimit = 3 * 60000; /**< The inactivity limit in milliseconds. */
    const unsigned long displayOffTimeLimit = 30 * 60000; /**< The display off time limit in milliseconds. */
    unsigned long idleStartTime = 0; /**< The time when the idle state started. */
    bool displayOff = false; /**< Whether the display is off. */

    // Hardware
    Adafruit_SSD1306 display; /**< The display object. */
    Adafruit_Encoder encoder; /**< The encoder object. */
    Lightbar *lightbar;      /**< A pointer to the Lightbar object. */
    Settings *settings;      /**< A pointer to the Settings object. */

    // Debounce
    unsigned long buttonDebounceTime = 0; /**< The last time the button was debounced. */
    const unsigned long buttonDebounceDelay = 800; /**< The debounce delay for the button. */
    unsigned long lastRotaryTime = 0; /**< The last time the rotary encoder was debounced. */
    const unsigned long rotaryDebounceDelay = 150; /**< The debounce delay for the rotary encoder. */

    // Methods
    /**
     * @brief Initializes the hardware.
     */
    void initHardware();

    /**
     * @brief Initializes the display.
     */
    void initDisplay();

    /**
     * @brief Updates the display.
     */
    void updateDisplay();

    /**
     * @brief Handles the rotary encoder input.
     */
    void handleRotaryInput();

    /**
     * @brief Handles the button presses.
     * @param currentMillis The current time in milliseconds.
     */
    void handleButtonPresses(unsigned long currentMillis);

    /**
     * @brief Handles the counting logic.
     * @param currentMillis The current time in milliseconds.
     */
    void handleCounting(unsigned long currentMillis);

    /**
     * @brief Handles inactivity.
     * @param currentMillis The current time in milliseconds.
     */
    void handleInactivity(unsigned long currentMillis);

    /**
     * @brief Checks if the button is pressed.
     * @return True if the button is pressed, false otherwise.
     */
    bool buttonPressed();

    /**
     * @brief Starts counting up.
     */
    void startCountingUp();

    /**
     * @brief Starts selecting the countdown duration.
     */
    void startSelectingDownDuration();

    /**
     * @brief Confirms the countdown selection.
     */
    void confirmCountdownSelection();

    /**
     * @brief Stops counting up.
     */
    void stopCountingUp();

    /**
     * @brief Stops counting down.
     */
    void stopCountingDown();

    /**
     * @brief Resets the flow minutes.
     */
    void resetFlowMinutes();

    /**
     * @brief Plays a success animation.
     */
    void successAnimation();

    /**
     * @brief Gets the rotation of the rotary encoder.
     * @return The rotation of the rotary encoder.
     */
    int getRotation();
};

#endif
