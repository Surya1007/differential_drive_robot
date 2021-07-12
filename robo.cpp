#include "RF24/RF24.h"
#include <ctime>
#include <iostream>
#include <string>
#include <softPwm.h>
#include <wiringPi.h>
#include <math.h>

using namespace std;

void motor_commander(int16_t power, int16_t rate_change_angle);
void direction_estimation(int16_t angular_direction, int16_t direction);

// Initialise nRF24L01+ using GPIO pins 22, 8
RF24 radio(22, 0);
uint64_t address[2] = {0x7878787878LL, 0xB3B4B5B6F1LL};

// Message structure passed from joystick controller
// Both the joystick and robot should have the same structure
struct payLoadstruct{
	int16_t nodeID;
	int16_t x_value;
	int16_t y_value;
};
payLoadstruct payload;

//Setup the pins for motor drivers in raspberry pi zero w
int STBY = 27;
int L0 = 29;
int L1 = 28;
int R0 = 25;
int R1 = 24;
int PWML = 23;
int PWMR = 26;

int main(){ 
	wiringPiSetup();		// Initialising for controlling GPIO of raspberry pi zero w
	pinMode(STBY,OUTPUT);		// STBY should be HIGH for motro controller to accpet inputs from raspberry pi
	pinMode(L0, OUTPUT);		// Left motor output 0
	pinMode(L1, OUTPUT);		// Left motor output 1
	pinMode(R0, OUTPUT);		// Right motor output 0
	pinMode(R1, OUTPUT);		// Right motor output 1
	pinMode(PWML, PWM_OUTPUT);	// PWM output for left motor
	pinMode(PWMR, PWM_OUTPUT);	// PWM output for right motor
	pwmSetRange(255);		// Set range of pwm to 0 - 255

	digitalWrite(STBY, HIGH);
	// Turns clockwise with respect user
	digitalWrite(L0, HIGH);
	digitalWrite(L1, LOW);
	// Turns anti-clockwise with respect user
	digitalWrite(R0, LOW);
	digitalWrite(R1, HIGH);

	// Check if radio is working
	if (!radio.begin()){
		cout << "Radio hardware is not responding" << endl;
		return 0;
	}

	radio.setPALevel(RF24_PA_LOW);				// Power output level of nrF24L01+
	radio.setPayloadSize(sizeof(payload));			// Payload size of rnRF24L01+
	radio.setChannel(0x76);					// Set common channel for both joystick and robot
	radio.setDataRate(RF24_250KBPS);			// Set common data transfer rate for both joystick and robot
	for (uint8_t i = 0; i < 2; i++){
		radio.openReadingPipe(i, address[i]);}
	radio.startListening();					// nRF24L01+ starts listening to RF signals
	time_t startTimer = time(nullptr);
	while (time(nullptr) - startTimer < 4){			// Timer checks if time taken to receive is more than 4 seconds
	uint8_t pipe;
	if (radio.available(&pipe)){
		uint8_t bytes = radio.getPayloadSize();		// Size of payload to receive
		radio.read(&payload, bytes);			// Reads the radio channel for singals
		cout << "Received from " << payload.nodeID << "    x value is " << payload.x_value << "    y value is " << payload.y_value << endl;
		motor_commander(payload.x_value, payload.y_value);
		startTimer = time(nullptr);			// Resets timer
		cout << "----------------------------------------------------" << endl;
		}
	}
	cout << "Connection lost with joystick" << endl;	// Error if connection is lost with joystick	
	pwmWrite(PWML, 0);
	pwmWrite(PWMR, 0);
	cout << "Robot halted" << endl;
	return 0;
}

void motor_commander(int16_t power, int16_t rate_change_angle){
	direction_estimation(rate_change_angle, power);
	if (rate_change_angle > 30){					
		if (abs(power) < 20){						// Sharp turning right
			pwmWrite(PWML, abs(rate_change_angle));
			pwmWrite(PWMR, abs(rate_change_angle));
			cout << "Robot turning sharp left" << endl;
			cout << "The value of pwml is " << abs(rate_change_angle) << " and pwmr is " << abs(rate_change_angle) << endl;
		}
		else{							// Moving in direction and turning right
			pwmWrite(PWML, abs(abs(power) - abs(rate_change_angle)));	// Reduces pwm to left motor
			pwmWrite(PWMR, abs(power));
			cout << "Robot moving in left curved path" << endl;
			cout << "The value of pwml is " << abs(abs(power) - abs(rate_change_angle)) << " and pwmr is " << abs(power) << endl;
		}
	}
	else if (rate_change_angle < -30){
		if (abs(power) < 20){						// Sharp turning left
			pwmWrite(PWML, abs(rate_change_angle));
			pwmWrite(PWMR, abs(rate_change_angle));
			cout << "Robot turning sharp right" << endl;
			cout << "The value of pwml is " << abs(rate_change_angle) << " and pwmr is " << abs(rate_change_angle) << endl;
		}
		else{							// Moving in direction and turning left
			pwmWrite(PWML, abs(power));
			pwmWrite(PWMR, abs(abs(power) - abs(rate_change_angle)));	// Reduces pwm to right motor
			cout << "Robot moving in right curved path" << endl;
			cout << "The value of pwml is " << abs(power) << " and pwmr is " << abs(abs(power) - abs(rate_change_angle)) << endl;
		}
	}
	else{								// Moving in direction straightly
		pwmWrite(PWML, abs(power));
		pwmWrite(PWMR, abs(power));
		cout << "Robot moving straight" << endl;
		cout << "The value of pwml is " << abs(power) << " and pwmr is " << abs(power) << endl;
	}
}

void direction_estimation(int16_t angular_direction, int16_t direction){
	if (direction < -20){						// Move Backward
		// Left motor rotating in clockwise wrt user
		digitalWrite(L0, HIGH);
		digitalWrite(L1, LOW);
		// Right motor rotating in anti-clockwise wrt user
		digitalWrite(R0, LOW);
		digitalWrite(R1, HIGH);
		cout << "Moving backward" << endl;
	}
	else if (direction > 20){					// Move Forward	
		// Left motor rotating in anti-clockwise wrt user
		digitalWrite(L0, LOW);
		digitalWrite(L1, HIGH);
		// Right motor rotating in clockwise wrt user
		digitalWrite(R0, HIGH);
		digitalWrite(R1, LOW);
		cout << "Moving forward" << endl;
	}
	else if (direction < 20){
		if (direction > -20){
			if (angular_direction < 20){			// Stop
				if (angular_direction > -20){
					// Both motors set LOW
					digitalWrite(L0, LOW);
					digitalWrite(L1, LOW);
					digitalWrite(R0, LOW);
					digitalWrite(R1, LOW);
					cout << "Robot halt" << endl;
				}
				else if (angular_direction < -20){		// Sharp Turn left
					// Left motor rotating in anti-clockwise wrt user
					digitalWrite(L0, LOW);
					digitalWrite(L1, HIGH);
					// Right motor rotating in anti-clockwise wrt user
					digitalWrite(R0, LOW);
					digitalWrite(R1, HIGH);
					cout << "Sharp Turning right" << endl;
				}
			}
			else if (angular_direction > 20){		// Sharp Turn right
				// Left motor rotating in clockwise wrt user
				digitalWrite(L0, HIGH);
				digitalWrite(L1, LOW);
				// Left motor rotating in clockwise wrt user
				digitalWrite(R0, HIGH);
				digitalWrite(R1, LOW);
				cout << "Sharp Turning left" << endl;
			}
		}
	}
}
