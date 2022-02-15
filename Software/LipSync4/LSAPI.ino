

//***API FUNCTIONS***// - DO NOT CHANGE
typedef void (*FunctionPointer)(bool,bool,String);        //Type definition for API function pointer

typedef struct {                                  //Type definition for API function list
  String endpoint;                               //Unique two character end point  
  String code;                                   //Unique one character command code
  String parameter;                              //Parameter that is passed to function
  FunctionPointer function;                      //API function pointer
} _functionList;                                  

// Declare individual API functions with command, parameter, and corresponding function
_functionList getModelNumberFunction =            {"MN","0","0",&getModelNumber};
_functionList getVersionNumberFunction =          {"VN","0","0",&getVersionNumber};
_functionList getJoystickSpeedFunction =          {"SS","0","0",&getJoystickSpeed};
_functionList setJoystickSpeedFunction =          {"SS","1","",&setJoystickSpeed};
_functionList getJoystickInitializationFunction = {"JI","0","0",&getJoystickInitialization};
_functionList setJoystickInitializationFunction = {"JI","1","1",&setJoystickInitialization};
_functionList getJoystickCalibrationFunction =    {"CA","0","0",&getJoystickCalibration};
_functionList setJoystickCalibrationFunction =    {"CA","1","1",&setJoystickCalibration};
_functionList getJoystickDeadZoneFunction =       {"DZ","0","0",&getJoystickDeadZone};
_functionList setJoystickDeadZoneFunction =       {"DZ","1","",&setJoystickDeadZone};

_functionList getPressureValueFunction =          {"PV","0","0",&getPressureValue};
//_functionList getPressureThresholdFunction =       {"DT","0","0",&getPressureThreshold};
_functionList getSipPressureThresholdFunction =   {"ST","0","0",&getSipPressureThreshold};
_functionList setSipPressureThresholdFunction =   {"ST","1","",&setSipPressureThreshold};
_functionList getPuffPressureThresholdFunction =  {"PT","1","0",&getPuffPressureThreshold};
_functionList setPuffPressureThresholdFunction =  {"PT","1","",&setPuffPressureThreshold};

_functionList getCommunicationModeFunction =      {"CM","0","0",&getCommunicationMode};
_functionList setCommunicationModeFunction =      {"CM","1","",&setCommunicationMode};

_functionList getDebugModeFunction =              {"DM","0","0",&getDebugMode};
_functionList setDebugModeFunction =              {"DM","1","",&setDebugMode};

_functionList resetSettingsFunction =             {"RS","1","1",&resetSettings};
_functionList factoryResetFunction =              {"FR","1","1",&factoryReset};

// Declare array of API functions
_functionList apiFunction[22] = {
  getModelNumberFunction,
  getVersionNumberFunction,
  getJoystickSpeedFunction,
  setJoystickSpeedFunction,
  getJoystickInitializationFunction,
  setJoystickInitializationFunction,
  getJoystickCalibrationFunction,
  setJoystickCalibrationFunction,
  getJoystickDeadZoneFunction,
  setJoystickDeadZoneFunction,
  getPressureValueFunction,
  //getPressureThresholdFunction,
  getSipPressureThresholdFunction,
  setSipPressureThresholdFunction,
  getPuffPressureThresholdFunction,
  setPuffPressureThresholdFunction,
  getCommunicationModeFunction,
  setCommunicationModeFunction,
  getDebugModeFunction,
  setDebugModeFunction,
  resetSettingsFunction,
  factoryResetFunction
};

//***SERIAL SETTINGS FUNCTION TO CHANGE SPEED AND COMMUNICATION MODE USING SOFTWARE***//
// Function   : serialSettings 
// 
// Description: This function confirms if serial settings should be enabled.
//              It returns true if it's in the settings mode and is waiting for a command.
//              It returns false if it's not in the settings mode or it needs to exit the settings mode.
// 
// Parameters :  enabled : bool : The input flag
// 
// Return     : bool
//*************************************************************************************//
bool serialSettings(bool enabled) {

    String commandString = "";  
    bool settingsFlag = enabled;  
    
    //Set the input parameter to the flag returned. This will help to detect that the settings actions should be performed.
     if (Serial.available()>0)  
     {  
       //Check if serial has received or read input string and word "SETTINGS" is in input string.
       commandString = Serial.readString();            
       if (settingsFlag==false && commandString=="SETTINGS") {
        //SETTING received
        //Set the return flag to true so settings actions can be performed in the next call to the function
        printResponseSingle(true,true,true,0,commandString,false,0);
        settingsFlag=true;                         
       } else if (settingsFlag==true && commandString=="EXIT") {
        //EXIT Recieved
        //Set the return flag to false so settings actions can be exited
        printResponseSingle(true,true,true,0,commandString,false,0);
        settingsFlag=false;                         
       } else if (settingsFlag==true && isValidCommandFormat(commandString)){ //Check if command's format is correct and it's in settings mode
        performCommand(commandString);                  //Sub function to process valid strings
        settingsFlag=false;   
       } else {
        printResponseSingle(true,true,false,0,commandString,false,0);
        settingsFlag=false;      
       }
       Serial.flush();  
     }  
    return settingsFlag;
}

