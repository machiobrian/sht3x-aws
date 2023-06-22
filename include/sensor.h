#include <Wire.h>
#include <Adafruit_SHT31.h>
#include "SPI.h"

bool enableHeater = false;
uint8_t loopCount = 0;
Adafruit_SHT31 sht31 = Adafruit_SHT31(); // Create sensor object

void sensorSetup(){
    // Serial.begin(115200); // initialize only a single instance in main.cpp 
    
    Serial.println("SHT31 t&h test");
    if (!sht31.begin(0x44)) {
        Serial.println("Couldn't find SHT31");
        while(1) delay(1);
    }
    Serial.print("Heater Enabled State");
    if(sht31.isHeaterEnabled()){
        Serial.println("Enabled");
    }else{Serial.println("Disabled");}
}

void sensorLoop(){
    extern float t;
    t = sht31.readTemperature(); // read temperature in degrees C (float) 
    extern float h;
    h = sht31.readHumidity(); // read humidity in % (float)

    if(!isnan(t) && !isnan(h)){//if both readings are not NaN
    // Print the readings
    Serial.print("Temperature = "); Serial.print(t); Serial.print(" C");
    Serial.print(" Humidity = "); Serial.print(h); Serial.println("%");
    }
    delay(1000);

    // we are going to toggle heater mode every 30 seconds: 
    // advantage of the "Heater Mode" is that changes are noted to the -3.0*C
    if(loopCount >= 30){
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("Heater Enabled State: ");
    
    if(sht31.isHeaterEnabled())
        Serial.println("Enabled");
    else
        Serial.println("Disabled");
    loopCount = 0;
    }
    loopCount++; // increment loop count
}