
/*******************wifi*********************/
//char ssid[] = "WindowsPhone5924";     //  your network SSID (name)
//char pass[] = "RanchoRelaxo1";    // your network password (use for WPA, or use as key for WEP)

char ssid[] = "William";     //  your network SSID (name)
char pass[] = "123456789";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//#define AIO_USERNAME    "jonsm100"
//#define AIO_KEY         "8e4958b7900248c8a6cfb83507c24a9a" //currrent?
#define AIO_USERNAME  "jonsm200"
#define AIO_KEY       "275266ec4c564324928fcc4753aea556"

//define pins for lighting status LEDs
//int ledPin0 = 14; //wired power to bb.  Any need for this one now?
int ledPin1 =  42; 
int ledPin2 =  38;
int ledPin3 =  36;
int ledPin4 =  24;
//define pin for sounding buzzer
int buzzerPin = 48;
