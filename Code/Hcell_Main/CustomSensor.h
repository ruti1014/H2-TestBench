/*  ----------- CustomSensor.h -----------
    Author: Tim Ruf
    Institution: Hochschule Karlsruhe
    Description: This File contains custom declaration of the Sensor Class

    How to add new Sensor: 
    - define a name for the sensor -> preferrably in constructor
    - every sensor needs to overwrite the getValue() and update() function.
    - define dataQuantity aka how many values can the sensor record
    - populate the _data list with a SensorData struct for every recorded value
    -> see Sensor.h for more information on the data structure
*/



#ifndef CUSTOMSENSOR_H
#define CUSTOMSENSOR_H

#include "Sensor.h"

//Dependencies
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>



// ######### generic analog sensor #########
class AnalogSensor : public Sensor {
public:
  AnalogSensor(int pin, String name, SensorType sensorType, int interval, String unit);
  void update() override;
  uint16_t getValue();

private:
  int _pin;
};

// ######### BME280 #########
class BmeSensor : public Sensor {
public:
  BmeSensor(int address, String name, TwoWire *theWire = &Wire);
  bool sensorInit() override;
  void update() override;
  uint16_t getValue(SensorType sensorType);
  bool isAvailable();
private:
  Adafruit_BME280 _bme;
  int _address = 0;
  TwoWire* _theWire;
  bool _bme_available = false;
};

// ######### HcellRS232 #########
class Hcell_RS232 : public Sensor {
public:
  Hcell_RS232(HardwareSerial* serial, String name);
  void update() override;
  uint16_t getValue(SensorType type);
private:
  HardwareSerial* _hcellSerial;
};



#endif
