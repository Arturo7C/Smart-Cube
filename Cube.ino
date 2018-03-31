// **** Smart Cube  *******
// **** Arturo Carcamo ****
// **** ED2 ***************
// **** CUBE **************

// Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <FastLED.h>
#define NUM_LEDS 6
#define DATA_PIN 6

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

CRGB leds[NUM_LEDS];

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

//int ledDown = 6;
//int ledWest = 7;
//int ledNorth = 8;
//int ledSouth = 9;
//int ledUp = 12;
//int ledEast = 13;

// Face orientation and shake functions:
int lastOrientation = - 1;              // previous orientation (for comparison)
unsigned long previousMillis = 0;       // last time update
unsigned long interval = 2000;          // time to wait in up position before the face activation
unsigned long SHAKEpreviousMillis = 0;  // last time update
unsigned long SHAKEinterval = 5000;     // time to wait during shaking for face deactivation
boolean keep = false;                   // this is used to count only one time the face orientation change
int lastFUNCTION = -1;                  // this is used to know what is the previous rientation

char c = ' '; 

// Faces initilization: at the beginning every face is false
boolean face0 = false;
boolean face1 = false;
boolean face2 = false;
boolean face3 = false;
boolean face4 = false;
boolean face5 = false;

int CountdownInMillis = 0;
const int SETminute =  2;    // set 2 minute timer [SET]
const int SETsecond =  30;   // set 30 seconds timer [SET]


