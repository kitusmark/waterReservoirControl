// waterReservoirControl.cpp
// Main code for the waterReservoirControl system
//Water level sensing in well deposits with Ultrasonic HC-SR04 sensor.
// Author: Marc Cobler Cosmen (kitusmark)
// https://github.com/kitusmark/waterReservoirControl
//Obtaining the distance of the surface and knowing the measures of the deposit, we can calculate the volume of water
//contained. All the data is displayed in a TFT qvga 2,2" display and stored in a SD card for datalogging.

//conversion from .ino to .cpp file
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <NewPing.h> //#176 in platformio
//#include <Wire.h>
#include <SparkFunDS1307RTC.h>
#include "configuration.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>

/*-------------------------FUNCTIONS DECLARATIONS-------------------------------------*/
// void initSDCard();
// void initRTC();
// void printTime();
// void printParameters();
// void getDistance();
// void getVolume();
// void getTime();
// void saveDataSD();
void handleRoot();
void handleNotFound();
void printNetworkParameters();

/*------------------------------------------------------------------------------------*/
//Creating a NewPing object
NewPing sensor(TRIGGER, ECHO, MAX_DISTANCE);

//Creating a Sleep object

//************** Program variables *******************************
bool cardPresent;   //Variable to know if the SD card is present or not
bool logFileExists; //Variable to know if the log file already exists

int timeStamp[7];   //int Array to store the time from the RTC Module
String dataString;  //String to save the information to the SD card

unsigned int waterLevel;    //Variable that stores the water level in meters
unsigned int liters;        //Variable that stores the amount of water in liters
unsigned long volume;       //Varible that stores the amount of water in cubic centimenters. MAX volume is HEIGHT*WIDTH*DEPTH

// webserver values and definitions
const char* ssid = "waterReservoirControl";
const char* password = "123456789";
const int channel = 11;
const bool hidden = false;

IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,0);
IPAddress subnet(255,255,255,0);

// ESP8266WebServer server(80);

//--------------------------------------------------------
void setup()
{
  ESP.wdtDisable();
  Serial.begin(SERIALSPEED);
  Serial.println("Water Reservoir Monitoring Starting...");

  #ifdef PARAMETERS
    printParameters();    //Uncomment #define PARAMETERS in configuration.h if you don't want to print this out
  #endif
  make sure that the default chip select pin is set to
  output, even if you don't use it:
  pinMode(SDCS, OUTPUT);
  // see if the card is present and can be initialized:
  initSDCard();
  // Init the RTC Module
  initRTC();
  //Init the Wifi Module
  Serial.print("Setting Soft-AP configuration...");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.println("Starting the WiFi module as SOFT AP...");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  //Print connection parametes to Serial
  //printNetworkParameters();

  //Starting the HTTP server
  // server.on("/", handleRoot);
  // server.onNotFound(handleNotFound);
  // server.begin();
  // Serial.println("HTTP server started");
  ESP.wdtEnable(WDTO_8S);
}
//--------------------------------------------------------
void loop()
{
  //Test the webserver
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
  delay(2000);
  getTime();
  getDistance();
  getVolume();
  saveDataSD();
  ESP.wdtFeed();
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
//
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
//
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
//
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
//
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
//
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
//
void printParameters () {
    //This function prints some of the parameters in the configuration.h file
    Serial.println();
    Serial.println("These are the parameters. Please modify them under configuration.h and rebuild.");
    Serial.println();
    Serial.println("--------------------DEPOSIT PARAMETERS------------------");
    Serial.print("DEPOSIT MEASURES (in centimeters): ");
    Serial.print("HEIGHT: ");
    Serial.print(HEIGHT);
    Serial.print(" WIDTH: ");
    Serial.print(WIDTH);
    Serial.print(" DEPTH: ");
    Serial.println(DEPTH);

}

/********************************* WEBSERVER FUNCTIONS **********************************/
void handleRoot() {
  server.send(200, "text/plain", "This is the Water Reservoir Control Webserver");
}
//
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
//
void printNetworkParameters() {
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //Print more detailed information about the network
  WiFi.printDiag(Serial);
}
