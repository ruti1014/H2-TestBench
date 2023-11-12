#ifndef DISPLAY_INIT_H
#define DISPLAY_INIT_H

// Display init:
void initDisplay(){
  // new SPI Class Object:
  Display_SPI = new SPIClass(HSPI);
  Display_SPI->begin(SPI_SCK, SPI_MISO, SPI_MOSI, Display_SPI_CS);

  // init Display with SPI Class Object:
  tft.begin(*Display_SPI);

  tft.setOrientation(0);  // 0=portrait, 1=right rotated landscape, 2=reverse portrait, 3=left rotated landscape
  tft.clear();
  tft.setBackgroundColor(COLOR_WHITE);
  tft.drawBitmap(0, 0, LogoHEK, 176, 220);
}
  
#endif