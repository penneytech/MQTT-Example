#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h" // Include the configuration header
#include <Adafruit_NeoPixel.h>

const char* clientId = "esp8266-"; // Prefix for client ID
const char* topic = "esp8266/light";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = LED_BUILTIN; // LED pin (builtin LED on ESP8266)
#define LED_STRIP_PIN D2          // LED strip data pin
#define NUMPIXELS 6               // Number of LEDs in the strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup_wifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String message;

  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  if (message == "1") {
    digitalWrite(ledPin, LOW); // Turn LED ON
    strip.fill(strip.Color(255, 255, 255)); // Set all strip LEDs to green color
    strip.show();
  } else if (message == "0") {
    digitalWrite(ledPin, HIGH); // Turn LED OFF
    strip.fill(strip.Color(0, 0, 0)); // Set all strip LEDs off
    strip.show();
  } else if (message == "green") {
    strip.fill(strip.Color(0, 255, 0)); // Set all strip LEDs to green color
    strip.show();
  } else if (message == "red") {
    strip.fill(strip.Color(255, 0, 0)); // Set all strip LEDs to green color
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
  } else if (message.startsWith("#") && message.length() == 7) {
    // Check if the message is a hex color value starting with a hashtag

    // Extract the RGB values from the hex color value
    int r = strtol(message.substring(1, 3).c_str(), NULL, 16);
    int g = strtol(message.substring(3, 5).c_str(), NULL, 16);
    int b = strtol(message.substring(5, 7).c_str(), NULL, 16);

    // Set all strip LEDs to the extracted color
    strip.fill(strip.Color(r, g, b)); 
    strip.show();
  }
}

void reconnect() {
  while (!client.connected()) {
    String clientIdWithGUID = clientId;
    clientIdWithGUID += generateGUID();  // Append random GUID to client ID
    Serial.print("Connecting to MQTT Server...");
    if (client.connect(clientIdWithGUID.c_str(), mqttUser, mqttPassword)) {
      Serial.println("\nConnected to MQTT Server.");
      client.subscribe(topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
    }
  }
  }

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
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

String generateGUID() {
  char guid[37];
  for (int i = 0; i < 36; i++) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      guid[i] = '-';
    } else if (i == 14) {
      guid[i] = '4'; // Version 4 UUID
    } else {
      guid[i] = "0123456789abcdef"[random(15)];
    }
  }
  guid[36] = '\0';
  return String(guid);
}
