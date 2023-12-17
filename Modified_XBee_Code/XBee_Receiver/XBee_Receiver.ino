/* Receiver ESP-32 along with XBee Pro S2C Module Code */
// Comment one line in MQTT.h for hotspot or custum WiFi connection
/******************************************************************************************************************************************************************************/
// To use various Hardware UART Ports available
#include <HardwareSerial.h>
// Custom header file for MQTT regarding operations
#include "MQTT.h"
// Custom define statements
#define SERIAL_PORT_0 0
#define SERIAL_PORT_2 2
/******************************************************************************************************************************************************************************/
// Setting up the Hardware Serial on ESP-32
/*
// Using TX0 RX0 on ESP-32 but will produce error when conencted while uploading the code
HardwareSerial xbee(SERIAL_PORT_0);
const byte rxPin = 3;
const byte txPin = 1;
*/
/******************************************************************************************************************************************************************************/
/* Using TX2 RX2 on ESP-32, no need to remove the TX2 RX2 cable */
HardwareSerial xbee(SERIAL_PORT_2);
// ESP-32 RX Pin --> D16 (RX2)
const byte rxPin = 16;
// ESP-32 TX Pin --> D17 (TX2)
const byte txPin = 17;
/******************************************************************************************************************************************************************************/
/* Global Variable Declarations */
unsigned long timeNow = 0;
const unsigned long delayTime = 5000;
/******************************************************************************************************************************************************************************/
/* Function definitions */
void receiveData();
void printData();
/******************************************************************************************************************************************************************************/
/* Checking the Serial Monitor for new data */
void receiveData() {
  if (Serial.available()) {
    char temp = Serial.read();
    if ((temp != 10 && temp != 13) && temp != choice) {
      choice = temp;
      Serial.printf("Received from Keyboard = %c\n", choice);
    }
    Serial.printf("Sent via XBee = %c\n", choice);
    xbee.printf("%c", choice);
  }
  if (choice == 'i' || choice == 'I')
    showInch = true;
  else
    showInch = false;
}
/******************************************************************************************************************************************************************************/
/* To acquire the XBee Data */
void printData() {
  if (xbee.available()) {
    sep(sepVal, '_');
    // String coming from XBee Module
    String val = xbee.readStringUntil(',');
    // Converting back to float values
    data = (int)val.toInt();
    Serial.printf("Recieved = %d%s\n", data, (showInch == 1) ? "inch" : "cm");
  }
}
/******************************************************************************************************************************************************************************/
void setup() {
  // Serial Monitor Baud Rate
  Serial.begin(9600);
  // Hardware Serial Baud Rate (Should be same as Baud Rate value set in XCTU)
  xbee.begin(9600, SERIAL_8N1, rxPin, txPin);
  // Indicator declaration
  pinMode(LED, OUTPUT);
  while (!Serial.available())
    ;
  Serial.read();
  // Normal Delay operation
  digitalWrite(LED, HIGH);
  delay(5000);
  digitalWrite(LED, LOW);
  // Connect to WiFi
  connectToWiFi();
}
/******************************************************************************************************************************************************************************/
void loop() {

  // Checking the availibility of data on Serial Port
  receiveData();

  // Collect the data if available
  printData();

  // Publish data after delayTime milliseconds
  if (millis() - timeNow >= delayTime) {
    MQTT_publish();
    // Update the variable
    timeNow = millis();
  }
  // Wait for 100 ms
  delay(100);
}
/******************************************************************************************************************************************************************************/