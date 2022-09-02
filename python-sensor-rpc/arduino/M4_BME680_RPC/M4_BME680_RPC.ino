#include <RPC.h>
#include <SerialRPC.h>
#include <Wire.h>

#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

void setup()
{
    pinMode(PA_12, INPUT);

    Serial.begin(115200);
    Serial.println("BME680 test on M4");
    Wire.begin();

    Serial.println("Trying to find sensor...");
    
    for (auto status = bme.begin(); !status; delay(250)) {
        Serial.println("Could not find a valid BME680 sensor, check wiring, address, sensor ID!");
    }

    // Set up oversampling and filter initialization
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms


    //RPC.bind("status", []{ return bme.sensorID() == 0x60; });
    RPC.bind("temperature", []{ return bme.temperature; });
    RPC.bind("humidity", []{ return bme.humidity; });
    RPC.bind("pressure", []{ return bme.pressure / 100.0F; });
    RPC.bind("gas", []{ return bme.gas_resistance / 1000.0; });
    RPC.bind("altitude", []{ return bme.readAltitude(SEALEVELPRESSURE_HPA); });

    Serial.println("Starting");
}

void loop()
{
   if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();

  delay(1000);
}
