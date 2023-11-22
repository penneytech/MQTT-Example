// Libraries

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"  // Include the configuration header
#include <Adafruit_NeoPixel.h>

// Setup Objects

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = LED_BUILTIN;  // LED pin (builtin LED on ESP8266)
#define LED_STRIP_PIN D2         // LED strip data pin
#define NUMPIXELS 6              // Number of LEDs in the strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// Functions

String generateGUID() {
  String guid = "";
  for (int i = 0; i < 16; ++i) {
    guid += String(random(0, 16), HEX);
  }
  return guid;
}

void setup_wifi() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  // Turn LED off initially

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin, LOW);  // Turn LED ON
    delay(500);
    digitalWrite(ledPin, HIGH);  // Turn LED OFF
    delay(500);
    Serial.print(".");
  }

  digitalWrite(ledPin, LOW);  // Keep LED ON when connected
  Serial.println("\nWiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(topic);

  if (message == "1") {
    digitalWrite(ledPin, LOW);               // Turn LED ON
    strip.fill(strip.Color(255, 255, 255));  // Turn all strip LEDs ON (white color)
    strip.show();
  } else if (message == "0") {
    digitalWrite(ledPin, HIGH);        // Turn LED OFF
    strip.fill(strip.Color(0, 0, 0));  // Turn all strip LEDs OFF
    strip.show();
  } else if (message == "green") {
    digitalWrite(ledPin, HIGH);          // Turn LED OFF
    strip.fill(strip.Color(0, 255, 0));  // Turn all strip LEDs OFF
    strip.show();
  } else if (message == "red") {
    digitalWrite(ledPin, HIGH);          // Turn LED OFF
    strip.fill(strip.Color(255, 0, 0));  // Turn all strip LEDs OFF
    strip.show();
  } else if (message.startsWith("#")) {
    long number = strtol(&message[1], NULL, 16);
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;
    strip.fill(strip.Color(r, g, b));
    strip.show();
  } else if (message == "blink") {
    // Perform the blinking and red center LEDs sequence
    blinkLED(0, strip.Color(255, 165, 0), 1000);     // Blink first LED orange
    blinkLED(5, strip.Color(255, 165, 0), 1000);     // Blink last LED orange
    strip.setPixelColor(2, strip.Color(255, 0, 0));  // Center LEDs red
    strip.setPixelColor(3, strip.Color(255, 0, 0));
    strip.show();
    delay(1000);
    strip.fill(strip.Color(0, 0, 0));  // Turn all LEDs off
    strip.show();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + generateGUID();
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("esp8266/light");  // Subscribe to topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  randomSeed(micros());  // Seed the random number generator with current micros

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);

  strip.begin();  // Initialize the NeoPixel strip
  strip.show();   // Turn off all pixels initially

  reconnect();  // Ensure MQTT connection
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
