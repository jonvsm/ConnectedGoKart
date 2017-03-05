/***************************************************
"Final" code 3.4.2017
   
****************************************************/
#include "allheaders.h"
#include "config.h"

/************************* WiFI Setup *****************************/

//alt mega config
#define WINC_CS   17
#define WINC_IRQ  15 //moved from 18...put GPS on 18 and 19
#define WINC_RST  20 //moved from 19

//#define WINC_CS   8
//#define WINC_IRQ  7
//#define WINC_RST  4
//#define WINC_EN   2     // EN tied directly to 5v.  Put it on 2 to control it from arduino, which we don't need

Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);
int status = WL_IDLE_STATUS;

/************ Global State (you don't need to change this!) ******************/

//Set up the wifi client
Adafruit_WINC1500Client client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// define halt
#define halt(s) { Serial.println(F( s )); while(1);  }

/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish temp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temp");
Adafruit_MQTT_Publish vel = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/vel");
Adafruit_MQTT_Publish loc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/loc");

// 'onoff' feed for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/**************************Temp Sensor setup********************************/
int DS18S20_Pin = 44;     //DS18S20 Signal pin
OneWire ds(DS18S20_Pin);  

/*************GPS Pins and default location config *****************************/
// Software serial TX & RX Pins for the GPS module
// Initiate the software serial connection
int ledPin = 13;                  // LED test pin
dGPS dgps = dGPS();               // Construct dGPS class
float Maple_Valley[]={47.42402, -122.0279}; //need a location to for gps "update" command

/*************************** Sketch Code ************************************/

//#define LEDPIN 13 //try without this

void setup() {
  #ifdef WINC_EN
    pinMode(WINC_EN, OUTPUT);
    digitalWrite(WINC_EN, HIGH);
  #endif

//setup LEDs
  pinMode(ledPin1,   OUTPUT);
  pinMode(ledPin2,   OUTPUT);
  pinMode(ledPin3,   OUTPUT);
  pinMode(ledPin4,   OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  SetState(0);

  while (!Serial);
  Serial.begin(115200);

  Serial.println(F("Connect Go Kart To the Cloud"));
  Serial.println("");

  
    // Initialise the Client
  Serial.print(F("\nStart WiFi ..."));
 // SetState(1); //light LED
  // check for the presence of the breakout
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WINC1500 wifi device not present");
    // don't continue:
    while (true);
  }
  Serial.println("Wifi started!");
  
  mqtt.subscribe(&onoffbutton);

  //gps pin
  pinMode(ledPin, OUTPUT);       // Initialize LED pin. Do I really need this?

  //Initialize GPS
  //SetState(2); //light LED
  Serial.println("Starting GPS.");
  dgps.init();                   // Initialize dGPS.
  delay(1000);  
  Serial.println("GPS started!");
}

void loop() {
  Watchdog.reset();  // Reset watchdog every loop
  //SetState(0);       //Turn off all LEDs  
  
  MQTT_connect(); //Connect / reconnect to mqtt if needed. see function def below

  Adafruit_MQTT_Subscribe *subscription;                  //loop to wait for incoming subscription packets
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);

      if (0 == strcmp((char *)onoffbutton.lastread, "OFF")) {
        digitalWrite(buzzerPin, LOW);
      }
      if (0 == strcmp((char *)onoffbutton.lastread, "ON")) {
        digitalWrite(buzzerPin, HIGH);
      }
    }
  }

  //Update GPS data
  dgps.update(Maple_Valley[0],Maple_Valley[1]); //call update for any location
  delay(500);                                  //added this based on input from Dexter, morning of 3.3.2017. Will it work with out this or with less?
  
  SetState(0);
  
  //set temp
  float temperatureC = getTemp();               //will take about 750ms to run. there is a delay() call in getTemp() 
  float temperatureF = temperatureC*1.8 + 32;

  //Set speed
  float veldata = dgps.Vel();
  
  //set location
  //char locdata[12] = "1, 47.42, -122.03, 1";  //old dummy value
  float la_float = dgps.Lat();                  //read lat from GPS
  float lo_float = dgps.Lon();                  //read lon from GPS
  char la_char[12]="";                          //buffer to store lat
  char lo_char[12]="";                          //buffer to store lon
  char comma[1]=",";                            //need a comma in locdata
  char locdata[27]="";                          //buffer for lat + comma + lon
  dtostrf(la_float, 11, 6, la_char);            //convert la_float to string
  dtostrf(lo_float, 11, 6, lo_char);            //convert lo_float to string
  strcat(locdata, la_char);                     //add la_char to coord
  strcat(locdata, comma);                       //add comma to coord
  strcat(locdata, lo_char);                     //add lo_char to coord
 
 // Publish data
 // SetState(3);
  Serial.print(F("\nSending temperature value "));
  Serial.print(temperatureF);
  Serial.print("...");
  if (! temp.publish(temperatureF)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending speed/velocity "));
  Serial.print(veldata);
  Serial.print("...");
  if (! vel.publish(veldata)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  Serial.print(F("\nSending location "));
  Serial.print(locdata);
  Serial.print("...");
  if (! loc.publish(locdata)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  SetState(1); //light final LED, showing last data point in loop has been sent

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connect wifi to: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    uint8_t timeout = 10;
    while (timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
  }
  
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connect to Adafruit.io ... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying adafruit.io connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("Connected!");
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  
  delay(750); // Wait for temperature conversion to complete

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
 }

 //function for controlling LEDs
 void SetState(int state){
   switch(state){
     //note power LED is lit directly from bboard
     case 0: //wifi connected
      digitalWrite(ledPin1,  LOW);    
      digitalWrite(ledPin2,  LOW);
      digitalWrite(ledPin3,  LOW);    
      digitalWrite(ledPin4,  LOW);
      break;
     case 1: //wifi connected
      digitalWrite(ledPin1,  HIGH);    
      digitalWrite(ledPin2,  LOW);
      digitalWrite(ledPin3,  LOW);    
      digitalWrite(ledPin4,  LOW);
      break;
    case 2: //adafruit connected
      digitalWrite(ledPin1,  LOW);    
      digitalWrite(ledPin2,  HIGH);
      digitalWrite(ledPin3,  LOW);    
      digitalWrite(ledPin4,  LOW);
      break;
    case 3: //gps initialized
      digitalWrite(ledPin1,  LOW);    
      digitalWrite(ledPin2,  LOW);
      digitalWrite(ledPin3,  HIGH);    
      digitalWrite(ledPin4,  LOW);
      break;
    case 4: //data published
      digitalWrite(ledPin1,  LOW);    
      digitalWrite(ledPin2,  LOW);
      digitalWrite(ledPin3,  LOW);    
      digitalWrite(ledPin4,  HIGH);
      break;
}
}

