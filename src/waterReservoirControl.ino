#include <Sleep_n0m1.h>
#include <SD.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <NewPing.h>
#include <configuration.h>
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
//Go to configuration.h and select if Arduino Leonardo is used by commenting the #define
#ifdef leonardo  //For the Arduino Leonardo or the micro
    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
#endif

#ifndef leonardo  //Only for the Uno
    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#endif

//Creating a NewPing object
NewPing sensor(TRIGGER, ECHO, MAX_DISTANCE);

//Creating a Sleep object
Sleep sleep;

//************** Program variables *******************************
uint8_t screen;
String dataString;  //String to save the information to the SD card
unsigned int waterLevel;
unsigned int liters;

//--------------------------------------------------------
void setup()
{
  Serial.begin(9600); //More than enough
  #ifdef leonardo
    while (!Serial) {
     ;    //Wait and do nothing... 32u4 microcrontroller only
    }
  #endif

  Serial.println(F("Water Reservoir Monitoring Starting...!"));
  Serial.println("initializing the SD card");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SDCS, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(SDCS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  Serial.println("Initializing the TFT Display");
  tft.begin();
  tft.setRotation(1); //Set the display in landscape mode
  Serial.println("Displaying the welcome text");
  welcomeText();
  mainScreen();
}
//--------------------------------------------------------
void loop()
{
  delay(100);
  //getTime();
  //getDistance();
  //getVolume();
  //saveDataSD();
  sleep.pwrDownMode();  //set sleep mode
  sleep.sleepDelay(SLEEPTIME);
}
//--------------------------------------------------------
float getDistance() {
  waterLevel = 0;
  unsigned int time = sensor.ping(); //get the time of a pulse in microseconds
  unsigned int distance = time / US_ROUNDTRIP_CM; //Convert to centimeters
  waterLevel = (HEIGHT * 1000) - distance;

  Serial.print("Water Level: "); //Water height display
  Serial.print(waterLevel);
  Serial.println(" cm");
}

void getVolume(){
  int volume = (waterLevel/1000) * WIDTH * DEPTH; //Volume in cubic meters
  liters = volume * 1000; //Liters of liquid

  //Send the data to the Serial port
  Serial.print("Volume: ");
  Serial.print(liters);
  Serial.println(" liters");
}

void getTime() {
 //function that gets the time and date from a RTC Module
}

void saveDataSD() {
  dataString = "";
  //First we get the time & date

  //Then we get the water level and the liters
  dataString += String(waterLevel);
  dataString += ",";
  dataString += String(liters);

  //Save it to the SD Card
  File dataFile = SD.open("deposito.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening deposito.txt");
  }

}

void welcomeText(){
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
