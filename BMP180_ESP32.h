#ifndef BMP180_ESP32_H
#define BMP180_ESP32_H

#include <Arduino.h>
#include <Wire.h>

// Default I2C address for BMP180
#define BMP180_I2C_ADDRESS 0x77

// BMP180 Registers
#define BMP180_REG_CALIBRATION 0xAA // Calibration registers start here
#define BMP180_REG_CHIP_ID 0xD0     // Chip ID register
#define BMP180_REG_CONTROL 0xF4     // Control register
#define BMP180_REG_RESULT 0xF6      // Data registers start here

// Commands
#define BMP180_CMD_TEMP 0x2E        // Temperature measurement command
#define BMP180_CMD_PRESS 0x34       // Pressure measurement command

class BMP180_ESP32 {
public:
    BMP180_ESP32(TwoWire &i2c = Wire); // Constructor with I2C instance

    bool begin();                // Initialize the sensor
    float readTemperature();     // Read temperature in Celsius
    float readPressure();        // Read pressure in hPa
    float readAltitude(float seaLevelPressure = 1013.25); // Read altitude

private:
    TwoWire *_i2c;               // Pointer to I2C instance
    int16_t readInt(uint8_t reg); // Read 16-bit integer from a register
    void writeByte(uint8_t reg, uint8_t value); // Write a byte to a register
    uint16_t readRawTemperature(); // Read raw temperature data
    uint32_t readRawPressure();    // Read raw pressure data

    // Calibration coefficients
    int16_t AC1, AC2, AC3, B1, B2, MB, MC, MD;
    uint16_t AC4, AC5, AC6;
    int32_t B5;
};

#endif
