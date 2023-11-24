
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
  tft_ili.setFont(Terminal6x8);

  tft_ili.setOrientation(0);  // 0=portrait, 1=right rotated landscape, 2=reverse portrait, 3=left rotated landscape
  tft_ili.clear();
  tft_ili.setBackgroundColor(COLOR_BLACK);
  tft_ili.drawBitmap(0, 0, LogoHEK, 176, 220);
  tft_ili.setOrientation(1);
}

void setupSensors() {
  sensorArray.addSensor(&h2flow);
  sensorArray.addSensor(&h2leak);
  sensorArray.addSensor(&bme1);
  sensorArray.addSensor(&bme2);
  // HWSerial.println(bme1.isAvailable() ? "true" : "false");
  // HWSerial.println(bme1.isAvailable() ? "true" : "false");
}

void resetFileIndex() {
  HWSerial.println("Resetting fileIndex...");
  recordingFileIndex = 1;
  preferences.putUInt(recordingKeyName, recordingFileIndex);
  HWSerial.print("new fileIndex: ");
  HWSerial.println(preferences.getUInt(recordingKeyName, 0));
}

void startHcell() {
  uint16_t h2_value = analogRead(PIN_leaksensor);
  if (!hCellState) {
    if (h2_value < leakSensorLowerLimit) {
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
  static int timeStamp = millis();
  if ((millis() - timeStamp) >= 10) {  //Check h2 value in MS
    uint16_t h2_value = analogRead(PIN_leaksensor);
    if (h2_value > leakSensorUpperLimit) {
      if (hCellState) {
        stopHcell();
        Serial.println("H-Cell was stopped due to high H2 values!");
        hCellState = false;
      }
    }
  }
}

void recording() {
  static bool lastState = recordingFlag;

  if (!sd_inited) return;
  if (recordingFlag) {
    HWSerial.print("Recording... ");   // recording pressed
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
      if (value == 1 && !sd_busy) gui.moveCursor(LEFT);
      break;
    case 1:  //OK
      if (value == 1 && !sd_busy) gui.selectCursor();
      else gui.clickCursor();
      break;
    case 2:  //right
      if (value == 1 && !sd_busy) gui.moveCursor(RIGHT);
      break;
    case 3:  //up
      if (value == 1 && !sd_busy) gui.moveCursor(UP);
      break;
    case 4:  //down
      if (value == 1 && !sd_busy) gui.moveCursor(DOWN);
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
        recordingFlag = true;
      } else {
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


void requestSerialInfo() {

  if (Serial0.available()) {
    String cmd = Serial0.readStringUntil('\n');

    if (cmd == "idn") {
      Serial0.print("\nSerialnumber: ");
      SerialHCELL.print("*idn?\r");
    } else if (cmd == "vol") {
      Serial0.print("\nVoltage (in mV): ");
      SerialHCELL.print("*vol?\r\n");
    } else if (cmd == "amp") {
      Serial0.print("\nCurrent (in mA): ");
      SerialHCELL.print("*cur?\r\n");
    } else if (cmd == "tmp") {
      Serial0.print("\nTemperature (in °C): ");
      SerialHCELL.print("*temp?\r\n");
    } else if (cmd == "prs") {
      Serial0.print("\nPressure (in mbar): ");
      SerialHCELL.print("*pres?\r\n");
    } else if (cmd == "err") {
      Serial0.print("\nError-Code: ");
      SerialHCELL.print("*error?\r\n");
    }
  }

  readSerial();
}

void readSerial() {
  char tmp = 'a';
  int bytesRemaining = SerialHCELL.available();
  if (bytesRemaining > 0) {
    //tmp = SerialHCELL.read();
    tmp = SerialHCELL.read();
    HWSerial.print(tmp, DEC);
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
    if (!sd_busy && USB_state == PLUGGED) {
      usbStatus.setText("USB ok");
    } else {
      usbStatus.setText("no USB");
    }

    if ((lastRecFlag != recordingFlag) && (sd_inited)) {
      if (recordingFlag) {
        recordingStatus.setText("rec..", COLOR_GREEN);
      } else {
        recordingStatus.setText("rec", COLOR_BLACK);
      }
      lastRecFlag = recordingFlag;
    }

    if (lastHcellStatus != hCellState) {
      if (hCellState) {
        hcellStatus.setText("on", COLOR_GREEN);
      } else {
        hcellStatus.setText("off", COLOR_BLACK);
      }
      lastHcellStatus = hCellState;
    }

    if (lastSDstate != sd_inited) {
      if (sd_inited) {
        sdStatus.setText("SD ok", COLOR_GREEN);
      } else {
        sdStatus.setText("no SD", COLOR_BLACK);
      }
      lastSDstate = sd_inited;
    }

    refreshTime = millis();
  }
}

void updateValues() {
  int refreshRate = 1000;  //minimum refreshrate in ms
  static int refreshTime = millis();


  if (millis() - refreshTime > refreshRate) {
    if (gui.getCurrentPage() == &p2) {
      //To do: update sensor values

      String text;
      text = String(bme1.getValue(SENS_TEMP)) + " ^C";
      bme1_temp.setText(text);
      text = String(bme1.getValue(SENS_HUM)) + " %";
      bme1_hum.setText(text);
      text = String(bme1.getValue(SENS_PRESSURE)) + " mbar";
      bme1_pres.setText(text);

      text = String(bme2.getValue(SENS_TEMP)) + " ^C";
      bme2_temp.setText(text);
      text = String(bme2.getValue(SENS_HUM)) + " %";
      bme2_hum.setText(text);
      text = String(bme2.getValue(SENS_PRESSURE)) + " mbar";
      bme2_pres.setText(text);

    } else if (gui.getCurrentPage() == &p3) {
      // if (SerialHCELL.available()) {
        String text = "";
        if (SerialHCELL.available()) text = "out: " + SerialHCELL.readStringUntil('\n');
        serialnr.setText(text);
        text = "Temp: " + String(hcell.getValue(SENS_SERIALID)) + "^C";
        h2temp.setText(text);
        text = "Druck: " + String(hcell.getValue(SENS_SERIALID)) + "mbar";
        h2pres.setText(text);
        text = "Strom: " + String(hcell.getValue(SENS_SERIALID)) + "mA";
        current.setText(text);
        text = "Spannung: " + String(hcell.getValue(SENS_SERIALID)) + "mV";
        voltage.setText(text);
      // } else {
      //   serialnr.setTextColor(COLOR_RED);
      //   serialnr.setText("Brennstoffzelle nicht gefunden!");
      // }
    } else if (gui.getCurrentPage() == &p5) {
      String text;
      text = "File Index: " + String(recordingFileIndex);
      resetFileIndexText.setText(text);
    }
    refreshTime = millis();
  }
}

void checkSD() {
  if (sd_busy) {
    if (millis() - sd_timeout > 100) {  //check last onRead more than 100ms
      sd_busy = false;                  //onRead done
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