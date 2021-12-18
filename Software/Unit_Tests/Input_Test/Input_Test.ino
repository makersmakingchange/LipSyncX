
#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include <Wire.h>  
#include <StopWatch.h>

#include "LSQueue.h"
#include "LSOutput.h"
#include "LSInput.h"

#define ACTION_NOTHING 0                              // No action
#define ACTION_LEFT_CLICK 1                           // Generates a short left click
#define ACTION_RIGHT_CLICK 2                          // Generates a short right click
#define ACTION_DRAG 3                                 // Initiates drag mode, holding down left click until cancelled
#define ACTION_SCROLL 4                               // Initiates scroll mode. Vertical motion generates mouse scroll wheel movement.
#define ACTION_MIDDLE_CLICK 5                         // Generates a short middle click
#define ACTION_CURSOR_HOME_RESET 6                    // Initiates the cursor home reset routine to reset center position. 



#define LED_BRIGHTNESS 50 

#define LED_ACTION_OFF 0
#define LED_ACTION_ON 1
#define LED_ACTION_BLINK 2


int outputAction;

uint8_t ledStateArray[3] = {LED_ACTION_OFF,LED_ACTION_OFF,LED_ACTION_OFF};

typedef struct { 
  uint8_t inputActionNumber;
  uint8_t inputActionState;
  uint8_t inputActionLedState;
  uint8_t inputActionLedNumber;
  uint8_t inputActionColorNumber;
  unsigned long inputActionStartTime;
  unsigned long inputActionEndTime;
} inputActionStruct;




int inputActionSize;
inputStruct inputActionState;

const inputActionStruct inputActionProperty[] {
    {ACTION_NOTHING,            0, LED_ACTION_OFF,    0,LED_CLR_NONE,   0,0},
    {ACTION_LEFT_CLICK,         1 , LED_ACTION_BLINK,  1,LED_CLR_RED,    0,1000},
    {ACTION_RIGHT_CLICK,        4, LED_ACTION_BLINK,  3,LED_CLR_BLUE,   0,1000},
    {ACTION_DRAG,               5 , LED_ACTION_BLINK,  1,LED_CLR_YELLOW, 1000,3000},
    {ACTION_SCROLL,             3, LED_ACTION_BLINK,  3,LED_CLR_GREEN,  1000,3000},
    {ACTION_CURSOR_HOME_RESET,  2, LED_ACTION_BLINK,  2,LED_CLR_PURPLE,  3000,5000}
};



LSInput ip;                     //Starts an instance of the LSPressure object

LSOutput led;                   //Starts an instance of the LSOutput led object



void setup() {
  
  Serial.begin(115200);
  // Wait until serial port is opened
  while( !TinyUSBDevice.mounted() ) delay(1);
  
  delay(2000);

  initInput();

  led.begin();  

  
  
  initLedFeedback();

  
  Scheduler.startLoop(inputDataloop);
  
} //end setup

void initInput(){
  
  ip.begin();
  inputActionSize=sizeof(inputActionProperty)/sizeof(inputActionStruct);

}


//The loop handling pressure polling, sip and puff state evaluation 
void inputDataloop() {
  
  ip.updateButton();              //Request new values 


  //Get the last state change 
  inputActionState = ip.getButtonState();

  printInputData();
  //Output action logic
  
  for (int i=0; i < inputActionSize; i++) {
    if(inputActionState.mainState==inputActionProperty[i].inputActionState && 
      inputActionState.secondaryState == INPUT_SEC_STATE_RELEASED &&
      inputActionState.elapsedTime >= inputActionProperty[i].inputActionStartTime &&
      inputActionState.elapsedTime < inputActionProperty[i].inputActionEndTime){
      
      performinputAction(i,
      inputActionProperty[i].inputActionLedState,
      inputActionProperty[i].inputActionLedNumber,
      inputActionProperty[i].inputActionColorNumber);
      
      break;
    }
  }
  
  delay(20);
}


void loop() {



  delay(20);
}




void printInputData() {

  Serial.print(" main: "); Serial.print(inputActionState.mainState);Serial.print(", ");
  Serial.print(" secondary: "); Serial.print(inputActionState.secondaryState);Serial.print(", ");
  Serial.print(" time: "); Serial.print(inputActionState.elapsedTime);Serial.print(", ");
  
  Serial.println();
 
}



void performinputAction(int action, int ledState, int ledNumber, int ledColor) {
  
    
    switch (action) {
      case ACTION_NOTHING: {
        //do nothing
        break;
      }
      case ACTION_LEFT_CLICK: {
        cursorLeftClick();
        led.setLedBlinkById(ledNumber,1,200,ledColor,LED_BRIGHTNESS);
        delay(5);
        break;
      }
      case ACTION_RIGHT_CLICK: {
        cursorRightClick();
        led.setLedBlinkById(ledNumber,1,200,ledColor,LED_BRIGHTNESS);
        delay(5);
        break;
      }
      case ACTION_DRAG: {
        cursorDrag();
        led.setLedBlinkById(ledNumber,1,200,ledColor,LED_BRIGHTNESS);
        delay(5);
        break;
      }
      case ACTION_SCROLL: {
        cursorScroll(); //Enter Scroll mode
        led.setLedBlinkById(ledNumber,1,200,ledColor,LED_BRIGHTNESS);
        delay(5);
        break;
      }
      case ACTION_MIDDLE_CLICK: {
        //Perform cursor middle click
        cursorMiddleClick();
        led.setLedBlinkById(ledNumber,1,200,ledColor,LED_BRIGHTNESS);
        delay(5);
        break;
      }
      case ACTION_CURSOR_HOME_RESET: {
        centerReset();
        led.setLedBlinkById(ledNumber,1,200,ledColor,LED_BRIGHTNESS);
        delay(5);
        break;
      }
   }
}

void cursorLeftClick(void) {
  Serial.println("Left Click");
  delay(100);
}

void cursorRightClick(void) {
  Serial.println("Right Click");  
  delay(100);
}

void cursorMiddleClick(void) {
  Serial.println("Middle Click");  
  delay(100);
}

void cursorDrag(void) {
  Serial.println("Drag");  
}

void cursorScroll(void) {
  Serial.println("Scroll");  
}

void centerReset(void) {
  Serial.println("Center Reset");  
}

void initLedFeedback(){
  led.setLedBlinkById(4,3,500,LED_CLR_GREEN,LED_BRIGHTNESS);
  delay(5);
}