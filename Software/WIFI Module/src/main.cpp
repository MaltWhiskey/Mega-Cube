#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <string.h>

// Start and stop bytes for commands (outside of ascii range)
const char ESP_START = 0xf0;
const char TEENSY_START = 0xf1;
const char WIO_START = 0xf2;
const char ESP_STOP = 0xf8;
const char TEENSY_STOP = 0xf9;
const char WIO_STOP = 0xfa;
const char START = ESP_START;
const char STOP = ESP_STOP;
// Buffers used for redirection
const uint8_t WIFI_TO_SERIAL = 0;
const uint8_t SERIAL_TO_WIFI = 1;

void startNTP();
void startWiFi();
void startOTA();
void startMDNS();
void startSerial();
void redirect(char c, uint8_t buf);
void execute(const char* char_buffer);
void rpc(String msg);

static struct {
  struct {
    struct {
      char ssid[32] = "-^..^-";
      char password[64] = "qazwsxedc";
    } wifi;
    struct {
      char hostname[64] = "MegaCube";
      uint16_t tcp_port = 8000;
      uint16_t ota_port = 8266;
    } server;
  } network;
} Config;

WiFiServer server(Config.network.server.tcp_port);
// Active WiFiClient tcp/ip connection
WiFiClient wifiClient;
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org");

void setup() {
  startSerial();
  startWiFi();
  startOTA();
  startMDNS();
  startNTP();
}

void loop() {
  // Check for OTA updates
  ArduinoOTA.handle();
  // Check for new incomming connections
  WiFiClient client = server.available();
  if (client) {
    // Terminate the old connection
    if (wifiClient) wifiClient.stop();
    // Set new wifiClient as active connection
    wifiClient = client;
  }
  // Redirect Serial to WiFi
  if (Serial.available()) {
    char c = Serial.read();
    redirect(c, SERIAL_TO_WIFI);
  }
  // Redirect WiFi to Serial
  if (wifiClient && wifiClient.connected() && wifiClient.available()) {
    char c = wifiClient.read();
    redirect(c, WIFI_TO_SERIAL);
  }
}

// Need to redirect this character or it is part of a command
void redirect(char c, uint8_t src) {
  static boolean command[] = {false, false};
  static String message[] = {"", ""};
  if (c == START) {
    message[src] = "";
    command[src] = true;
  } else if (c == STOP) {
    if (command[src]) {
      execute(message[src].c_str());
    }
    message[src] = "";
    command[src] = false;
  } else {
    if (command[src]) {
      // Accumulate command until stop character
      message[src] += c;
    } else {
      // Serial passthrough (WIFI to Serial)
      if (src == WIFI_TO_SERIAL) Serial.write(c);
      // Serial passthrough (Serial to WiFi)
      if (src == SERIAL_TO_WIFI && wifiClient && wifiClient.connected())
        wifiClient.write(c);
    }
  }
}

void startSerial() {
  Serial.begin(460800);
  Serial.println("ESP8266 Serial ready");
}

void startWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(Config.network.wifi.ssid, Config.network.wifi.password);
  Serial.println("Connecting to Wifi");
  uint16_t retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (retries++ >= 20) {
      Serial.println();
      Serial.println("Unable to connect to Wifi. Rebooting the ESP..");
      Serial.flush();
      ESP.reset();
    }
    delay(500);
  }
  // Functions below may or may not work
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  // Alternatively implement event handler on disconnect
  WiFi.onStationModeDisconnected(
      [](const WiFiEventStationModeDisconnected& event) {
        Serial.println("Disconnected from Wi-Fi, trying to reconnect...");
        WiFi.disconnect();
        startWiFi();
      });
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(Config.network.wifi.ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Port: ");
  Serial.println(Config.network.server.tcp_port);
  server.begin(Config.network.server.tcp_port);
  Serial.println("ESP8266 WiFi ready");
}

void startOTA() {
  ArduinoOTA.setPort(Config.network.server.ota_port);
  ArduinoOTA.setHostname(Config.network.server.hostname);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else
      type = "filesystem";
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("ESP8266 OTA ready");
}

void startMDNS() {
  MDNS.begin(Config.network.server.hostname);
  Serial.printf("ESP8266 mDNS responder %s.local ready\n",
                Config.network.server.hostname);
}

void startNTP() { ntpClient.begin(); }

void rpc(String msg) {
  Serial.write(TEENSY_START);
  Serial.print(msg);
  Serial.write(TEENSY_STOP);
}

// Executes a json command present in the char_buffer
void execute(const char* char_buffer) {
  char buffer[1024];
  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, char_buffer);
  if (err) {
    Serial.printf("Deserialization error: %s\n", err.c_str());
    return;
  }
  String event = doc["event"];
  Serial.printf("Executing: %s\n", event.c_str());
  if (event.equals("time")) {
    doc.clear();
    ntpClient.update();
    doc["event"] = "time";
    doc["epoc"] = ntpClient.getEpochTime();
    serializeJson(doc, buffer);
    doc.clear();
    rpc(buffer);
  }
}