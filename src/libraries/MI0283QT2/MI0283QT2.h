#ifndef MI0283QT2_h
#define MI0283QT2_h


#ifdef __cplusplus
extern "C" {
#endif
  #include <inttypes.h>
  #include <avr/pgmspace.h>
  #include "fonts.h"
#ifdef __cplusplus
}
#endif
#include "Print.h"


#define RGB(r,g,b)   (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3)) //5 red | 6 green | 5 blue

#define COLOR_WHITE  RGB(255,255,255)
#define COLOR_BLACK  RGB(  0,  0,  0)
#define COLOR_RED    RGB(255,  0,  0)
#define COLOR_GREEN  RGB(  0,255,  0)
#define COLOR_BLUE   RGB(  0,  0,255)
#define COLOR_YELLOW RGB(255,255,  0)

#ifndef DEC
# define DEC (10)
#endif
#ifndef HEX
# define HEX (16)
#endif
#ifndef OCT
# define OCT (8)
#endif
#ifndef BIN
# define BIN (2)
#endif

#define Display_Mode_Control 0x01
#define OSC_Control_1        0x18
#define OSC_Control_2        0x19
#define Power_Control_1      0x1A
#define Power_Control_2   	 0x1B
#define Power_Control_3  	 0x1C
#define Power_Control_4  	 0x1D
#define Power_Control_5  	 0x1E
#define Power_Control_6  	 0x1F
#define Display_Control_1	 0x26
#define Display_Control_2	 0x27
#define Display_Control_3	 0x28


#define OSC_Control_2_OSC_EN  		  0x01
#define Display_Control_3_GON		  0x20
#define Display_Control_3_DTE		  0x10
#define Display_Control_3_D0		  0x04
#define Display_Control_3_D1  		  0x08
#define Power_Control_6_STB   		  0x01
#define Display_Mode_Control_DP_STB_S 0x40
#define Display_Mode_Control_DP_STB   0x80


class MI0283QT2 : public Print
{
  public:
    uint16_t lcd_orientation;
    uint16_t lcd_width, lcd_height;

    MI0283QT2();
    void init(uint8_t clock_div); //2 4 8 16 32
    void led(uint8_t power); //0-100
    void standby(bool deep);
	  void wakeup(bool deep);

    void setOrientation(uint16_t o); //0 90 180 270
    uint16_t getWidth(void);
    uint16_t getHeight(void);
    void setArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void setCursor(uint16_t x, uint16_t y);

    void clear(uint16_t color);
    void drawStart(void);
    void draw(uint16_t color);
    void drawStop(void);
    void drawPixel(uint16_t x0, uint16_t y0, uint16_t color);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void drawRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t color);

    uint16_t drawChar(uint16_t x, uint16_t y, char c, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, char *s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, int i, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, unsigned int i, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, long l, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, unsigned long l, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawText(uint16_t x, uint16_t y, String &s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawTextPGM(uint16_t x, uint16_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color);

    uint16_t drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawMLText(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, String &s, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawMLTextPGM(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color);

    uint16_t drawInteger(uint16_t x, uint16_t y, char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color); //base = DEC, HEX, OCT, BIN
    uint16_t drawInteger(uint16_t x, uint16_t y, unsigned char val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawInteger(uint16_t x, uint16_t y, int val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);
    uint16_t drawInteger(uint16_t x, uint16_t y, long val, uint8_t base, uint8_t size, uint16_t color, uint16_t bg_color);

    void printOptions(uint8_t size, uint16_t color, uint16_t bg_color);
    void printClear(void);
    void printXY(uint16_t x, uint16_t y);
    uint16_t printGetX(void);
    uint16_t printGetY(void);
    void printPGM(PGM_P s);
    virtual size_t write(uint8_t c);
    virtual size_t write(const char *s);
    virtual size_t write(const uint8_t *s, size_t size);

  private:
    uint8_t p_size;
    uint16_t p_fg, p_bg;
    uint16_t p_x, p_y;

    void reset(void);
    void wr_cmd(uint8_t reg, uint8_t param);
    void wr_data(uint16_t data);
    void wr_spi(uint8_t data);
    void delay_10ms(uint8_t ms);
    void displayOnFlow(void);
    void displayOffFlow(void);
};


#endif //MI0283QT2_h
