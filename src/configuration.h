/*###################################################################

Marc Cobler - August 2016
waterReservoirControl Project
Water level sensing in well deposits.
Ultrasonic HC-SR04 sensor.

Configuration File
Modify the parameters in order to adapt the code to your system
Modify the pins in order to adapt to your hardware
###################################################################*/
// webserver values and definitions
const char* ssid = "Deposit";
const char* password = "12345678910";
const int channel = 11;
const bool hidden = false;

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
