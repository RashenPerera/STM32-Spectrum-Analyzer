#include "ili9341.h"

// --- Hardware Control Macros (MIRRORED) ---
#define LCD_WR_LOW     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define LCD_WR_HIGH    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define LCD_RS_CMD     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET)
#define LCD_RS_DATA    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET)
#define LCD_CS_LOW     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define LCD_CS_HIGH    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)
#define LCD_RST_LOW    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)
#define LCD_RST_HIGH   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET)

// --- Custom 8-bit Data Bus Write (MIRRORED) ---
void LCD_Write_Bus(uint8_t data) {
    // Re-mapped to match your upside-down ribbon cable
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,  (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D0
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,  (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D1
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,  (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D2
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,  (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D3
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (data & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D4
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, (data & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D5
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, (data & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D6
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET); // D7
}

void LCD_Write_Cmd(uint8_t cmd) {
    LCD_RS_CMD;
    LCD_Write_Bus(cmd);
    LCD_WR_LOW;
    LCD_WR_HIGH;
}

void LCD_Write_Data(uint8_t data) {
    LCD_RS_DATA;
    LCD_Write_Bus(data);
    LCD_WR_LOW;
    LCD_WR_HIGH;
}

void ILI9341_Init(void) {
    LCD_CS_HIGH;
    LCD_WR_HIGH;

    // Hardware Reset
    LCD_RST_LOW;
    HAL_Delay(50);
    LCD_RST_HIGH;
    HAL_Delay(50);

    LCD_CS_LOW; // Select display

    // ILI9341 Magic Init Sequence
    LCD_Write_Cmd(0x01); // Software Reset
    HAL_Delay(100);

    LCD_Write_Cmd(0x3A); // Pixel Format Set
    LCD_Write_Data(0x55); // 16-bit colors

    LCD_Write_Cmd(0x36); // Memory Access Control (Orientation)
    LCD_Write_Data(0xE8); // Landscape mode

    LCD_Write_Cmd(0x11); // Sleep Out
    HAL_Delay(120);

    LCD_Write_Cmd(0x29); // Display ON
    LCD_CS_HIGH;
}

void ILI9341_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD_Write_Cmd(0x2A); // Column Address Set
    LCD_Write_Data(x1 >> 8); LCD_Write_Data(x1 & 0xFF);
    LCD_Write_Data(x2 >> 8); LCD_Write_Data(x2 & 0xFF);

    LCD_Write_Cmd(0x2B); // Page Address Set
    LCD_Write_Data(y1 >> 8); LCD_Write_Data(y1 & 0xFF);
    LCD_Write_Data(y2 >> 8); LCD_Write_Data(y2 & 0xFF);

    LCD_Write_Cmd(0x2C); // Memory Write
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    LCD_CS_LOW;
    ILI9341_SetWindow(x, y, x, y);
    LCD_RS_DATA;
    LCD_Write_Bus(color >> 8);   // Send upper 8 bits
    LCD_WR_LOW; LCD_WR_HIGH;
    LCD_Write_Bus(color & 0xFF); // Send lower 8 bits
    LCD_WR_LOW; LCD_WR_HIGH;
    LCD_CS_HIGH;
}

void ILI9341_FillScreen(uint16_t color) {
    LCD_CS_LOW;
    ILI9341_SetWindow(0, 0, ILI9341_WIDTH - 1, ILI9341_HEIGHT - 1);
    LCD_RS_DATA;

    uint8_t c_high = color >> 8;
    uint8_t c_low = color & 0xFF;

    // We have to write 320 * 240 = 76,800 pixels!
    for (uint32_t i = 0; i < 76800; i++) {
        LCD_Write_Bus(c_high);
        LCD_WR_LOW; LCD_WR_HIGH;
        LCD_Write_Bus(c_low);
        LCD_WR_LOW; LCD_WR_HIGH;
    }
    LCD_CS_HIGH;
}

void ILI9341_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    while (*str) {
        // Draw each character pixel by pixel (we can optimize this later!)
        for (uint16_t i = 0; i < font.height; i++) {
            uint32_t b = font.data[(*str - 32) * font.height + i];
            for (uint16_t j = 0; j < font.width; j++) {
                if ((b << j) & 0x8000) {
                    ILI9341_DrawPixel(x + j, y + i, color);
                } else {
                    ILI9341_DrawPixel(x + j, y + i, bgcolor);
                }
            }
        }
        x += font.width;
        str++;
    }
}

void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    LCD_CS_LOW;
    ILI9341_SetWindow(x, y, x + w - 1, y + h - 1);
    LCD_RS_DATA;

    uint8_t c_high = color >> 8;
    uint8_t c_low = color & 0xFF;

    uint32_t pixels = w * h;
    for (uint32_t i = 0; i < pixels; i++) {
        LCD_Write_Bus(c_high);
        LCD_WR_LOW; LCD_WR_HIGH; // Strobe high byte immediately
        LCD_Write_Bus(c_low);
        LCD_WR_LOW; LCD_WR_HIGH; // Strobe low byte immediately
    }
    LCD_CS_HIGH;
}

// Custom function to draw 3 horizontal dotted lines for dB reference
void ILI9341_DrawDottedGrid(void) {
    // 0x4A49 is a nice subtle dark grey color in RGB565
    uint16_t grid_color = 0x4A49;

    // Draw dots every 4 pixels across the 320px width
    for (uint16_t x = 0; x < 320; x += 4) {
        ILI9341_DrawPixel(x, 170, grid_color); // Top Line (e.g., High dB)
        ILI9341_DrawPixel(x, 110, grid_color); // Mid Line
        ILI9341_DrawPixel(x, 50,  grid_color); // Low Line
    }
}
