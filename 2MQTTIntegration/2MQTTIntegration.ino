#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "MYHDSB";  // Replace with your WiFi network name

// MQTT Server settings
const char* mqtt_server = "mqtt_server_address";  // Replace with your MQTT server address
const int mqtt_port = "mqtt_server_port";         // Replace with your MQTT port if applicable
const char* mqtt_user = "mqtt_username";          // Replace with your MQTT username if applicable
const char* mqtt_password = "mqtt_password";      // Replace with your MQTT password if applicable

const int ledPin = LED_BUILTIN;  // LED pin (builtin LED on ESP8266)

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  // Turn LED off initially

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

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
  // MQTT callback code goes here
  // Example: Turn LED ON or OFF based on the received message
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received message: ");
  Serial.println(message);

  if (message == "1") {
    digitalWrite(ledPin, LOW);  // Turn LED ON
  } else if (message == "0") {
    digitalWrite(ledPin, HIGH);  // Turn LED OFF
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("your_topic");  // Subscribe to your MQTT topic
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

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqtt_callback);

  reconnect();  // Ensure MQTT connection
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
