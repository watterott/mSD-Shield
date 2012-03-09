/*
  Demo 1 (Display + Touch-Panel)
 */

#include <MI0283QT2.h>
#include <ADS7846.h>

#define TP_EEPROMADDR (0x00) //eeprom address for calibration data

MI0283QT2 lcd;
ADS7846 tp;


void setup()
{
  //init display
  lcd.init(4); //spi-clk = Fcpu/4

  //init touch controller
  tp.init();

  //clear screen
  lcd.clear(COLOR_WHITE);
  
  //touch-panel calibration
  tp.service();
  if(tp.getPressure() > 5)
  {
    tp.doCalibration(&lcd, TP_EEPROMADDR, 0); //dont check EEPROM for calibration data
  }
  else
  {
    tp.doCalibration(&lcd, TP_EEPROMADDR, 1); //check EEPROM for calibration data
  }

  //clear screen
  lcd.clear(COLOR_WHITE);

  //Orientation test
/*
  lcd.setOrientation(0); tp.setOrientation(0);
  lcd.drawText(10, 10, "test", 1, RGB(0,0,0), RGB(255,255,255));
  lcd.setOrientation(90); tp.setOrientation(90);
  lcd.drawText(10, 10, "test", 1, RGB(0,0,0), RGB(255,255,255));
  lcd.setOrientation(180); tp.setOrientation(180);
  lcd.drawText(10, 10, "test", 1, RGB(0,0,0), RGB(255,255,255));
  lcd.setOrientation(270); tp.setOrientation(270);
  lcd.drawText(10, 10, "test", 1, RGB(0,0,0), RGB(255,255,255));
*/

  //DrawRect, drawCircle, drawLine, drawPixel test
  lcd.fillRect( 10,20, 50,60, RGB(255,  0,  0));
  lcd.fillRect( 60,20,100,60, RGB(  0,255,  0));
  lcd.fillRect(110,20,150,60, RGB(  0,  0,255));
  lcd.fillRect(160,20,200,60, RGB(255,255,  0));
  lcd.drawRect( 10,20, 50,60, RGB(0  ,  0,  0));
  lcd.fillCircle( 30,40,10, RGB(100,  0,  0));
  lcd.fillCircle( 80,40,10, RGB(  0,100,  0));
  lcd.fillCircle(130,40,10, RGB(  0,  0,100));
  lcd.fillCircle(180,40,10, RGB(100,100,  0));
  lcd.drawCircle( 30,40,10, RGB(  0,  0,  0));
  lcd.drawLine(10, 20, 50, 60, RGB(  0,  0,  0));
  lcd.drawLine(10, 60, 50, 20, RGB(  0,  0,  0));
  lcd.drawPixel(30, 40, RGB(255,255,255));
  
  //DrawLine test
/*
  for(uint16_t y=0; y < lcd.getHeight(); y+=8)
  {
    lcd.drawLine(0, 0, lcd.getWidth()-1, y, RGB(  0,  0,  0));
  }
  for(uint16_t x=0; x < lcd.getWidth(); x+=8)
  {
    lcd.drawLine(0, 0, x, lcd.getHeight()-1, RGB(  0,  0,  0));
  }
*/

  //DrawText test
/*
  lcd.drawTextPGM(50, 50, PSTR("String from Flash"), 1, COLOR_BLACK, COLOR_WHITE); //string from flash
  lcd.drawText(50, 62, "String from RAM", 1, COLOR_BLACK, COLOR_WHITE);            //string from ram
  lcd.drawText(50, 74, 12345, 1, COLOR_BLACK, COLOR_WHITE);                        //int value
  lcd.drawText(50, 86, 123456789L, 1, COLOR_BLACK, COLOR_WHITE);                   //long value
*/

  //DrawMLText test
/*
  lcd.drawMLText(10, 20, 90, 100, "String from RAM\nnext line 123456789ABCDEF", 1, COLOR_BLACK, COLOR_GREEN); 
  lcd.drawMLTextPGM(100, 20, 180, 100, PSTR("String from FLASH\nnext line 123456789ABCDEF"), 1, COLOR_BLACK, COLOR_RED);
  String test("String from RAM\nnext line 123456789ABCDEF");
  lcd.drawMLText(190, 20, 270, 100, test, 1, COLOR_BLACK, COLOR_YELLOW); 
*/

  //DrawInteger test
/*
  lcd.drawInteger(50, 50, 1234, DEC, 1, COLOR_BLACK, COLOR_WHITE); //dec
  lcd.drawInteger(50, 62, 1234, HEX, 1, COLOR_BLACK, COLOR_WHITE); //hex
  lcd.drawInteger(50, 74, 1234, OCT, 1, COLOR_BLACK, COLOR_WHITE); //oct
  lcd.drawInteger(50, 86, 1234, BIN, 1, COLOR_BLACK, COLOR_WHITE); //bin
*/

  //Print test
/*
  lcd.printOptions(1, COLOR_BLUE, COLOR_WHITE);
  lcd.printClear(); //clear screen
  lcd.printXY(2, 100); //set cursor
  lcd.print("String from RAM\n next line1\n"); //string from ram
  lcd.printPGM(PSTR("String from Flash\n next line2\n")); //string from flash
  lcd.println(12345);        //int
  lcd.println('A');        //char
  lcd.println(123456789L); //long
  lcd.println(98765.43); //float/double
*/

  //Backlight text
  lcd.led(60); //backlight 0...100%
  lcd.drawText( 2, 2, "BL", 1, COLOR_RED, COLOR_WHITE);
  lcd.drawText(20, 2, 60,   1, COLOR_RED, COLOR_WHITE);

  //Calibration text
  lcd.drawText(lcd.getWidth()-30, 2, "CAL", 1, COLOR_RED, COLOR_WHITE);
}


