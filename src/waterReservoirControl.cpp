// waterReservoirControl.cpp
// Main code for the waterReservoirControl system
// Author: Marc Cobler Cosmen (kitusmark)
// https://github.com/kitusmark/waterReservoirControl

//conversion from .ino to .cpp file
#include <Arduino.h>
#include <Sleep_n0m1.h> //#412 in platformio
#include <SD.h>
#include <SPI.h>
#include <NewPing.h> //#176 in platformio
#include <Wire.h>
#include <SparkFunDS1307RTC.h>
#include <genieArduino.h>
#include "configuration.h"

/********************************************************************************************
Marc Cobler Cosmen - March 2015
waterReservoirControl Project
Water level sensing in well deposits.
Ultrasonic HC-SR04 sensor.

Obtaining the distance of the surface and knowing the measures of the deposit, we can calculate the volume of water
contained. All the data is displayed in a TFT qvga 2,2" display and stored in a SD card for datalogging.

We'll use the library NewPing.h from https://code.google.com/p/arduino-new-ping/ that converts to distance
the pings from the ultrasonic sensor
*********************************************************************************************/

/*-------------------------FUNCTIONS DECLARATIONS-------------------------------------*/
void initSDCard();
void initRTC();
void printTime();
void printParameters();
void getDistance();
void getVolume();
void getTime();
void saveDataSD();

/*------------------------------------------------------------------------------------*/
//Creating a NewPing object
NewPing sensor(TRIGGER, ECHO, MAX_DISTANCE);

//Creating a Sleep object
Sleep sleep;

//************** Program variables *******************************
uint8_t screen;     //Variable to track screens
bool cardPresent;   //Variable to know if the SD card is present or not
bool logFileExists; //Variable to know if the log file already exists

int timeStamp[7];   //int Array to store the time from the RTC Module
String dataString;  //String to save the information to the SD card

unsigned int waterLevel;    //Variable that stores the water level in meters
unsigned int liters;        //Variable that stores the amount of water in liters
unsigned long volume;       //Varible that stores the amount of water in cubic centimenters. MAX volume is HEIGHT*WIDTH*DEPTH
unsigned int litersHistory[HISTORY];

//--------------------------------------------------------
void setup()
{
  Serial.begin(SERIALSPEED); //More than enough
  #ifdef LEONARDO
    while (!Serial) {
     ;    //Wait and do nothing... 32u4 microcrontroller only
    }
  #endif

  Serial.println(F("Water Reservoir Monitoring Starting..."));

  //Init the litersHistory Array
  for (uint8_t i = 0; i < HISTORY; i++) {
      litersHistory[i] = 0;
  }

  #ifdef PARAMETERS
    printParameters();    //Uncomment #define PARAMETERS in configuration.h if you don't want to print this out
  #endif
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SDCS, OUTPUT);
  // see if the card is present and can be initialized:
  initSDCard();
  // Init the RTC Module
  initRTC();

  Serial.println("Initializing the Display...");
}
//--------------------------------------------------------
void loop()
{
  delay(1000);
  getTime();
  getDistance();
  getVolume();
  //saveDataSD();
  //sleep.pwrDownMode();  //set sleep mode
  //sleep.sleepDelay(SLEEPTIME);
}
//--------------------------------------------------------
void getDistance() {
    unsigned int distance = 0;
    waterLevel = 0;
    for (uint8_t i = 0; i < SENSORSAMPLES; i++) {       //Let's average out the signal
      unsigned int time = sensor.ping();    //get the time of a pulse in microseconds
      distance += time / US_ROUNDTRIP_CM;   //Convert to centimeters
      delay(30);                            //Specified by the library
    }

    distance = round(distance / SENSORSAMPLES);
    //Let's check if distance is in range
    if (distance >= HEIGHT) {
        //Ultrasonic sensor error
        Serial.println("Ultrasonic sensor range overflow. Please Check!");
        waterLevel = 0;
        return;
    } else {
        //Everything seems fine...
        waterLevel = HEIGHT - distance;
    }

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    Serial.print("Water Level: ");  //Water height display
    Serial.print(waterLevel);
    Serial.println(" cm");
}

