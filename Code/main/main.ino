#include <Arduino.h>

#define PIN_startHcell 4      // Gate 1 MOSFET to start-pin H-Cell (12 V)
#define PIN_cutoff 5          // Gate 2 MOSFET to cut-off and flowsensor (24 V)
#define PIN_recording 8       // recording-switch
#define PIN_leaksensor 15     // leaksensor analog in
#define PIN_flowsensor 16     // flowsensor analog in
#define PIN_emergencystop 17  // emergency-stop
#define PIN_start 18          // start-stop-switch

int upper_leaklimit = 25;       // todo: correct values
int lower_leaklimit = 15;  

struct switches{
  bool Hcell;             // 0: off   	    | 1: on
  bool start;             // 0: not pressed | 1: pressed
  bool recording;         // 0: not pressed | 1: pressed
  bool emergencystop;     // 0: not pressed | 1: pressed
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

  setup_pins();
  setup_state();
  attachInterrupt(PIN_emergencystop, emergencystopISR, RISING);      // Triggers the interrupt when the pin goes from LOW to HIGH  
}

void loop() {

  update_state();

  if(state.leakvalue >= upper_leaklimit)       // leak-value exceeds limit
  {
    stop_Hcell();
  }
  else      // leak-value below limit
  {
    if(state.start)       // start switch is pressed
    {
      start_Hcell();
    }
    else            // start switch off
    {
      stop_Hcell();
    }
  }

}

void setup_pins() {
  pinMode(PIN_startHcell, OUTPUT);
  pinMode(PIN_cutoff, OUTPUT);
  pinMode(PIN_recording, INPUT);
  pinMode(PIN_leaksensor, INPUT);
  pinMode(PIN_flowsensor, INPUT);
  pinMode(PIN_emergencystop, INPUT);
  pinMode(PIN_start, INPUT);
}

void start_Hcell() {
  if(state.Hcell == 0 && state.leakvalue < lower_leaklimit)
  {
    digitalWrite(PIN_cutoff, HIGH);         // open cut-off
    digitalWrite(PIN_startHcell, HIGH);     // start H-Cell
    state.Hcell = 1;
  }
}

void stop_Hcell() {
  if(state.Hcell == 1)
  {
    digitalWrite(PIN_cutoff, LOW);        // close cut-off
    digitalWrite(PIN_startHcell, LOW);    // turn off H-Cell
    state.Hcell = 0;
  }
}

void setup_state()
{
  state.Hcell = 0;
  state.start = 0;
  state.recording = 0;
  state.emergencystop = 0;
  state.leakvalue = 0;
  state.flowvalue = 0;
}

void update_state() {
  switch(PIN_start)
  {
    case HIGH:
      state.start = 1;
      break;
    case LOW:
      state.start = 0;
      break;
  }

  switch(PIN_recording)
  {
    case HIGH:
      state.recording = 1;
      break;
    case LOW:
      state.recording = 0;
      break;
  }

  switch(PIN_emergencystop)
  {
    case HIGH:
      state.emergencystop = 1;
      break;
    case LOW:
      state.emergencystop = 0;
      break;
  }

  state.leakvalue = analogRead(PIN_leaksensor);
  state.flowvalue = analogRead(PIN_flowsensor);
}
