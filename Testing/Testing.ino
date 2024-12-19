#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi
const char *ssid = "Busler"; // Enter your WiFi name
const char *password = "7AAA8FEC10";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.50.155";
const char *tempTopic = "esp/dryer/temp";
const char *humTopic = "esp/dryer/hum";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;

//sensor 
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish
    dht.begin();
    client.publish(tempTopic, "Hi, I'm alive");
    client.publish(humTopic, "Hi, I'm alive");
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    delay(1000);
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");

    client.publish(tempTopic, String(temp).c_str());
    client.publish(humTopic, String(hum).c_str());

    client.loop();

}
