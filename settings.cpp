/**
 * @file settings.cpp
 * @brief Implements the Settings class.
 */

#include "settings.h"

Settings::Settings()
{
}

void Settings::setup()
{
    // Initialize EEPROM with a size of 512 bytes
    EEPROM.begin(512);
    // Load the settings from EEPROM
    load();
}

void Settings::loop()
{
}

uint8_t Settings::getBrightness()
{
    return data.brightness;
}

uint8_t Settings::getTemperature()
{
    return data.temperature;
}

void Settings::setBrightness(uint8_t brightness)
{
    data.brightness = brightness;
    // Save the settings to EEPROM
    save();
}

void Settings::setTemperature(uint8_t temperature)
{
    data.temperature = temperature;
    // Save the settings to EEPROM
    save();
}

void Settings::load()
{
    // Read the settings from EEPROM
    EEPROM.get(0, data);
}

void Settings::save()
{
    // Write the settings to EEPROM
    EEPROM.put(0, data);
    // Commit the changes to EEPROM
    EEPROM.commit();
}
