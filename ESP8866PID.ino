

// this example is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple
#include <PID_v1.h>
#include "max6675.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define wifi
//#define invertSSR

//webstuff
float temp_f;
#if defined(wifi)
String webString = "";
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

/* Set these to your desired credentials. */
const char *ssid = "HendoNail";
const char *password = "";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}
#endif
//#define PIN_INPUT 0
//#define RELAY_PIN 3
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 13;

const int ledPin =  6;      // the number of the LED pin
const int RELAY_PIN =  3;
// Variables will change :
int ledState = LOW;

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;

//PID!!
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT);


int WindowSize = 5000;
unsigned long windowStartTime;


void setup() {
  windowStartTime = millis();
  Setpoint = 100;
  myPID.SetOutputLimits(0, 100);
  myPID.SetMode(AUTOMATIC);
  
  //pinMode(ledPin, OUTPUT);
  display.begin();
  // use Arduino pins
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  
  // wait for MAX chip to stabilize

  //webstuff
  delay(45);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
#if defined(wifi)
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
#endif
}

void loop() {
  // basic readout test, just print the current temp
  temp_f = thermocouple.readFahrenheit();
#if defined(wifi)
  webString = "Temperature: " + String((int)temp_f) + " F";
  webString = "Output: " + String((int)Output);
#endif
Input = temp_f;
  myPID.Compute();
  runrelay();
 
  

 
  
#if defined(wifi)
  server.send(200, "text/plain", webString);
  server.handleClient();
#endif
}


void runrelay(){
  //setup code here to use output from PID loop to apply it towards relay
   if(millis() - windowStartTime>WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
#if defined(invertSSR)
  if(Output < millis() - windowStartTime) digitalWrite(RELAY_PIN,HIGH);
  else digitalWrite(RELAY_PIN,LOW);
#endif
#if !defined(invertSSR)
if(Output < millis() - windowStartTime) digitalWrite(RELAY_PIN,LOW);
  else digitalWrite(RELAY_PIN,HIGH);
#endif
  if(Output < millis() -windowStartTime) digitalWrite(ledPin,HIGH);
  else digitalWrite(ledPin,LOW);
  
}

void drawscreen(){
   //display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(27, 0);
  display.print("Current Temp");
  display.setTextSize(2);
  display.setCursor(25, 15);
  display.println(thermocouple.readFahrenheit());
  display.setCursor(90, 15);
  display.print("F");
  display.display();
  
}

