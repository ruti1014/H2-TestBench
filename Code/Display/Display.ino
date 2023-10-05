#include <SPI.h>
#include <TFT_22_ILI9225.h>

// define SPI Pins:
  #define SPI_MOSI 13
  #define SPI_MISO 25
  #define SPI_SCK 14
  // #define MicroSD_SPI_CS 39
  #define Display_SPI_CS  15

// TFT
  #define TFT_RST 26          // IO 26
  #define TFT_RS  25          // IO 25
  #define TFT_LED 0           // 0 if wired to +5V directly
  #define TFT_BRIGHTNESS 200  // Initial brightness of TFT backlight (optional)

TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, TFT_RS, Display_SPI_CS, SPI_MOSI, SPI_SCK, TFT_LED, TFT_BRIGHTNESS);

void setup() {
  Serial.begin(9600);
  Serial.println("setup");

  // pinMode(MicroSD_SPI_CS, OUTPUT);
  pinMode(Display_SPI_CS, OUTPUT);

  // new SPI Class Objects:
  static SPIClass* Display_SPI = NULL;
  Display_SPI = new SPIClass(HSPI);
  Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);

  // static SPIClass* MicroSD_SPI = NULL;
  // MicroSD_SPI = new SPIClass(HSPI);
  // MicroSD_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, MicroSD_SPI_CS);

  tft.begin(*Display_SPI);

}

void loop() {
  tft.drawText(10, 10, "Test");
  delay(200);
}

