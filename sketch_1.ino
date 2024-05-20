#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "BRUNO 2G";
const char* password = "26061704";

const char* mqtt_server = "industrial.api.ubidots.com";
const char* token = "BBUS-SzFESx6tkq0s0Hxqq8cjjbqVLFStIo";
const char* device_label = "esp32";
const char* variable_label = "temperatura";

const int oneWireBus = 4;

OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
WiFi.begin("BRUNO 2G", "26061704");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");
}

void callback(char* topic, byte* payload, unsigned int length) {
}

void setup() {
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  sensors.begin();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Client", token, "")) {
      Serial.println("conectado");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors.requestTemperatures();

  float temperatureC = sensors.getTempCByIndex(0);
  char tempString[8];
  dtostrf(temperatureC, 1, 2, tempString);
  Serial.print("Temperatura: ");
  Serial.println(tempString);

  char payload[100];
  sprintf(payload, "{\"%s\": %s}", variable_label, tempString);

  char topic[150];
  sprintf(topic, "/v1.6/devices/%s", device_label);
  client.publish(topic, payload);

  delay(10000);
}