void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  //pinMode(ledDown, OUTPUT);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}
void loop() {
  //const char text[] = "Hello Bitches";
  //radio.write(&text, sizeof(text));
  //delay(1000);

  int orientation = - 1;
  String orientationString;
  unsigned long currentMillis = millis();
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)


  int absX = abs(AcX);
  int absY = abs(AcY);
  int absZ = abs(AcZ);

    if ( (absZ > absX) && (absZ > absY)) {
    // base orientation on Z
    if (AcZ > 0) {
      orientationString = "up";
      orientation = 0;  
    } else {
      orientationString = "down";
      orientation = 1;
    }
  } else if ( (absY > absX) && (absY > absZ)) {
    // base orientation on Y
    if (AcY > 0) {
      orientationString = "digital pins up";
      orientation = 2;
    } else {
      orientationString = "analog pins up";
      orientation = 3;
    }
  } else {
    // base orientation on X
    if (AcX < 0) {
      orientationString = "connector up";
      orientation = 4;
    } else {
      orientationString = "connector down";
      orientation = 5;
    }
  }

  // if the orientation has changed, print out a description:
 if (orientation != lastOrientation) {
    //******Serial.println(orientationString);
    lastOrientation = orientation;
    previousMillis = currentMillis;      // memorize the time when the face has changed
    keep = false;
  } else if (currentMillis - previousMillis > interval && keep == false && lastFUNCTION != orientation) {
    Serial.println(orientationString);
    
    if (orientation == 1) {              // TV face
      face1 = true;                      // TV face becomes true
      lastFUNCTION = orientation;        // memorize this activation in [lastFUNCTION]
      Serial.println("1 true");         // print the activated face
      leds[5] = CRGB::Magenta;
      FastLED.show();
      c = 'D';
      radio.write(&c, sizeof(c));
    }
    if (orientation == 4) {              // LIGHTS face
      face4 = true;                     
      lastFUNCTION = orientation;
      Serial.println("LIGHTS true");
      leds[4] = CRGB::Magenta;
      FastLED.show();
      c = 'E';
      radio.write(&c, sizeof(c));
    }
    if (orientation == 3) {              // OFF face
      face3 = true;
      lastFUNCTION = orientation;
      Serial.println("OFF true");
      leds[3] = CRGB::Magenta;
      FastLED.show();
      c = 'S';
      radio.write(&c, sizeof(c));
    }
    if (orientation == 5) {              // SOUND face
      face5 = true;
      lastFUNCTION = orientation;
      Serial.println("SOUND true");
      leds[0] = CRGB::Magenta;
      FastLED.show();
      c = 'W';
      radio.write(&c, sizeof(c));
    }
    if (orientation == 2) {             // TEMPERATURE face
      face2 = true;
      lastFUNCTION = orientation;
      Serial.println("TEMPERATURE true");
      //digitalWrite(ledNorth, HIGH);
      leds[1] = CRGB::Magenta;
      FastLED.show();
      c = 'N';
      radio.write(&c, sizeof(c));
    }
    if (orientation == 0) {             // TIMER face
      face0 = true;
      lastFUNCTION = orientation;
      Serial.println("0, UP true");
      //digitalWrite(ledUp,HIGH);
      leds[2] = CRGB::Magenta;
      FastLED.show();
      c = 'U';
      radio.write(&c, sizeof(c));
    }
    keep = true;  // [keep] change value so that, in the next loop, you can't enter in this condition if the face don't change (avoid to activate another time the same face)
 
  }

  int absGX = abs(GyX);
  int absGY = abs(GyY);
  int absGZ = abs(GyZ);
  
 if( (GyX > 30000 || GyY > 30000 || GyZ > 30000) && ((currentMillis - SHAKEpreviousMillis) > SHAKEinterval))
    {
       Serial.println("SHAKE");

       // the last activated face [lastFUNCTION] is set as false (that means that the face function is deactivated)
     
    //TV deactivation
    if (lastFUNCTION == 1) {                //
      //Serial.println("1 false - CLOSE");   // print the closed face
      //digitalWrite(ledDown, LOW);
      face1 = false;                        // TV face becomes false
      c = 'B';
      radio.write(&c, sizeof(c));
      leds[5] = CRGB::Aqua;
      FastLED.show();
      delay(120);
      leds[5] = CRGB::Black;
      FastLED.show();
    }
    //LIGHTS deactivation
    if (lastFUNCTION == 4) {                // LIGHTS face
      //Serial.println("4 false - CLOSE");
      //digitalWrite(ledEast, LOW);
      face4 = false;                        // LIGHTS face become false
      c = 'C';
      radio.write(&c, sizeof(c));
      leds[4] = CRGB::Aqua;
      FastLED.show();
      delay(120);
      leds[4] = CRGB::Black;
      FastLED.show();
    }
    // OFF 
    if (lastFUNCTION == 3) {                // OFF face
      //Serial.println("3 false - CLOSE");
       //digitalWrite(ledSouth, LOW);
      face3 = false;                        // OFF face becomes false
      c = 'G';
      radio.write(&c, sizeof(c));
      leds[3] = CRGB::Aqua;
      FastLED.show();
      delay(120);
      leds[3] = CRGB::Black;
      FastLED.show();
    }
    // SOUND deactivation
    if (lastFUNCTION == 5) {                // SOUND face
      //Serial.println("5 false - CLOSE");
      // digitalWrite(ledWest, LOW);
      face5 = false;                        // SOUND face becomes false
      c = 'F';
      radio.write(&c, sizeof(c));
      leds[0] = CRGB::Aqua;
      FastLED.show();
      delay(120);
      leds[0] = CRGB::Black;
      FastLED.show();
    }
    // TEMPERATURE deactivation
    if (lastFUNCTION == 2) {                // TEMPERATURE face
      //Serial.println("2 false - CLOSE");
      //digitalWrite(ledNorth, LOW);
      face2 = false;                        // TEMPERATURE face became false
      c = 'H';
      radio.write(&c, sizeof(c));
      leds[1] = CRGB::Aqua;
      FastLED.show();
      delay(120);
      leds[1] = CRGB::Black;
      FastLED.show();
    }
    // TIMER deactivation
    if (lastFUNCTION == 0) {                // TIMER face
      //Serial.println("TIMER false - CLOSE");
      //digitalWrite(ledUp,LOW);
      face0 = false;                        // TIMER face became false
      c = 'A';
      radio.write(&c, sizeof(c));
      leds[2] = CRGB::Aqua;
      FastLED.show();
      delay(120);
      leds[2] = CRGB::Black;
      FastLED.show();
    }
      
    SHAKEpreviousMillis = currentMillis;    // memorize the value for the [SHAKEinterval] calculation
  }

    
}


