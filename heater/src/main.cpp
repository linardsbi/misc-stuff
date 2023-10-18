#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <dht11.h>
#include <string_view>

#define ssid "HUAWEI-B935-32"
#define password "verysecret"

using namespace std::string_view_literals;

static ESP8266WebServer server(80);

constexpr auto heater_pin = D4;

static void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

constexpr auto fields = std::array{"temp-limit-value"sv, "temp-value"sv,
                                   "humidity-value"sv, "heater-value"sv};

static auto field_values = std::array<uint16_t, 4>{0, 0, 0, 0};

static void handleSet() {
  for (std::uint8_t i = 0; i < server.args(); i++) {
    for (uint8_t j = 0; j < fields.size(); ++j) {
      if (fields[j] == std::string_view{server.argName(i).c_str()}) {
        field_values[j] = static_cast<uint16_t>(server.arg(i).toInt());
      }
    }
  }

  EEPROM.put(0, field_values[0]);
  EEPROM.commit();
  server.send(200, "success");
}

static void handleData() {
  String values;

  for (std::size_t i = 0; i < fields.size(); ++i) {
    char buffer[50]{};
    sprintf(buffer, "%s=%i,", fields[i].data(), field_values[i]);
    values += buffer;
  }

  server.send(200, "text/plain", values);
}

void setup() {
  EEPROM.begin(sizeof(field_values[0]));
  Serial.begin(115200);

  IPAddress local_ip(192, 168, 1, 69);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  MDNS.begin("heater");

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);

  server.on("/", []() {
    server.sendHeader("Location", "/manage");
    server.send(302);
  });
  server.on("/manage", []() { server.send(200, "text/html", {{"manage"}}); });
  server.on("/set", handleSet);
  server.on("/data", handleData);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  EEPROM.get(0, field_values[0]);
}

uint32_t last_dht_poll = 0;

dht11 DHT11;
constexpr uint8_t DHT_PIN = D6;
constexpr std::pair<uint16_t, uint16_t> temp_limits = {140, 260};
int error = 0;

void loop(void) {
  if (millis() - last_dht_poll > 2000) {
    error = DHT11.read(DHT_PIN);

    field_values[1] = static_cast<uint16_t>(DHT11.temperature * 10);
    field_values[2] = DHT11.humidity;

    Serial.print("Temperature  limit: ");
    Serial.println(field_values[0]);

    Serial.print("Temperature  (C): ");
    Serial.println(field_values[1]);

    Serial.print("Humidity (%): ");
    Serial.println(field_values[2]);

    Serial.print("Heater: ");
    Serial.println(field_values[3] > 0 ? "ON" : "OFF");

    Serial.print("Error: ");
    Serial.println(error);

    last_dht_poll = millis();
  }

  if (field_values[0] < temp_limits.first) {
    field_values[0] = temp_limits.first;
  } else if (field_values[0] > temp_limits.second) {
    field_values[0] = temp_limits.second;
  }

  if (error == DHTLIB_OK && field_values[0] > field_values[1]) {
    field_values[3] = HIGH;
  } else {
    field_values[3] = LOW;
  }

  digitalWrite(heater_pin, field_values[3]);

  server.handleClient();
}