//conversion from .ino to .cpp file
#include <Arduino.h>
#include <Sleep_n0m1.h> //#412 in platformio
#include <SD.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h> //#571 in platformio
#include <Adafruit_GFX.h> //#13 in platformio
#include <NewPing.h> //#176 in platformio
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
void getDistance();
void getVolume();
void getTimeStamp();
void saveDataSD();
void initSDCard();
void printParameters();
void welcomeText();
void mainScree();
void statisticsScreen();
void updateScreen();
/*------------------------------------------------------------------------------------*/
//Go to configuration.h and select if Arduino Leonardo is used by commenting the #define
#ifdef LEONARDO  //For the Arduino Leonardo or the micro
    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

#ifdef MEGA  //Only for the Uno
    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

//Creating a NewPing object
NewPing sensor(TRIGGER, ECHO, MAX_DISTANCE);

//Creating a Sleep object
Sleep sleep;

//************** Program variables *******************************
uint8_t screen;     //Variable to track screens
bool cardPresent;   //Variable to know if the SD card is present or not
bool logFileExists; //Variable to know if the log file already exists

String timeStamp;   //String to store the time from the RTC Module
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

  Serial.println(F("Water Reservoir Monitoring Starting...!"));

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

  Serial.println("Initializing the TFT Display");
  tft.begin();
  tft.setRotation(1); //Set the display in landscape mode
  //welcomeText();
  //mainScreen();
}
//--------------------------------------------------------
void loop()
{
  delay(1000);
  //getTime();
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

void getTimeStamp() {
 //function that gets the time and date from a RTC Module
 //and stores the time stamps in timeStamp
}

void saveDataSD() {
    if (cardPresent && logFileExists) {      //The SD card is present and we can log
        dataString = "";
        //First we get the time & date
        dataString += timeStamp;
        dataString += ",";
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

void printParameters () {
    //This function prints some of the parameters in the configuration.h file
    Serial.println();
    Serial.println("Your parameters are the following. Please modify them under configuration.h and rebuild.");
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
void welcomeText(){
    Serial.println("Displaying the welcome text");
    screen = 0;
    tft.fillScreen(WHITE);
    tft.setCursor(10,10);
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.println("Nivel de Agua");
    tft.println("en el Pozo");

    tft.setCursor(10, PIXEL_HEIGHT - 18);
    tft.setTextSize(1);
    tft.println("Version 0.9. Marc Cobler. 2015");
    delay(2000);
}

void mainScreen() {
  screen = 1;
  tft.fillScreen(WHITE);
  uint8_t waterLevel = 75;

  //let's draw the deposit itself with some rectangles!
  tft.fillRect(DEPOSIT_FIG1_X,DEPOSIT_FIG1_Y,DEPOSIT_FIG1_WIDTH, DEPOSIT_FIG1_HEIGHT,BLACK); //Left side
  tft.fillRect(DEPOSIT_FIG1_X,DEPOSIT_FIG2_Y,DEPOSIT_FIG2_WIDTH,DEPOSIT_FIG1_WIDTH,BLACK); //bottom side
  tft.fillRect(DEPOSIT_FIG3_X,DEPOSIT_FIG1_Y,DEPOSIT_FIG1_WIDTH,DEPOSIT_FIG1_HEIGHT,BLACK); //right side

  //Now let's draw the water inside the deposit
  tft.fillRect(VOLUME_POSITION_X,waterLevel,VOLUME_WIDTH,(VOLUME_HEIGHT - waterLevel) + DEPOSIT_FIG1_Y,BLUE);

  //Water level in percentage
  tft.setCursor(DEPOSIT_FIG3_X + 25,DEPOSIT_FIG1_Y);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.print("Capacidad al ");
  tft.setCursor(195,66);
  tft.setTextSize(3);
  tft.print(75);
  tft.println("%");
}

void statisticsScreen() {
  screen = 2;
  tft.fillScreen(WHITE);
}

void updateScreen (uint8_t screen) {    //Function that updates the screen periodycally

}
