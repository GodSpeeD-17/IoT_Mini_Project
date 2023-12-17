/* Transmitter Arduino UNO R3 along with XBee S2C Module and Ultrasonic Sensor Code */
/******************************************************************************************************************************************************************************/
/* SotwareSerial Library included for XBee */
#include "SoftwareSerial.h"
/******************************************************************************************************************************************************************************/
/* Global Variable Declaration for XBee */
// Data to be sent in the format of String
String data = "";
// Do we want the data in inches 
bool getInch = false;
// For XBee RX and TX using SoftwareSerial
const uint8_t rxPin = 2;
const uint8_t txPin = 3;
/******************************************************************************************************************************************************************************/
/* Global Variable Declaration for Ultrasonic Sensor HC-SR04 */
// Define Echo Pin Conenction --> D12
const uint8_t echoPin = 12;
// Define Trigger Pin Conenction --> D10
const uint8_t trigPin = 10;
// Speed of Sound = 343m/s => 34300cm/s => 0.0343cm/us
const float soundSpeed = 0.0343;  
// Distance value to be measured
float distance = 0.0;
// Total Delay Time for measurement
uint16_t delayTime = 2000;
/******************************************************************************************************************************************************************************/
// Declaring the pins as RX and TX for UART
SoftwareSerial mySerial(rxPin, txPin);
/******************************************************************************************************************************************************************************/
/* Function Declaration */
float get_distance(bool returnInch);
void sep(int a);
void send_cal_val(uint16_t delay);
/******************************************************************************************************************************************************************************/
/* Function definition */
float get_distance(bool returnInch) {  // a represents the return value in inches or not
  float time = 0.0, dist = 0.0;
  /* Reset the state */
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  /* Start the pulse and waiting for 10us */
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  /* Stop the pulse */
  digitalWrite(trigPin, LOW);

  /* Calculate the amount of time required for echoPin to turn High*/
  time = pulseIn(echoPin, HIGH);
  // Serial.println("Speed of Sound = " + String(soundSpeed) + "cm/us");
  // Serial.println("Time = " + String(time) + "us");
  /* Basic Maths for Calculation of distance and division by 2 because sound has to travel forward as well as backward */
  if (returnInch) {
    dist = (time / 2.54) * (soundSpeed / 2);  // Unit: inches
    // Serial.println("Dist = " + String(dist) + "inch");
  } else {
    dist = time * (soundSpeed / 2);  // Unit: cm
    // Serial.println("Dist = " + String(dist) + "cm");
  }
  return dist;
}
/******************************************************************************************************************************************************************************/
/* Function for sending the averaged value */
void send_cal_val(uint16_t delay){

}
/******************************************************************************************************************************************************************************/
/* Function for better display on Serial Monitor */
void sep(int a) {
  for (int j = 0; j < a; j++) {
    Serial.print("_");
  }
  Serial.print("\n");
}
/******************************************************************************************************************************************************************************/
void setup() {
  /* For Serial Monitor */
  Serial.begin(9600);
  /* For XBee Module communication */
  mySerial.begin(9600);
  /* Declarations for Software Serial */
  pinMode(rxPin, INPUT);   // Receiver Pin as Input for Microcontroller
  pinMode(txPin, OUTPUT);  // Transmission Pin as Output for Microcontroller
  /* Declarations for Ultrasonic Sensor */
  pinMode(trigPin, OUTPUT);  // Trigger Pin as Output for Microcontroller
  pinMode(echoPin, INPUT);   // Echo Pin as Input for Microcontroller
}
/******************************************************************************************************************************************************************************/
void loop() {
  // Function called to obtain distance
  distance = get_distance(getInch);
  // Convert the data into String format for transmission of data
  data = String(distance);
  // Send this data to XBee Module with ',' bein the terminating character
  mySerial.print(data + ",");
  // Modify the String as per our requirement
  if (getInch) {
    // Serial Monitor Display
    Serial.println("Distance = " + data + "inch");
  } else {
    Serial.println("Distance = " + data + "cm");
  }
  // Partition line
  sep(30);
  // Empty the String
  data = "";
  // Delay for delayTime (ms)
  delay(delayTime);
}
/******************************************************************************************************************************************************************************/