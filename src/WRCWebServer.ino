#include "configuration.h"
#include <ESP8266WiFi.h>

#include <SparkFunDS1307RTC.h> //https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library
#include <SD.h>
#include <Wire.h>
//Webserver stuff
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,0);
IPAddress subnet(255,255,255,0);
//Creating the server object
WiFiServer server(webServerPort);

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
  Serial.println(" ");
  Serial.println("Water Reservoir Monitoring Starting...");
  pinMode(SDCS, OUTPUT);
  // see if the card is present and can be initialized:
  initSDCard();
  yield();
  // Init the RTC Module
  initRTC();
  yield();
  //Init server
  initSoftAP();
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
  delay(1000);
}

//------------------------WEBSERVER FUNCTIONS---------------------------
void initSoftAP() {
  Serial.print("Setting Soft-AP configuration...");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Starting the WiFi module as SOFT AP...");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}
void getClientsConnected() {
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
}
void initWebServer() {
  server.begin();
}
void handleClient() {
  WiFiClient client = server.available();
  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
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
    Serial.println("[Client disonnected]");
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
  //Serial.print(rtc.dayStr()); // Print day string
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
