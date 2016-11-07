waterReservoirControl
===========

[![Build Status](https://travis-ci.org/kitusmark/waterReservoirControl.svg?branch=master)](https://travis-ci.org/kitusmark/waterReservoirControl)

Code for an automated water level control system in a deposit or reservoir. Implemented with `arduino`.

The idea is to pump water from a well to a reservoir and track the amount of water in it. The control panel of the pump has a system to activate or deactivate the pump, so I've used a couple of probes to notice the lowest and the highest level and a relay output to control the pump.

In order to track the amount of water I've used an Ultrasonic sensor to measure the distance to the water surface and calculating the volume with the deposit dimensions.
By doing that we can leave the system unattended and ensure an automatic irrigation.

The microcontroller's function is to monitor the water level every *X* seconds and store all the data in a SD card in order to process the data later in the computer or see the basic info on the system display.

The future will be to implement a WiFi module with a web server to get and display the data.

## Hardware

----
>I've linked the parts and pieces matching my needs. I'm aware there are other methods and parts to use. To any suggestion, please make an issue or contact me!

In order to achieve the specifications, we need the following parts:

+ 12V power supply.
+ Microcontroller. This code has been made for Arduino but It's easily portable.
+ Water Level Relay board.
+ LCD display for data visualization. I've selected a uLCD-35DT from 4D Systems.
+ SD card breakout.
+ Ultrasonic Distance Sensor.The basic HC-SR04.
+ RTC Module to track the time.
+ Switches, plugs and cables


## Firmware

The firmware is organised within a PlatformIO project. `Lib` folder conains the libraries and the `src` folder contains the main code.

You'll need to set some parameters under the `src/configuration.h` in order to work properly with your system. Everything is well documented. The firmware is compatible with **Arduino Mega, UNO and Leonardo**.

Furthermore, I've configured this repository to work with Travis CI so the label at the top of this wiki shows the state of the compilation.

---
Libraries used in this project:

+ NewPing.h
+ SD.h
+ SPI.h
+ Adafruit_ILI9341
+ Sleep_n0m1
+ SparkFun DS1307 Real Time Clock
+ Wire.h for I2C communications
+ genieArduino.h for 4D Systems uLCD Serial communications.


### To-Do

- [x] Implement data system to store in the SD card
- [x] Use SD library and code for that
- [x] Implement Sleep mode of the ATmega
- [x] Implement RTC Module. Based on the DS1307
- [ ] Design the interface of the display and decide the menus
- [ ] Code for the uLCD Display
- [x] Functions to calculate volume (*liters*)
- [ ] Implement LIFO buffer to calculate flows
- [x] configuration file to make it easy for other systems
- [x] Configure Travis CI to deploy
