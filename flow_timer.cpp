#include "flow_timer.h"
#include "constants.h"

FlowTimer::FlowTimer(Lightbar *lightbar, Settings *settings) : display(128, 64, &Wire, -1),
                                                              encoder(PIN_CLK, PIN_DT, PIN_SW)
{
    this->lightbar = lightbar;
    this->settings = settings;
}

void FlowTimer::setup()
{
    setupOled();
    setupRotaryEncoder();
    currentState = STATE_MENU;
    menuPosition = 0;
    totalFocusTime = 0;
    sessionStartTime = 0;
    sessionDuration = 0;
    lastActivityTime = millis();
    countDownTime = 25;
}

void FlowTimer::loop()
{
    handleRotaryEncoder();

    switch (currentState)
    {
    case STATE_MENU:
        displayMenu();
        break;
    case STATE_COUNT_UP:
        displayCountUp();
        break;
    case STATE_COUNT_DOWN_SETUP:
        displayCountDownSetup();
        break;
    case STATE_COUNT_DOWN:
        displayCountDown();
        break;
    case STATE_IDLE:
        displayIdle();
        break;
    }

    if (millis() - lastActivityTime > constants::DISPLAY_OFF_TIMEOUT)
    {
        display.clearDisplay();
        display.display();
    }
    else if (millis() - lastActivityTime > constants::IDLE_TIMEOUT)
    {
        currentState = STATE_IDLE;
    }
}

void FlowTimer::setupOled()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("IGOR");
    display.display();
    delay(1000);
}

void FlowTimer::setupRotaryEncoder()
{
    encoder.begin();
}

void FlowTimer::handleRotaryEncoder()
{
    static int lastPos = 0;
    int newPos = encoder.getPosition();
    if (lastPos != newPos)
    {
        handleRotate();
        lastPos = newPos;
    }

    RotaryEncoder::Direction dir = encoder.getDirection();
    if (dir != RotaryEncoder::NOROTATION)
    {
        handleRotate();
    }

    if (encoder.isEncoderButtonClicked())
    {
        handleClick();
    }
}

void FlowTimer::handleClick()
{
    lastActivityTime = millis();
    switch (currentState)
    {
    case STATE_MENU:
        if (menuPosition == 0)
        {
            currentState = STATE_COUNT_UP;
            sessionStartTime = millis();
        }
        else if (menuPosition == 1)
        {
            currentState = STATE_COUNT_DOWN_SETUP;
        }
        else if (menuPosition == 2)
        {
            reset();
        }
        break;
    case STATE_COUNT_UP:
        sessionDuration = (millis() - sessionStartTime) / 60000;
        totalFocusTime += sessionDuration;
        currentState = STATE_MENU;
        break;
    case STATE_COUNT_DOWN_SETUP:
        currentState = STATE_COUNT_DOWN;
        sessionStartTime = millis();
        break;
    case STATE_COUNT_DOWN:
        sessionDuration = (millis() - sessionStartTime) / 60000;
        totalFocusTime += sessionDuration;
        currentState = STATE_MENU;
        break;
    case STATE_IDLE:
        currentState = STATE_MENU;
        break;
    }
}

void FlowTimer::handleRotate()
{
    lastActivityTime = millis();
    RotaryEncoder::Direction dir = encoder.getDirection();

    if (currentState == STATE_MENU)
    {
        if (dir == RotaryEncoder::UP)
        {
            menuPosition = (menuPosition + 1) % 3;
        }
        else if (dir == RotaryEncoder::DOWN)
        {
            menuPosition = (menuPosition - 1 + 3) % 3;
        }
    }
    else if (currentState == STATE_COUNT_DOWN_SETUP)
    {
        if (dir == RotaryEncoder::UP)
        {
            countDownTime++;
        }
        else if (dir == RotaryEncoder::DOWN)
        {
            countDownTime--;
            if (countDownTime < 1)
            {
                countDownTime = 1;
            }
        }
    }
}

void FlowTimer::displayMenu()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Total: ");
    display.print(totalFocusTime);
    display.println(" min");

    display.setTextSize(2);
    display.setCursor(20, 20);
    if (menuPosition == 0)
    {
        display.println("> UP");
    }
    else
    {
        display.println("  UP");
    }

    display.setCursor(20, 40);
    if (menuPosition == 1)
    {
        display.println("> DOWN");
    }
    else
    {
        display.println("  DOWN");
    }

    display.setTextSize(1);
    display.setCursor(20, 60);
    if (menuPosition == 2)
    {
        display.println("> Reset");
    }
    else
    {
        display.println("  Reset");
    }
    display.display();
}

void FlowTimer::displayCountUp()
{
    unsigned long elapsed = (millis() - sessionStartTime) / 60000;
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.println("Focus!");
    display.setTextSize(3);
    display.setCursor(40, 40);
    display.print(elapsed);
    display.display();
}

void FlowTimer::displayCountDownSetup()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.println("Set Time");
    display.setTextSize(3);
    display.setCursor(40, 40);
    display.print(countDownTime);
    display.display();
}

void FlowTimer::displayCountDown()
{
    unsigned long elapsed = (millis() - sessionStartTime) / 1000;
    unsigned long remaining = (countDownTime * 60) - elapsed;
    if (remaining < 0)
    {
        remaining = 0;
    }

    if (remaining == 0)
    {
        displaySuccess();
        totalFocusTime += countDownTime;
        currentState = STATE_MENU;
        return;
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 20);
    display.println("Focus!");
    display.setTextSize(3);
    display.setCursor(40, 40);
    display.print(remaining / 60);
    display.print(":");
    if (remaining % 60 < 10)
    {
        display.print("0");
    }
    display.print(remaining % 60);
    display.display();
}

void FlowTimer::displayIdle()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 30);
    display.println("IDLE?");
    display.display();
}

void FlowTimer::displaySuccess()
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20, 30);
    display.println("Success!");
    display.display();
    delay(2000);
}

void FlowTimer::reset()
{
    totalFocusTime = 0;
}
