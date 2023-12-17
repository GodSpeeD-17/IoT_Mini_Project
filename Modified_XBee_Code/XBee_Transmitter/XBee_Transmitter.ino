/* Transmitter Arduino UNO R3 along with XBee Pro S2C Module and Ultrasonic Sensor Code */
/******************************************************************************************************************************************************************************/
/* SotwareSerial Library included for XBee */
#include "SoftwareSerial.h"
#define LED 13
/******************************************************************************************************************************************************************************/
/* Global Variable Declaration for XBee */
// Data which is to be sent in the format of String
String data = "";
// Do we want the data in inches?
bool getInch = false;
// For XBee RX and TX using SoftwareSerial
const uint8_t rxPin = 2;  // Connect XBee Pin Number 2 (XBee.TX [2] -> rxPin)
const uint8_t txPin = 3;  // Connect XBee Pin Number 3 (XBee.RX [3] -> txPin)
// Just for sake of ease
const uint8_t sepVal = 50;
// Data transmisison indicator
char choice = '0';
/******************************************************************************************************************************************************************************/
/* Global Variable Declaration for Ultrasonic Sensor HC-SR04 */
// Define Echo Pin Conenction --> D12
const uint8_t echoPin = 12;
// Define Trigger Pin Conenction --> D10
const uint8_t trigPin = 10;
// Speed of Sound = 343m/s => 34300cm/s => 0.0343cm/us
const float soundSpeed = 0.0343;
// Distance value to be measured
float distance = 0.0f;
// Total Delay Time for measurement
const uint16_t delayTime = 1000;
// Iteration count
uint8_t count = 0;
// For millis() loop
uint16_t timeNow = 0;
// Declare an array
float dist_arr[25] = { 0 };
// Average Distance
unsigned int dist_avg = 0;
/******************************************************************************************************************************************************************************/
// Declaring the pins as RX and TX for UART
SoftwareSerial xbee(rxPin, txPin);
/******************************************************************************************************************************************************************************/
/* Function Declaration */
float get_distance(bool returnInch);
void sep(int a, char disp);
void get_val(bool takeAverage);
void send_val();
/******************************************************************************************************************************************************************************/
/* Function definition */
float get_distance(bool returnInch) {  // a represents the return value in inches or not
  float time = 0.0, dist = 0.0;
  /* Reset the state */
  digitalWrite(trigPin, LOW);
  delayMicroseconds(3);
  /* Start the pulse and waiting for 10us */
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  /* Stop the pulse */
  digitalWrite(trigPin, LOW);

  /* Calculate the amount of time required for echoPin to turn HIGH */
  time = pulseIn(echoPin, HIGH);

  /* Basic Maths for Calculation of distance and division by 2 because sound has to travel forward as well as backward */
  dist = time * (soundSpeed / 2);  // Unit: cm
  if (returnInch) {
    return (dist / 2.54);  // Unit: inches
  } else {
    return dist;
  }
}
/******************************************************************************************************************************************************************************/
/* Function for sending the averaged value */
void get_val(bool takeAverage) {
  if (takeAverage) {
    while (count < 25) {
      // Function called to obtain distance and storing in array
      dist_arr[count] = get_distance(getInch);
      // Adding the raw values
      distance += dist_arr[count];
      // Index updation
      count++;
      // To limit the loop iteration
      delay(19);
    }
    // Average Distance Calculation
    dist_avg = (int)(distance / count);
  } else {
    while (count < 25) {
      count++;
      delay(19);
    }
    // Average Distance Calculation
    dist_avg = (int)get_distance(getInch);
  }
  // For better display
  sep(sepVal, '_');

  // Convert the data into String format for transmission of data
  data = String(dist_avg);

  // Reset condition
  distance = 0.0f;
  dist_avg = 0;
  count = 0;
}
/******************************************************************************************************************************************************************************/
// Function for transmitting the value to XBee
void send_val() {
  if (xbee.available()) {
    Serial.println("Data available!");
    char temp = xbee.read();
    if ((temp != 10 && temp != 13) && temp != choice) {
      choice = temp;
    }
  }
  Serial.print("Choice = ");
  Serial.print((char)choice);
  Serial.print("\tgetInch = ");
  Serial.println(getInch);
  switch (choice) {
    // Send this data to XBee Module with ',' being the terminating character (unit: cm)
    case 'C':
    case 'c':
      getInch = false;
      xbee.print(data + ",");
      Serial.print("Distance Sent = ");
      Serial.print(data);
      Serial.println("cm");
      break;
    // Send this data to XBee Module with ',' being the terminating character (unit: inch)
    case 'I':
    case 'i':
      getInch = true;
      xbee.print(data + ",");
      Serial.print("Distance Sent = ");
      Serial.print(data);
      Serial.println("inch");
      break;
    // Do not tranmsit any data; just display the data
    default:
      Serial.print("Distance Measured = ");
      Serial.print(data);
      if (getInch) {
        Serial.println("inch");
      } else {
        Serial.println("cm");
      }
      break;
  }
  // Empty the data string
  data = "";
  delay(100);
}
/******************************************************************************************************************************************************************************/
/* Function for better display on Serial Monitor */
void sep(int a, char disp) {
  for (int j = 0; j < a; j++) {
    Serial.print(disp);
  }
  Serial.print("\n");
}
/******************************************************************************************************************************************************************************/
void setup() {
  /* For Serial Monitor */
  Serial.begin(9600);

  /* For XBee Module communication */
  xbee.begin(9600);

  /* Declarations for Software Serial */
  pinMode(rxPin, INPUT);   // Receiver Pin as Input for Microcontroller
  pinMode(txPin, OUTPUT);  // Transmission Pin as Output for Microcontroller

  /* Declarations for Ultrasonic Sensor */
  pinMode(trigPin, OUTPUT);  // Trigger Pin as Output for Microcontroller
  pinMode(echoPin, INPUT);   // Echo Pin as Input for Microcontroller

  /* Declaration for Indicator */
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  delay(
    5000);
  digitalWrite(LED, LOW);
}
/******************************************************************************************************************************************************************************/
void loop() {
  // Blocking function to get distance with average as input
  get_val(true);
  // Checking the status for tranmission
  send_val();
  // Small delay for infinte loop
  delay(100);
}
/******************************************************************************************************************************************************************************/