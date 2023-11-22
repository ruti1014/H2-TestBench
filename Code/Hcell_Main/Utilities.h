
void setup_pins() {
  pinMode(PIN_startHcell, OUTPUT);
  pinMode(PIN_cutoff, OUTPUT);
  pinMode(PIN_leaksensor, INPUT);
  pinMode(PIN_flowsensor, INPUT);
  pinMode(MicroSD_SPI_CS, OUTPUT);
  pinMode(Display_SPI_CS, OUTPUT);

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

void setupSPI() {
  //MicroSD SPI Class:
  MicroSD_SPI = new SPIClass(HSPI);
  MicroSD_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, MicroSD_SPI_CS);

  //Display SPI Class:
  Display_SPI = new SPIClass(HSPI);
  Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);


  HWSerial.print("MicroSD CS: ");
  HWSerial.println(MicroSD_SPI->pinSS());
  HWSerial.print("Display CS: ");
  HWSerial.println(Display_SPI->pinSS());
}

void setupI2C() {
  I2C.begin(I2C_SDA, I2C_SCL, 100000);
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
  HWSerial.println(bme1.isAvailable() ? "true" : "false");
  HWSerial.println(bme1.isAvailable() ? "true" : "false");
}

void resetFileIndex() {
  HWSerial.println("Resetting fileIndex...");
  recordingFileIndex = 1;
  preferences.putUInt(recordingKeyName, recordingFileIndex);
  HWSerial.print("new fileIndex: ");
  HWSerial.println(preferences.getUInt(recordingKeyName, 0));
}

void startHcell() {
  uint16_t h_value = analogRead(PIN_leaksensor);
  if (!hCellState) {
    if (h_value < leakSensorLowerLimit) {
      HWSerial.println("Starting Hcell! ");
      digitalWrite(PIN_cutoff, HIGH);      // open cut-off
      digitalWrite(PIN_startHcell, HIGH);  // start H-Cell
      hCellState = true;
    }
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

void checkHlimits() {
  uint16_t h2_value = analogRead(PIN_leaksensor);
  if (h_value > leakSensorUpperLimit) {
    if (hCellState) {
      stopHcell();
      Serial.println("H-Cell was stopped due to high H2 values!");
      hCellState = false;
    }
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
      HWSerial.println("rec off, appending...");
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

void updateSensorArray() {
  int refreshRate = sampleRateMS;  //minimum refreshrate in ms
  static int refreshTime = millis();
  if ((millis() - refreshTime) > refreshRate) {
    sensorArray.updateSensorValues();
    refreshTime = millis();
  }
}

void addDataToBuffer() {
  int refreshRate = sampleRateMS;  //minimum refreshrate in ms
  static int refreshTime = millis();
  static uint16_t tmpSensorData = 0;

  if ((millis() - refreshTime) > refreshRate) {
    if (!appendBufferFlag && recordingFlag) {  // to prevent buffer override
      // read current sensor data and write to buffer
      for (int i = 0; i < numData; i++) {
        tmpSensorData = sensorArray.getData(i)->value;
        SensorBuffer[i][bufferIndex] = tmpSensorData;
      }
      bufferIndex++;
      if (bufferIndex >= sensorBufferSize) {
        bufferIndex = 0;  // wrap around
        appendBufferFlag = true;
      }
    }
    refreshTime = millis();
  }
}

void updateDisplay() {
  updateStatusBar();
  updateValues();
}

void updateStatusBar() {
  static bool lastHcellStatus = false;
  static bool lastRecFlag = false;
  static bool lastSDstate = false;
  int refreshRate = 50;  //minimum refreshrate in ms
  static int refreshTime = millis();


  if (millis() - refreshTime > refreshRate) {
    //Update statusbar
    if (lastRecFlag != recordingFlag) {
      if (recordingFlag) {
        recordingStatus.setText("rec..");
        recordingStatus.setTextColor(COLOR_DARKRED);
      } else {
        recordingStatus.setText("rec");
        recordingStatus.setTextColor(COLOR_BLACK);
      }
      lastRecFlag = recordingFlag;
    }

    if (lastHcellStatus != hCellState) {
      if (hCellState) {
        hcellStatus.setText("on");
        hcellStatus.setTextColor(COLOR_GREEN);
      } else {
        hcellStatus.setText("off");
        hcellStatus.setTextColor(COLOR_BLACK);
      }
      lastHcellStatus = hCellState;
    }

    if (lastSDstate != sd_inited) {
      if (sd_inited) {
        sdStatus.setText("SD ok");
        sdStatus.setTextColor(COLOR_GREEN);
      } else {
        sdStatus.setText("no SD");
        sdStatus.setTextColor(COLOR_BLACK);
      }
      lastSDstate = sd_inited;
    }
    refreshTime = millis();
  }
}

void updateValues() {
  int refreshRate = 1000;  //minimum refreshrate in ms
  static int refreshTime = millis();
  String data = "BME1 Temp: ";

  if (millis() - refreshTime > refreshRate) {
    if (gui.getCurrentPage() == &p2) {
      // int div = (int)bme1.isAvailable() + (int)bme2.isAvailable();
      // int sum = bme1.getValue(SENS_TEMP) + bme2.getValue(SENS_TEMP);
      // if (div != 0) data += String(sum / div);
      // else data += "No Data";
      data += bme1.getValue(SENS_TEMP);
      p2t1.setText(data);
    }
  }
}

void loopTime() {
  static int frameTime = 0;
  static int timeStamp = micros();
  static int avgCount = 0;
  int avgNum = 3000;  //Number of loopcycles averaged over


  if (avgCount >= avgNum) {
    frameTime = frameTime / avgNum;
    String time;
    String unit = "us";
    if ((frameTime / 1000) > 0) {
      frameTime = frameTime / 1000;
      unit = "ms";
    }
    time = String(frameTime) + unit;
    looptimeText.setText(time);
    frameTime = 0;
    avgCount = 0;
  } else {
    frameTime += micros() - timeStamp;
    timeStamp = micros();
    avgCount++;
  }
}