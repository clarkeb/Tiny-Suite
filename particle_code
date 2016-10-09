// This #include statement was automatically added by the Particle IDE.
#include "Ubidots/Ubidots.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"
#include "ADAFRUIT_DH.h"

// DHT parameters
#define DHTPIN 5
#define DHTTYPE DHT22

// Ubidots parameters
#define UBIDOTS_TOKEN "Enter_Ubidots_Token_Here"
#define UBIDOTS_DATASOURCE_NAME "Tiny Suite"

// Variables
int temperature;
int humidity;
int light;
// Pins
int light_sensor_pin = A0;

float sensor_volt; 
float RS_air; //  Get the value of RS via in a clear air
float R0;  // Get the value of R0 via in H2
float sensorValue;
float RS_gas;
float ratio;
String airquality;

// motion sensor
int inputPin = D2;              // choose the input pin (for PIR sensor)
int ledPin = D3;                // LED Pin
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int motionDetected = 0;         // 0 if no motion, 1 if detected motion

int calibrateTime = 10000;      // wait for the thingy to calibrate

// DHT sensor
DHT dht(DHTPIN, DHTTYPE);
Ubidots ubidots(UBIDOTS_TOKEN);

void setup() {
    
    ubidots.setDatasourceName(UBIDOTS_DATASOURCE_NAME);
    
    // Start DHT sensor
    dht.begin();
    
    for(int x = 0 ; x < 100 ; x++)
    {
      sensorValue = sensorValue + analogRead(D0);
    }
  sensorValue = sensorValue/100.0;
  /*
    -----------------------------------------------
  */
  sensor_volt = sensorValue/1024*5.0;
  RS_air = (5.0 - sensor_volt)/sensor_volt; //omit *RL
  R0 = RS_air/10.0; // The ratio of RS/R0 is 10 in a clear air
  
  pinMode( ledPin, OUTPUT );
  pinMode(inputPin, INPUT);     // declare sensor as input
}
void loop() {
    
    // Humidity measurement
    temperature = dht.getTempFarenheit();
    // temperature = 10;
    
    // Humidity measurement
    humidity = dht.getHumidity();
    
    // Light level measurement
    float light_measurement = analogRead(light_sensor_pin);
    light = (int)(light_measurement/4096*100);
    
    // Publish data
    Particle.publish("temperature", String(temperature) + " °F");
    delay(1000);
    Particle.publish("humidity", String(humidity) + "%");
    delay(1000);
    Particle.publish("light", String(light) + "%");
    delay(1000);
    
     sensorValue = analogRead(D0);
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0 - sensor_volt)/sensor_volt; // omit *RL
  /*
    -
    Replace the name "R0" with the value of R0 in the demo of First Test 
    -
  */
  ratio = RS_gas/R0;  // ratio = RS/R0 
  /*
    ------------------------------
    -----------------------------------------
  */
  //https://build.particle.io/build/57f984d70280dde53900063d#verify
  //Particle.publish("sensor voltage", String(sensor_volt)+ " v");
  //delay(1000);
  //Particle.publish("RS ratio", String(RS_gas));
  //delay(1000);
  //Particle.publish("gas vs. air ratio RS/R0", String(ratio));
  if (ratio >= 5) {
      airquality = "good";
  Particle.publish("air quality",String(airquality));
  }
  else 
  {
      airquality = "unhealthy";
    Particle.publish("air quality",String(airquality));
  }
  ubidots.add("gas vs. air ratio", ratio); // carbon monoxide between 1.5 and 5, propane between 0.2 and 1.5, above 5 is good
  ubidots.sendAll();
  // if the sensor is calibrated
  if ( calibrated() )
  {
  // get the data from the sensor
    readTheSensor();

    // report it out, if the state has changed
    reportTheData();
  }
  
  ubidots.add("temperature", temperature);
  ubidots.add("humidity", humidity);
  ubidots.add("light", light);
  ubidots.sendAll();
}

void readTheSensor() {
  val = digitalRead(inputPin);
}

bool calibrated() {
  return millis() - calibrateTime > 0;
}

void reportTheData() {

  // if the sensor reads high
  // or there is now motion
  if (val == HIGH) {
    motionDetected = 1;
    Particle.publish("motion", String(motionDetected));
    ubidots.add("motion", motionDetected);
    ubidots.sendAll();
    // the current state is no motion
    // i.e. it's just changed
    // announce this change by publishing an eent
    if (pirState == LOW) {
      // we have just turned on
      // Update the current state
      pirState = HIGH;
      setLED( pirState );
    }
  } else {
      motionDetected = 0;
      Particle.publish("motion", String(motionDetected));
    ubidots.add("motion", motionDetected);
    ubidots.sendAll();
    if (pirState == HIGH) {
      // we have just turned of
      // Update the current state
      pirState = LOW;
      setLED( pirState );
    }
  }
}

void setLED( int state )
{
  digitalWrite( ledPin, state );
}
