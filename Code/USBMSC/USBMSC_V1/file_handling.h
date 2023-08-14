#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

/* todo: 
    - change USBMSC_V1 to use HWSerial and USBSerial instead of only Serial
*/

//--------------------------------
#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial
USBCDC USBSerial;
#endif
//--------------------------------

void writeFile(fs::FS &fs, const char * path, const char * message){
  HWSerial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    HWSerial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    HWSerial.println("File written");
  } else {
    HWSerial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  HWSerial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    HWSerial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
    HWSerial.println("Message appended");
  } else {
    HWSerial.println("Append failed");
  }
  file.close();
}

void readFile(fs::FS &fs, const char * path){
  HWSerial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file){
    HWSerial.println("Failed to open file for reading");
    return;
  }

  HWSerial.print("Read from file: ");
  while(file.available()){
    HWSerial.write(file.read());
  }
  file.close();
}

void removeDir(fs::FS &fs, const char * path){
  HWSerial.printf("Removing Dir: %s\n", path);
  if(fs.rmdir(path)){
    HWSerial.println("Dir removed");
  } else {
    HWSerial.println("rmdir failed");
  }
}

void createDir(fs::FS &fs, const char * path){
  HWSerial.printf("Creating Dir: %s\n", path);
  if(fs.mkdir(path)){
    HWSerial.println("Dir created");
  } else {
    HWSerial.println("mkdir failed");
  }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  HWSerial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
    HWSerial.println("Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    HWSerial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      HWSerial.print("  DIR : ");
      HWSerial.println(file.name());
      if(levels){
        listDir(fs, file.name(), levels -1);
      }
    } else {
      HWSerial.print("  FILE: ");
      HWSerial.print(file.name());
      HWSerial.print("  SIZE: ");
      HWSerial.println(file.size());
    }
     file = root.openNextFile();
  }
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
  HWSerial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    HWSerial.println("File renamed");
  } else {
    HWSerial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path){
  HWSerial.printf("Deleting file: %s\n", path);
  if(fs.remove(path)){
    HWSerial.println("File deleted");
  } else {
    HWSerial.println("Delete failed");
  }
}


#endif