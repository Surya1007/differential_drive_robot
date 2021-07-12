# Two-Wheeled Differential Drive Robot

The following components have been used in making this project:

## Joystick:
1. Arduino Uno
2. nRF24L01+ module
3. Joystick module

## Robot:
1. Raspberry Pi Zero-W
2. nRF24L01+ module
3. TB6612FNG motor driver
4. BO-motors
5. Cardboard chasis

### #Code has been tested and is functional using Raspberry Pi Zero-W and Arduino Uno.

## Joystick schematics using Arduino Uno:    

### Joystick module:

| Pin | Function |          
|-----|----------|
| A0  |  x-axis  |
| A1  |	 y-axis  |
| GND |  GND     |
| VCC |  5V      |

### nRF24L01+ module:

| Pin | Function |
|-----|----------|
| 7   |   CE     |
| 8   |   CSN    |
| 11  |   MOSI   |
| 12  |   MISO   |
| 13  |   SCK    |
| GND |   GND    |
| VCC |   3.3V   |

## Differential-Drive Robot Schematics using Raspberry Pi Zero-W:    

### TB6612FNG Motor Driver:

| GPIO Pin | Function   | wiringPi |          
|----------|------------|----------|
| 23       |  PWML      | 23       |
| 24       |  BIN1 / R1 | 24       |
| 25       |  BIN2 / R0 | 25       |
| 26       |  PWMR      | 26       |
| 27       |  STBY      | 27       |
| 28       |  AIN1 / L1 | 28       |
| 29       |  AIN2 / L0 | 29       |
| GND      |  Ground    | -        |
| VCC      |  3.3V      | -        |

### nRF24L01+ module:

| GPIO Pin | Function | wiringPi |
|----------|----------|----------|
| 22       |   CE     | 22       |
| CE0      |   CSN    | 10       |
| MOSI     |   MOSI   | 12       |
| MISO     |   MISO   | 13       |
| SCLK     |   SCK    | 14       |
| GND      |  Ground  | -        |
| VCC      |  3.3V    | -        |
