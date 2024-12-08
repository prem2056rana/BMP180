#include "BMP180_ESP32.h"
#include <Wire.h>

SFE_BMP180 bmp180; // Create an instance of the BMP180 class

// Define custom pins for I2C
#define SDA_PIN 36  // Change this to your desired SDA pin
#define SCL_PIN 35  // Change this to your desired SCL pin
#define P0 1013.25 

void setup() {
  Serial.begin(115200);
  
  // Start I2C communication with custom SDA and SCL pins
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Initialize BMP180 sensor
  if (!bmp180.begin()) {
    Serial.println("Failed to initialize BMP180 sensor");
    while (1);  // Stop execution if sensor is not found
  }
  
  Serial.println("BMP180 sensor initialized successfully.");
}

void loop() {
  double temperature, pressure, Altitude;
  
  // Start temperature reading
  if (bmp180.startTemperature()) {
    delay(5); // Wait for the temperature reading to complete
    
    // Get the temperature value
    if (bmp180.getTemperature(temperature)) {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");
    } else {
      Serial.println("Error reading temperature");
    }
  }

  // Start pressure reading (using oversampling = 3 for higher resolution)
  if (bmp180.startPressure(3)) {
    delay(26); // Wait for the pressure reading to complete

    // Get the pressure value
    if (bmp180.getPressure(pressure, temperature)) {
      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println(" mb");
    } else {
      Serial.println("Error reading pressure");
    }
  }
  Altitude = bmp180.altitude(pressure, P0); // Store result in 'altitude' variable
  
  // Check if altitude calculation is valid and print the result
  if (Altitude != 0) {
    Serial.print("Current Altitude: ");
    Serial.print(Altitude);
    Serial.println(" m");
  } else {
    Serial.println("Error reading altitude");
  }

  delay(1000); // Wait before next reading
}
