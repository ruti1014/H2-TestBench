/*  ----------- Sensor.h -----------
    Author: Tim Ruf
    Institution: Hochschule Karlsruhe
    Description: This File contains the declaration of the Sensor Class and the SensorArray Class
*/

#ifndef SENSOR_H
#define SENSOR_H

//Dependencies
#include <Adafruit_BME280.h>

//Sensor enum type
enum SensorType {
  SENS_TEMP,
  SENS_HUM,
  SENS_PRESSURE,
  SENS_H2FLOW,
  SENS_H2LEAK,
  SENS_CURRENT,
  SENS_VOLTAGE,
  SENS_SERIALID,
  SENS_ERRCODE
};

//Sensor data Struct
struct SensorData {
  String sensorName;
  String unit;
  SensorType type;
  uint16_t value;
  int interval; //interval in which the sensor is read
};


//General Sensor class
class Sensor {
public:
  Sensor(String name, int dataQuantity);  //how many values is the sensor capturing e.g. bme280 hum/pres/tmep -> dataQuantity = 3;
  virtual void update() = 0;
  int getDataQuantity();
  SensorData* getSensorData(int index = 0);
  String getSensorName();
protected:
  String _sensorName;
  int _dataQuantity;
  SensorData* _data;
};


//Sensorarray class combining all sensors for easy and generalized sensor control
//Class has to be initialized with the amount of sensors AND the amount of values it has to manage e.g. 1: Analog(1 value) 2: BME280(3 Values) -> 2 Sensors, 4 Values
class SensorArray {
public:
  SensorArray(int sensorAmount, int dataAmount);
  bool addSensor(Sensor* sensor);
  void updateSensorValues();
  String getSensorList();
  SensorData* getData(int i);

private:
  Sensor** _sensorList;
  SensorData* _dataList;
  int _sensorIndex;
  int _dataIndex;
  int _sensorAmount;
  int _dataAmount;
};


#endif