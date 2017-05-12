/*###################################################################
Marc Cobler - August 2016
waterReservoirControl Project
Water level sensing in well deposits.
Ultrasonic HC-SR04 sensor.
ESP8266 in the nodemcuv2 module.

Configuration File
Modify the parameters in order to adapt the code to your system
Modify the pins in order to adapt to your hardware
###################################################################*/
//#include guards
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#define LOG
#ifdef LOG
  #define log(...) Serial.print(__VA_ARGS__)
  #define logln(...) Serial.println(__VA_ARGS__)
#else
  #define log(...)
  #define logln(...)
#endif

// webserver Soft-AP values and definitions
const char* ssid = "WRC";
const char* password = "123456789";
const int channel = 11;
const bool hidden = false;
//Credentials for updating the firmware
const char* updatePath = "/update";
const char* updateUsername = "admin";
const char* updatePassword = "admin";
//mDNS definitions
const char* hostnameMDNS = "wrc";
const int webServerPort = 80;
//Comment if you don't need to print the parameters during the start via serial port
#define PARAMETERS

//Set the name of the log file saved to the SD card
const char* LOGFILENAME = "log.csv";

//Set the Serial port communication speed
#define SERIALSPEED 115200

//The number of samples taken when getting the distance
#define SENSORSAMPLES 5

//The number of samples stored to calculate flows and stuff
#define HISTORY 10

//Deposit measures in centimeters
#define HEIGHT 250
#define WIDTH 540
#define DEPTH 180

//Define if you want to set the time with the next compilation
//This is just needed once. Then comment
#define SETTIME
//SD Card
const int SDCS = 4; //Need to change because TFT is on the same CS pin

//Ultrasonic sensor definitions
#define TRIGGER 9 //pin for the signal emitter
#define ECHO 8 //pin for the return wave
//Max distance that the NewPing library can compute. In centimeters
#define MAX_DISTANCE 250

//RTC - Real Time Module pins
#define RTCDATA 4
#define RTCCLOCK 3

#endif //configuration.h