//***PERFORM COMMAND FUNCTION TO CHANGE SETTINGS USING SOFTWARE***//
// Function   : performCommand 
// 
// Description: This function takes processes an input string from the serial and calls the 
//              corresponding API function, or outputs an error.
// 
// Parameters :  inputString : String : The input command as a string.
// 
// Return     : void
//*********************************//
void performCommand(String inputString) {
  int inputIndex = inputString.indexOf(':');

  //Extract command string from input string
  String inputCommandString = inputString.substring(0, inputIndex);

  int inputCommandIndex = inputCommandString.indexOf(',');
  
  String inputEndpointString = inputCommandString.substring(0, inputCommandIndex);

  String inputCodeString = inputCommandString.substring(inputCommandIndex+1);
  
  //Extract parameter string from input string
  String inputParameterString = inputString.substring(inputIndex+1);

  // Determine total number of API commands
  int apiTotalNumber=sizeof(apiFunction)/sizeof(apiFunction[0]);

  //Iterate through each API command
  for(int apiIndex = 0; apiIndex < apiTotalNumber; apiIndex++){
    
    // Test if input command string matches API command and input parameter string matches API parameter string
    if(inputEndpointString == apiFunction[apiIndex].endpoint && 
    inputCodeString == apiFunction[apiIndex].code && 
    (inputParameterString == apiFunction[apiIndex].parameter ||
    apiFunction[apiIndex].parameter == "" || 
    apiFunction[apiIndex].parameter == "r" )){
      
      // Matching Command String found
      if( isValidCommandParameter( inputParameterString )) {   //Check if parameter is valid
        //Valid Parameter
        apiFunction[apiIndex].function(true, true, inputParameterString);
        //Handle parameters that are an array as a special case.
/*         if(apiFunction[apiIndex].parameter=="r"){   //"r" denotes an array parameter 
          
          int inputParameterArray[inputParameterString.length() + 1];
          for(unsigned int arrayIndex=0; arrayIndex<inputParameterString.length(); arrayIndex++)
          {
            inputParameterArray[arrayIndex]=inputParameterString.charAt(arrayIndex)-'0';
          }
          
          // Call matching API function with input parameter array
          apiFunction[apiIndex].function(true, true, inputParameterArray);
          delay(5);     
        } else {
          int tempParameterArray[1] = {(int)inputParameterString.toInt()};
          // Call matching API function with input parameter string
          apiFunction[apiIndex].function(true, true, tempParameterArray);
          delay(5);
        } */
      } else { // Invalid input parameter
      
      // Outut error message
      printResponseSingle(true,true,false,2,inputString,false,0);
      }
      break;
    } else if(apiIndex== (apiTotalNumber-1)) { // api doesn’t exist
    
    //Output error message
    printResponseSingle(true,true,false,1,inputString,false,0);

    delay(5);
    break;
    }
  } //end iterate through API functions

}

//***VALIDATE INPUT COMMAND FORMAT FUNCTION***//
// Function   : isValidCommandFormat 
// 
// Description: This function confirms command string has correct format.
//              It returns true if the string has a correct format.
//              It returns false if the string doesn't have a correct format.
// 
// Parameters :  inputCommandString : String : The input string
// 
// Return     : boolean
//***********************************************//
bool isValidCommandFormat(String inputCommandString) {
  bool isValidFormat = false;
  if ((inputCommandString.length()==(6) || //XX,d:d
       inputCommandString.length()==(7) || //XX,d:dd
       inputCommandString.length()==(8) || //XX,d:ddd
       inputCommandString.length()==(9) || //XX,d:dddd
       inputCommandString.length()==(11)) && inputCommandString.charAt(2)==',' && inputCommandString.charAt(4)==':'){ 
    isValidFormat = true;
   }
  return isValidFormat;
}

//***VALIDATE INPUT COMMAND PARAMETER FUNCTION***//
// Function   : isValidCommandParameter 
// 
// Description: This function checks if the input string is a valid command parameters. 
//              It returns true if the string includes valid parameters.
//              It returns false if the string includes invalid parameters.
// 
// Parameters :  inputParamterString : String : The input string
// 
// Return     : boolean
//*************************************************//
bool isValidCommandParameter(String inputParamterString) {
  if (isStrNumber(inputParamterString)){ 
    return true;
   }
  return false;
}

//***CHECK IF STRING IS A NUMBER FUNCTION***//
// Function   : isStrNumber 
// 
// Description: This function checks if the input string is a number. 
//              It returns true if the string includes all numeric characters.
//              It returns false if the string includes a non numeric character.
// 
// Parameters :  str : String : The input string
// 
// Return     : boolean
//******************************************//
boolean isStrNumber(String str){
  boolean isNumber = false;
  for(byte i=0;i<str.length();i++)
  {
    isNumber = isDigit(str.charAt(i)) || str.charAt(i) == '+' || str.charAt(i) == '.' || str.charAt(i) == '-';
    if(!isNumber) return false;
  }
  return true;
}

