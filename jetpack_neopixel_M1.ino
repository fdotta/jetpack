// ================================================
// Grmis @ RCGroups, march 2017
// GNU General Public Licence v3
//=================================================
#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "config.h"


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRBW + NEO_KHZ800);

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

int nloops_tf = floor(TFULL * 1000 / DLOOP);
int nloops_tc = floor(TCRUIZE * 1000 / DLOOP);
int nloops_tz = floor(TFULLZ * 1000 / DLOOP);
int ron       = floor(TSMOKE * 1000 / DLOOP);
int roff      = floor(ron/(TRSMOKE + 1));
int rdelay    = floor(TSDELAY * 1000 / DLOOP);

int i, counter, rcounter;
bool brdelay;

//----------------------------------------------------------

void setup() {
  i = 0;
  counter  = 0; 
  rcounter = 0;
  brdelay  = true;
  
  pinMode(PIN, OUTPUT);
  pinMode(RPIN, OUTPUT);
  digitalWrite(RPIN, HIGH);
  
  Serial.begin(115200);

  Serial.println(F("jetPack Light and Sound Config"));
  Serial.println(F("------------------------------"));
  Serial.print(F("NeoPixel PIN     = "));
  Serial.println(PIN);
  Serial.print(F("RELAY PIN        = "));
  Serial.println(RPIN);
  Serial.print(F("NLEDS            = "));
  Serial.println(NUM_LEDS);
  Serial.print(F("DLOOP            = "));
  Serial.print(DLOOP);
  Serial.println(F(" ms"));
  Serial.print(F("TFULL            = "));
  Serial.print(TFULL);
  Serial.println(F(" s"));
  Serial.print(F("TCRUIZE          = "));
  Serial.print(TCRUIZE);
  Serial.println(F(" s"));
  Serial.print(F("TFULLZ           = "));
  Serial.print(TFULLZ);
  Serial.println(F(" s"));
  Serial.print(F("TSMOKE           = "));
  Serial.print(TSMOKE);
  Serial.println(F(" s"));
  Serial.print(F("TDELAY           = "));
  Serial.print(TSDELAY);
  Serial.println(F(" s"));
  Serial.print(F("Smoke Time       = "));
  Serial.print(TSMOKE);
  Serial.println(F(" s"));
  Serial.print(F("Smoke Time Ratio = "));
  Serial.println(TRSMOKE);
  Serial.print(F("NUM RPUNDS       = "));
  Serial.println(ROUNDS);
  Serial.print(F("Max BRIGHTNESS   = "));
  Serial.print(BRIGHTNESS);
  Serial.println(F(" (0 to 255)"));
  Serial.print(F("VOLUME           = "));
  Serial.print(VOLUME);
  Serial.println(F(" (0 to 30)\n"));
  
 
  Serial.println(F("Loop Controls"));
  Serial.println(F("-------------"));
  Serial.print(F("[nloops_tf, nloops_tc, nloops_z] = ["));
  Serial.print(nloops_tf); 
  Serial.print(F(", "));
  Serial.print(nloops_tc);
  Serial.print(F(", "));
  Serial.print(nloops_tz);
  Serial.println(F("]"));
  
  Serial.print(F("[ron, roff, rdelay]              = ["));
  Serial.print(ron); 
  Serial.print(F(", "));
  Serial.print(roff);
  Serial.print(F(", "));
  Serial.print(rdelay);
  Serial.println(F("]"));

// init dfplayer mini
  Serial.println();
  Serial.print(F("Initializing DFPlayer  ... "));
  mySoftwareSerial.begin(9600);

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true){
        delay(0); // Code to compatible with ESP8266 watch dog.
      }
    }
 
  myDFPlayer.volume(VOLUME);  //Set volume value. From 0 to 30
  Serial.println(F("online"));

