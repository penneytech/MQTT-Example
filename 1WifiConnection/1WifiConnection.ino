#include <ESP8266WiFi.h>

const int ledPin = LED_BUILTIN;  // LED pin (builtin LED on ESP8266)

const char* ssid = "MYHDSB";     // Replace with your WiFi network name

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

void setup() {
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  // Loop code goes here
  // You can add any functionality as needed
}
