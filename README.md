waterReservoirControl
===========

Code for an automated water level control system in a reservoir. Implemented with `arduino`.

The idea is to pump water from a well to a reservoir and track the amount of water in it. The control panel of the pump has a system to activate or deactivate the water level, so i'll use a couple of probes to notice the lowest and the highest level.

By doing that we can leave the system unattended and ensure an automatic irrigation. 

The function of the microcontroller is to monitor the water level every *X* seconds and store all the data in a SD card in order to process the data later in the computer or see the basic info on the system display.

##Hardware

----
>I've linked the parts and pieces matching my needs. I'm aware there are other methods and parts to use. To any suggestion, please make an issue or contact me!

In order to achieve the function, we need the following parts:

+ 12V power supply 
+ Arduino UNO or MEGA
+ Water Level Relay board
+ LCD display for data visualization
+ SD card breakout
+ Ultrasonic Distance Sensor
+ A couple of momentary push buttons

##Software

---
Libraries used in this project:

+ NewPing.h
+ SDAdafruit.h
+ SPI.h


#TO-DO
===

- [ ] Implement data system to store in the SD card
- [ ] Use SD library and code for that
- [ ] Implement Sleep mode of the ATmega
- [ ] Design the interface of the display and decide the menus
- [x] Functions to calculate volume (*liters*)