//***CHECK IF CHAR IS A VALID DELIMITER FUNCTION***//
// Function   : isValidDelimiter 
// 
// Description: This function checks if the input char is a valid delimiter. 
//              It returns true if the character is a valid delimiter.
//              It returns false if the character is not a valid delimiter.
// 
// Parameters :  inputDelimiter : char : The input char delimiter
// 
// Return     : boolean
//******************************************//
bool isValidDelimiter(char inputDelimiter){
  bool validOutput;
  
  (inputDelimiter == ',' || inputDelimiter == ':' || inputDelimiter == '-') ? validOutput = true : validOutput = false;

  return validOutput;
}

//***SERIAL PRINT OUT COMMAND RESPONSE WITH SINGLE PARAMETER FUNCTION***//
// Function   : printResponseSingle 
// 
// Description: Serial Print output of the responses from APIs with single parameter as the output 
// 
// Parameters :  responseEnabled : bool : Print the response if it's set to true, and skip the response if it's set to false.
//               apiEnabled : bool : Print the response and indicate if the the function was called via the API if it's set to true. 
//                                   Print Manual response if the function wasn't called via API.
//               responseStatus : bool : The response status (SUCCESS,FAIL) 
//               responseNumber : int : 0,1,2 (Different meanings depending on the responseStatus)
//               responseCommand : String : The End-Point command which is returned as output.
//               responseParameterEnabled : bool : Print the parameter if it's set to true, and skip the parameter if it's set to false.
//               responseParameter : int : The response parameters printed as output.
// 
// Return     : void
//***********************************************************************//
void printResponseSingle(bool responseEnabled,bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand,bool responseParameterEnabled,int responseParameter) {
  if(responseEnabled) {
    
    if(responseStatus){
      (apiEnabled) ? Serial.print("SUCCESS") : Serial.print("MANUAL");
    }else{
      Serial.print("FAIL");
    } 
    Serial.print(",");
    Serial.print(responseNumber);
    Serial.print(":");
    Serial.print(responseCommand);
    
    if(responseParameterEnabled){
      Serial.print(":");
      Serial.println(responseParameter);    
    } else {
      Serial.println("");  
    } 
  }
}



//***GET MODEL NUMBER FUNCTION***//
// Function   : getModelNumber 
// 
// Description: This function retrieves the current LipSync firmware model number.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getModelNumber(bool responseEnabled, bool apiEnabled) {
  String commandKey = "MN";
  int tempModelNumber = mem.readInt(CONF_SETTINGS_FILE, commandKey);
  if (tempModelNumber != CONF_LIPSYNC_MODEL) {                          //If the previous firmware was different model then reset the settings 
    resetSettings(responseEnabled, apiEnabled);

    tempModelNumber = CONF_LIPSYNC_MODEL;                               //And store the model number
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempModelNumber);
  }
  printResponseInt(responseEnabled,apiEnabled,true,0,"MN,0",true,tempModelNumber);

}
//***GET MODEL NUMBER API FUNCTION***//
// Function   : getModelNumber 
// 
// Description: This function is redefinition of main getModelNumber function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getModelNumber(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getModelNumber(responseEnabled, apiEnabled);
  }
}

//***GET VERSION FUNCTION***//
// Function   : getVersionNumber 
// 
// Description: This function retrieves the current LipSync firmware version number.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getVersionNumber(bool responseEnabled, bool apiEnabled) {
  String commandKey = "VN";
  int tempVersionNumber = mem.readFloat(CONF_SETTINGS_FILE, commandKey);
  if (tempVersionNumber != CONF_LIPSYNC_VERSION) {                          //If the previous firmware was different version then reset the settings 
    resetSettings(responseEnabled, apiEnabled);

    tempVersionNumber = CONF_LIPSYNC_MODEL;                               //And store the version number
    mem.writeFloat(CONF_SETTINGS_FILE, commandKey, tempVersionNumber);
  } 
  printResponseFloat(responseEnabled,apiEnabled,true,0,"VN,0",true,tempVersionNumber);
}
//***GET VERSION API FUNCTION***//
// Function   : getVersionNumber 
// 
// Description: This function is redefinition of main getVersionNumber function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getVersionNumber(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getVersionNumber(responseEnabled, apiEnabled);
  }
}

