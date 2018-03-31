// **** Smart Cube  *******
// **** Arturo Carcamo ****
// **** ED2 ***************
// **** WALL OUTLET **************

// Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

int relay = 5;
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  pinMode(relay, OUTPUT);
}
void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);

  if(text[0] == 'U'){//test variable U this variable changes for each individual wall outlet unit
    digitalWrite(relay, LOW);
  }
  else if(text[0] == 'S'){//test variable S this variable changes for each individual wall outlet unit
    digitalWrite(relay, HIGH);
  }
  }
}
