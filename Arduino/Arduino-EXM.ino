// Transport ex Machina
// for Arduino Yun, with LM35 temp sensor and MG135 CO2 sensor
// Button posts latlon to ExM msg broker
// crowdhackathon demo 19/7/2015

#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <Bridge.h>
#include <HttpClient.h>
#include "MQ135.h"
#define NUMSAMPLES 10
#define RZERO 45

// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
MQ135 gasSensor = MQ135(2);


const int buttonPin = 8;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

int counter = 0;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
String responce;
String wifiStatus;

String latlon[10] = {"lat=37.986323241&lon=23.760765179","lat=37.982718466&lon=23.756552262", "lat=37.982914983&lon=23.730919154", "lat=37.990644262&lon=23.737234228", "lat=37.989977366&lon=23.741324983", "lat=37.976082744&lon=23.738698022", "lat=37.99036872&lon=23.738966232", "lat=37.989354379&lon=23.744948598", "lat=37.976570801&lon=23.747577499", "lat=37.9755773&lon=23.742457196"};

void setup()   
{
  Serial.begin(9600);  // Used to type in characters

  lcd.begin(20, 4);        // initialize the lcd for 20 chars 4 lines, turn on backlight

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);

  Bridge.begin();

  //Serial.begin(9600);
  //while (!Serial);

  // ------- Quick 3 blinks of backlight  -------------
  for (int i = 0; i < 3; i++)
  {
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }
  lcd.backlight(); // finish with backlight on

  //-------- Write characters on the display ------------------
  // NOTE: Cursor Position: Lines and Characters start at 0
  lcd.setCursor(0, 0); //Start at character 4 on line 0
  delay(150);
  lcd.print("TRANSPORT EX MACHINA");
  lcd.setCursor(0, 1);
  delay(150);
  lcd.print("Initializing system!");
  lcd.setCursor(0, 2);
  delay(150);
  lcd.print("please wait...");
  delay(500);
  lcd.setCursor(0, 3);
  lcd.print("Connected to server");



}/*--(end setup )---*/


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{

//  Process wifiCheck;  // initialize a new process
//
//  wifiCheck.runShellCommand("/usr/bin/pretty-wifi-info.lua");  // command you want to run

  // while there's any characters coming back from the
  // process, print them to the serial monitor:
//  while (wifiCheck.available() > 0) {
//    char c = wifiCheck.read();
//    //wifiStatus += c;
//       Serial.print(c);
//  }
//
//  Serial.println();
//




  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        // set the LED:
        digitalWrite(ledPin, ledState);

        // LM35 temperature sensor input
        int Tvalue = analogRead(0);
        delay(100);
        Tvalue = analogRead(0);
        delay(100);
        Tvalue = analogRead(0);

        float millivolts = (Tvalue / 1024.0) * 5000;
        float celsius = millivolts / 10; // LM35 sensor output is 10mV per degree Celsius


        // CO2 sensor input
        float rzero = gasSensor.getRZero();
        float ppm = gasSensor.getPPM();
/*
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
    analogRead(2);
    delay(10);
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  reading = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     reading += samples[i];
  }
  reading /= NUMSAMPLES;
*/
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Temp:" + String(celsius));
        lcd.setCursor(0, 2);
        lcd.print("CO2:" + String(rzero/10));
        
        // Serial.println(latlon[counter]);

        HttpClient client;
        String url = "http://159.8.41.197:8088/update?" + latlon[counter] + "&icon=7&title=EXM_BUS&snippet=Temp:"+ String(celsius)+"%20CO2:"+String(rzero/10);
        //String url = "http://159.8.41.197:8088/update?lat=37.9" + String(counter) + "&lon=23.63&icon=7&title=2&snippet=3";
        client.get(url);
        //Serial.println(url);
        responce = "";
        while (client.available()) {
          char c = client.read();
          responce += c;
        }
        lcd.setCursor(0, 0);
        lcd.print("Server said:" + responce);
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        if(counter == 9) {
          counter = 0;
        }else {
        counter ++;
        }

      }
    }
  }



  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;


}/* --(end main loop )-- */


/* ( THE END ) */
