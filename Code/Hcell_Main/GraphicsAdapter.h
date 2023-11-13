#ifndef GRAPHICSADAPTER_H
#define GRAPHICSADAPTER_H

//include your library and fillout functions
#include <TFT_22_ILI9225.h>

class GraphicsAdapter {

public:
  virtual void drawText(int x, int y, String text) = 0;
  virtual void drawPixel(int x, int y, uint16_t color) = 0;
  virtual void drawLine(int x1, int y1, int x2, int y2, uint16_t color) = 0;
  virtual void drawRect(int x1, int y1, int x2, int y2, uint16_t color) = 0;
  virtual void drawFillRect(int x1, int y1, int x2, int y2, uint16_t color) = 0;
  virtual void drawCircle(int x, int y, int Radius, uint16_t color) = 0;
};


class TFTGraphics : public GraphicsAdapter {
public:
  TFTGraphics(TFT_22_ILI9225* tft);
  void drawPixel(int x, int y, uint16_t color) override;
  void drawText(int x, int y, String text) override;
  void drawLine(int x1, int y1, int x2, int y2, uint16_t color) override;
  void drawRect(int x1, int y1, int x2, int y2, uint16_t color) override;
  void drawFillRect(int x1, int y1, int x2, int y2, uint16_t color) override;
  void drawCircle(int x, int y, int Radius, uint16_t color) override;
private:
  TFT_22_ILI9225* _tft;
};

#endif