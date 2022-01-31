
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  //For I2C communication
#include "LSTimer.h"
#include "LSOutput.h"

#define LED_BRIGHTNESS 50          

LSOutput led;

void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  
  delay(2000);

  led.begin();       
  
  initLedFeedback();


  
} //end setup


void loop() {

  for (int i = 1; i < 4; i++) {
    for (int j = 1; j < 9; j++) {
      led.setLedColor(i,j,LED_BRIGHTNESS);
      delay(500);
      led.setLedColor(i,0,LED_BRIGHTNESS);
    }
  }

  delay(500);
}




void initLedFeedback(){
  
}
