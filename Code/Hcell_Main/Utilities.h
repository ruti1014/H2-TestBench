


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

void setupPreferences() {
  // permanent storage of recordingFileIndex
  preferences.begin("permaStorage", false);  // false --> read/write       true --> read-only


  recordingFileIndex = preferences.getUInt(recordingKeyName, 0);  // get previous FileIndex (returns 0 if key doesn't exist)
  restartCounter = preferences.getInt(restartKeyName, -1);

  if (recordingFileIndex == 0) {
    recordingFileIndex = 1;
    preferences.putUInt(recordingKeyName, recordingFileIndex);
  } else if (recordingFileIndex == 4294967295) {
    resetFileIndex();
  }

  if (restartCounter == -1) {
    restartCounter = 0;
    preferences.putInt(restartKeyName, restartCounter);
  }
}

void setupTimer() {
  isr_timer = timerBegin(0, 80, true);  //Prescaler 80 with cpu clock at 80 MHZ -> 1ms timer count
  timerAttachInterrupt(isr_timer, &onTimer, true);
  timerAlarmWrite(isr_timer, sampleRateMS, true);
  timerAlarmEnable(isr_timer);
}

void setupSPI() {
  //MicroSD SPI Class:
  MicroSD_SPI = new SPIClass(HSPI);
  MicroSD_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, MicroSD_SPI_CS);

  //Display SPI Class:
  Display_SPI = new SPIClass(HSPI);
  Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);
}

void setupDisplay() {

  tft_ili.begin(*Display_SPI);
  tft_ili.setOrientation(1);
  tft_ili.setFont(Terminal6x8);
  tft_ili.clear();
}

void setupSensors() {
  sensorArray.addSensor(&h2flow);
  sensorArray.addSensor(&h2leak);
  sensorArray.addSensor(&bme1);
  sensorArray.addSensor(&bme2);
}

void resetFileIndex() {
  HWSerial.println("Resetting fileIndex...");
  recordingFileIndex = 1;
  preferences.putUInt(recordingKeyName, recordingFileIndex);
  HWSerial.print("new fileIndex: ");
  HWSerial.println(preferences.getUInt(recordingKeyName, 0));
}

void resetRestartCounter() {
  restartCounter = 0;
  preferences.putInt(restartKeyName, restartCounter);
}

void startHcell() {
  //TO-DO implement leaksensor limits
  if (!hCellState) {
    HWSerial.println("Starting Hcell! ");
    digitalWrite(PIN_cutoff, HIGH);      // open cut-off
    digitalWrite(PIN_startHcell, HIGH);  // start H-Cell
    hCellState = true;
  }
}

void stopHcell() {
  if (hCellState) {
    HWSerial.println("Stopping Hcell! ");
    digitalWrite(PIN_cutoff, LOW);      // close cut-off
    digitalWrite(PIN_startHcell, LOW);  // turn off H-Cell
    hCellState = false;
  }
}

void recording() {
  static bool lastState = recordingFlag;

  if (recordingFlag) {                 // recording pressed
    if (lastState != recordingFlag) {  // recording pressed for the first time
      connectMS(false);                //disconnect MS to prevent data corruption
      createCSV(SD, &recordingFileIndex);
    }
    // append new data if available
    if (appendBufferFlag) {
      HWSerial.println("appendBufferFlag == true");
      appendCSV(SD, &recordingFileIndex, sensorBufferSize);
      appendBufferFlag = false;
    }
  } else {                             // recording not pressed
    if (lastState != recordingFlag) {  // recording switched off
      appendCSV(SD, &recordingFileIndex, bufferIndex);
      bufferIndex = 0;
      connectMS(true);  //connect MS to transfer recorded data
    }
  }
  lastState = recordingFlag;
}

void buttonInterpreter(int button, int value) {
  //value == 0 button release
  //value == 1 button pressed
  switch (button) {
    case 0:  //left
      if (value == 1) gui.moveCursor(LEFT);
      break;
    case 1:  //OK
      if (value == 1) gui.selectCursor();
      else gui.clickCursor();
      break;
    case 2:  //right
      if (value == 1) gui.moveCursor(RIGHT);
      break;
    case 3:  //up
      if (value == 1) gui.moveCursor(UP);
      break;
    case 4:  //down
      if (value == 1) gui.moveCursor(DOWN);
      break;
    case 5:  //HCell start == 1, stop == 0
      if (value == 1) {
        startHcell();
      } else {
        stopHcell();
      }
      break;
    case 6:  //recording start == 1, stop == 0
      if (value == 1) {
        HWSerial.print("Recording: ");
        HWSerial.println(value);
        recordingFlag = true;
      } else {
        HWSerial.print("Recording: ");
        HWSerial.println(value);
        recordingFlag = false;
      }
      break;
    default:
      break;
  }
}

// void buttonInterpreter(int button, int value) {
//   switch (button) {
//     case 0:
//       HWSerial.println("CH0");
//       break;
//     case 1:
//       HWSerial.println("CH1");
//       break;
//     case 2:
//       HWSerial.println("CH2");
//       break;
//     case 3:
//       HWSerial.println("CH3");
//       break;
//     case 4:
//       HWSerial.println("CH4");
//       break;
//     case 5:
//       HWSerial.println("CH5");
//       break;
//     case 6:
//       HWSerial.println("CH6");
//       break;
//     default:
//       break;
//   }
// }

void addDataToBuffer() {
  static uint16_t tmpSensorData = 0;

  if (!appendBufferFlag) {  // to prevent buffer override
    // read current sensor data and write to buffer
    for (int i = 0; i < numData; i++) {
      tmpSensorData = sensorArray.getData(i)->value;
      SensorBuffer[i][bufferIndex] = tmpSensorData;
    }
    bufferIndex++;
    if (bufferIndex == sensorBufferSize) {
      bufferIndex = 0;  // wrap around
      appendBufferFlag = true;
    }
  }
}

void loopTime() {
  static int frameTime = 0;
  static int timeStamp = micros();
  static int avgCount = 0;
  int avgNum = 1000;

  if (avgCount >= avgNum) {
    frameTime = frameTime / avgNum;
    String looptime = String(frameTime) + "us";
    t1.setText(looptime);
    frameTime = 0;
    avgCount = 0;
  } else {
    frameTime += micros() - timeStamp;
    timeStamp = micros();
    avgCount++;
  }
}