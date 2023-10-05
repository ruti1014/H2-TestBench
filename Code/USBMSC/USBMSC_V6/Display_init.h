#ifndef DISPLAY_INIT_H
#define DISPLAY_INIT_H

// Display init:

  void initDisplay(){
    // new SPI Class Object:
    static SPIClass* Display_SPI = NULL;
    Display_SPI = new SPIClass(HSPI);
    Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);

    // init Display with SPI Class Object:
    TFT_22_ILI9225 tft = TFT_22_ILI9225(0, SPI_MISO, Display_SPI_CS, Display_LED);
    // TFT_22_ILI9225(int8_t RST, int8_t RS, int8_t CS, int8_t SDI, int8_t CLK, int8_t LED, uint8_t brightness);

    tft.begin(*Display_SPI);

    tft.setOrientation(0);  // 0=portrait, 1=right rotated landscape, 2=reverse portrait, 3=left rotated landscape
    tft.clear();
    tft.drawBitmap(0, 0, LogoHEK_white_on_black, 176, 220, COLOR_WHITE);
  }
  
#endif