# IGOR - Focus Timer & Lightbar Controller

This project combines a powerful focus timer with a smart light bar controller, all in one device. It's designed to help you stay on track with your focus sessions while also allowing you to control your Xiaomi Mi Monitor Light Bar via MQTT.

## Features

- **IGOR Focus Timer:**
  - **Count Up/Down:** Time your focus sessions with either a count-up or count-down timer.
  - **Rotary Encoder Control:** Easily navigate menus and set timers with a rotary encoder.
  - **OLED Display:** Get real-time feedback on your timer and total focus time.
  - **IDLE Mode:** The device enters a low-power idle mode after a period of inactivity.
- **Lightbar Control:**
  - **MQTT Integration:** Control your light bar's power, brightness, and color temperature via MQTT.
  - **Home Assistant Discovery:** Seamlessly integrate with Home Assistant.

## Requirements

- ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
- NRF24L01+ SMD Module
- Adafruit SSD1306 OLED Display (128x64)
- KY-040 Rotary Encoder
- Arduino IDE or PlatformIO
- [RotaryEncoder](https://github.com/mathertel/RotaryEncoder) library by Matthias Hertel

## Pinout

| Component | Pin ESP8266 (NodeMCU) |
| :--- | :--- |
| NRF24L01+ VCC | 3.3V |
| NRF24L01+ GND | GND |
| NRF24L01+ CSN | D8 (GPIO15) |
| NRF24L01+ CE | D1 (GPIO5) |
| NRF24L01+ MOSI | D7 (GPIO13) |
| NRF24L01+ MISO | D6 (GPIO12) |
| NRF24L01+ SCK | D5 (GPIO14) |
| OLED VCC | 5V |
| OLED GND | GND |
| OLED SCL | D2 (GPIO4) |
| OLED SDA | D3 (GPIO0) |
| Rotary CLK | D0 (GPIO16) |
| Rotary DT | D4 (GPIO2) |
| Rotary SW | A0 |
| Rotary + | 5V |
| Rotary GND | GND |

## Setup

1.  **Hardware:** Connect all the components according to the pinout table above.
2.  **Software:**
    - Open the project in the Arduino IDE or PlatformIO.
    - Install the `RotaryEncoder` library by Matthias Hertel from the Arduino IDE's library manager.
    - Create a `config.h` file by copying `config-example.h`.
    - In `config.h`, configure your WiFi credentials, MQTT broker details, and the serial numbers for your light bar and remote.
    - Upload the firmware to your ESP8266.

## How to Use

### IGOR Focus Timer

- **Main Menu:**
  - **UP:** Start a count-up timer for open-ended focus sessions.
  - **DOWN:** Set a countdown timer for focused work intervals.
  - **Reset:** Reset your total accumulated focus time.
- **Controls:**
  - **Rotate:** Navigate through the menu options.
  - **Click:** Select an option or start/stop a timer.

### Lightbar Control

The light bar can be controlled via MQTT. The device will automatically publish its availability and state to your MQTT broker.

- **Topics:**
  - **Command:** `lightbar2mqtt/<client_id>/<serial>/command`
  - **State:** `lightbar2mqtt/<client_id>/<serial>/state`
- **Payload:**
  ```json
  {
      "state": "ON" or "OFF",
      "brightness": 0-15,
      "color_temp": 153-370
  }
  ```

## Roadmap

- Sound notifications
- Internet time via WiFi
- More animations and transitions
- API for external app integration

This project is open-source and contributions are welcome! If you have ideas for new features or improvements, please open an issue or submit a pull request.
