// WRCWebServer.ino - Water Reservoir Control Web Server
// Main code for the waterReservoirControl system based on the nodemcuv2 ESP8266
//Water level sensing in well deposits with Ultrasonic HC-SR04 sensor and.
// Author: Marc Cobler Cosmen (@kitusmark)
// https://github.com/kitusmark/waterReservoirControl
//Obtaining the distance of the surface and knowing the measures of the deposit, we can calculate the volume of water
//contained. All the data is served in a simple webpage and stored in a SD card for datalogging.
//Comment to disable Serial debug
#define LOG
#ifdef LOG
  #define log(...) Serial.print(__VA_ARGS__)
  #define logln(...) Serial.println(__VA_ARGS__)
#else
  #define log(...)
  #define logln(...)
#endif

#include "configuration.h"
#include "networking.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <SparkFunDS1307RTC.h> //https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library
#include <SD.h>
#include <Wire.h>

//Some ESP stuff
ADC_MODE(ADC_VCC);

//************** Program variables *******************************
bool cardPresent;   //Variable to know if the SD card is present or not
bool logFileExists; //Variable to know if the log file already exists

int timeStamp[7];   //int Array to store the time from the RTC Module
String timeStampString;
String dataString;  //String to save the information to the SD card

unsigned int waterLevel;    //Variable that stores the water level in meters
unsigned int liters;        //Variable that stores the amount of water in liters
unsigned long volume;       //Varible that stores the amount of water in cubic centimenters. MAX volume is HEIGHT*WIDTH*DEPTH

void setup()
{
  ESP.wdtDisable();
  Serial.begin(SERIALSPEED);
  delay(50);
  logln("BOOTLOADER GARBAGE...");
  logln(" ");
  logln("Water Reservoir Monitoring Starting...");
  pinMode(SDCS, OUTPUT);
  // see if the card is present and can be initialized:
  initSDCard();
  yield();
  // Init the RTC Module
  initRTC();
  yield();
  //Get information of the ESP8266 Module
  printModuleInfo();
  //Init server
  initSoftAP();
  initMDNSServer();
  initOTAServer();
  initWebServer();
  yield();
  ESP.wdtEnable(WDTO_8S);
}

void loop()
{
  handleClient();
  getTime();
  // getDistance();
  getVolume();
  saveDataSD();
  delay(5000);
}

void handleClient() {
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    logln("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        log(line);
        // wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareHtmlPage());
          break;
        }
      }
    }
    delay(1); // give the web browser time to receive the data
    // close the connection:
    client.stop();
    logln("[Client disonnected]");
  }
}
//--------------------------USER FUNCTIONS-------------------------------
String prepareHtmlPage()  {
  String htmlPage =
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +  // the connection will be closed after completion of the response
            "Refresh: 5\r\n" +  // refresh the page automatically every 5 sec
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html>" +
            "Welcome to the Water Reservoir Control Server! \r\n" +
            "Current time is: " + timeStampString + "\r\n" +
            "The deposit has: " + String(liters) + " liters" "\r\n" +
            "</html>" +
            "\r\n";
  return htmlPage;
}

void printModuleInfo() {
  logln("ESP8266 Module info: ");
  log(" - Flash Chip ID: ");
  logln(ESP.getFlashChipId());
  log(" - Flash Chip Size: ");
  log(ESP.getFlashChipSize()/1024);
  logln(" MB");
  log(" - Flash Chip Real Size: ");
  log(ESP.getFlashChipRealSize()/1024);
  logln(" MB");
  log(" - Flash Chip Speed: ");
  log(ESP.getFlashChipSpeed()/1000000);
  logln(" MHz");
  //Read the VCC Supply Voltage
  log(" - VCC Voltage: ");
  log(ESP.getVcc());
  logln(" mV");
}

void getVolume(){
    liters = 0;
    volume = 0;
    volume = waterLevel * WIDTH;     //Volume in cubic centimeters
    volume = volume * DEPTH;
    //logln(volume);
    liters = volume / 1000;         //Liters of liquid

    //Append the latest measure to litersHistory

    //Send the data to the Serial port
    log("Volume: ");
    log(liters);
    logln(" liters");
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

  for (size_t i = 0; i < 7; i++) {
    timeStampString += timeStamp[i];
  }

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
          logln(dataString);
        }
        // if the file isn't open, pop up an error:
        else {
          log("error opening ");
          logln(LOGFILENAME);
        }
    } else {      //The SD card is not present. Let's try and initialize again
        initSDCard();
    }
}
//
void initSDCard () {
    //This function initializes the SD Card and stores some status
    logln("initializing the SD card");
    if (!SD.begin(SDCS)) {
      logln("Card failed, or not present. Not logging data!");
      cardPresent = false;
    } else {
        cardPresent = true;
        if (SD.exists(LOGFILENAME)) {
            //the log file already exists
            log("the log file ");
            log(LOGFILENAME);
            logln(" already exists");
            logFileExists = true;
        } else {
          //the log file does not exists
          logln("The log file does NOT exists. Creating it...");
          File logFile = SD.open(LOGFILENAME, FILE_WRITE);
          logFile.close();
          logln("Log file created.");
        }
        logln("card initialized.");
      }
}
//
void printTime() {
  rtc.update();
  log(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    log('0'); // Print leading '0' for minute
  log(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    log('0'); // Print leading '0' for second
  log(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) log(" PM"); // Returns true if PM
    else log(" AM");
  }
  log(" | ");
  // Few options for printing the day, pick one:
  //log(rtc.dayStr()); // Print day string
  //log(rtc.dayC()); // Print day character
  //log(rtc.day()); // Print day integer (1-7, Sun-Sat)
  log(" - ");
  log(String(rtc.date()) + "/" +    // (or) print date
                   String(rtc.month()) + "/"); // Print month
  logln(String(rtc.year()));        // Print year
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
    logln();
    logln("These are the parameters. Please modify them under configuration.h and rebuild.");
    logln();
    logln("--------------------DEPOSIT PARAMETERS------------------");
    log("DEPOSIT MEASURES (in centimeters): ");
    log("HEIGHT: ");
    log(HEIGHT);
    log(" WIDTH: ");
    log(WIDTH);
    log(" DEPTH: ");
    logln(DEPTH);
}