//***GET JOYSTICK SPEED FUNCTION***//
// Function   : getJoystickSpeed 
// 
// Description: This function retrieves the current joystick speed level.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
int getJoystickSpeed(bool responseEnabled, bool apiEnabled) {
  String commandKey = "SS";
  int tempJoystickSpeedLevel = CONF_JOY_SPEED_LEVEL;
  if(CONF_API_ENABLED) {
    tempJoystickSpeedLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey);
    if(tempJoystickSpeedLevel < 0 || tempJoystickSpeedLevel > 10){
      tempJoystickSpeedLevel = CONF_JOY_SPEED_LEVEL;
      mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempJoystickSpeedLevel);
    }
  } 
  printResponseInt(responseEnabled,apiEnabled,true,0,"SS,0",true,tempJoystickSpeedLevel);

  return CONF_JOY_SPEED_LEVEL;
}
//***GET JOYSTICK SPEED API FUNCTION***//
// Function   : getJoystickSpeed 
// 
// Description: This function is redefinition of main getJoystickSpeed function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getJoystickSpeed(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getJoystickSpeed(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK SPEED FUNCTION***//
// Function   : setJoystickSpeed 
// 
// Description: This function sets the current joystick speed level.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSpeedLevel : bool : The new joystick speed level.
// 
// Return     : void
//*********************************//
void setJoystickSpeed(bool responseEnabled, bool apiEnabled, int inputSpeedLevel) {
  String commandKey = "SS";
  bool isValidSpeed = true;
  int tempJoystickSpeedLevel = inputSpeedLevel;
  if(tempJoystickSpeedLevel >= 0 && tempJoystickSpeedLevel <= 10){ //Check if inputSpeedCounter is valid
    // Valid inputSpeedLevel
    mem.writeInt(CONF_SETTINGS_FILE, commandKey, tempJoystickSpeedLevel);
    
    if(!CONF_API_ENABLED){ tempJoystickSpeedLevel = CONF_JOY_SPEED_LEVEL; }
      isValidSpeed = true;
  }
  else {
    //Invalid inputSpeedLevel
    setLedState(LED_ACTION_BLINK, LED_CLR_PURPLE, 4, 6, 50,CONF_LED_BRIGHTNESS);    //Blink 6 times
    performLedAction(ledCurrentState); 
    tempJoystickSpeedLevel = mem.readInt(CONF_SETTINGS_FILE, commandKey); 
    isValidSpeed = false;
  }

  js.setOutputScale(inputSpeedLevel);
  
  int responseCode=0;
  (isValidSpeed) ? responseCode = 0 : responseCode = 3;
  printResponseInt(responseEnabled,apiEnabled,true,0,"SS,1",true,tempJoystickSpeedLevel);
}
//***SET JOYSTICK SPEED API FUNCTION***//
// Function   : setJoystickSpeed 
// 
// Description: This function is redefinition of main setJoystickSpeed function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setJoystickSpeed(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setJoystickSpeed(responseEnabled, apiEnabled, optionalParameter.toInt());
}

//***INCREASE JOYSTICK SPEED LEVEL FUNCTION***//
// Function   : increaseJoystickSpeed 
// 
// Description: This function increases the joystick speed level by one.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void increaseJoystickSpeed(bool responseEnabled, bool apiEnabled) {
  int tempJoystickSpeedLevel = js.getOutputScale();
  tempJoystickSpeedLevel++;

  setJoystickSpeed(responseEnabled, apiEnabled, tempJoystickSpeedLevel);
}

//***DECREASE JOYSTICK SPEED LEVEL FUNCTION***//
// Function   : decreaseJoystickSpeed 
// 
// Description: This function decreases the joystick speed level by one.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void decreaseJoystickSpeed(bool responseEnabled, bool apiEnabled) {
  int tempJoystickSpeedLevel = js.getOutputScale();
  tempJoystickSpeedLevel--;

  setJoystickSpeed(responseEnabled, apiEnabled, tempJoystickSpeedLevel);
}


//***GET JOYSTICK INITIALIZATION FUNCTION***//
/// Function   : getJoystickInitialization 
// 
// Description: This function retrieves the FSR Neutral values from joystick Initialization.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getJoystickInitialization(bool responseEnabled, bool apiEnabled) {
  pointFloatType tempCenterPoint = js.getInputComp();
  printResponseFloatPoint(responseEnabled,apiEnabled,true,0,"JI,0",true,tempCenterPoint);
}
//***GET JOYSTICK INITIALIZATION API FUNCTION***//
// Function   : getJoystickInitialization 
// 
// Description: This function is redefinition of main getJoystickInitialization function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getJoystickInitialization(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getJoystickInitialization(responseEnabled, apiEnabled);
  }
}

//***SET JOYSTICK INITIALIZATION FUNCTION***//
/// Function   : setJoystickInitialization 
// 
// Description: This function performs joystick Initialization.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void setJoystickInitialization(bool responseEnabled, bool apiEnabled) {
  js.updateInputComp();
  pointFloatType tempCenterPoint = js.getInputComp();
  printResponseFloatPoint(responseEnabled,apiEnabled,true,0,"JI,1",true,tempCenterPoint);
}
//***SET JOYSTICK INITIALIZATION API FUNCTION***//
// Function   : setJoystickInitialization 
// 
// Description: This function is redefinition of main setJoystickInitialization function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setJoystickInitialization(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==1){
    setJoystickInitialization(responseEnabled, apiEnabled);
  }
}

