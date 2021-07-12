#include <SPI.h>
#include "printf.h"
#include "RF24.h"

RF24 radio(7, 8);
uint64_t address[2] = {0x7878787878LL, 0xB3B4B5B6F1LL}; // Addresses for both nRF24L01 modules to communicate with each other
int role = 0; // Identification number for nRF24L01+ module
struct PayloadStruct{ // Defines the structure of messages to be sent
  int16_t nodeID;  // Identification number
  int16_t x_value; // x value from joystick module
  int16_t y_value; // y value from joystick module
};
PayloadStruct payload;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  if (!radio.begin()) {
    Serial.println(F("Radio hardware is not responding!!"));
    while (1) {}
  }
  Serial.println(F("Joystick module"));
  radio.setPALevel(RF24_PA_LOW); // Sets power level to low
  radio.setPayloadSize(sizeof(payload));
  payload.nodeID = role;
  payload.x_value = 0;
  payload.y_value = 0;
  // Set the address on pipe 0 to the RX node.
  radio.stopListening(); // put radio in TX mode
  radio.setChannel(0x76); // Common channel for both transmitter and receiver
  radio.openWritingPipe(address[role]); // Starts to prepare writing into address[role]
  radio.setDataRate(RF24_250KBPS); // Speed of communication
  radio.setRetries(((role * 3) % 12) + 3, 15);
  printf_begin();
  radio.printPrettyDetails();
}

void loop() {
    int x_value = analogRead(A0); // Reads value from x-axis of joystick module
    int y_value = analogRead(A1); // Reads value from y-axis of joystick module

    payload.x_value = map(x_value, 220, 800, -255, 255); // Maps the x-axis of joystick modules into a common range
    payload.y_value = map(y_value, 220, 800, -255, 255); // Maps the y-axis of joystick modules into a common range

    Serial.print("X_value is ");
    Serial.print(x_value);
    Serial.print("  ");
    Serial.print(payload.x_value);
    Serial.print("   Y_value is ");
    Serial.print(y_value);
    Serial.print("  ");
    Serial.print(payload.y_value); 
    Serial.print("  ");   
    
    unsigned long start_timer = micros();
    bool report = radio.write(&payload, sizeof(payload)); // Starts sending data
    unsigned long end_timer = micros();
    
    if (report) { // Prints values if transmission is success
      Serial.print(F("Transmission of payloadID "));
      Serial.print(payload.nodeID);
      Serial.print(F("with x value as"));
      Serial.print(payload.x_value);
      Serial.print(F(" and with y value as"));
      Serial.print(payload.y_value);      
      Serial.print(F(" successful!"));
      Serial.print(F(" Time to transmit: "));
      Serial.print(end_timer - start_timer);
      Serial.println(F(" us"));
    }
    else {
      Serial.println(F("Transmission failed or timed out"));
    }
}
