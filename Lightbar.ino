#include <ESP8266WiFi.h>

#include "constants.h"
#include "config.h"
#include "config.h"
#include "radio.h"
#include "lightbar.h"
#include "mqtt.h"
#include "Igor.h"
#include "settings.h"

WiFiClient wifiClient;
Radio radio(RADIO_PIN_CE, RADIO_PIN_CSN);
MQTT mqtt(&wifiClient, MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, MQTT_ROOT_TOPIC, HOME_ASSISTANT_DISCOVERY, HOME_ASSISTANT_DISCOVERY_PREFIX);
Settings settings;
Igor igor;

void setupWifi()
{
  Serial.print("[WiFi] Connecting to network \"");
  Serial.print(WIFI_SSID);
  Serial.print("\"...");

  WiFi.hostname(mqtt.getClientId());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint retries = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    retries++;
    if (retries > 60)
      ESP.restart();
  }
  Serial.println();
  Serial.println("[WiFi] connected!");

  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("##########################################");
  Serial.println("# LIGHTBAR2MQTT            (Version " + constants::VERSION + ") #");
  Serial.println("# https://github.com/ebinf/lightbar2mqtt #");
  Serial.println("##########################################");

  settings.setup();
  igor.setup();

  Lightbar *lightbar = new Lightbar(&radio, LIGHTBARS[0].serial, LIGHTBARS[0].name);

  radio.setup();

  setupWifi();

  for (int i = 0; i < sizeof(REMOTES) / sizeof(SerialWithName); i++)
  {
    Remote *remote = new Remote(&radio, REMOTES[i].serial, REMOTES[i].name);
    mqtt.addRemote(remote);
  }

  mqtt.addLightbar(lightbar);

  mqtt.setup();
}

void loop()
{
  if (!WiFi.isConnected())
  {
    Serial.println("[WiFi] connection lost!");
    setupWifi();
  }

  mqtt.loop();
  radio.loop();
  igor.loop();
}