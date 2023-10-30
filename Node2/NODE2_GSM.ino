#define BLYNK_TEMPLATE_ID "TMPL3ROAZLSjQ"
#define BLYNK_TEMPLATE_NAME "NODE2"
#define BLYNK_AUTH_TOKEN "NyZr9ajLekJSDuRItg1b36wWzlLNs9fG"

#define BLYNK_PRINT Serial

#define TINY_GSM_MODEM_SIM800


#define SIM800L_IP5306_VERSION_20190610


#include "utilities.h"


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>



Adafruit_MPU6050 mpu;
#define PIN_Soil 15

#define USE_WROVER_BOARD
BlynkTimer timer;

sensors_event_t event;
float px,py,pz,dx,dy,dz;
const int dry = 3312 ; // value for dry sensor
const int wet = 2500 ; // value for wet sensor
int SoilValue;
int SoilMoisture;

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1


#define BMP280_I2C_ADDRESS  0x76
// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
const char apn[] = "airtelgprs.com";  // search for apn of your network provider on Google *
const char gprsUser[] = "";
const char gprsPass[] = "";


#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);


const char auth[] = BLYNK_AUTH_TOKEN; 



void sendSensor()
{
mpu.getAccelerometerSensor()->getEvent(&event);
dx = event.acceleration.x - px;
dy = event.acceleration.y - py;
dz = event.acceleration.z - pz;

Blynk.virtualWrite(V0,dx);
Blynk.virtualWrite(V1,dy);
Blynk.virtualWrite(V2,dz);

px = event.acceleration.x;
py = event.acceleration.y;
pz = event.acceleration.z;

if (dx>=1 && dy>=1 && dz>=1){
   Blynk.notify("Earthquake Detected!");
}


Blynk.virtualWrite(V3,100);
SoilValue=analogRead(PIN_Soil);
int SoilMoisture=map(SoilValue,0,4095,0,100);
Blynk.virtualWrite(V3, SoilMoisture);
if (SoilMoisture < 50 )  
  {  
   Blynk.notify("Moisture Level in soil is less!");

  }

}


void setup()
{
 
SerialMon.begin(115200);
Serial.begin(115200);
setupModem();
SerialMon.println("Wait...");
  // Set GSM module baud rate and UART pins
SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(6000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
SerialMon.println("Initializing modem...");
  modem.restart();
  // modem.init();

String modemInfo = modem.getModemInfo();
SerialMon.print("Modem Info: ");
SerialMon.println(modemInfo);
Blynk.begin(auth, modem, apn, gprsUser, gprsPass);


delay(500);
mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
mpu.setGyroRange(MPU6050_RANGE_500_DEG);
mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
mpu.getAccelerometerSensor()->getEvent(&event);
px = event.acceleration.x;
py = event.acceleration.y;
pz = event.acceleration.z;

delay(2000);

timer.setInterval(1000L, sendSensor);


}

void loop()
{
  Blynk.run();
  timer.run();

}

