//#include "soc/uart_struct.h"
/*  ----------- file_handling.h -----------
*   Author: Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: contains functions to read/write/etc. files to a file system
*/

#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

void writeFile(fs::FS &fs, const char *path, const char *message) {
  HWSerial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    debugprintln("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    debugprintln("File written");
  } else {
    debugprintln("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  HWSerial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);

  if (!file) {
    debugprintln("Failed to open file for appending");
    return;
  }

  if (file.print(message)) {
    debugprintln("Message appended");
  } else {
    debugprintln("Append failed");
  }

  file.close();
}

void readFile(fs::FS &fs, const char *path) {
  HWSerial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    debugprintln("Failed to open file for reading");
    return;
  }

  debugprint("Read from file: ");
  while (file.available()) {
    HWSerial.write(file.read());
  }
  file.close();
}

void removeDir(fs::FS &fs, const char *path) {
  HWSerial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    debugprintln("Dir removed");
  } else {
    debugprintln("rmdir failed");
  }
}

void createDir(fs::FS &fs, const char *path) {
  HWSerial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    debugprintln("Dir created");
  } else {
    debugprintln("mkdir failed");
  }
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  HWSerial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    debugprintln("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    debugprintln("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      debugprint("  DIR : ");
      debugprintln(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      debugprint("  FILE: ");
      debugprint(file.name());
      debugprint("  SIZE: ");
      debugprintln(file.size());
    }
    file = root.openNextFile();
  }
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  HWSerial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    debugprintln("File renamed");
  } else {
    debugprintln("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  HWSerial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    debugprintln("File deleted");
  } else {
    debugprintln("Delete failed");
  }
}

// generates a new .csv-file with current fileIndex and saves it on fs, increments fileIndex by 1
void createCSV(fs::FS &fs, uint32_t *fileIndex) {
  // get file path
  String name = "/H2TestBench_recording_";
  String indexStr = String(*fileIndex);
  String format = ".csv";
  String path = name + indexStr + format;
  int pathLen = path.length() + 1;
  char pathChar[pathLen];
  path.toCharArray(pathChar, pathLen);

  // get CSV header from sensor names and units
  String tmpSensorName = "";
  String tmpSensorUnit = "";
  String headerCSV = "";

  for(int i=0; i<numData; i++){
    // TO-DO: 
    tmpSensorName = "TestSensor";      //sensorArray.getData(i)->sensorName;
    tmpSensorUnit = "°C";              //sensorArray.getData(i)->unit;
    headerCSV += tmpSensorName + " (" + tmpSensorUnit + ");";
  }
  headerCSV += "\n";

  int headerLen = headerCSV.length() + 1;
  char headerChar[headerLen];
  headerCSV.toCharArray(headerChar, headerLen);

  // write file
  writeFile(fs, pathChar, headerChar);
  *fileIndex = *fileIndex + 1;
  preferences.putUInt(recordingKeyName, recordingFileIndex);
}

// appends newest sensor data to .csv-file
void appendCSV(fs::FS &fs, uint32_t *fileIndex, int bufSize) {
  String data = "";
  String appendix = "";

  // get file path
  String name = "/H2TestBench_recording_";
  String indexStr = String((*fileIndex) - 1);  // to get current fileIndex
  String format = ".csv";
  String path = name + indexStr + format;

  int pathLen = path.length() + 1;
  char pathChar[pathLen];
  path.toCharArray(pathChar, pathLen);

  // open file for appending
  File file = fs.open(pathChar, FILE_APPEND);
  if (!file) {
    debugprintln("Failed to open file for appending");
    return;
  }

  // append new content
  for (int j = 0; j < bufSize; j++) {
    // get next row
    for (int i = 0; i < numSensors; i++) {
      // TO-DO:
      data = i;       //SensorBuffer[i][j];
      appendix += data + ";";
    }
    appendix += "\n";

    // append row to file
    int appendixLen = appendix.length() + 1;
    char appendixChar[appendixLen];
    appendix.toCharArray(appendixChar, appendixLen);

    if (file.print(appendixChar)) {
      debugprintln("Message appended");
    } else {
      debugprintln("Append failed");
    }
  }

  file.close();
}

#endif