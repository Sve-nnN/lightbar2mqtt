#include "settings.h"

Settings::Settings()
{
}

void Settings::setup()
{
    EEPROM.begin(512);
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
    save();
}

void Settings::setTemperature(uint8_t temperature)
{
    data.temperature = temperature;
    save();
}

void Settings::load()
{
    EEPROM.get(0, data);
}

void Settings::save()
{
    EEPROM.put(0, data);
    EEPROM.commit();
}
