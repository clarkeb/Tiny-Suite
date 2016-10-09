
float sensor_volt; 
float RS_air; //  Get the value of RS via in a clear air
float R0;  // Get the value of R0 via in H2
float sensorValue;
float ratio;

void setup() {
  for(int x = 0 ; x < 100 ; x++)
    {
      sensorValue = sensorValue + analogRead(A0);
    }
  sensorValue = sensorValue/100.0;
  /*
    -----------------------------------------------
  */
  sensor_volt = sensorValue/1024*5.0;
  RS_air = (5.0 - sensor_volt)/sensor_volt; //omit *RL
  R0 = RS_air/10.0; // The ratio of RS/R0 is 10 in a clear air
}

void loop() {
  
  sensorValue = analogRead(A0);
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
  
  Particle.publish("sensor voltage", String(sensor_volt)+ " v");
  delay(2000);
  Particle.publish("RS ratio", String(RS_gas));
  delay(2000);
  Particle.publish("RS/R0", String(ratio));
}
