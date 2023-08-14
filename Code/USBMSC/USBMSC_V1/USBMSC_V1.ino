#include "SD.h"
#include "SPI.h"
#include "USB.h"
#include "USBMSC.h"


/* todo: 
    - move functions and definitions to .h-file
    - header and description
    - change device name shown in windows explorer
    - #include "file_handling.h"
*/


#define NAND_SPI_MOSI 11
#define NAND_SPI_MISO 13
#define NAND_SPI_SCK 12
#define NAND_SPI_CS 10

USBMSC msc;

bool sd_changed = false;
bool sd_inited = false;
bool testwrite = true;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
Serial.printf("MSC WRITE: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
return SD.writeRAW( (uint8_t*) buffer, lba) ? bufsize : -1 ;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
Serial.printf("MSC READ: lba: %u, offset: %u, bufsize: %u\n", lba, offset, bufsize);
return SD.readRAW( (uint8_t*) buffer, lba) ? 512 : -1 ;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject){
Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
return true;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  long time = millis();
  while (!Serial && ( millis() < time + 5000) ); // wait up to 5 seconds for Arduino Serial Monitor
  Serial.println("");
  Serial.println("MSC research for Hoober");

  pinMode(NAND_SPI_CS, OUTPUT);
  digitalWrite(NAND_SPI_CS, LOW);

  static SPIClass* spi = NULL;
  spi = new SPIClass(FSPI);
  spi->begin(NAND_SPI_SCK, NAND_SPI_MISO, NAND_SPI_MOSI, NAND_SPI_CS);

  if ( !SD.begin( NAND_SPI_CS, *spi, 20000000 ) )
  {
  Serial.println(F("Storage initialization failed"));
  Serial.println("Stopped");
  while(1);
  }
  else
  {
  Serial.println(F("Storage initialization success"));
  }

  Serial.print( "card size = " );
  Serial.print( SD.cardSize() / (1024 * 1024) );
  Serial.print( " MB, numSectors = " );
  Serial.print( SD.numSectors() );
  Serial.print( ", bytes per sector = " );
  Serial.print( SD.cardSize() / SD.numSectors() );
  Serial.print( ", total bytes = " );
  Serial.print( SD.totalBytes() );
  Serial.print( ", usedBytes = " );
  Serial.print( SD.usedBytes() );

  Serial.print(", SD Card Type: ");
  if(SD.cardType() == CARD_MMC){
  Serial.println("MMC");
  } else if(SD.cardType() == CARD_SD){
  Serial.println("SDSC");
  } else if(SD.cardType() == CARD_SDHC){
  Serial.println("SDHC");
  } else if(SD.cardType() == CARD_NONE){
  Serial.println("No SD card attached");
  }else {
  Serial.println("UNKNOWN");
  }

  msc.vendorID("REF32");
  msc.productID("USB_MSC");
  msc.productRevision("1.0");
  msc.onRead(onRead);
  msc.onWrite(onWrite);
  msc.onStartStop(onStartStop);
  msc.mediaPresent(true);
  msc.begin(SD.numSectors(), 512);     //msc.begin(SD.numSectors(), SD.cardSize() / SD.numSectors() );

  USB.begin();

  sd_changed = true; // to print contents initially
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if(!root){
  Serial.println("Failed to open directory");
  return;
  }
  if(!root.isDirectory()){
  Serial.println("Not a directory");
  return;
  }

  File file = root.openNextFile();
  while(file){
  if(file.isDirectory()){
  Serial.print(" DIR : ");
  Serial.println(file.name());
  if(levels){
  listDir(fs, file.path(), levels -1);
  }
  } else {
  Serial.print(" FILE: ");
  Serial.print(file.name());
  Serial.print(" SIZE: ");
  Serial.println(file.size());
  }
  file = root.openNextFile();
  }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void loop() {
  if ( sd_changed )
  {
    Serial.println(" ");
    listDir(SD, "/", 0);
    Serial.println();

    sd_changed = false;
  }

  if( testwrite )
  {
    writeFile(SD, "/Hello_World.txt", "Hello World!");
    writeFile(SD, "/Hello_World2.txt", "Hello World2!");
    appendFile(SD, "/Hello_World.txt", "\nTest");
    sd_changed = true;
    testwrite = false;
  }

  

  delay(1000); // refresh every 1 second
}