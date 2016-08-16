/*###################################################################

Marc Cobler - August 2016
waterReservoirControl Project
Water level sensing in well deposits.
Ultrasonic HC-SR04 sensor.

Configuration File
Modify the parameters in order to adapt the code to your system
Modify the pins in order to adapt to your hardware
###################################################################*/

/********************* PARAMETERS **************************/
// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
//This pins are just for the Leonardo and Micro Arduinos, 32u4 based.
//Comment the next line if Arduino UNO is used
//#define leonardo true
#ifdef leonardo  //For the Arduino Leonardo or the micro
    #define TFT_MOSI 16
    #define TFT_MISO 14
    #define TFT_CLK 15
    #define TFT_RST 8
#endif

//Deposit measures in meters
#define HEIGHT 2
#define WIDTH 5.4
#define DEPTH 1.8

//Max distance that the NewPing library can compute. In centimeters
#define MAX_DISTANCE 250

//Define sleeptime in miliseconds
#define SLEEPTIME 300000  //Sleep for 5 min

//Display constants
#define PIXEL_WIDTH 320
#define PIXEL_HEIGHT 240

#define DEPOSIT_FIG1_X 20
#define DEPOSIT_FIG1_Y 40
#define DEPOSIT_FIG1_WIDTH 12
#define DEPOSIT_FIG1_HEIGHT 175

#define DEPOSIT_FIG2_Y 215
#define DEPOSIT_FIG2_WIDTH 112

#define DEPOSIT_FIG3_X 120

#define VOLUME_POSITION_X (DEPOSIT_FIG1_X + DEPOSIT_FIG1_WIDTH)
#define VOLUME_WIDTH (DEPOSIT_FIG3_X - VOLUME_POSITION_X)
#define VOLUME_HEIGHT (DEPOSIT_FIG2_Y - DEPOSIT_FIG1_Y)

// Color definitions for the display usage
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF


/********************* PINS ********************************/
//SD Card
const int SDCS = 10; //Need to change because TFT is on the same CS pin

//Ultrasonic sensor definitions
#define TRIGGER 12 //pin for the signal emitter
#define ECHO 11 //pin for the return wave

//Pushbuttons for navigation and such
#define NAVBUTTON 3
#define BUTTON 4