//*** GET JOYSTICK CALIBRATION FUNCTION***//
/// Function   : getJoystickCalibration 
// 
// Description: This function retrieves FSR maximum values from joystick Calibration.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getJoystickCalibration(bool responseEnabled, bool apiEnabled) {
  String commandKey;
  pointFloatType calibrationPointArray[5];
  calibrationPointArray[0] = js.getInputComp(); 
  for (int i = 1; i < 5; i++)
  {
    commandKey = "CA" + String(i);
    calibrationPointArray[i] = mem.readPoint(CONF_SETTINGS_FILE, commandKey);
    js.setInputMax(i, calibrationPointArray[i]);
  }
  printResponseFloatPointArray(responseEnabled,apiEnabled,true,0,"CA,0",true, "", 5, ',', calibrationPointArray);
  
}
//***GET JOYSTICK CALIBRATION API FUNCTION***//
// Function   : getJoystickCalibration 
// 
// Description: This function is redefinition of main getJoystickCalibration function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getJoystickCalibration(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getJoystickCalibration(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK CALIBRATION FUNCTION***//
/// Function   : setJoystickCalibration 
// 
// Description: This function starts the joystick Calibration.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void setJoystickCalibration(bool responseEnabled, bool apiEnabled) {
  js.clear();                                                                                           //Clear previous calibration values 
  int stepNumber = 0;
  canOutputAction = false;
  calibTimerId[0] = calibTimer.setTimeout(CONF_JOY_CALIB_BLINK_TIME, performJoystickCalibration, (int *)stepNumber);  //Start the process  
}
//***SET JOYSTICK CALIBRATION API FUNCTION***//
// Function   : setJoystickCalibration 
// 
// Description: This function is redefinition of main setJoystickCalibration function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setJoystickCalibration(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==1){
    setJoystickCalibration(responseEnabled, apiEnabled);
  }
}


//*** GET JOYSTICK DEADZONE FUNCTION***//
/// Function   : getJoystickDeadZone
// 
// Description: This function retrieves the joystick deadzone.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
float getJoystickDeadZone(bool responseEnabled, bool apiEnabled) {
  String deadZoneCommand = "DZ";  
  float tempDeadzone;
  tempDeadzone = mem.readFloat(CONF_SETTINGS_FILE, deadZoneCommand);

  if((tempDeadzone <= CONF_JOY_MIN_DEADZONE) || (tempDeadzone>=CONF_JOY_MAX_DEADZONE)){
    tempDeadzone = CONF_JOY_DEADZONE;
    mem.writeFloat(CONF_SETTINGS_FILE,deadZoneCommand,tempDeadzone);   
  }
  js.setDeadzone(true,tempDeadzone);
  printResponseFloat(responseEnabled,apiEnabled,true,0,"DZ,0",true,tempDeadzone);
  return tempDeadzone;
}
//***GET JOYSTICK DEADZONE API FUNCTION***//
// Function   : getJoystickDeadZone 
// 
// Description: This function is redefinition of main getJoystickDeadZone function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getJoystickDeadZone(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getJoystickDeadZone(responseEnabled, apiEnabled);
  }
}

//*** SET JOYSTICK DEADZONE FUNCTION***//
/// Function   : setJoystickDeadZone 
// 
// Description: This function starts the joystick deadzone.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void setJoystickDeadZone(bool responseEnabled, bool apiEnabled, float inputDeadZone) {
  String deadZoneCommand = "DZ";  
  if((inputDeadZone > CONF_JOY_MIN_DEADZONE) && (inputDeadZone < CONF_JOY_MAX_DEADZONE)){
    mem.writeFloat(CONF_SETTINGS_FILE,deadZoneCommand,inputDeadZone); 
    js.setDeadzone(true,inputDeadZone);  
    printResponseFloat(responseEnabled,apiEnabled,true,0,"DZ,0",true,inputDeadZone);

  }

}
//***SET JOYSTICK DEADZONE API FUNCTION***//
// Function   : setJoystickDeadZone  
// 
// Description: This function is redefinition of main setJoystickDeadZone function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setJoystickDeadZone(bool responseEnabled, bool apiEnabled, String optionalParameter) {
    setJoystickDeadZone(responseEnabled, apiEnabled, optionalParameter.toFloat());
}

//***GET PRESSURE VALUE FUNCTION***//
// Function   : getPressureValue 
// 
// Description: This function returns pressure difference.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getPressureValue(bool responseEnabled, bool apiEnabled) {

  float tempPressureValue = (float) ps.getDiffPressure();

  printResponseFloat(responseEnabled,apiEnabled,true,0,"PV,0",true,tempPressureValue);

}
//***GET PRESSURE VALUE API FUNCTION***//
// Function   : getPressureValue 
// 
// Description: This function is redefinition of main getPressureValue function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getPressureValue(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getPressureValue(responseEnabled, apiEnabled);
  }
}

