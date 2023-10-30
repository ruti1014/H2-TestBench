//function prototypes
//Utilities
void setup_pins();
void setupPreferences();
void resetFileIndex();
bool startHcell();
bool stopHcell();
void recording();
void buttonInterpreter(int, int);
void addDataToBuffer();

// enum SensorType;
// struct SensorData;

// class Sensor;
// class SensorArray;
// class AnalogSensor;
// class BmeSensor;
// class HCell;



//global variables
//flags
bool hCellState = false; //true == ON, false == OFF
bool recordingFlag = false;
bool appendBufferFlag = false;
bool sd_inited = false;
bool testwrite = true;      // to write the testfiles only once
int counter = 0;            // to count 20 sec for the second write-test

bool button_pressed[8] = {false, false, false, false, false, false, false, false};

uint32_t recordingFileIndex = 0;   // index for .csv-files
const char recordingKeyName[] = "recStorageKey";      // max. 15 chars

uint16_t SensorBuffer[numData][sensorBufferSize];

//structs
struct directionalPad{
  bool ok = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
};

//objects
Preferences preferences;
HardwareSerial SerialHCELL(2);  //Using Serial2
struct directionalPad dPad;

//create sensor objects
AnalogSensor h2flow(PIN_flowsensor, "h2flow", SENS_H2FLOW);
AnalogSensor h2leak(PIN_leaksensor, "h2leak", SENS_H2LEAK);
BmeSensor bme1(0x76, "bme1");
BmeSensor bme2(0x77, "bme2");

//create sensor array
SensorArray sensorArray(4, 6);  //Amount of sensors, amount of captuered data
