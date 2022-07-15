#include "TCP.h"

// Start and stop bytes for commands
const char ESP_START = 0xf0;
const char TEENSY_START = 0xf1;
const char WIO_START = 0xf2;
const char ESP_STOP = 0xf8;
const char TEENSY_STOP = 0xf9;
const char WIO_STOP = 0xfa;
const char START = WIO_START;
const char STOP = WIO_STOP;

void execute(const char* char_buffer);

TCP::TCP() { begin(); }

void TCP::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(network.ssid, network.password);
  Serial.println("Connecting to Wifi");
  uint16_t retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (retries++ >= 20) {
      Serial.println();
      Serial.println("Unable to connect to Wifi. Rebooting the WIO Terminal..");
      NVIC_SystemReset();
    }
    delay(500);
  }
  // Functions below may or may not work
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  if (retries > 0) Serial.println();
  Serial.print("Connected to ");
  Serial.println(network.ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("WiFi ready");
}

bool TCP::connected() {
  if (!client.connected()) {
    client.stop();
    if (!client.connect(network.hostname, network.tcp_port)) {
      Serial.println("Connection failed...");
      return false;
    }
  }
  return true;
}
// Check input from WiFI connected to ESP8266
void TCP::loop() {
  if (!connected()) return;
  static boolean command = false;
  static String message = "";
  while (client.available()) {
    char c = (char)client.read();
    if (c == START) {
      message = "";
      command = true;
    } else if (c == STOP) {
      if (command) {
        execute(message.c_str());
      }
      message = "";
      command = false;
    } else {
      if (command) {
        // Accumulate command until stop character
        message += c;
      } else {
        // Serial passthrough (USB console logger)
        Serial.write(c);
      }
    }
  }
}

// Executes a json command present in the char_buffer
void execute(const char* char_buffer) {
  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, char_buffer);
  if (err) {
    Serial.printf("Deserialization error: %s\n", err.c_str());
    return;
  }
  String event = doc["event"];
  Serial.printf("Executing: %s\n", event.c_str());
  Serial.println(char_buffer);
}

void TCP::rpc(String msg) {
  if (!connected()) return;
  client.write(TEENSY_START);
  client.print(msg);
  client.write(TEENSY_STOP);
  client.flush();
  //  Serial.println(msg);
}