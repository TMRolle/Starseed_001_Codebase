#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>


#define OE_PIN D0
#define MR_PIN D1
#define LATCH_PIN D4
#define CLOCK_PIN D3
#define DATA_PIN D2
#define DIGITAL_IN D5
#define SERIAL_RX D6
#define SERIAL_TX D7
#define NUM_MODULES 1

// Update these with values suitable for your network.
const char* deviceUID = "Hera";
const char* ssid = "Starseed 001";
const char* password = "IfYouBuildItTheyWillCome";
const char* mqtt_server = "192.168.1.100";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

DHT dht22(DIGITAL_IN, DHT22);
DHT dht11(DIGITAL_IN, DHT11);

byte currentState[3];

SoftwareSerial softSerial(SERIAL_RX,SERIAL_TX);

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(OE_PIN, OUTPUT);
    pinMode(MR_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW);
    digitalWrite(MR_PIN, HIGH);

    pinMode(A0, INPUT);
    pinMode(DIGITAL_IN, INPUT);

    softSerial.begin(9600);

    digitalWrite(LATCH_PIN, LOW);
    for(int i = NUM_MODULES; i>=0;i--){
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, 0);
        }
    digitalWrite(LATCH_PIN, HIGH);

    dht22.begin();
    dht11.begin();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicString = topic;
  if(topicString.substring(0,sizeof(deviceUID)) == deviceUID){
    if(topicString.substring(sizeof(deviceUID)) == "/update"){
      if(length == NUM_MODULES){
        digitalWrite(LATCH_PIN, LOW);
        for(int i = length; i>=0;i--){
        shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, payload[i]);
        }
        digitalWrite(LATCH_PIN, HIGH);
      }
      else {
        String debug = "ERROR: Requested update is ";
        debug += length;
        debug +=" bytes long, expected ";
        debug += NUM_MODULES;
        Serial.println(debug);
        String topicToPub = deviceUID;
        topicToPub += "/debug";
        client.publish(topicToPub.c_str(), debug.c_str());
      }
     }
    else if(topicString.substring(sizeof(deviceUID)) == "/readAnalog"){
      String topicToPub = deviceUID;
      topicToPub+="/AnalogValue";
      String value = "";
      value += analogRead(A0);
      client.publish(topicToPub.c_str(),value.c_str());
    }
    else if(topicString.substring(sizeof(deviceUID)) == "/readDigital"){
      String topicToPub = deviceUID;
      topicToPub+="/DigitalValue";
      String value = "";
      value+=digitalRead(DIGITAL_IN);
      client.publish(topicToPub.c_str(),value.c_str());
    }
    else if(topicString.substring(sizeof(deviceUID)) == "/readDHT22"){
      String topicToPub = deviceUID;
      topicToPub+="/DHT22Temp";
      String value = "";
      value+= dht22.readTemperature();
      client.publish(topicToPub.c_str(),value.c_str());
      String topicToPub = deviceUID;
      topicToPub+="/DHT22Humidity";
      String value = "";
      value+= dht22.readHumidity();
      client.publish(topicToPub.c_str(),value.c_str());
    }
    else if(topicString.substring(sizeof(deviceUID)) == "/readDHT11"){
      String topicToPub = deviceUID;
      topicToPub+="/DHT11Temp";
      String value = "";
      value+= dht11.readTemperature();
      client.publish(topicToPub.c_str(),value.c_str());
      String topicToPub = deviceUID;
      topicToPub+="/DHT11Humidity";
      String value = "";
      value+= dht11.readHumidity();
      client.publish(topicToPub.c_str(),value.c_str());
    }
    else if(topicString.substring(sizeof(deviceUID)) == "/setDimmer"){
      softSerial.write(payload[0]);
    }
    //Put in commands to read DHT sensor, send serial to dimmer, etc.

  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      String subTopic = deviceUID;
      subTopic += "/#";
      client.subscribe(subTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);

    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
