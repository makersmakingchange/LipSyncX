#include <Adafruit_LPS35HW.h>
#include <Adafruit_BMP280.h>   //onboard pressure sensor on Adafruit Sense Micro 

#define PRESS_BUFF_SIZE 5

#define FILTER_NONE 0
#define FILTER_AVERAGE 1

#define PRESS_TYPE_ABS 0
#define PRESS_TYPE_DIFF 1

Adafruit_LPS35HW lps35hw = Adafruit_LPS35HW();

Adafruit_BMP280 bmp; // use I2C interface

Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();
StopWatch pressureTimer[1];

typedef struct {
  float mainPressure;
  float refPressure;
  float rawPressure;
} pressureStruct;

LSCircularBuffer <pressureStruct> pressureBuffer(PRESS_BUFF_SIZE);

class LSPressure {
  private: 
    void resetTimer();
    unsigned long getTime();
    int filterMode;
    int pressureType;
    float mainVal;
    float refVal;
    float compVal;
    float rawVal;
    sensors_event_t pressure_event;
  public:
    LSPressure();
    void begin(int type);                                    
    void clear();  
    void update();    
    void setFilterMode(int mode); 
    float getMainPressure();
    float getRefPressure();
    float getRawPressure();
    float getCompPressure();
    void setCompPressure();
    void setZeroPressure();
    pressureStruct getAllPressure();
    float getAveragePressure();  
};

LSPressure::LSPressure() {

}

void LSPressure::begin(int type) {

  //BMP280 Pressure sensor setups
  if (!lps35hw.begin_I2C()) {
    Serial.println(F("Couldn't find LPS35HW chip"));
  }
  //BMP280 Onboard Pressure sensor setups
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor"));
    while (1) delay(10);
  } 

  pressureType = type;

  filterMode = FILTER_NONE;

  lps35hw.setDataRate(LPS35HW_RATE_75_HZ);  // 1,10,25,50,75

  if(pressureType==PRESS_TYPE_DIFF){
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. 
                  Adafruit_BMP280::SAMPLING_NONE,     // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X4,    // Pressure oversampling 
                  Adafruit_BMP280::FILTER_X16,      // Filtering. 
                  Adafruit_BMP280::STANDBY_MS_1000); // Standby time.       
  }
  clear();

  setZeroPressure();
}

void LSPressure::clear() {

  while(pressureBuffer.getLength()<PRESS_BUFF_SIZE){
    pressureBuffer.pushElement({0.0, 0.0, 0.0});   
  }

}

void LSPressure::update() {
  //resetTimer();
  
  bmp_pressure->getEvent(&pressure_event);
  
  mainVal = lps35hw.readPressure();

  float tempRefVal = pressure_event.pressure;

  //Update compensation pressure value if reference pressure is changed 
  if(refVal!=tempRefVal && tempRefVal > 0.00 && pressureType==PRESS_TYPE_DIFF){ 
    compVal+=refVal-tempRefVal;
    refVal=tempRefVal; 
  }
  
  
  //Make sure pressure readings are valid 
  if(mainVal > 0.00 && refVal > 0.00){
    rawVal = mainVal - refVal - compVal;

    pressureBuffer.pushElement({mainVal, refVal, rawVal});
  }
  
  //Serial.println(getTime());  
}

void LSPressure::setFilterMode(int mode) {
  filterMode = mode;
}

float LSPressure::getMainPressure() {
  return pressureBuffer.getLastElement().mainPressure;
}

float LSPressure::getRefPressure() {
  return pressureBuffer.getLastElement().refPressure;
}


float LSPressure::getRawPressure() {
  return pressureBuffer.getLastElement().rawPressure;
}

float LSPressure::getCompPressure() {

  bmp_pressure->getEvent(&pressure_event);

  mainVal = lps35hw.readPressure();
  
  if(pressureType==PRESS_TYPE_DIFF){
    refVal=pressure_event.pressure;
    compVal = mainVal - refVal;
  }
  else{
    compVal = 0.0;
  }
  return compVal;
}

void LSPressure::setCompPressure() {
  getCompPressure();
}

void LSPressure::setZeroPressure() {
  if(pressureType==PRESS_TYPE_DIFF){
    getCompPressure();
  } else{
    refVal = lps35hw.readPressure();
  }
}


pressureStruct LSPressure::getAllPressure() {
  return pressureBuffer.getLastElement();
}


float LSPressure::getAveragePressure()  
{
  float sum = 0.0;
  for (int i = 0 ; i < PRESS_BUFF_SIZE ; i++){
    sum += pressureBuffer.getElement(i).rawPressure;
  }
  return(((float) sum) / PRESS_BUFF_SIZE);
}


//***RESET TIMER FUNCTION***//

void LSPressure::resetTimer() {
  pressureTimer[0].stop();                                //Reset and start the timer         
  pressureTimer[0].reset();                                                                        
  pressureTimer[0].start(); 
}

//***GET TIME FUNCTION***//

unsigned long LSPressure::getTime() {
  unsigned long finalTime = pressureTimer[0].elapsed(); 
  pressureTimer[0].stop();                                //Reset and start the timer         
  pressureTimer[0].reset(); 
  return finalTime;
}
