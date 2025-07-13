# lightbar2mqtt

Este proyecto permite controlar la barra de luz de monitor de Xiaomi (MJGJD01YL) a través de MQTT. Utiliza un ESP8266 y un módulo NRF24L01 para comunicarse con la barra de luz.

## Características

- Encender y apagar la barra de luz.
- Ajustar el brillo.
- Ajustar la temperatura de color.
- Emparejar con nuevas barras de luz.
- Integración con Home Assistant a través de MQTT Discovery.

## Requisitos

- Placa ESP8266 (por ejemplo, NodeMCU, Wemos D1 Mini)
- Módulo NRF24L01+ SMD
- Arduino IDE o PlatformIO

## Conexión del NRF24L01+ SMD al ESP8266

| NRF24L01+ | ESP8266 (NodeMCU) |
| :--- | :--- |
| VCC | 3.3V |
| GND | GND |
| CSN | D8 (GPIO15) |
| CE | D4 (GPIO2) |
| MOSI | D7 (GPIO13) |
| MISO | D6 (GPIO12) |
| SCK | D5 (GPIO14) |
| IRQ | No conectado |

**Diagrama de conexión:**

```
        +-----------------+      +-----------------+
        |   NRF24L01+     |      |      ESP8266      |
        |                 |      |                 |
        |             VCC |<---->| 3.3V            |
        |             GND |<---->| GND             |
        |             CSN |<---->| D8 (GPIO15)     |
        |              CE |<---->| D4 (GPIO2)      |
        |            MOSI |<---->| D7 (GPIO13)     |
        |            MISO |<---->| D6 (GPIO12)     |
        |             SCK |<---->| D5 (GPIO14)     |
        +-----------------+      +-----------------+
```

## Configuración

1.  Abre el archivo `config.h`.
2.  Configura tus credenciales de WiFi:
    ```c
    #define WIFI_SSID "TU_WIFI_SSID"
    #define WIFI_PASSWORD "TU_WIFI_PASSWORD"
    ```
3.  Configura los detalles de tu servidor MQTT:
    ```c
    #define MQTT_SERVER "TU_SERVIDOR_MQTT"
    #define MQTT_PORT 1883
    #define MQTT_USER "TU_USUARIO_MQTT"
    #define MQTT_PASSWORD "TU_CONTRASEÑA_MQTT"
    ```
4.  Define los números de serie de tus controles remotos y barras de luz:
    ```c
    #define REMOTES         \
        {                   \
            {0x1234, "LR1"} \
        }

    #define LIGHTBARS         \
        {                     \
            {0x5678, "Light1"} \
        }
    ```

## Uso

Una vez que el ESP8266 esté en funcionamiento, se conectará a tu red WiFi y al servidor MQTT. La barra de luz aparecerá en Home Assistant a través de MQTT Discovery.

### Temas MQTT

-   **Comando:** `lightbar2mqtt/<client_id>/<serial>/command`
-   **Estado:** `lightbar2mqtt/<client_id>/<serial>/state`
-   **Emparejamiento:** `lightbar2mqtt/<client_id>/<serial>/pair`
-   **Disponibilidad:** `lightbar2mqtt/<client_id>/availability`

### Carga útil del comando

La carga útil del comando debe ser un objeto JSON con las siguientes propiedades:

-   `state`: `"ON"` o `"OFF"`
-   `brightness`: 0-15
-   `color_temp`: 153-370

**Ejemplo:**

```json
{
    "state": "ON",
    "brightness": 10,
    "color_temp": 250
}
```