//***GET PRESSURE THRESHOLD FUNCTION***//
// Function   : getPressureThreshold  
// 
// Description: This function returns pressure Threshold.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
void getPressureThreshold(bool responseEnabled, bool apiEnabled) {

  float tempPressureThreshold[2];
  tempPressureThreshold[0] = getSipPressureThreshold(false,false);
  tempPressureThreshold[1] = getPuffPressureThreshold(false,false);

  printResponseFloatArray(responseEnabled,apiEnabled,true,0,"DT,0",true, "", 2, ',', tempPressureThreshold);

}
//***GET PRESSURE VALUE API FUNCTION***//
// Function   : getPressureThreshold 
// 
// Description: This function is redefinition of main getPressureThreshold function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getPressureThreshold(responseEnabled, apiEnabled);
  }
}

//***GET PRESSURE THRESHOLD FUNCTION***//
// Function   : getSipPressureThreshold 
// 
// Description: This function returns the current sip pressure threshold.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
float getSipPressureThreshold(bool responseEnabled, bool apiEnabled) {
  String commandKey = "ST";  
  float tempSipThreshold;
  tempSipThreshold = mem.readFloat(CONF_SETTINGS_FILE, commandKey);

  if((tempSipThreshold <= 0.0) || (tempSipThreshold >= CONF_PRESS_MAX_THRESHOLD)){
    tempSipThreshold = CONF_SIP_THRESHOLD;
    mem.writeFloat(CONF_SETTINGS_FILE,commandKey,tempSipThreshold);   
  }
  ps.setSipThreshold(tempSipThreshold);
  printResponseFloat(responseEnabled,apiEnabled,true,0,"ST,0",true,tempSipThreshold);
  return tempSipThreshold;
}
//***GET PRESSURE THRESHOLD API FUNCTION***//
// Function   : getSipPressureThreshold 
// 
// Description: This function is redefinition of main getPressureThreshold function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getSipPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getSipPressureThreshold(responseEnabled, apiEnabled);
  }
}

//***SET SIP PRESSURE THRESHOLD FUNCTION***//
// Function   : setSipPressureThreshold 
// 
// Description: This function sets the current Sip pressure threshold.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputSipThreshold : float : The new sip pressure threshold.
// 
// Return     : void
//*********************************//
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, float inputSipThreshold) {
  String commandKey = "ST";  

  if(inputSipThreshold>CONF_PRESS_MIN_THRESHOLD && inputSipThreshold<CONF_PRESS_MAX_THRESHOLD){
    mem.writeFloat(CONF_SETTINGS_FILE,commandKey,inputSipThreshold);   
    ps.setSipThreshold(inputSipThreshold);
    printResponseFloat(responseEnabled,apiEnabled,true,0,"ST,1",true,inputSipThreshold);

  }
}
//***SET SIP PRESSURE THRESHOLD API FUNCTION***//
// Function   : setSipPressureThreshold 
// 
// Description: This function is redefinition of main setSipPressureThreshold function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setSipPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setSipPressureThreshold(responseEnabled,apiEnabled, optionalParameter.toFloat());
}

//***GET PUFF THRESHOLD FUNCTION***//
// Function   : getPuffPressureThreshold 
// 
// Description: This function returns the puff pressure threshold.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//*********************************//
float getPuffPressureThreshold(bool responseEnabled, bool apiEnabled) {
  String commandKey = "PT";  
  float tempPuffThreshold;
  tempPuffThreshold = mem.readFloat(CONF_SETTINGS_FILE, commandKey);

  if((tempPuffThreshold <= 0.0) || (tempPuffThreshold >= CONF_PRESS_MAX_THRESHOLD)){
    tempPuffThreshold = CONF_PUFF_THRESHOLD;
    mem.writeFloat(CONF_SETTINGS_FILE,commandKey,tempPuffThreshold);   
  }
  ps.setPuffThreshold(tempPuffThreshold);
  printResponseFloat(responseEnabled,apiEnabled,true,0,"PT,0",true,tempPuffThreshold);

  return tempPuffThreshold;
}

//***GET PUFF THRESHOLD API FUNCTION***//
// Function   : getPuffPressureThreshold 
// 
// Description: This function is redefinition of main getPuffPressureThreshold function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getPuffPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getPuffPressureThreshold(responseEnabled, apiEnabled);
  }
}


//***SET PUFF PRESSURE THRESHOLD FUNCTION***//
// Function   : setPuffPressureThreshold 
// 
// Description: This function sets the current Puff pressure threshold in percentage.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputPuffThreshold : float : The new sip pressure threshold in percentage.
// 
// Return     : void
//*********************************//
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, float inputPuffThreshold) {
  String commandKey = "PT";  

  if(inputPuffThreshold>CONF_PRESS_MIN_THRESHOLD && inputPuffThreshold<CONF_PRESS_MAX_THRESHOLD){
    mem.writeFloat(CONF_SETTINGS_FILE,commandKey,inputPuffThreshold);   
    ps.setPuffThreshold(inputPuffThreshold);
    printResponseFloat(responseEnabled,apiEnabled,true,0,"PT,1",true,inputPuffThreshold);

  }
}
//***SET PUFF PRESSURE THRESHOLD API FUNCTION***//
// Function   : setPuffPressureThreshold 
// 
// Description: This function is redefinition of main setPuffPressureThreshold function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setPuffPressureThreshold(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  setPuffPressureThreshold(responseEnabled,apiEnabled, optionalParameter.toFloat());
}

