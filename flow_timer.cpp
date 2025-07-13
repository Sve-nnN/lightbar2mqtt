#include "flow_timer.h"
#include "constants.h"

FlowTimer::FlowTimer(Lightbar *lightbar, Settings *settings) : display(128, 64, &Wire, -1),
                                                              encoder(PIN_DT, PIN_CLK)
{
    this->lightbar = lightbar;
    this->settings = settings;
}

void FlowTimer::setup()
{
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
    pinMode(PIN_SW, INPUT_PULLUP);
    encoder.begin();
}

void FlowTimer::initDisplay()
{
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
    display.setTextColor(WHITE);
    display.clearDisplay();

    String topRowText;

    if (currentState == COUNTING_UP)
    {
        topRowText = "Focus! \\x18";
    }
    else if (currentState == COUNTING_DOWN)
    {
        topRowText = "Focus! \\x19";
    }
    else
    {
        topRowText = "Flow: " + String(flowMinutes);
    }

    int topRowTextWidth = topRowText.length() * 12;
    int topRowX = (128 - topRowTextWidth) / 2;

    display.setTextSize(2);
    display.setCursor(topRowX, 0);
    display.print(topRowText);

    String mainRowText;

    if (currentState == MENU)
    {
        mainRowText = menuOptions[menuIndex];
    }
    else if (currentState == COUNTING_UP)
    {
        mainRowText = String(elapsedMinutes);
    }
    else if (currentState == COUNTING_DOWN || currentState == SELECTING_DOWN_DURATION)
    {
        mainRowText = String(countdownValue);
    }
    else if (currentState == IDLE)
    {
        mainRowText = "IDLE?";
    }

    int mainRowTextWidth = mainRowText.length() * 24;
    int mainRowX = (128 - mainRowTextWidth) / 2;

    display.setTextSize(4);
    display.setCursor(mainRowX, 30);
    display.print(mainRowText);

    display.display();
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

    switch (currentState)
    {
    case MENU:
        if (menuIndex == 0)
        {
            startCountingUp();
        }
        else if (menuIndex == 1)
        {
            startSelectingDownDuration();
        }
        else if (menuIndex == 2)
        {
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
    return encoder.read();
}

void FlowTimer::handleRotaryInput()
{
    int rotation = getRotation();
    if (rotation == 0)
        return;

    lastActivityTime = millis();
    Serial.print(millis());
    Serial.print(" - Rotation detected, activity timer reset. Rotation: ");
    Serial.println(rotation);

    if (currentState == MENU)
    {
        if (rotation > 0)
        {
            menuIndex = (menuIndex + 1) % 3;
        }
        else
        {
            menuIndex = (menuIndex - 1 + 3) % 3;
        }
        updateDisplay();
        Serial.print(millis());
        Serial.print(" - Menu option: ");
        Serial.println(menuOptions[menuIndex]);
    }
    else if (currentState == SELECTING_DOWN_DURATION)
    {
        if (rotation > 0)
        {
            countdownValue++;
        }
        else
        {
            countdownValue--;
            if (countdownValue < 1)
            {
                countdownValue = 1;
            }
        }
        updateDisplay();
        Serial.print(millis());
        Serial.print(" - Countdown value: ");
        Serial.println(countdownValue);
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
                Serial.print(millis());
                Serial.println(" - IDLE state entered due to inactivity.");
            }
        }
    }

    if (currentState == IDLE && !displayOff && (currentMillis - idleStartTime > displayOffTimeLimit))
    {
        displayOff = true;
        display.ssd1306_command(SSD1306_DISPLAYOFF);
        Serial.print(millis());
        Serial.println(" - Display turned off after 30 minutes of IDLE.");
    }

    if (currentState == IDLE && (getRotation() != 0 || buttonPressed()))
    {
        currentState = MENU;
        lastActivityTime = millis();

        if (displayOff)
        {
            display.ssd1306_command(SSD1306_DISPLAYON);
            displayOff = false;
            Serial.print(millis());
            Serial.println(" - Display turned back on.");
        }

        updateDisplay();
        Serial.print(millis());
        Serial.println(" - Exiting IDLE mode. Back to MENU.");
    }
}
