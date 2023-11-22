#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h" // Include the configuration header
#include <Adafruit_NeoPixel.h>

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = LED_BUILTIN; // LED pin (builtin LED on ESP8266)
#define LED_STRIP_PIN D2          // LED strip data pin
#define NUMPIXELS 6               // Number of LEDs in the strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup_wifi() {
  // Existing setup_wifi code...
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (message == "1") {
    digitalWrite(ledPin, LOW); // Turn LED ON
    strip.fill(strip.Color(255, 255, 255)); // Turn all strip LEDs ON (white color)
    strip.show();
  } else if (message == "0") {
    digitalWrite(ledPin, HIGH); // Turn LED OFF
    strip.fill(strip.Color(0, 0, 0)); // Turn all strip LEDs OFF
    strip.show();
  } else if (message == "blink") {
    // Perform the blinking and red center LEDs sequence
    blinkLED(0, strip.Color(255, 165, 0), 1000); // Blink first LED orange
    blinkLED(5, strip.Color(255, 165, 0), 1000); // Blink last LED orange
    strip.setPixelColor(2, strip.Color(255, 0, 0)); // Center LEDs red
    strip.setPixelColor(3, strip.Color(255, 0, 0));
    strip.show();
    delay(1000);
    strip.fill(strip.Color(0, 0, 0)); // Turn all LEDs off
    strip.show();
  }
}

void reconnect() {
  // Existing reconnect code...
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);

  strip.begin(); // Initialize the NeoPixel strip
  strip.show();  // Turn off all pixels initially

  reconnect(); // Ensure MQTT connection
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void blinkLED(int index, uint32_t color, int wait) {
  for (int i = 0; i < 2; i++) {
    strip.setPixelColor(index, color);
    strip.show();
    delay(wait / 2);
    strip.setPixelColor(index, strip.Color(0, 0, 0));
    strip.show();
    delay(wait / 2);
  }
}
