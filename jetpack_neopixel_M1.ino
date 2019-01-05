// ================================================
// Grmis @ RCGroups, march 2017
// GNU General Public Licence v3
//=================================================
#include <Adafruit_NeoPixel.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


// jetpack setup
#define PIN 6          // NeoPixel strip data pin
#define RPIN 5         // Reley pin
#define NUM_LEDS 48    // neopixel num leds
#define DLOOP 40       // delay per loop in ms
#define TFULL 25       // tolal time to full power in seconds  
#define TCRUIZE 21     // total time in cruise in seconds
#define TFULLZ  17     // total time full 2 zero in seconds
#define TSMOKE  7      // total time in seconds smoke on
#define TSDELAY 1      // delay time to the smoke in seconds
#define TRSMOKE 2.25   // ratio smoke on/off  
#define ROUNDS 5       // num of led ronds on start up
#define BRIGHTNESS 255 // led brightness (0 off, 255 max)
#define VOLUME 15      // Set volume value. From 0 to 30

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

bool debug = false;

int i, counter, rcounter;
bool brdelay;

//----------------------------------------------------------

void setup() {
  i = 0;
  counter  = 0; 
  rcounter = 0;
  brdelay  = true;
  
  mySoftwareSerial.begin(9600);
  pinMode(PIN, OUTPUT);
  pinMode(RPIN, OUTPUT);
  digitalWrite(RPIN, HIGH);

  
  Serial.begin(9600);
  Serial.print("DLOOP   = ");
  Serial.print(DLOOP);
  Serial.print(" ms\n");
  Serial.print("TFULL   = ");
  Serial.print(TFULL);
  Serial.print(" s\n");
  Serial.print("TCRUIZE = ");
  Serial.print(TCRUIZE);
  Serial.print(" s\n");
  Serial.print("TFULLZ  = ");
  Serial.print(TFULLZ);
  Serial.print(" s\n");
  
  Serial.print("nloops  = [");
  Serial.print(nloops_tf); 
  Serial.print(" ");
  Serial.print(nloops_tc);
  Serial.print(" ");
  Serial.print(nloops_tz);
  Serial.print("]\n");

// init dfplayer mini
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true){
        delay(0); // Code to compatible with ESP8266 watch dog.
      }
    }
  Serial.println(F("DFPlayer Mini online."));
 

  myDFPlayer.volume(VOLUME);  //Set volume value. From 0 to 30

   
// init neopixels
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

  if (debug) {
    Serial.print("[i counter c time = [");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(counter);
    Serial.print(" ");
    Serial.print(c);
    Serial.print(" ");
    Serial.print(float(i)*DLOOP/1000);
    Serial.print("]\n");
  }

// DFplayer control 
  if (i < 2) {
    myDFPlayer.play(1);  //Play the first mp3
  }

// Relay control (smoke)


  if (brdelay) {
    digitalWrite(RPIN, HIGH);    
  }

  if (rcounter == rdelay) {
    brdelay = false;
    digitalWrite(RPIN, LOW);
  }
  
  rcounter = rcounter + 1;
  
  delay(DLOOP);
 
}
//----------------------------------------------------------

