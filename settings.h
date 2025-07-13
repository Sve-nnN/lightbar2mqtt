/**
 * @file settings.h
 * @brief Defines the Settings class, which manages the application settings.
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>

/**
 * @class Settings
 * @brief Manages the application settings, storing them in EEPROM.
 */
class Settings
{
public:
    /**
     * @brief Construct a new Settings object.
     */
    Settings();

    /**
     * @brief Sets up the Settings.
     */
    void setup();

    /**
     * @brief The main loop for the Settings.
     */
    void loop();

    /**
     * @brief Gets the brightness.
     * @return The brightness.
     */
    uint8_t getBrightness();

    /**
     * @brief Gets the temperature.
     * @return The temperature.
     */
    uint8_t getTemperature();

    /**
     * @brief Sets the brightness.
     * @param brightness The brightness to set.
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Sets the temperature.
     * @param temperature The temperature to set.
     */
    void setTemperature(uint8_t temperature);

private:
    /**
     * @struct data
     * @brief The data to be stored in EEPROM.
     */
    struct
    {
        uint8_t brightness; /**< The brightness of the lightbar. */
        uint8_t temperature; /**< The temperature of the lightbar. */
    } data;

    /**
     * @brief Loads the settings from EEPROM.
     */
    void load();

    /**
     * @brief Saves the settings to EEPROM.
     */
    void save();
};

#endif