// init neopixels
  Serial.print(F("Initializing NeoPixels ... "));

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  const int turns = ROUNDS;
  for (int i = 0; i < NUM_LEDS * turns; i++) {

    int c = (i < NUM_LEDS * turns / 2) ?
            map(i, 0, NUM_LEDS * turns / 2 - 1, 0, 255) :
            map(i, NUM_LEDS * turns / 2, NUM_LEDS * turns - 1, 255, 0);

    int r, g, b;
    AB_color(c, r, g, b);
    strip.setPixelColor(i % NUM_LEDS, r, g, b); strip.show();
    delay(15);
    strip.setPixelColor(i % NUM_LEDS, 0, 0, 0);
  }
  strip.show();
  Serial.println(F("online"));
  Serial.print(F("Initializing Smoke     ... "));

}
//----------------------------------------------------------
void set_color(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    int r_ = random((1 * r) / 2, min(255, (4 * r) / 3));
    int g_ = random((1 * g) / 2, min(255, (4 * g) / 3));
    int b_ = random((1 * b) / 2, min(255, (4 * b) / 3));
    strip.setPixelColor(i, r_, g_, b_);
  }
  strip.show();
}
//----------------------------------------------------------
void AB_color(int c, int &r, int &g, int &b) {
  const int c1 = 5, c2 = 64, c3 = 128, c4 = 192;
  if (c < c1) {
    r = 0; g = 0; b = 0;
  } else if (c < c2) {
    r = map(c, c1, c2 - 1, 0, 32);
    g = map(c, c1, c2 - 1, 0, 11);
    b = map(c, c1, c2 - 1, 0, 0);
  } else if (c < c3) {
    r = map(c, c2, c3 - 1, 32, 100);
    g = map(c, c2, c3 - 1, 11, 25 );
    b = map(c, c2, c3 - 1, 0, 8);
  } else if (c < c4) {
    r = map(c, c3, c4 - 1, 100, 240 );
    g = map(c, 128, c4 - 1, 25, 45 );
    b = map(c, 128, c4 - 1, 8, 18);
  } else {
    r = map(c, c4, 255, 240, 255 );
    g = map(c, c4, 255, 45, 63 );
    b = map(c, c4, 255, 18, 60);
  }
}
//----------------------------------------------------------
void set_color(int c) {
  int r, g, b;
  if (c > 128) c = random(c / 2, c);
  AB_color(c, r, g, b);
  set_color(r, g, b);
}
//----------------------------------------------------------

void loop() {
// neopixel control sync with audio
    if (i <= nloops_tf) {
         counter = i;
    } else if (i > nloops_tf and i <= nloops_tf + nloops_tc) {
      counter = nloops_tf;  
    } else if (i > nloops_tf + nloops_tc and counter >= 0) {
      counter = counter - 1;
    }
     
    int c = map(counter, 0, nloops_tf, 0, 255);
    set_color(c);
    i = i + 1;

// DFplayer control 
  if (i < 2) {
    myDFPlayer.play(1);  //Play the first mp3
  }

// Relay control (smoke)
  if (brdelay) {
    digitalWrite(RPIN, HIGH);
   } 
    
  if (rcounter >= rdelay) {
    if (brdelay){
      brdelay = false;
      Serial.println(F("online"));   
    }
    
  }

  if (counter == -1) {
    digitalWrite(RPIN, HIGH);
    Serial.println();
    Serial.println(F("NeoPixels              ... offline"));
    Serial.println(F("DFPlayer               ... offline"));
    Serial.println(F("Smoke                  ... offline"));
    Serial.println(F("SYSTEM HALTED!!!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  
  } else {
    if (rcounter <= ron && not brdelay) {
      digitalWrite(RPIN, LOW);
    
    } else if (rcounter > ron && rcounter <= ron + roff) {
      digitalWrite(RPIN, HIGH);

    } else if (rcounter > ron + roff) {
      rcounter = 0;
    }  
  }
  
  rcounter = rcounter + 1;
  
  delay(DLOOP);
 
}
//----------------------------------------------------------

