
/*-------------------------- H2-Testbench HKA --------------------------
Description:      This File contains the main logic for controlling the testbench, the H2-Cell and the Sensors.
                  Also the communication via usb and the storage of the recorded sensor values.
Authors:          Jonas Geckle, Tim Ruf
Last edited:      06.08.2023
Planned features: - sensor class containing all sensors -> easy use, modular
                  - sensor data capture using the sensor class
                  - data Storage on SD-Card
                  - data transfer via USB emulating a mass storage device
                  - small display for easy test bench usage
*/


#include <Arduino.h>

#define PIN_startHcell 4      // Gate 1 MOSFET to start-pin H-Cell (12 V)
#define PIN_cutoff 5          // Gate 2 MOSFET to cut-off and flowsensor (24 V)
#define PIN_recording 18      // recording-switch
#define PIN_leaksensor 15     // leaksensor analog in
#define PIN_flowsensor 16     // flowsensor analog in
#define PIN_emergencystop 17  // emergency-stop
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

void setup_pins();
void setup_state();
void start_Hcell();
void stop_Hcell();
void update_state();

void setup() {
  Serial.begin(9600);
  Serial.println("Booting H2-Testbench");
  setup_pins();
  setup_state();
}

void loop() {
  update_state();


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
}

void start_Hcell() {
  if (state.Hcell == 0 && state.leakvalue < lower_leaklimit) {
    Serial.print("TimeStamp: ");
    Serial.print(micros());
    Serial.println(": Starting Hcell! ");
    digitalWrite(PIN_cutoff, HIGH);      // open cut-off
    digitalWrite(PIN_startHcell, HIGH);  // start H-Cell
    state.Hcell = 1;
  }
}

void stop_Hcell() {
  if (state.Hcell == 1) {
    Serial.print("TimeStamp: ");
    Serial.print(micros());
    Serial.println(" stopping Hcell! ");
    digitalWrite(PIN_cutoff, LOW);      // close cut-off
    digitalWrite(PIN_startHcell, LOW);  // turn off H-Cell
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
  state.leakvalue = 5;  // temp. placeholder
  state.flowvalue = analogRead(PIN_flowsensor);
}