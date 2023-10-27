//function prototypes
//Utilities
void setup_pins();
void setupPreferences();
void resetFileIndex();
bool startHcell();
bool stopHcell();
void recording();
void buttonInterpreter(int, int);




//global variables
//flags
bool hCellState = false; //true == ON, false == OFF
bool recordingFlag = false;
bool sd_inited = false;
bool testwrite = true;      // to write the testfiles only once
int counter = 0;            // to count 20 sec for the second write-test

bool button_pressed[8] = {false, false, false, false, false, false, false, false};

uint32_t recordingFileIndex = 0;   // index for .csv-files
const char recordingKeyName[] = "recStorageKey";      // max. 15 chars

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
