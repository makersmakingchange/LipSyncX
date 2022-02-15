//DO NOT CHANGE 
#define CONF_LIPSYNC_MODEL 1
#define CONF_LIPSYNC_VERSION 1


#define CONF_BUTTON1_PIN 5                                 // Pushbutton S1
#define CONF_BUTTON2_PIN 6                                 // Pushbutton S2
#define CONF_BUTTON3_PIN 9                                 // Pushbutton S3
#define CONF_SWITCH1_PIN A2                                // 3.5mm jack SW1
#define CONF_SWITCH2_PIN 11                                // 3.5mm jack SW2
#define CONF_SWITCH3_PIN 10                                // 3.5mm jack SW3

#define CONF_BUTTON_NUMBER 3
#define CONF_SWITCH_NUMBER 3

#define CONF_ACTION_NOTHING 0                              // No action
#define CONF_ACTION_LEFT_CLICK 1                           // Generates a short left click
#define CONF_ACTION_RIGHT_CLICK 2                          // Generates a short right click
#define CONF_ACTION_DRAG 3                                 // Initiates drag mode, holding down left click until cancelled
#define CONF_ACTION_SCROLL 4                               // Initiates scroll mode. Vertical motion generates mouse scroll wheel movement.
#define CONF_ACTION_CURSOR_CALIBRATION 5                   // Initiates the joystick calibration.
#define CONF_ACTION_CURSOR_CENTER 6                        // Center the joystick 
#define CONF_ACTION_MIDDLE_CLICK 7                         // Generates a short middle click
#define CONF_ACTION_DEC_SPEED 8                            // Decrease cursor movement speed
#define CONF_ACTION_INC_SPEED 9                            // Increase cursor movement speed
#define CONF_ACTION_CHANGE_MODE 10                         // Change communication mode

#define CONF_SETTINGS_FILE    "/settings.txt"
#define CONF_SETTINGS_JSON    "{\"CF\":5,\"MN\":0,\"VN\":0.0,\"CM\":0,\"SL\":0,\"ST\":0,\"PT\":0,\"DZ\":0.0,\"CA0\":[0.0,0.0],\"CA1\":[25.0,25.0],\"CA2\":[-25.0,25.0],\"CA3\":[-25.0,-25.0],\"CA4\":[25.0,-25.0],\"DM\":0}"

#define CONF_JOYSTICK_POLL_RATE 50    //50ms
#define CONF_PRESSURE_POLL_RATE 50 
#define CONF_INPUT_POLL_RATE 50 
#define CONF_BLE_FEEDBACK_POLL_RATE 1000
#define CONF_DEBUG_POLL_RATE 100

#define CONF_COMM_MODE_NONE 0 
#define CONF_COMM_MODE_USB  1
#define CONF_COMM_MODE_BLE  2

#define CONF_DEBUG_MODE  0

//CAN BE CHANGED 
#define CONF_API_ENABLED true

#define CONF_JOY_OUTPUT_SCALE 5
#define CONF_JOY_DEADZONE 0.12
#define CONF_JOY_MIN_DEADZONE 0.0
#define CONF_JOY_MAX_DEADZONE 1.0
#define CONF_JOY_CALIB_BLINK_TIME 300
#define CONF_JOY_CALIB_READING_DELAY 200
#define CONF_JOY_CALIB_READING_NUMBER 10
#define CONF_JOY_CALIB_STEP_DELAY 2000
#define CONF_JOY_SPEED_LEVEL 5

#define CONF_SIP_THRESHOLD 2.5                    //hPa
#define CONF_PUFF_THRESHOLD 2.5
#define CONF_PRESS_MIN_THRESHOLD 1.0
#define CONF_PRESS_MAX_THRESHOLD 100.0

#define CONF_LED_BRIGHTNESS 50 
#define CONF_LED_BRIGHTNESS_LOW 10 
#define CONF_LED_BRIGHTNESS_HIGH 150 

#define CONF_COM_MODE 1        //0 = None , 1 = USB , 2 = Wireless  
#define CONF_COM_MODE_MIN 1
#define CONF_COM_MODE_MAX 2

#define CONF_ACTION_TIMEOUT 60000         //Rest timer?

#define CONF_LED_REACTION_TIME 200        //Led ON time for input actions 

#define CONF_LED_STARTUP_COLOR_TIME 500 
