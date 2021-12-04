#include <Adafruit_NeoPixel.h>

#define OUTPUT_RGB_LED_PIN A3 //output pin for neopixel
#define OUTPUT_RGB_LED_NUM 3

#define LED_CLR_NONE 0
#define LED_CLR_RED 1
#define LED_CLR_BLUE 2
#define LED_CLR_GREEN 3
#define LED_CLR_YELLOW 4
#define LED_CLR_PINK 5
#define LED_CLR_ORANGE 6
#define LED_CLR_PURPLE 7
#define LED_CLR_TEAL 8

Adafruit_NeoPixel ledPixels = Adafruit_NeoPixel(OUTPUT_RGB_LED_NUM, OUTPUT_RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

struct rgb_t {
    int r;    // red value 0 to 255
    int g;   // green value
    int b;   // blue value
 };

//Color structure 
typedef struct { 
  uint8_t colorNumber;
  String colorName;
  rgb_t colorCode;
} colorType;

//Color properties 
const colorType colorProperty[] {
    {LED_CLR_NONE,"None",{0,0,0}},
    {LED_CLR_RED,"Red",{60,0,0}},
    {LED_CLR_BLUE,"Blue",{0,0,60}},
    {LED_CLR_GREEN,"Green",{0,60,0}},
    {LED_CLR_YELLOW,"Yellow",{50,60,0}}, 
    {LED_CLR_PINK,"Pink",{60,0,50}},   
    {LED_CLR_ORANGE,"Orange",{60,20,0}},
    {LED_CLR_PURPLE,"Purple",{50,0,128}},
    {LED_CLR_TEAL,"Teal",{0,128,128}}       
};

class LSOutput {
  public:
    LSOutput();
    pressureType pressureArray[PS_ARRAY_SIZE];
    void begin();  
    void clearLedAll();                                  
    void clearLed(int ledNumber);
    uint32_t getLedColor(int ledNumber);
    void setLedColor(int ledNumber, uint32_t ledColor, uint8_t ledBrightness);
    void setLedColorById(int ledNumber, int ledColorNumber, uint8_t ledBrightness); 
    void setLedBlink(int ledNumber, int numBlinks, int delayBlinks, uint32_t ledColor,uint8_t ledBrightness);
    void setLedBlinkById(int ledNumber, int numBlinks, int delayBlinks, int ledColorNumber,uint8_t ledBrightness);
    uint8_t getLedBrightness();
    void setLedBrightness(uint8_t ledBrightness); 
};

LSOutput::LSOutput() {

}

void LSOutput::begin() {

  pinMode(OUTPUT_RGB_LED_PIN, OUTPUT);

  ledPixels.begin();
  clearLedAll();
}

//***CLEAR ALL RGB LED FUNCTION***//

void LSOutput::clearLedAll() {
  clearLed(OUTPUT_RGB_LED_NUM+1);
}

//***CLEAR RGB LED FUNCTION***//

void LSOutput::clearLed(int ledNumber) {

    if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
      ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(0,0,0));
    }
    else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
        //ledPixels.clear();
      for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
        ledPixels.setPixelColor(i, ledPixels.Color(0,0,0));
      }
    }
  ledPixels.show(); 
}

//***GET RGB LED COLOR FUNCTION***//

uint32_t LSOutput::getLedColor(int ledNumber) {

  uint32_t colorValue = ledPixels.getPixelColor(ledNumber-1);
  
  return colorValue;
}

//***SET RGB LED COLOR FUNCTION***//

void LSOutput::setLedColor(int ledNumber, uint32_t ledColor, uint8_t ledBrightness){
  if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
      ledPixels.setPixelColor(ledNumber-1, ledColor);
  } else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
    for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
      ledPixels.setPixelColor(i, ledColor);
    }    
  }

  ledPixels.setBrightness(ledBrightness);
  ledPixels.show(); 

}



//***SET RGB LED COLOR BY ID FUNCTION***//

void LSOutput::setLedColorById(int ledNumber, int ledColorNumber, uint8_t ledBrightness) {
    if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
      ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));
    }
    else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
      for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
        ledPixels.setPixelColor(i, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));            
      }
    }
    ledPixels.setBrightness(ledBrightness);
    ledPixels.show();
}

//***SET RGB LED BLINK FUNCTION***//

void LSOutput::setLedBlink(int ledNumber, int numBlinks, int delayBlinks, uint32_t ledColor,uint8_t ledBrightness) {
      for (int i = 0; i < numBlinks; i++) {
        if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
          ledPixels.setPixelColor(ledNumber-1,ledColor);
        }
        else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
          for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
            ledPixels.setPixelColor(i, ledColor);            
          }
        }
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
        if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
          ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(0,0,0));
        }
        else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
          for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
            ledPixels.setPixelColor(i, ledPixels.Color(0,0,0));
          }
        }
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
      }
}

//***SET RGB LED BLINK BY ID FUNCTION***//

void LSOutput::setLedBlinkById(int ledNumber, int numBlinks, int delayBlinks, int ledColorNumber,uint8_t ledBrightness) {

      for (int i = 0; i < numBlinks; i++) {
        if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
          ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));
        }
        else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
          for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
            ledPixels.setPixelColor(i, ledPixels.Color(colorProperty[ledColorNumber].colorCode.g,colorProperty[ledColorNumber].colorCode.r,colorProperty[ledColorNumber].colorCode.b));            
          }
        }
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
        if(ledNumber>=1 && ledNumber <=OUTPUT_RGB_LED_NUM) {
          ledPixels.setPixelColor(ledNumber-1, ledPixels.Color(0,0,0));
        }
        else if (ledNumber==OUTPUT_RGB_LED_NUM+1) {
          for (int i = 0; i < OUTPUT_RGB_LED_NUM; i++) {
            ledPixels.setPixelColor(i, ledPixels.Color(0,0,0));
          }
        }
        ledPixels.setBrightness(ledBrightness);
        ledPixels.show(); 
        delay(delayBlinks);
      }
}


//***GET RGB LED BRIGHTNESS FUNCTION***//

uint8_t LSOutput::getLedBrightness() {
  return (ledPixels.getBrightness());
}



//***SET RGB LED BRIGHTNESS FUNCTION***//

void LSOutput::setLedBrightness(uint8_t ledBrightness) {
  ledPixels.setBrightness(ledBrightness);
  ledPixels.show();
}
