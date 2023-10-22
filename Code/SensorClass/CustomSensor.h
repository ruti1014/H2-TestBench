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
#include <Adafruit_BME280.h>



// ######### generic analog sensor #########
class AnalogSensor : public Sensor {
public:
  AnalogSensor(int pin, String name, SensorType sensorType);
  void update() override;
  double getValue();

private:
  int _pin;
};

// ######### BME280 #########
class BmeSensor : public Sensor {
public:
  BmeSensor(int address, String name);
  void update() override;
  double getValue(SensorType sensorType);
private:
  Adafruit_BME280 _bme;
};




#endif
