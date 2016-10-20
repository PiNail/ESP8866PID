

// this example is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple
#include <PID_v1.h>
#include "max6675.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define PIN_INPUT 0
//#define RELAY_PIN 6
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 13;

//const int ledPin =  6;      // the number of the LED pin
const int RELAY_PIN =  6;
// Variables will change :
//int ledState = LOW;    

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
PID myPID(&Input, &Output, &Setpoint,2,5,1, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;

  
void setup() {
  windowStartTime = millis();
  Setpoint = 89;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetMode(AUTOMATIC);
  
  //pinMode(ledPin, OUTPUT);
  display.begin();
  display.clearDisplay();  
  display.setTextColor(WHITE);
  // use Arduino pins 
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
   Input = thermocouple.readFahrenheit();
   myPID.Compute();
   unsigned long now = millis();
   if(now - windowStartTime>WindowSize)
   { //time to shift the Relay Window
     windowStartTime += WindowSize;
   }
   if(Output > now - windowStartTime) digitalWrite(RELAY_PIN,HIGH);
   else digitalWrite(RELAY_PIN,LOW);
   display.setTextSize(1);
   display.setCursor(27,0);
   display.print("Current Temp");
   display.setTextSize(2);
   display.setCursor(25,15);  
   display.println(thermocouple.readFahrenheit());
   display.setCursor(90,15);
   display.print("F");
   //digitalWrite(RELAY_PIN, LOW);
   delay(300);
   display.display();
   display.println();
   display.clearDisplay();
   //digitalWrite(RELAY_PIN, LOW);
   //if (thermocouple.readFahrenheit() <= 87) {
     //ledState = LOW;
   //} else {
     //ledState = HIGH;
   //}
   //digitalWrite(ledPin, ledState);
}
