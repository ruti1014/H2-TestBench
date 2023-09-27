
/*-------------------------- H2-Testbench HKA --------------------------
Description:      This File contains the main logic for controlling the testbench, the H2-Cell and the Sensors.
                  Also the communication via usb and the storage of the recorded sensor values.
Authors:          Jonas Geckle, Tim Ruf
Planned features: - sensor class containing all sensors -> easy use, modular
                  - sensor data capture using the sensor class
                  - data Storage on SD-Card
                  - data transfer via USB emulating a mass storage device
                  - small display for easy test bench usage
*/


#include <Arduino.h>

#define PIN_startHcell 4      // Gate 1 MOSFET to start-pin H-Cell (12 V)
#define PIN_cutoff 5          // Gate 2 MOSFET to cut-off and flowsensor (24 V)
#define PIN_recording 23      // recording-switch
#define PIN_leaksensor 98     // leaksensor analog in
#define PIN_flowsensor 15     // flowsensor analog in
#define PIN_emergencystop 99  // emergency-stop
#define PIN_RX 16
#define PIN_TX 17
#define PIN_start 18          // start-stop-switch

int upper_leaklimit = 25;  // todo: correct values
int lower_leaklimit = 15;


struct switches {
  bool Hcell;          // 0: off   	     | 1: on
  bool start;          // 0: not pressed | 1: pressed
  bool recording;      // 0: not pressed | 1: pressed
  bool emergencystop;  // 0: not pressed | 1: pressed
  int leakvalue;
  int flowvalue;
};

struct switches state;
HardwareSerial SerialH(2);  //Using Serial2

void setup_pins();
void setup_state();
void start_Hcell();
void stop_Hcell();
void update_state();
void requestSerialInfo();
void readSerial();

void setup() {
  Serial.begin(9600);
  Serial.println("Booting H2-Testbench");
  SerialH.begin(9600, SERIAL_8N1, PIN_RX, PIN_TX);
  setup_pins();
  setup_state();
}

void loop() {
  update_state();
  requestSerialInfo();

  //H-Cell control logic
  if (state.leakvalue >= upper_leaklimit)  // leak-value exceeds limit -> danger of explosion
  {
    stop_Hcell();
  } else  // leak-value below limit
  {
    if (state.start)  // start switch is pressed
    {
      start_Hcell();
    } else  // start switch off
    {
      stop_Hcell();
    }
  }
}

void setup_pins() {
  pinMode(PIN_startHcell, OUTPUT);
  pinMode(PIN_cutoff, OUTPUT);
  pinMode(PIN_recording, INPUT_PULLDOWN);
  pinMode(PIN_leaksensor, INPUT);
  pinMode(PIN_flowsensor, INPUT);
  pinMode(PIN_emergencystop, INPUT_PULLDOWN);
  pinMode(PIN_start, INPUT_PULLDOWN);  // pull-down --> unpressed == low

  digitalWrite(PIN_startHcell, LOW);     // to 12 V switch 
  digitalWrite(PIN_cutoff, LOW);          // to 24 V switch
}

void start_Hcell() {
  if (state.Hcell == 0 && state.leakvalue < lower_leaklimit) {
    Serial.print("TimeStamp: ");
    Serial.print(micros());
    Serial.println(": Starting Hcell! ");
    digitalWrite(PIN_cutoff, HIGH);     // open cut-off
    digitalWrite(PIN_startHcell, HIGH);  // start H-Cell
    state.Hcell = 1;
  }
}

void stop_Hcell() {
  if (state.Hcell == 1) {
    Serial.print("TimeStamp: ");
    Serial.print(micros());
    Serial.println(" stopping Hcell! ");
    digitalWrite(PIN_cutoff, LOW);       // close cut-off
    digitalWrite(PIN_startHcell, LOW);  // turn off H-Cell       !!! LOW: 12V -- HIGH: 0V
    state.Hcell = 0;
  }
}

void setup_state() {
  //initial state values
  state.Hcell = 0;
  state.start = 0;
  state.recording = 0;
  state.emergencystop = 0;
  state.leakvalue = 0;
  state.flowvalue = 0;
}

void update_state() {
  //Update button positions
  int buttonDebounceDelay = 50;  //debounce delay in ms

  if (millis() % buttonDebounceDelay == 0) {
    state.start = digitalRead(PIN_start);
    state.recording = digitalRead(PIN_recording);
    state.emergencystop = digitalRead(PIN_emergencystop);
  }

  //Update sensor values will be moved to own function!
  // state.leakvalue = analogRead(PIN_leaksensor);
  state.leakvalue = 5;  // temporary placeholder
  state.flowvalue = analogRead(PIN_flowsensor);
}

//Serial Communication
//Temporary function for testing
//this functions sends a command via Serial2 to the hcell to get basic information
//triggered via PC by the serial monitor. Enter the commands below to send request to hcell
void requestSerialInfo(){

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');

    if (cmd == "idn") {
      Serial.print("\nRetrieving Serialnumber... ");
      SerialH.print("*idn?\r\n");
    } else if (cmd == "vol") {
      Serial.print("\nVoltage (in mV): ");
      SerialH.print("*vol?\r\n");   
    } else if (cmd == "amp") {
      Serial.print("\nCurrent (in mA): ");
      SerialH.print("*cur?\r\n");
    } else if (cmd == "tmp"){
      Serial.print("\nTemperature (in °C): ");
      SerialH.print("*temp?\r\n");
    } else if (cmd == "prs"){
      Serial.print("\nPressure (in mbar): ");
      SerialH.print("*pres?\r\n");
    } else if (cmd == "flow"){
      Serial.print("\nflowvalue:");
      Serial.println(state.flowvalue);
    } else if (cmd == "err"){
      Serial.print("\nError-Code: ");
      SerialH.print("*error?\r\n");
    }
  }

  readSerial();
}

void readSerial(){
  if (SerialH.available()){
    char tmp = SerialH.read();
    Serial.print(String(tmp));
  }
}
