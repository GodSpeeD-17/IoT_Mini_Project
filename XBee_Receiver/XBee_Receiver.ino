/* Receiver ESP-32 along with XBee S2C Module Code */
/******************************************************************************************************************************************************************************/
// To use various Hardware UART Ports available
#include <HardwareSerial.h>
// WiFi Library
#include "WiFi.h"
// Basic Library requirements for Adafruit MQTT
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/******************************************************************************************************************************************************************************/
/* Setting up the Hardware Serial on ESP-32 */
/* Using TX0 RX0 on ESP-32 but will produce error when conencted while uploading the code */
// HardwareSerial SerialPort(0);
// const byte rxPin = 3;
// const byte txPin = 1;
/* Using TX2 RX2 on ESP-32, no need to remove teh TX2 RX2 cable */
HardwareSerial SerialPort(2);
// ESP-32 RX Pin --> D16 (RX2)
const byte rxPin = 16;
// ESP-32 TX Pin --> D17 (TX2)
const byte txPin = 17;
/******************************************************************************************************************************************************************************/
/* Adafruit Setup */
// Fill the first 4 define statements
#define WLAN_SSID "ENTER_YOUR_WiFI_NAME"
#define WLAN_PASS "ENTER_WiFI_PASSWORD"
#define AIO_USERNAME "ENETR_ADAFRUIT_AIO_USERNAME"
#define AIO_KEY "ENTER_ADAFRUIT_AIO_KEY"
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
/******************************************************************************************************************************************************************************/
// WiFiClient Object initialsied
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/******************************************************************************************************************************************************************************/
// Setup a feed called 'iot-mini-project' for publishing.
Adafruit_MQTT_Publish test = Adafruit_MQTT_Publish(&mqtt, "godspeed_17/feeds/iot-mini-project");
/******************************************************************************************************************************************************************************/
/* Global Variable Declarations */
String val = "";
float data = 0.0;
/******************************************************************************************************************************************************************************/
/* Function definitions */
void sep(int a);
void receiveData();
void connectToWiFi();
void MQTT_connect();
void MQTT_publish();
/******************************************************************************************************************************************************************************/
/* For better display on the Serial Monitor */
void sep(int a) {
  for (int j = 0; j < a; j++) {
    Serial.print("_");
  }
  Serial.print("\n");
}
/******************************************************************************************************************************************************************************/
/* To acquire the XBee Data */
void receiveData() {
  if (SerialPort.available()) {
    sep(30);
    // String coming from XBee Module
    val = SerialPort.readStringUntil(',');
    // Converting back to float values
    data = val.toFloat();
    Serial.print("Recieved = ");
    Serial.println(data);
    val = "";
  }
}
/******************************************************************************************************************************************************************************/
/* Basic WiFi Details logged */
void connectToWiFi() {
  sep(30);
  Serial.print("Connecting to ");
  Serial.print(WLAN_SSID);
  Serial.print(" ");

  delay(500);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(1000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/******************************************************************************************************************************************************************************/
/* Function to connect and reconnect as necessary to the MQTT server */
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
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}
/******************************************************************************************************************************************************************************/
/* Publish the data acquired to Adafruit Server */
void MQTT_publish() {
  sep(30);
  // Basic Serial Monitor Logging
  Serial.print(F("Sending data "));
  Serial.print(data);
  // Confirm if data is succesfully sent or not
  if (!test.publish(data)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
}
/******************************************************************************************************************************************************************************/
void setup() {
  // Serial Monitor Baud Rate
  Serial.begin(9600);
  // Hardware Serial Baud Rate (Shoudl be same as Baud Rate value set in XCTU)
  SerialPort.begin(9600, SERIAL_8N1, rxPin, txPin);
  // Normal Delay operation
  delay(3000);
  // Connect to WiFi
  connectToWiFi();
}
/******************************************************************************************************************************************************************************/
void loop() {
  // Trying to establish a communication
  MQTT_connect();
  // We first collect the data
  receiveData();
  // Now we can publish
  MQTT_publish();
  // Wait for 2 seconds
  delay(2000);
}
/******************************************************************************************************************************************************************************/