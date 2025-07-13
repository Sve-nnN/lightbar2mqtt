#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>

class Settings
{
public:
    Settings();
    void setup();
    void loop();

    // Getters
    uint8_t getBrightness();
    uint8_t getTemperature();

    // Setters
    void setBrightness(uint8_t brightness);
    void setTemperature(uint8_t temperature);

private:
    struct
    {
        uint8_t brightness;
        uint8_t temperature;
    } data;

    void load();
    void save();
};

#endif
