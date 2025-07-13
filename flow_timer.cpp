/**
 * @file flow_timer.cpp
 * @brief Implements the FlowTimer class.
 */

#include "flow_timer.h"
#include "constants.h"

FlowTimer::FlowTimer(Lightbar *lightbar, Settings *settings) : display(128, 64, &Wire, -1)
{
    if (lightbar != nullptr)
    {
        this->lightbar = lightbar;
    }
    this->settings = settings;
}

void FlowTimer::setup()
{
    Serial.println("--- FlowTimer Setup ---");
    initHardware();
    initDisplay();
    updateDisplay();
    Serial.println("FlowTimer setup complete, starting loop...");
}

void FlowTimer::loop()
{
    unsigned long currentMillis = millis();

    handleRotaryInput();
    handleButtonPresses(currentMillis);
    handleCounting(currentMillis);
    handleInactivity(currentMillis);
}

void FlowTimer::initHardware()
{
    Serial.println("Initializing hardware...");
    pinMode(PIN_CLK, INPUT);
    pinMode(PIN_DT, INPUT);
    pinMode(PIN_SW, INPUT);
    Serial.println("Hardware initialized.");
}

void FlowTimer::initDisplay()
{
    Wire.begin(PIN_SDA, PIN_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.clearDisplay();
    Serial.println("Display initialized.");
}

void FlowTimer::updateDisplay()
{
    Serial.println("Updating display...");
    display.setTextColor(WHITE);
    display.clearDisplay();

    // Display top row
    String topRowText;

    if (currentState == COUNTING_UP)
    {
        topRowText = "Focus! \x18"; // Focus with upward triangle for counting UP
    }
    else if (currentState == COUNTING_DOWN)
    {
        topRowText = "Focus! \x19"; // Focus with downward triangle for counting DOWN
    }
    else
    {
        topRowText = "Flow: " + String(flowMinutes); // Display total flow minutes when not counting
    }

    Serial.print("Top row text: ");
    Serial.println(topRowText);

    int topRowTextWidth = topRowText.length() * 12; // TextSize 2, so 12 pixels per char
    int topRowX = (128 - topRowTextWidth) / 2;      // Center the text on the top row

    display.setTextSize(2);      // Larger size for top row
    display.setCursor(topRowX, 0); // Centered on top row
    display.print(topRowText);

    // Display main row (menu or counting values)
    String mainRowText;

    if (currentState == MENU)
    {
        mainRowText = menuOptions[menuIndex]; // Display UP, DOWN, or Reset in the menu
    }
    else if (currentState == COUNTING_UP)
    {
        mainRowText = String(elapsedMinutes); // Display counting up minutes
    }
    else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION)
    {
        mainRowText = String(countdownValue); // Display countdown minutes
    }
    else if (currentState == IDLE)
    {
        mainRowText = "IDLE?";
    }

    Serial.print("Main row text: ");
    Serial.println(mainRowText);

    int mainRowTextWidth = mainRowText.length() * 24; // TextSize 4, so 24 pixels per char
    int mainRowX = (128 - mainRowTextWidth) / 2;      // Calculate centered X position

    display.setTextSize(4);       // Larger size for main row
    display.setCursor(mainRowX, 30); // Centered on main row
    display.print(mainRowText);

    display.display(); // Show the updated display
    Serial.println("Display updated.");
}

bool FlowTimer::buttonPressed()
{
    if (digitalRead(PIN_SW) == LOW && (millis() - buttonDebounceTime > buttonDebounceDelay))
    {
        buttonDebounceTime = millis();
        lastActivityTime = millis();
        return true;
    }
    return false;
}

void FlowTimer::handleButtonPresses(unsigned long currentMillis)
{
    if (!buttonPressed())
        return;

    Serial.print("Button pressed. Current state: ");
    Serial.println(currentState);

    switch (currentState)
    {
    case MENU:
        Serial.print("Menu index: ");
        Serial.println(menuIndex);
        if (menuIndex == 0)
        { // UP selected
            startCountingUp();
        }
        else if (menuIndex == 1)
        { // DOWN selected
            startSelectingDownDuration();
        }
        else if (menuIndex == 2)
        { // Reset selected
            resetFlowMinutes();
        }
        break;

    case SELECTING_DOWN_DURATION:
        confirmCountdownSelection();
        break;

    case COUNTING_UP:
        stopCountingUp();
        break;

    case COUNTING_DOWN:
        stopCountingDown();
        break;
    }
    updateDisplay();
}

void FlowTimer::startCountingUp()
{
    currentState = COUNTING_UP;
    elapsedMinutes = 0;
    isCounting = true;
    lastActivityTime = millis();
    Serial.println("Counting UP started.");
}