void getVolume(){
    liters = 0;
    volume = 0;
    volume = waterLevel * WIDTH;     //Volume in cubic centimeters
    volume = volume * DEPTH;
    //Serial.println(volume);
    liters = volume / 1000;         //Liters of liquid

    //Append the latest measure to litersHistory

    //Send the data to the Serial port
    Serial.print("Volume: ");
    Serial.print(liters);
    Serial.println(" liters");
}

void getTime() {
  //function that gets the time and date from a RTC Module
  //and stores the time stamps in timeStamp
  rtc.update(); // Update RTC data
  // Read the time and store it in the timeStamp Array:
  timeStamp[0] = rtc.second();
  timeStamp[1] = rtc.minute();
  timeStamp[2] = rtc.hour();

  // Read the day/date:
  timeStamp[3] = rtc.day();
  timeStamp[4] = rtc.date();
  timeStamp[5] = rtc.month();
  timeStamp[6] = rtc.year();

  printTime();
}

void saveDataSD() {
    if (cardPresent && logFileExists) {      //The SD card is present and we can log
        dataString = "";
        //First we get the time & date
        for (size_t i = 0; i < 7; i++) {
          dataString += timeStamp[i];
          dataString += ",";
        }
        //Then we get the water level and the liters
        dataString += String(waterLevel);
        dataString += ",";
        dataString += String(liters);
        dataString += ";";

        //Save it to the SD Card
        File logFile = SD.open(LOGFILENAME, FILE_WRITE);

        // if the file is available, write to it:
        if (logFile) {
          logFile.println(dataString);
          logFile.close();
          // print to the serial port too:
          Serial.println(dataString);
        }
        // if the file isn't open, pop up an error:
        else {
          Serial.print("error opening ");
          Serial.println(LOGFILENAME);
        }
    } else {      //The SD card is not present. Let's try and initialize again
        initSDCard();
    }
}

void initSDCard () {
    //This function initializes the SD Card and stores some status
    Serial.println("initializing the SD card");
    if (!SD.begin(SDCS)) {
      Serial.println("Card failed, or not present. Not logging data!");
      cardPresent = false;
    } else {
        cardPresent = true;
        if (SD.exists(LOGFILENAME)) {
            //the log file already exists
            Serial.print("the log file ");
            Serial.print(LOGFILENAME);
            Serial.println(" already exists");
            logFileExists = true;
        } else {
          //the log file does not exists
          Serial.println("The log file does NOT exists. Creating it...");
          File logFile = SD.open(LOGFILENAME, FILE_WRITE);
          logFile.close();
          Serial.println("Log file created.");
        }
        Serial.println("card initialized.");
      }
}

void printTime() {
  rtc.update();
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }
  Serial.print(" | ");
  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                   String(rtc.month()) + "/"); // Print month
  Serial.println(String(rtc.year()));        // Print year
}

void initRTC() {
  rtc.begin();
  // Let's set the time & date with the compilation timeStamp
  #ifdef SETTIME
    rtc.autoTime();
  #endif
  //Fill the timeStamp
  getTime();
  // Print the time to see if it works properly
  printTime();
}

void printParameters () {
    //This function prints some of the parameters in the configuration.h file
    Serial.println();
    Serial.println("These are the parameters. Please modify them under configuration.h and rebuild.");
    Serial.println();
    Serial.println("-------------------TFT DISPLAY--------------------");
    Serial.print("TFT_MISO: ");
    Serial.println(TFT_MISO);
    Serial.print("TFT_MOSI: ");
    Serial.println(TFT_MOSI);
    Serial.print("TFT_CLK: ");
    Serial.println(TFT_CLK);
    Serial.print("TFT_CS: ");
    Serial.println(TFT_CS);
    Serial.print("TFT_RST: ");
    Serial.println(TFT_RST);

    Serial.println("--------------------DEPOSIT PARAMETERS------------------");
    Serial.print("DEPOSIT MEASURES (in centimeters): ");
    Serial.print("HEIGHT: ");
    Serial.print(HEIGHT);
    Serial.print(" WIDTH: ");
    Serial.print(WIDTH);
    Serial.print(" DEPTH: ");
    Serial.println(DEPTH);

}

/********************************* GRAPHIC FUNCTIONS **********************************/
