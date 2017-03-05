
/*******************wifi*********************/
//char ssid[] = "";     //  your network SSID (name)
//char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

char ssid[] = "";     //  your network SSID (name)
char pass[] = "";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//#define AIO_USERNAME    ""
//#define AIO_KEY         "" //currrent?
#define AIO_USERNAME  ""
#define AIO_KEY       ""

//define pins for lighting status LEDs
//int ledPin0 = 14; //wired power to bb.  Any need for this one now?
int ledPin1 =  42; 
int ledPin2 =  38;
int ledPin3 =  36;
int ledPin4 =  24;
//define pin for sounding buzzer
int buzzerPin = 48;
