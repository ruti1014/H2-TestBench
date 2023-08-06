#ifndef SENSOR_H
#define SENSOR_H

//Dependencies
#include <Adafruit_BME280.h>



//Sensor enum type
enum SensorType{
  SENS_TEMP,
  SENS_HUM,
  SENS_PRESSURE,
  SENS_H2FLOW_A,
  SENS_H2LEAK_A,
  SENS_CELLSTACKVOL,
  SENS_CELLSTACKAMP,
  SENS_CELLSTACKTEMP
};

//Sensor data Struct
struct SensorData{
  String name;
  SensorType type;
  double value;
};


//General Sensor class
class Sensor {
public:
  Sensor(int dataQuantity = 1); //how many values is the sensor capturing e.g. bme280 hum/pres/tmep -> dataQuantity = 3;
  virtual double getValue() = 0;
  virtual double readValue() = 0;
  int getDataQuantity();
  void getSensorData(SensorData* dataPtr, int index = 0);
protected:
  int _dataQuantity;
  SensorData* _data;
};


//Specific Sensor class
class BmeSensor : public Sensor {
public:
  BmeSensor(int address, String name);
  double getValue() override;
  double readValue() override;
  double updateValue(double val);
private:
  Adafruit_BME280 _bme;
};



class AnalogSensor : public Sensor {
public:
  AnalogSensor(int pin);
  double getValue() override;
  double readValue() override;
  double updateValue(double val);

private:
  double _value;
  int _pin;
};



//sensorarray class combining all sensors for easy and generalized sensor control 
//Class has to be initialized with the amount of sensors AND the amount of values it has to manage e.g. 1: Analog(1 value) 2: BME280(3 Values) -> 2 Sensors, 4 Values
class SensorArray {
public:
  SensorArray(int sensorAmount, int dataAmount);
  bool addSensor(Sensor* sensor);
  void updateSensorValues();
  double returnSensorValue();

private:
  Sensor** _sensorList;
  SensorData* _dataList;
  int _sensorIndex;
  int _dataIndex;
  int _sensorAmount;
  int _dataAmount;

};


#endif