#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Adafruit_WINC1500.h>
#include <Adafruit_SleepyDog.h>

/*************************GPS headers***************************/
#include "string.h"
#include "ctype.h"
#include "SoftwareSerial.h"
#include "dGPS.h"
#include "Arduino.h"
#include "OneWire.h"  //for temp sense