void loop()
{
  char tmp[128];
  static uint16_t last_x=0, last_y=0;
  static uint8_t led=60;
  unsigned long m;
  static unsigned long prevMillis=0;

  //service routine for touch panel
  tp.service();

  //show tp data
  sprintf(tmp, "X:%03i|%04i Y:%03i|%04i P:%03i", tp.getX(), tp.getXraw(), tp.getY(), tp.getYraw(), tp.getPressure());
  lcd.drawText(45, 2, tmp, 1, COLOR_BLACK, COLOR_WHITE);

  if(tp.getPressure() > 3) //touch press?
  {
  
    //change backlight power
    if((tp.getX() < 45) && (tp.getY() < 15))
    {
      m = millis();
      if((m - prevMillis) > 800) //change only every 800ms
      {
        prevMillis = m;

        led += 10;
        if(led > 100)
        {
          led = 10;
        }
        lcd.led(led);
      }
    }

    //calibrate touch panel
    else if((tp.getX() > (lcd.getWidth()-30)) && (tp.getY() < 15))
    {
      tp.doCalibration(&lcd, TP_EEPROMADDR, 0);
    }

    //draw line
    else
    {
      if(last_x == 0)
      {
        lcd.drawPixel(tp.getX(), tp.getY(), COLOR_BLACK);
      }
      else
      {
        lcd.drawLine(last_x, last_y, tp.getX(), tp.getY(), COLOR_BLACK);
      }
      last_x = tp.getX();
      last_y = tp.getY();
    }

    //show backlight power and cal text
    lcd.drawText( 2,                2, "BL    ", 1, COLOR_RED, COLOR_WHITE);
    lcd.drawText(20,                2, led,      1, COLOR_RED, COLOR_WHITE);
    lcd.drawText(lcd.getWidth()-30, 2, "CAL",    1, COLOR_RED, COLOR_WHITE);
  }
  else
  {
    last_x = 0;
  }
}
