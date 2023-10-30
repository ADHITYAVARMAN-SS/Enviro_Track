#define BLYNK_TEMPLATE_ID "TMPL3fniNG7AT"
#define BLYNK_TEMPLATE_NAME "NODE1"
#define BLYNK_AUTH_TOKEN "H94NRDwtO9XDd3cAfd00IOpaAWApSvYX"

#define BLYNK_PRINT Serial
#define TINY_GSM_MODEM_SIM800


#define SIM800L_IP5306_VERSION_20190610

#include <SoftwareSerial.h>
#include "utilities.h"

#include "DHT.h"
#define DHTTYPE DHT22
#include <MQ135.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>

#define DHTPIN 15
#define PIN_MQ2 26
#define PIN_MQ135 25
#define PIN_Turbidity 27
int RXPin = 17;
int TXPin = 16;
// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);
Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

MQ135 gasSensor= MQ135(PIN_MQ135);
#define USE_WROVER_BOARD
BlynkTimer timer;

DHT dht(DHTPIN, DHTTYPE);
float t, h;
int FireThrushold=50;
int FireValue;
int AQIValue;
float pressure;
sensors_event_t event;
float px,py,pz,dx,dy,dz;

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
  h = dht.readHumidity();
t = dht.readTemperature();
Blynk.virtualWrite(V0, h);
Blynk.virtualWrite(V1, t);

FireValue=analogRead(PIN_MQ2);
int OrgFireValue=map(FireValue,0,4095,0,100);
Blynk.virtualWrite(V2, OrgFireValue);
if (OrgFireValue > FireThrushold )  
  {  
   Blynk.notify("Smoke Detected!");
     while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      if (gps.location.isValid())
  {
    float latitude=gps.location.lat();
    float longitude=gps.location.lng();

  }
  else
  {
    Serial.println("Location: Not Available");
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
  Blynk.email("uchitmodi8@gmail.com", "EARTHQUAKE DETECTED","Latitude:",latitude,"Longitude:",longitude );
  
  }

float air_quality = gasSensor.getPPM();
Blynk.virtualWrite(V3, air_quality);

pressure = bmp.readPressure();
Blynk.virtualWrite(V4,pressure/100);


int TurbidityValue = analogRead(PIN_Turbidity);
int OrgTurbidity = map(TurbidityValue, 0, 750, 100, 0);
Blynk.virtualWrite(V5, OrgTurbidity);
if (OrgTurbidity < 20) {
    Serial.print(" its CLEAR ");
    Blynk.notify(" its CLEAR ");
  }
if ((OrgTurbidity > 20) && (OrgTurbidity < 50)) {
    Serial.print(" its CLOUDY ");
    Blynk.notify(" its CLOUDY ");
  }
if (OrgTurbidity > 50) {
    Serial.print(" its DIRTY ");
    Blynk.notify(" its DIRTY ");
    }
mpu.getAccelerometerSensor()->getEvent(&event);
dx = event.acceleration.x - px;
dy = event.acceleration.y - py;
dz = event.acceleration.z - pz;

Blynk.virtualWrite(V6,dx);
Blynk.virtualWrite(V7,dy);
Blynk.virtualWrite(V8,dz);

if (dx>=1 && dy>=1 && dz>=1){
   Blynk.notify("Earthquake Detected!");
   
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      if (gps.location.isValid())
  {
    float latitude=gps.location.lat();
    float longitude=gps.location.lng();

  }
  else
  {
    Serial.println("Location: Not Available");
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    while(true);
  }
  Blynk.email("uchitmodi8@gmail.com", "EARTHQUAKE DETECTED","Latitude:",latitude,"Longitude:",longitude );
  
}

px = event.acceleration.x;
py = event.acceleration.y;
pz = event.acceleration.z;


}


void setup()
{
 
SerialMon.begin(115200);
Serial.begin(115200);
gpsSerial.begin(9600);
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
mpu.begin();
   if (!bmp.begin(0x76))
    {
      Serial.println("Check");
      while (1);
    }
dht.begin();
bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
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
