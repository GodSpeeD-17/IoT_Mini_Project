#ifndef MQTT_H
#define MQTT_H
// WiFi Library
#include "WiFi.h"
// Basic Library requirements for Adafruit MQTT
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
// Changing WiFi SSID and WiFi Paddword according to the situation
#define CASE_WIFI       // WiFi Connection
/******************************************************************************************************************************************************************************/
// WiFi Setup
#ifdef CASE_WIFI
#define WLAN_SSID "Shrey"
#define WLAN_PASS "9653176088"
#else
#define WLAN_SSID "realme X7 Max"
#define WLAN_PASS "SHREYSHAH"
#endif
/******************************************************************************************************************************************************************************/
// Adafruit Setup
#define AIO_USERNAME "godspeed_17"
#define AIO_KEY "aio_Zvrh445UFikz0TQpMSckE1YYQbKs"
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
/******************************************************************************************************************************************************************************/
// Global Variable Declaration
#define LED 2
int data = -1;
char choice = '0';
bool showInch = false;
const uint8_t sepVal = 50;
/******************************************************************************************************************************************************************************/
// WiFiClient Object initialsied
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/******************************************************************************************************************************************************************************/
// Setup a feed called 'iot-mini-project' for publishing.
Adafruit_MQTT_Publish ultrasonic_cm = Adafruit_MQTT_Publish(&mqtt, "godspeed_17/feeds/iot-mini-project.distance-cm");
Adafruit_MQTT_Publish ultrasonic_inch = Adafruit_MQTT_Publish(&mqtt, "godspeed_17/feeds/iot-mini-project.distance-inch");
/******************************************************************************************************************************************************************************/
/* Function definitions */
void connectToWiFi();
void MQTT_connect();
void MQTT_publish();
void sep(uint8_t a, char disp);
/******************************************************************************************************************************************************************************/
/* For better display on the Serial Monitor */
void sep(uint8_t a, char disp) {
  for (int j = 0; j < a; j++)
    Serial.print(disp);
  Serial.print("\n");
}
/******************************************************************************************************************************************************************************/
/* Basic WiFi Details logged */
void connectToWiFi() {
  sep(sepVal, '_');
  Serial.print("Connecting to ");
  Serial.print(WLAN_SSID);
  Serial.print(" ");
  digitalWrite(LED, HIGH);
  delay(500);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(1000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED, LOW);
}
/******************************************************************************************************************************************************************************/
// Should be called in the loop function and it will take care of connecting.
void MQTT_connect() {
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {  // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1) {
        Serial.println("Inside Infinte Loop of MQTT_Connect!");
        delay(1000);
      }
    }
  }
  Serial.println("MQTT Connected!");
}
/******************************************************************************************************************************************************************************/
/* Publish the data acquired to Adafruit Server */
void MQTT_publish() {

  // Making sure the data is valid!
  if (data < 1) {
    // Serial.printf("Data not received!\n");
    return;
  }

  // Better Data Logging
  sep(sepVal, '_');

  // Trying to establish a communication
  MQTT_connect();

  digitalWrite(LED, HIGH);

  Serial.printf("Published Distance = ");
  if(showInch){
    // Basic Serial Monitor Logging
    Serial.printf("%dinch\n", data);
    // Confirm if data is succesfully sent or not
    if (!ultrasonic_inch.publish(data)) {
      Serial.println(F("Failed!!!!"));
    } else {
      Serial.println(F("OK!"));
    }
  }else{
    // Basic Serial Monitor Logging
    Serial.printf("%dcm\n", data);
    // Confirm if data is succesfully sent or not
    if (!ultrasonic_cm.publish(data)) {
      Serial.println(F("Failed!!!!"));
    } else {
      Serial.println(F("OK!"));
    }

  }

  digitalWrite(LED, LOW);

  // Reset the data value
  data = 0;
  return;
}
/******************************************************************************************************************************************************************************/
#endif