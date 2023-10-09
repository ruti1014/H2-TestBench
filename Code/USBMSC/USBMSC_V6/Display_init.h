#ifndef DISPLAY_INIT_H
#define DISPLAY_INIT_H

// Display init:

  void initDisplay(){
    // new SPI Class Object:
    static SPIClass* Display_SPI = NULL;
    Display_SPI = new SPIClass(HSPI);
    Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);

    // init Display with SPI Class Object:
    TFT_22_ILI9225 tft = TFT_22_ILI9225(TFT_RST, SPI_MISO, Display_SPI_CS, Display_LED);
    // TFT_22_ILI9225(int8_t RST, int8_t RS, int8_t CS, int8_t SDI, int8_t CLK, int8_t LED, uint8_t brightness);

    tft.begin(*Display_SPI);

    tft.setOrientation(0);  // 0=portrait, 1=right rotated landscape, 2=reverse portrait, 3=left rotated landscape
    tft.clear();
    // tft.drawRectangle(0, 0, tft.maxX() - 1, tft.maxY() - 1, COLOR_WHITE);
    // tft.setFont(Terminal6x8);
    // tft.drawText(10, 10, "hello!");
    tft.setBackgroundColor(COLOR_BLACK);
    tft.drawBitmap(0, 0, zwei_graphen, 176, 220);
  }
  
#endif