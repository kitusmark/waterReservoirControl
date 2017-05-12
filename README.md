waterReservoirControl
===========

[![Build Status](https://travis-ci.org/kitusmark/waterReservoirControl.svg?branch=master)](https://travis-ci.org/kitusmark/waterReservoirControl)

Code for an automated water level control system in a deposit or reservoir. Implemented with the `ESP8266` module inside of a `nodemcuv2` board.

The idea is to pump water from a well to a reservoir and track the amount of water in it. The control panel of the pump has a system to activate or deactivate the pump, so I've used a couple of probes to notice the lowest and the highest level and a relay output to control the pump.

In order to track the amount of water I've used an Ultrasonic sensor to measure the distance to the water surface and calculating the volume within the deposit dimensions.
By doing that we can leave the system unattended and ensure an automatic irrigation.

The `ESP8266` makes readings every X minutes and stores the log into an SD card. It also serves a little webpage to monitor the current state of the reservoir.

## Hardware

----
>I've linked the parts and pieces matching my needs. I'm aware there are other methods and parts to use. To any suggestion, please make an issue or contact me!

In order to achieve the specifications, we need the following parts:

+ 12V power supply.
+ Microcontroller. In this case the `ESP8266` included in the `nodemcuv2` development board.
+ Water Level Relay board. [link here](https://google.es)
+ SD card breakout.
+ Ultrasonic Distance Sensor. Specifically the HC-SR04 module.
+ RTC Module DS1307 from SparkFun to track the time.[link here](https://google.es)
+ Switches, plugs and cables and what not.


## Firmware

The firmware is organised within a PlatformIO project. `Lib` folder conains the libraries and the `src` folder contains the main code.

You'll need to set some parameters under the `src/configuration.h` in order to work properly with your system. Everything is well commented.

Furthermore, I've configured this repository to work with Travis CI so the label at the top of this wiki shows the state of the compilation.

---
if you clone this project maybe you'll need to configure some things in order to work included and external libraries.
Libraries used in this project:

+ NewPing.h `This library does not work with the ESP8266! `
+ SD.h
+ SPI.h
+ SparkFun DS1307 Real Time Clock
+ Wire.h for I2C communications

### To-Do

- [x] Implement data system to store in the SD card
- [x] Use SD library and code for that
- [x] Implement RTC Module. Based on the DS1307
- [ ] Design the interface for the webpage
- [x] Functions to calculate volume (*liters*)
- [ ] Implement LIFO buffer to calculate flows
- [x] Implement OTA updates for the ESP8266
- [x] Implement mDNS server
- [ ] Implement File System for the ESP8266
- [x] configuration file to make it easy for other systems
- [x] Configure Travis CI to deploy
