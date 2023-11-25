/*  ----------- GraphicsAdapter.cpp -----------
*   Authors: Tim Ruf, Jonas Geckle
*   Institution: Hochschule Karlsruhe
*   Description: 
*
*
*
*
*/

#include "GraphicsAdapter.h"

TFTGraphics::TFTGraphics(TFT_22_ILI9225* tft)
  :_tft(tft) {}

void TFTGraphics::drawText(int x, int y, String text, int color) {
  _tft->drawText(x, y, text, color);
}

int TFTGraphics::getTextWidth(String text){
  return _tft->getTextWidth(text);
}

int TFTGraphics::getFontHeight(){
  return _tft->getFont().height;
}

void TFTGraphics::setBGColor(int color){
    _tft->setBackgroundColor(color);
}

void TFTGraphics::drawPixel(int x, int y, uint16_t color) {
  _tft->drawPixel(x, y, color);
}

void TFTGraphics::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
  _tft->drawLine(x1, y1, x2, y2, color);
}

void TFTGraphics::drawRect(int x1, int y1, int x2, int y2, uint16_t color) {
  _tft->drawRectangle(x1, y1, x2, y2, color);
}

void TFTGraphics::drawFillRect(int x1, int y1, int x2, int y2, uint16_t color) {
  _tft->fillRectangle(x1, y1, x2, y2, color);
}

void TFTGraphics::drawCircle(int x, int y, int r, uint16_t color) {
  _tft->drawCircle(x, y, r, color);
}
