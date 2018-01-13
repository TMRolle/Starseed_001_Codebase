#include <TriacDimmer.h>

// arduino pins
unsigned char sync = 8; //sync pin
unsigned char channel_1 = 9; // channel 1 pin

void setup() {
  //initialize the dimmer library. We want
  TriacDimmer::begin();
    // Open serial communications and wait for port to open:
  Serial.begin(9600);

}

void loop() {
  if (Serial.available()) {
    int val = Serial.read();
    float valf = val / 255.0;
    TriacDimmer::setBrightness(channel_1, valf);
    Serial.println(valf);
  }

}
