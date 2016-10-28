

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
#define Timer
int msLoop=0;
//#define TempChange

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

const int RELAY_PIN =  16;
// Variables will change :
int ledState = LOW;
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;

//PID!!
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
//PID myPID(&Input, &Output, &Setpoint, 1, 0020, 0010, DIRECT);  // = original
//PID myPID(&Input, &Output, &Setpoint, 100, 20, 0.1, DIRECT); // = first refined
//PID myPID(&Input, &Output, &Setpoint, 38.04, 2.35, 147.63, DIRECT);


//PID myPID(&Input, &Output, &Setpoint, 2, 5, 1, DIRECT);
//PID myPID(&Input, &Output, &Setpoint, 16, 0.5, 4, DIRECT);
PID myPID(&Input, &Output, &Setpoint, 0.64, 29, 7, DIRECT);
//PID myPID(&Input, &Output, &Setpoint, 100, 10, 1, DIRECT); // = 810f
//PID myPID(&Input, &Output, &Setpoint, 100, 40, 1, DIRECT); // = 810f
//PID myPID(&Input, &Output, &Setpoint, 400, 10, 1, DIRECT); // = 814f
//PID myPID(&Input, &Output, &Setpoint, 5, 10, 1, DIRECT);  // = 828f

int WindowSize = 500;
unsigned long windowStartTime;


int PrevTemp;

// set pin numbers:
const int buttonPinR = 14 ;     // the number of the pushbutton pin
const int buttonPinL = 2;     // the number of the pushbutton pin
const int ledPin =  LED_BUILTIN;      // the number of the LED pin

// variables will change:
int buttonStateR = 0;         // variable for reading the pushbutton status
int buttonStateL = 0;
int mult = 10;
int menuAct = 0;

void setup() {
  pinMode(buttonPinR, INPUT);
  pinMode(buttonPinL, INPUT);
  
  windowStartTime = millis();
  Setpoint = 70;
  myPID.SetOutputLimits(0, 100);
  myPID.SetMode(AUTOMATIC);

 
  display.begin();
  // use Arduino pins
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  pinMode(RELAY_PIN, OUTPUT);
  display.clearDisplay();
  display.setTextColor(WHITE);
  // wait for MAX chip to stabilize
  digitalWrite(RELAY_PIN,LOW);
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
  display.clearDisplay();
  // read the state of the pushbutton value:
  buttonStateR = digitalRead(buttonPinR);
  buttonStateL = digitalRead(buttonPinL);
  if (buttonStateR == HIGH && buttonStateL == HIGH) menusys();
  if (menuAct == 1) menusys();
  else butLogic();
   
  // basic readout test, just print the current temp
  //temp_f = round(thermocouple.readFahrenheit()*10)/10.0;
  temp_f = thermocouple.readFahrenheit();
#if defined(wifi)
  webString = "Temperature: " + String((int)temp_f) + " F";
  //webString = "Output: " + String((int)Output);
#endif
  Input = temp_f;
  myPID.Compute();
  //drawscreen();
  //if(thermocouple.readFahrenheit() < Setpoint-150) digitalWrite(RELAY_PIN,HIGH);
    //else runrelay();
  runrelay();  

//trying a countdown timer
#if defined(Timer)
  msLoop = msLoop + 1;
  if (msLoop = 199000) drawscreen();
  if (msLoop = 200000) msLoop = 0;
#endif
//#if !defined(msLoop)
  //drawscreen();
//#endif

#if defined(TempChange)
  if round(thermocouple.readFahrenheit() > PrevTemp) drawscreen();
  if round(thermocouple.readFahrenheit() < PrevTemp) drawscreen();
  PrevTemp = round(thermocouple.readFahrenheit());
#endif
//#if !defined(TempChange)
  //drawscreen();
//#endif
  display.display(); 
  
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
 
}

void drawscreen(){
  
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(thermocouple.readFahrenheit());
  display.setCursor(80, 0);
  display.print("F");
  display.setCursor(0, 20);
  display.println(Setpoint);
  display.setCursor(80, 20);
  display.print("F");
  display.display();
  delay(120);
}

void setSetpointUP(){
    Setpoint = Setpoint + mult;
}

void setSetpointDOWN(){
    Setpoint = Setpoint - mult;
}

void menusys(){
  menuAct = 1;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 40);
  display.print("Menu1");  
  delay(120);
  
}

void butLogic(){
  if (buttonStateR == HIGH) setSetpointUP();
  if (buttonStateL == HIGH) setSetpointDOWN();
}