void FlowTimer::startSelectingDownDuration()
{
    currentState = SELECTING_DOWN_DURATION;
    countdownValue = 20;
    lastActivityTime = millis();
    Serial.println("Selecting DOWN duration.");
}

void FlowTimer::confirmCountdownSelection()
{
    initialCountdownValue = countdownValue;
    currentState = COUNTING_DOWN;
    isCounting = true;
    lastActivityTime = millis();
    Serial.print("Counting DOWN started with ");
    Serial.print(countdownValue);
    Serial.println(" minutes.");
}

void FlowTimer::stopCountingUp()
{
    flowMinutes += elapsedMinutes;
    successAnimation();
    currentState = MENU;
    isCounting = false;
    Serial.println("Counting UP stopped. Returning to MENU.");
}

void FlowTimer::stopCountingDown()
{
    flowMinutes += (initialCountdownValue - countdownValue);
    successAnimation();
    currentState = MENU;
    isCounting = false;
    Serial.println("Counting DOWN stopped. Returning to MENU.");
}

void FlowTimer::resetFlowMinutes()
{
    flowMinutes = 0;
    Serial.println("Flow minutes reset to 0.");
    updateDisplay();
}

void FlowTimer::handleCounting(unsigned long currentMillis)
{
    if (!isCounting || (currentMillis - previousMillis < 60000))
        return;

    previousMillis = currentMillis;

    if (currentState == COUNTING_UP)
    {
        elapsedMinutes++;
        updateDisplay();
        Serial.print("Counting UP: ");
        Serial.println(elapsedMinutes);
    }
    else if (currentState == COUNTING_DOWN)
    {
        countdownValue--;
        if (countdownValue <= 0)
        {
            flowMinutes += initialCountdownValue;
            successAnimation();
            currentState = MENU;
            isCounting = false;
            Serial.println("Countdown finished, returning to MENU.");
        }
        updateDisplay();
        Serial.print("Counting DOWN: ");
        Serial.println(countdownValue);
    }
}

void FlowTimer::successAnimation()
{
    display.clearDisplay();
    int centerX = 64, centerY = 32;

    for (int radius = 2; radius <= 30; radius += 2)
    {
        display.drawCircle(centerX, centerY, radius, WHITE);
        display.display();
        delay(100);

        if (radius % 4 == 0)
        {
            display.clearDisplay();
            display.display();
            delay(2);
        }
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.print("SUCCESS!");
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
}

int FlowTimer::getRotation()
{
    static int previousCLK = digitalRead(PIN_CLK);
    int currentCLK = digitalRead(PIN_CLK);

    if (currentCLK == LOW && previousCLK == HIGH && (millis() - lastRotaryTime > rotaryDebounceDelay))
    {
        lastRotaryTime = millis();
        int DTValue = digitalRead(PIN_DT);

        previousCLK = currentCLK;

        return (DTValue != currentCLK) ? 1 : -1;
    }

    previousCLK = currentCLK;
    return 0;
}

void FlowTimer::handleRotaryInput()
{
    int rotation = getRotation();
    if (rotation == 0)
        return;

    lastActivityTime = millis();
    Serial.print("Rotary input detected. Rotation: ");
    Serial.println(rotation);

    if (currentState == MENU)
    {
        menuIndex = (menuIndex + rotation + 3) % 3;
        Serial.print("New menu index: ");
        Serial.println(menuIndex);
        updateDisplay();
    }
    else if (currentState == SELECTING_DOWN_DURATION)
    {
        countdownValue = max(1, countdownValue + rotation);
        Serial.print("New countdown value: ");
        Serial.println(countdownValue);
        updateDisplay();
    }
}

void FlowTimer::handleInactivity(unsigned long currentMillis)
{
    if (currentMillis >= lastActivityTime)
    {
        unsigned long timeSinceLastActivity = currentMillis - lastActivityTime;

        if ((currentState == MENU || currentState == SELECTING_DOWN_DURATION) &&
            (timeSinceLastActivity > inactivityLimit))
        {
            if (currentState != IDLE)
            {
                currentState = IDLE;
                idleStartTime = millis();
                updateDisplay();
                Serial.println("IDLE state entered due to inactivity.");
            }
        }
    }

    if (currentState == IDLE && !displayOff && (currentMillis - idleStartTime > displayOffTimeLimit))
    {
        displayOff = true;
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        Serial.println("Display turned off after 30 minutes of IDLE.");
    }

    if (currentState == IDLE && (getRotation() != 0 || buttonPressed()))
    {
        currentState = MENU;
        lastActivityTime = millis();

        if (displayOff)
        {
            display.ssd1306_command(SSD1306_DISPLAYON);
            displayOff = false;
            Serial.println("Display turned back on.");
        }

        updateDisplay();
        Serial.println("Exiting IDLE mode. Back to MENU.");
    }
}
