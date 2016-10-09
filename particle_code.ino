// particle library includes
#include "Ubidots/Ubidots.h"
#include "Adafruit_DHT/Adafruit_DHT.h"

// DHT parameters (temp humidity sensor)
#define DHTTYPE DHT22

// Ubidots parameters 
#define UBIDOTS_TOKEN "Enter_Ubidots_Token_Here"
#define UBIDOTS_DATASOURCE_NAME "Tiny Suite"

// Variables

bool home = true;
int testvar;

// DHT variables (temp humidity sensor)
int temperature;
int humidity;
int light;

// Pins
int dht_sensor_pin = 5;         // input pin for DHT sensor
int light_sensor_pin = A0;      // input pin for light sensor
int gas_sensor_pin = D0;        // input pin for gas sensor
int motion_sensor_pin = D2;     // input pin for PIR sensor
int motion_led_pin = D3;        // LED output pin

// gas sensor variables
float sensor_volt;              //voltage of gas sensor 
float RS_air;                   //  value of gas sensor in air (startup calibration)
float R0;                       // baseline sensor value
float sensorValue;              // input from gas sensor
float RS_gas;                   // value of gas sensor in gas (testing for gasses)
float ratio;                    // ratio of gas to baseline value.
                                // carbon monoxide between 1.5 and 5, propane between 0.2 and 1.5, above 5 is good, 10 is value for clean air.  
String airquality;              // String giving air quality indication good/bad

// motion sensor variables
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
int motionDetected = 0;         // 0 if no motion, 1 if detected motion

int calibrateTime = 10000;      // wait for the sensor to calibrate

//init classes.
DHT dht(dht_sensor_pin, DHTTYPE);
Ubidots ubidots(UBIDOTS_TOKEN);

void setup() {
  ubidots.setDatasourceName(UBIDOTS_DATASOURCE_NAME);  
  // Start DHT sensor
  dht.begin();
  //calibrate gas sensor  
  for(int i = 0 ; i < 100 ; i++)
    {
      sensorValue = sensorValue + analogRead(gas_sensor_pin);
    }
  sensorValue = sensorValue/100.0;
  sensor_volt = sensorValue/1024*5.0;
  RS_air = (5.0 - sensor_volt)/sensor_volt;
  R0 = RS_air/10.0; // The ratio of RS/R0 is 10 in clear air
  
  //init motion sensor pin.
  pinMode(motion_led_pin, OUTPUT);
  pinMode(motion_sensor_pin, INPUT);
  
  //init particle functions.
  Particle.function("home status", changeHomeStatus);
  Particle.function("edit variable", changeVariable);
  Particle.variable("testvar", testvar);
}

void loop() {
  
  // Get measurements.

  // Temperature measurement
  temperature = dht.getTempFarenheit();
  // Humidity measurement
  humidity = dht.getHumidity();
  // Light level measurement
  float light_measurement = analogRead(light_sensor_pin);
  light = (int)(light_measurement/4096*100);
  // Gas level measurement
  sensorValue = analogRead(gas_sensor_pin);
  sensor_volt=(float)sensorValue/1024*5.0;
  RS_gas = (5.0 - sensor_volt)/sensor_volt; // omit *RL
  ratio = RS_gas/R0;
  // Motion sensor measurement
  if (calibrated()) {
    val = digitalRead(motion_sensor_pin);
  }
  
  //process measurements.
  
  // Gas level processing
  if (ratio >= 5.0) {
    airquality = "Good";
  } else {
    airquality = "Unhealthy";
  }
  // Motion sensor processing
  if (val == HIGH) {
    motionDetected = 1;
    if (pirState == LOW) {
      pirState = HIGH;
      setLED(pirState);
    }
  } else {
    motionDetected = 0;
    if (pirState == HIGH) {
      pirState = LOW;
      setLED(pirState);
    }
  }

  // Add all the ubidots info we will be publishing and then send.
  ubidots.add("temperature", temperature);
  ubidots.add("humidity", humidity);
  ubidots.add("light", light);
  ubidots.add("motion", motionDetected);
  ubidots.add("gas vs. air ratio", ratio); 
  // carbon monoxide between 1.5 and 5, propane between 0.2 and 1.5, above 5 is good
  ubidots.sendAll();
  
  // Publish data through particle's publish
  Particle.publish("temperature", String(temperature) + " °F");
  delay(1000);
  Particle.publish("humidity", String(humidity) + "%");
  delay(1000);
  Particle.publish("light", String(light) + "%");
  delay(1000);
  Particle.publish("air quality",String(airquality));
  delay(1000);
  if (!home) {
    Particle.publish("motion", String(motionDetected));
    delay(1000);
  }
  Particle.publish("gas vs. air ratio", String(ratio));
}

bool calibrated() {
  return millis() - calibrateTime > 0;
}

void setLED( int state )
{
  digitalWrite( motion_led_pin, state );
}

int changeHomeStatus (String command) {
  if (command == "home") {
    home = true;
    return 1;
  } else if (command == "away") {
    home = false;
    return 2;
  } else {
    return -1;
  }
}

/// takes a command string of the form 'variable=value' and sets the variable to the value.
int changeVariable (String command) {
  int indexvar = command.indexOf("=", 0);
  if (indexvar > 0) {
    // found a =
    String varstr = command.substring(0, indexvar-1);
    String varval = command.substring(indexvar+1);
    varstr = varstr.toLowerCase();
    if (varstr.equals("testvar")) {
      testvar = varval.toInt();
    }
  } else {
    varval = -1;
  }
  return 1;
}
