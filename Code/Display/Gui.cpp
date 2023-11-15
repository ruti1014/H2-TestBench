#include <stdexcept>
#include "Gui.h"


//########### GUI ###########
Gui::Gui(GraphicsAdapter* ga, int posX, int posY, int width, int height, int pageNum)
  : _ga(ga), _posX(posX), _posY(posY), _width(width), _height(height), _pageNum(pageNum) {

  _pageCount = 0;
  _pageList = new GuiPage*[_pageNum];
  _currentPage = NULL;
}

void Gui::addPage(GuiPage* page) {
  _pageList[_pageCount] = page;
}

GuiPage* Gui::getCurrentPage() {
  return _currentPage;
}

void Gui::loadPage(GuiPage* page) {
  _currentPage = page;
  _currentPage->drawPage();
}

void Gui::loadLastPage() {}

GraphicsAdapter* Gui::getGraphicsAdapter() {
  return _ga;
}

void Gui::selectCursor() {
  _currentPage->getSelectedElement()->selectElement();
}

void Gui::clickCursor() {
  _currentPage->getSelectedElement()->click();
  _currentPage->getSelectedElement()->drawElement();
  _currentPage->getSelectedElement()->hoverElement();
}

void Gui::moveCursor(Direction d) {
  _currentPage->getSelectedElement()->drawElement();
  _currentPage->moveCursor(d);
  _currentPage->getSelectedElement()->hoverElement();
}

int Gui::getX() {
  return _posX;
}
int Gui::getY() {
  return _posY;
}
int Gui::getWidth() {
  return _width;
}
int Gui::getHeight() {
  return _height;
}



//########### GUI Page ###########
GuiPage::GuiPage(Gui* gui, int row, int column)
  : _gui(gui), _rows(row), _columns(column) {

  //allocate memory for element matrix
  _elementMatrix = new Element**[_rows];
  for (int i = 0; i < _rows; i++) {
    _elementMatrix[i] = new Element*[_columns];
    //initialize matrix
    for (int j = 0; j < _columns; j++) {
      _elementMatrix[i][j] = NULL;
    }
  }
}

void GuiPage::drawPage() {
  fillPage(_BGcolor);
  for (int r = 0; r < _rows; r++) {
    for (int c = 0; c < _columns; c++) {
      if (_elementMatrix[r][c] != NULL) _elementMatrix[r][c]->drawElement();
      else Serial.println("Failed to load element");
    }
  }
}

void GuiPage::fillPage(uint16_t color) {
  _gui->getGraphicsAdapter()->drawFillRect(_gui->getX(), _gui->getY(), _gui->getX() + _gui->getWidth(), _gui->getY() + _gui->getHeight(), color);
}

void GuiPage::addElement(Element* element, int row, int _columns) {
  _elementMatrix[row][_columns] = element;
  _elementMatrix[row][_columns]->setGui(_gui);
  _elementMatrix[row][_columns]->setBGColor(_BGcolor);
}

Element* GuiPage::getSelectedElement() {
  return _elementMatrix[_cursorR][_cursorC];
}

void GuiPage::setCursor(int row, int column) {
  getSelectedElement()->drawElement();
  if (row < _rows) _cursorR = row;
  if (column < _columns) _cursorC = column;
  getSelectedElement()->hoverElement();
}

void GuiPage::moveCursor(Direction d) {
  int numChecks;
  int checkCounter = 0;
  int directionX = 0;
  int directionY = 0;
  int tempC = _cursorC;
  int tempR = _cursorR;
  Element* tempElementCursor = NULL;

  switch (d) {
    case LEFT:
      numChecks = _columns;
      directionX = -1;
      break;
    case RIGHT:
      numChecks = _columns;
      directionX = 1;
      break;
    case UP:
      numChecks = _rows;
      directionY = -1;
      break;
    case DOWN:
      numChecks = _rows;
      directionY = 1;
      break;
  }

  bool selectable = false;
  while ((checkCounter < numChecks) && (tempElementCursor == NULL) && (!selectable)) {  //search nearest valid element -> selectable and not null
    tempC += directionX;
    tempR += directionY;
    if (tempC >= _columns) tempC = 0;
    if (tempC < 0) tempC = _columns - 1;
    if (tempR >= _rows) tempR = 0;
    if (tempR < 0) tempR = _rows - 1;


    checkCounter++;
    tempElementCursor = _elementMatrix[tempR][tempC];
    if (tempElementCursor != NULL) {
      selectable = tempElementCursor->isSelectable();
      if (!selectable) {
        tempElementCursor = NULL;  //Element found but not selectable -> reset tempElementCursor
      }
    }
  }
  _cursorC = tempC;
  _cursorR = tempR;
}

