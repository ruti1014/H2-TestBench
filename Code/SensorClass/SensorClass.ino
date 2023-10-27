

#include <Adafruit_BME280.h>
#include "Sensor.h"
#include "CustomSensor.h"


//create sensor objects
AnalogSensor h2flow(19, "h2flow", SENS_H2FLOW);
AnalogSensor h2leak(19, "h2leak", SENS_H2LEAK);
AnalogSensor rndhum(19, "rndhum", SENS_HUM);
BmeSensor bme1(0x76, "bme1");

//create sensor array
SensorArray sensorArray(4, 6);  //Amount of sensors, amount of captuered data

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  Serial.println(sizeof(uint8_t));
  Serial.println(sizeof(SensorData));

  sensorArray.addSensor(&h2flow);
  sensorArray.addSensor(&h2leak);
  sensorArray.addSensor(&rndhum);


  sensorArray.addSensor(&bme1);
  Serial.println("Active sensors: ");

  String tmp = sensorArray.getSensorList();
  Serial.println(tmp);

  // uint16_t tmpData;
  // for(int i=0; i<sensorArray.getDataQuantity(); i++){
  //   tmpData = sensorArray.getData(i).value;
  // }
}

void loop() {}