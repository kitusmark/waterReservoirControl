// networking.h - ESP8266 Wifi and networking functions
// Main code for the waterReservoirControl system based on the nodemcuv2 ESP8266
//Water level sensing in well deposits with Ultrasonic HC-SR04 sensor and.
// Author: Marc Cobler Cosmen (@kitusmark)
// https://github.com/kitusmark/waterReservoirControl
//Obtaining the distance of the surface and knowing the measures of the deposit, we can calculate the volume of water
//contained. All the data is served in a simple webpage and stored in a SD card for datalogging.
#include "configuration.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

//Webserver stuff
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,0);
IPAddress subnet(255,255,255,0);
//Creating the server object
WiFiServer server(webServerPort);
ESP8266WebServer httpServer(webServerPort);
ESP8266HTTPUpdateServer httpUpdater;
//------------------------WEBSERVER FUNCTIONS---------------------------
void initSoftAP() {
  log("Setting Soft-AP configuration...");
  logln(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  log("Starting the WiFi module as SOFT AP...");
  logln(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  log("Soft-AP IP address = ");
  logln(WiFi.softAPIP());
}
void getClientsConnected() {
  printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
}
void initWebServer() {
  server.begin();
}
void initMDNSServer() {
  if (!MDNS.begin(hostnameMDNS)) {
    logln("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
      //mDNS server not working
    }
  }
  //The mDNS server has been started correctly
  logln("mDNS responder started correctly");
  //Add a service to mDNS-SD
  MDNS.addService("http", "tcp", webServerPort);
}

void initOTAServer() {
  logln("initializing the http OTA Server...");
  httpUpdater.setup(&httpServer, updatePath, updateUsername, updatePassword);
  httpServer.begin();
  printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login \n", hostnameMDNS, updatePath);
}
