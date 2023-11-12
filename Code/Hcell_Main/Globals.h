#include "esp32-hal-timer.h"
//function prototypes
//Utilities
void setup_pins();
void setupPreferences();
void setupTimer();
void resetFileIndex();
void startHcell();
void stopHcell();
void recording();
void buttonInterpreter(int, int);
void addDataToBuffer();
void loopTimeMS();
void IRAM_ATTR onTimer();

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

int8_t restartCounter = 0;
const char restartKeyName[] = "restStorageKey";

uint16_t SensorBuffer[numData][sensorBufferSize];
int bufferIndex = 0;

// TO-DO: maybe implement updateIntervall into SensorClass ?
int updateIntervall = 100;    // sensor update intervall in ms

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
hw_timer_t *isr_timer = NULL;
SPIClass* Display_SPI = NULL;
SPIClass* MicroSD_SPI = NULL;
TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, SPI_MISO, Display_SPI_CS, Display_LED);

//create sensor objects
AnalogSensor h2flow(PIN_flowsensor, "h2flow", SENS_H2FLOW);
AnalogSensor h2leak(PIN_leaksensor, "h2leak", SENS_H2LEAK);
BmeSensor bme1(0x76, "bme1");
BmeSensor bme2(0x77, "bme2");

//create sensor array
SensorArray sensorArray(4, 6);  //Amount of sensors, amount of captuered data