//***GET COMMUNICATION MODE FUNCTION***//
// Function   : getCommunicationMode 
// 
// Description: This function retrieves the state of communication mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : communicationMode : int : The current state of communication method.
//*********************************//
int getCommunicationMode(bool responseEnabled, bool apiEnabled) {
  String comModeCommand = "CM";   
  int tempComMode;
  tempComMode = mem.readInt(CONF_SETTINGS_FILE, comModeCommand);

  if(tempComMode<1 || tempComMode>2){
    tempComMode = CONF_COM_MODE;
    mem.writeInt(CONF_SETTINGS_FILE,comModeCommand,tempComMode);   
  }

  printResponseInt(responseEnabled,apiEnabled,true,0,"CM,0",true,tempComMode);

  return tempComMode;
}
//***GET COMMUNICATION MODE API FUNCTION***//
// Function   : getCommunicationMode
// 
// Description: This function is redefinition of main getCommunicationMode function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getCommunicationMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getCommunicationMode(responseEnabled, apiEnabled);
  }
}

//***SET COMMUNICATION MODE FUNCTION***//
// Function   : setCommunicationMode
// 
// Description: This function sets the state of communication mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputCommunicationMode : int : The new communication mode state ( 0 = None, 1 = USB , 2 = BLE )
// 
// Return     : void
//*********************************//
void setCommunicationMode(bool responseEnabled, bool apiEnabled, int inputCommunicationMode) {
  String comModeCommand = "CM";   
  if ((inputCommunicationMode >= CONF_COM_MODE_MIN) && (inputCommunicationMode <= CONF_COM_MODE_MAX))
  {
    comMode = inputCommunicationMode;
    mem.writeInt(CONF_SETTINGS_FILE,comModeCommand,inputCommunicationMode);    
    printResponseInt(responseEnabled,apiEnabled,true,0,"CM,1",true,inputCommunicationMode);

  }

 
}
//***SET COMMUNICATION MODE API FUNCTION***//
// Function   : setCommunicationMode 
// 
// Description: This function is redefinition of main setCommunicationMode function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setCommunicationMode(bool responseEnabled, bool apiEnabled, String optionalParameter){
  setCommunicationMode(responseEnabled, apiEnabled, optionalParameter.toFloat());
}


//***TOGGLE COMMUNICATION MODE FUNCTION***//
// Function   : toggleCommunicationMode 
// 
// Description: This function is used to toggle or increment communication mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
void toggleCommunicationMode(bool responseEnabled, bool apiEnabled){
  if (comMode < CONF_COM_MODE_MAX)
  {
    comMode++;
  }
  else
  {
    comMode = CONF_COM_MODE_MIN;
  }  
  setCommunicationMode(responseEnabled,apiEnabled,comMode);
}

void setDebugState(bool inputDebugState) {
  if(inputDebugState){
    pollTimer.disable(0);
    pollTimer.enable(4);
  } else {
    pollTimer.enable(0);
    pollTimer.disable(4);
  }
}


//***GET DEBUG MODE STATE FUNCTION***//
// Function   : getDebugMode 
// 
// Description: This function retrieves the state of debug mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : debugState : bool : The current state of debug mode.
//*********************************//
bool getDebugMode(bool responseEnabled, bool apiEnabled) {
  String commandKey = "DM";   
  int tempDebugMode;
  tempDebugMode = mem.readInt(CONF_SETTINGS_FILE, commandKey);

  if(tempDebugMode<0 || tempDebugMode>1){
    tempDebugMode = CONF_DEBUG_MODE;
    mem.writeInt(CONF_SETTINGS_FILE,commandKey,tempDebugMode);   
  }

  setDebugState(tempDebugMode);
  
  printResponseInt(responseEnabled,apiEnabled,true,0,"DM,0",true,tempDebugMode);

  return tempDebugMode;
}
//***GET DEBUG MODE STATE API FUNCTION***//
// Function   : getDebugMode 
// 
// Description: This function is redefinition of main getDebugMode function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void getDebugMode(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    getDebugMode(responseEnabled, apiEnabled);
  }
}

