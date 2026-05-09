#ifndef ILI9341_H
#define ILI9341_H

#include "main.h"
#include "fonts.h"

#define ILI9341_WIDTH  320
#define ILI9341_HEIGHT 240

// Colors
#define ILI9341_BLACK       0x0000
#define ILI9341_GREEN       0x07E0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_RED         0xF800
#define ILI9341_YELLOW      0xFFE0

void ILI9341_Init(void);
void ILI9341_FillScreen(uint16_t color);
void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ILI9341_DrawDottedGrid(void);

#endif
