


void setup_pins() {
  pinMode(PIN_startHcell, OUTPUT);
  pinMode(PIN_cutoff, OUTPUT);
  pinMode(PIN_recording, INPUT_PULLDOWN);
  pinMode(PIN_leaksensor, INPUT);
  pinMode(PIN_flowsensor, INPUT);
  pinMode(PIN_start, INPUT_PULLDOWN);  // pull-down --> unpressed == low

  digitalWrite(PIN_startHcell, LOW);  // to 12 V switch
  digitalWrite(PIN_cutoff, LOW);      // to 24 V switch

  initMultiplexer();
}

void setupPreferences(){
  // permanent storage of recordingFileIndex
  preferences.begin("permaStorage", false);  // false --> read/write       true --> read-only


  recordingFileIndex = preferences.getUInt(recordingKeyName, 0);  // get previous FileIndex (returns 0 if key doesn't exist)

  if (recordingFileIndex == 0) {
    recordingFileIndex = 1;
    preferences.putUInt(recordingKeyName, recordingFileIndex);
  } else if (recordingFileIndex == 4294967295) {
    resetFileIndex();
  }

}

void resetFileIndex() {
  HWSerial.println("Resetting fileIndex...");
  recordingFileIndex = 1;
  preferences.putUInt(recordingKeyName, recordingFileIndex);
  HWSerial.print("new fileIndex: ");
  HWSerial.println(preferences.getUInt(recordingKeyName, 0));
}

bool startHcell() {
  //TO-DO implement leaksensor limits
  if (!hCellState) {
    HWSerial.println("Starting Hcell! ");
    digitalWrite(PIN_cutoff, HIGH);      // open cut-off
    digitalWrite(PIN_startHcell, HIGH);  // start H-Cell
    hCellState = true;
  }
}

bool stopHcell() {
  if (hCellState) {
    HWSerial.println("Stopping Hcell! ");
    digitalWrite(PIN_cutoff, LOW);      // close cut-off
    digitalWrite(PIN_startHcell, LOW);  // turn off H-Cell
    hCellState = false;
  }
}

void recording() {
  static bool lastState = recordingFlag;

  if (recordingFlag) {
    if (lastState != recordingFlag) {
      connectMS(false);  //disconnect MS to prevent data corruption
      createCSV(SD, &recordingFileIndex);
    }
    // append new data if available
    if (appendBufferFlag){
      appendCSV(SD, &recordingFileIndex, sensorBufferSize);
      appendBufferFlag = false;
    }
  } else {
    if (lastState != recordingFlag) {
      connectMS(true);  //connect MS to transfer recorded data
    }
  }
  lastState = recordingFlag;
}

void buttonInterpreter(int button, int value) {
  //value == 0 button release
  //value == 1 button pressed
  switch (button) {
    case 0:  //hcell start == 1, stop == 0
      if (value == 1) startHcell();
      else stopHcell();
      break;
    case 1:  //recording start == 1, stop == 0
      if (value == 1) recordingFlag = true;
      else recordingFlag = false;
      break;
    case 2:  //OK
      if (value == 1) dPad.ok = true;
      else dPad.ok = false;
      break;
    case 3:  //left
      if (value == 1) dPad.left = true;
      else dPad.left = false;
      break;
    case 4:  //right
      if (value == 1) dPad.right = true;
      else dPad.right = false;
      break;
    case 5:  //up
      if (value == 1) dPad.up = true;
      else dPad.up = false;
      break;
    case 6:  //down
      if (value == 1) dPad.down = true;
      else dPad.down = false;
      break;
    default:
      break;
  }
}

void addDataToBuffer(){
  static int bufferIndex = 0;
  static uint16_t tmpSensorData = 0;

  for(int i=0; i<numData; i++){
    tmpSensorData = sensorArray.getData(i)->value;
    SensorBuffer[i][bufferIndex] = tmpSensorData;
  }

  bufferIndex++;
  if(bufferIndex == sensorBufferSize){
    bufferIndex = 0;                // wrap around
    appendBufferFlag = true;
  }
}