void GuiPage::setBGColor(uint16_t color) {
  _BGcolor = color;
}

int GuiPage::getBGColor() {
  return _BGcolor;
}



//########### Elements ###########
Element::Element(int posX, int posY, int width, int height, int color)
  : _posX(posX), _posY(posY), _width(width), _height(height), _color(color) {
  _hoverColor = _color / 3;
  _selectColor = COLOR_DARKGREY;
  _selectTextColor = COLOR_WHITE;
}



void Element::onClick(void (*clickCallback)(void)) {
  _clickCallback = clickCallback;
}

void Element::setSelectable(bool selectable) {
  _selectable = selectable;
}

bool Element::isSelectable() {
  return _selectable;
}

void Element::click() {
  if (_clickCallback != NULL) _clickCallback;
  else Serial0.println("No Callback");
}

void Element::setGui(Gui* gui) {
  _gui = gui;
  _posX = _posX + gui->getX();
  _posY = _posY + gui->getY();
}

void Element::setBGColor(int color) {
  _bgColor = color;
}

void Element::setHoverColor(int color) {
  _hoverColor = color;
}

void Element::setSelectColor(int color) {
  _selectColor = color;
}

void Element::setSelectTextColor(int color) {
  _selectTextColor = color;
}


//Button element
Button::Button(int posX, int posY, int width, int height, String text, int color)
  : Element(posX, posY, width, height, color) {
  _gui = NULL;
  _text = text;
  _textColor = COLOR_BLACK;
}

void Button::drawElement() {
  drawButton(_textColor, _bgColor, _color);
}

void Button::hoverElement() {
  _gui->getGraphicsAdapter()->drawRect(_posX, _posY, _posX + _width, _posY + _height, _hoverColor);
}

void Button::selectElement() {
  //_gui->getGraphicsAdapter()->drawFillRect(_posX + 1, _posY + 1, _posX + _width - 1, _posY + _height - 1, _selectColor);
  drawButton(_selectTextColor, _selectColor, _hoverColor);
}

void Button::setText(String text) {
  _text = text;
  drawElement();
}

void Button::setTextColor(int color) {
  _textColor = color;
  drawElement();
}

void Button::drawButton(int textColor, int bgColor, int lineColor) {

  GraphicsAdapter* ga = _gui->getGraphicsAdapter();
  ga->setBGColor(bgColor);
  ga->drawFillRect(_posX + 1, _posY + 1, _posX + _width - 1, _posY + _height - 1, bgColor);
  ga->drawRect(_posX, _posY, _posX + _width, _posY + _height, lineColor);
  //calculate Text Position, default to relative 0,0 Warning: Text may overlap
  int xText = _posX;
  int yText = _posY;
  int paddingX = (_width - ga->getTextWidth(_text)) / 2;
  int paddingY = (_height - ga->getFontHeight()) / 2;
  if (paddingX > 0) xText = xText + paddingX;
  if (paddingY > 0) yText = yText + paddingY;

  ga->drawText(xText, yText, _text, textColor);
}


//Textbox element
TextBox::TextBox(int posX, int posY, int width, int height, String text, int color)
  : Element(posX, posY, width, height, color) {
  _selectable = false;
  _gui = NULL;
  _text = text;
  _textColor = COLOR_BLACK;
}

void TextBox::drawElement() {
  GraphicsAdapter* ga = _gui->getGraphicsAdapter();
  ga->setBGColor(_bgColor);
  ga->drawFillRect(_posX + 1, _posY + 1, _posX + _width - 1, _posY + _height - 1, _bgColor);
  ga->drawRect(_posX, _posY, _posX + _width, _posY + _height, _color);
  //calculate Text Position, default to relative 0,0 Warning: Text may overlap
  int xText = _posX;
  int yText = _posY;
  int paddingX = (_width - ga->getTextWidth(_text)) / 2;
  int paddingY = (_height - ga->getFontHeight()) / 2;
  if (paddingX > 0) xText = xText + paddingX;
  if (paddingY > 0) yText = yText + paddingY;

  ga->drawText(xText, yText, _text, _textColor);
}

void TextBox::hoverElement() {}

void TextBox::selectElement() {}

void TextBox::setText(String text) {
  _text = text;
  drawElement();
}

void TextBox::setTextColor(int color) {
  _textColor = color;
  drawElement();
}
