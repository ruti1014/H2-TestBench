#include <SPI.h>
#include <TFT_22_ILI9225.h>
#include "GuiSetup.h"

int Buttons[5] = { 0, 0, 0, 0, 0 };


void setup() {
  Serial.begin(9600);
  Serial.println("start");

  hspi.begin();
  tft_ili.begin(hspi);
  tft_ili.setOrientation(3);
  tft_ili.setFont(Terminal6x8);
  tft_ili.drawText(90, 30, "START", COLOR_RED);
  delay(1000);
  tft_ili.clear();
  setupGui();
}

void loop() {
  delay(1000);
  gui.moveCursor(RIGHT);
  loopTimeMS();
}
