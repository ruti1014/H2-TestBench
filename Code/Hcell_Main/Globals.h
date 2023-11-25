/*  ----------- Globals.h -----------
*   Authors: Tim Ruf, Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: contains function prototypes (defined in Utilities.h) and global flags, variables and objects
*/

//function prototypes (Utilities.h)
void setup_pins();
void setupPreferences();
void setupSPI();
void setupDisplay();
void setupSensors();
void setupI2C();
void setupGui();
void resetFileIndex();
void updateSensorArray();
void startHcell();
void stopHcell();
void checkHlimits();
void recording();
void buttonInterpreter(int, int);
void addDataToBuffer();
void requestSerialInfo();
void readSerial();
void updateDisplay();
void updateStatusBar();
void updateValues();
void checkSD();
void loopTimeMS();

//global flags, variables and objects
// ######## flags ################################
bool hCellState = false;  //true == ON, false == OFF
bool recordingFlag = false;
bool appendBufferFlag = false;
bool sd_inited = false;
bool sd_busy = false;
bool onReadFlag = false;

// ######## variables ################################
int sd_timeout = 0;

uint32_t recordingFileIndex = 0;                  // index for .csv-files
const char recordingKeyName[] = "recStorageKey";  // max. 15 chars

uint16_t SensorBuffer[numData][sensorBufferSize];
int bufferIndex = 0;

// ######## objects ################################
Preferences preferences;
HardwareSerial SerialHCELL(2);  //Using Serial2
SPIClass* Display_SPI = NULL;
SPIClass* MicroSD_SPI = NULL;
TwoWire I2C = TwoWire(0);

// TFT_22_ILI9225 tft_ili = TFT_22_ILI9225(TFT_RST, SPI_MISO, Display_SPI_CS, Display_LED);
TFT_22_ILI9225 tft_ili = TFT_22_ILI9225(TFT_RST, TFT_RS, Display_SPI_CS, Display_LED);

//create sensor objects
AnalogSensor h2flow(PIN_flowsensor, "h2flow", SENS_H2FLOW, 1, "ml/s");
AnalogSensor h2leak(PIN_leaksensor, "h2leak", SENS_H2LEAK, 1, "ppm");
BmeSensor bme1(0x76, "bme1", &I2C);
BmeSensor bme2(0x77, "bme2", &I2C);
Hcell_RS232 hcell(&SerialHCELL, "H2_Cell");

//create sensor array
SensorArray sensorArray(numSensors, numData);  //Amount of sensors, amount of captuered data