//***SET DEBUG MODE STATE FUNCTION***//
// Function   : setDebugMode 
// 
// Description: This function sets the state of debug mode.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               inputDebugStategMode : int : The new debug mode state ( true = ON , false = OFF )
// 
// Return     : void
//*********************************//
void setDebugMode(bool responseEnabled, bool apiEnabled, int inputDebugMode) {
  String commandKey = "DM";   
  
  if(inputDebugMode>=0 && inputDebugMode<=1){
    mem.writeInt(CONF_SETTINGS_FILE,commandKey,inputDebugMode);   
    setDebugState(inputDebugMode);

  }
  printResponseInt(responseEnabled,apiEnabled,true,0,"DM,1",true,inputDebugMode);
 
}
//***SET DEBUG MODE STATE API FUNCTION***//
// Function   : setDebugMode 
// 
// Description: This function is redefinition of main setDebugMode function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void setDebugMode(bool responseEnabled, bool apiEnabled, String optionalParameter){
  setDebugMode(responseEnabled, apiEnabled, optionalParameter.toInt());
}

 //***RESET SETTINGS FUNCTION***//
// Function   : resetSettings 
// 
// Description: This function performs reset. 
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//***************************//
void resetSettings(bool responseEnabled, bool apiEnabled) { 

  printResponseInt(responseEnabled,apiEnabled,true,0,"RS,1",true,0);

}
//***RESET SETTINGS API FUNCTION***//
// Function   : resetSettings 
// 
// Description: This function is redefinition of main resetSettings function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void resetSettings(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    resetSettings(responseEnabled, apiEnabled);
  }
 }

 //***FACTORY RESET FUNCTION***//
// Function   : factoryReset 
// 
// Description: This function performs factory reset. It can perform a soft or hard reset.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
// 
// Return     : void
//***************************//
void factoryReset(bool responseEnabled, bool apiEnabled) { 
  resetMemory();
  printResponseInt(responseEnabled,apiEnabled,true,0,"FR,1",true,0);

}
//***FACTORY RESET API FUNCTION***//
// Function   : factoryReset 
// 
// Description: This function is redefinition of main factoryReset function to match the types of API function arguments.
// 
// Parameters :  responseEnabled : bool : The response for serial printing is enabled if it's set to true.
//                                        The serial printing is ignored if it's set to false.
//               apiEnabled : bool : The api response is sent if it's set to true.
//                                   Manual response is sent if it's set to false.
//               optionalParameter : String : The input parameter string should contain one element with value of zero.
// 
// Return     : void
void factoryReset(bool responseEnabled, bool apiEnabled, String optionalParameter) {
  if(optionalParameter.length()==1 && optionalParameter.toInt()==0){
    factoryReset(responseEnabled, apiEnabled);
  }
}

void printResponseString(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responseParameter) {
    if(responseEnabled) {
   
    if(responseStatus){
      (apiEnabled) ? Serial.print("SUCCESS") : Serial.print("MANUAL");
    }else{
      Serial.print("FAIL");
    } 
    Serial.print(",");
    Serial.print(responseNumber);
    Serial.print(":");
    Serial.print(responseCommand);
    if(responseParameterEnabled){
      Serial.print(":");
      Serial.println(responseParameter);    
    } else {
      Serial.println("");  
    }  
  }
}

void printResponseInt(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, int responseParameter) {
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, String(responseParameter));

}

void printResponseIntArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, int responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix) + ":";
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex]);  
       if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}


void printResponseIntPoint(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, pointIntType responseParameter) {
  String responseParameterString = "";
  responseParameterString.concat(responseParameter.x);
  responseParameterString.concat("|");
  responseParameterString.concat(responseParameter.y);
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseIntPointArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, pointIntType responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix);
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex].x);
    responseParameterString.concat("|");
    responseParameterString.concat(responseParameter[parameterIndex].y);
    if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseFloat(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, float responseParameter) {
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, String(responseParameter));

}

void printResponseFloatArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, float responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix);
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex]);  
       if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseFloatPoint(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, pointFloatType responseParameter) {
  String responseParameterString = "";
  responseParameterString.concat(responseParameter.x);
  responseParameterString.concat("|");
  responseParameterString.concat(responseParameter.y);
  
  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}

void printResponseFloatPointArray(bool responseEnabled, bool apiEnabled, bool responseStatus, int responseNumber, String responseCommand, bool responseParameterEnabled, String responsePrefix, int responseParameterSize, char responseParameterDelimiter, pointFloatType responseParameter[]) {
  char tempParameterDelimiter[1];

  (isValidDelimiter(responseParameterDelimiter)) ? tempParameterDelimiter[0]={responseParameterDelimiter} : tempParameterDelimiter[0]={'\0'};
  
  String responseParameterString = String(responsePrefix);
  for(int parameterIndex = 0; parameterIndex< responseParameterSize; parameterIndex++){
    responseParameterString.concat(responseParameter[parameterIndex].x);
    responseParameterString.concat("|");
    responseParameterString.concat(responseParameter[parameterIndex].y);
    if(parameterIndex < (responseParameterSize-1)){ responseParameterString.concat(tempParameterDelimiter[0]);  };
  }   

  printResponseString(responseEnabled, apiEnabled, responseStatus, responseNumber, responseCommand, responseParameterEnabled, responseParameterString);

}
