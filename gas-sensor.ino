
float sensor_volt; 
float RS_air; //  Get the value of RS via in a clear air
float R0;  // Get the value of R0 via in H2
float sensorValue;

void setup() {
}

void loop() {
    
    /*
    ---
    Get a average data by testing 100 times 
    ---
    */   
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

    
    Particle.publish("voltage", String(sensor_volt) + " v");
    Particle.publish("r0", String(R0));
    
    delay(1000);
}