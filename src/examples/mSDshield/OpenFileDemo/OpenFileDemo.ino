/*
  OpenFileDemo (Display + SD-Card)
 */

#include <MI0283QT2.h>
#include <SDcard.h>

MI0283QT2 lcd;
SDcard sd;


ISR(TIMER2_OVF_vect) //Timer 2 Interrupt, called every 1ms
{
  static uint8_t count=1;

  TCNT2 -= 250; //1000 Hz

  if(--count == 0) //100 Hz
  {
    count = 10;
    sd.service();
  }
}


void ReadFile(char *file)
{
  FIL fsrc;     //file objects
  FRESULT res;  //result code
  char buf[32]; //file buffer
  UINT rd;      //file R/W count
  
  lcd.clear(RGB(255,255,255));

  res = f_open(&fsrc, file, FA_OPEN_EXISTING | FA_READ);
  if(res == FR_OK)
  {
    res = f_read(&fsrc, buf, sizeof(buf), &rd);
    if(res == FR_OK)
    {
      if(rd >=sizeof(buf))
      {
        rd = sizeof(buf)-1;
      }
      buf[rd] = 0;
      lcd.drawText(5, 5, buf, 1, RGB(0,0,0), RGB(255,255,255));
      return;
    }
    f_close(&fsrc);
  }

  Serial.println("File not found!");
  lcd.drawTextPGM(5, 5, PSTR("File not found!"), 1, RGB(0,0,0), RGB(255,255,255));
}


void setup()
{
  //init Serial port
  Serial.begin(38400); 

  //init LCD
  Serial.println("Init Display...");
  lcd.init(4); //spi-clk = Fcpu/4
  lcd.led(60); //backlight 0...100%

  //init SD-Card
  Serial.println("Init SD-Card...");
  sd.init(4); //spi-clk = Fcpu/4

  //init Timer2 for SD service routine
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt
  
  //interrupts on
  sei();

  //clear screen
  lcd.clear(RGB(255,255,255));
  lcd.drawTextPGM(5, 5, PSTR("Mount card..."), 1, RGB(0,0,0), RGB(255,255,255));

  if(sd.mount() == 0)
  {
    Serial.println("Read file...");
    lcd.clear(RGB(0,255,0));
    lcd.drawTextPGM(5, 5, PSTR("Read file..."), 1, RGB(255,255,255), RGB(0,255,0));
    ReadFile("test.txt");
    sd.unmount();
  }
  else
  {
    Serial.println("No card found!");
    lcd.clear(RGB(255,0,0));
    lcd.drawTextPGM(5, 5, PSTR("No card!"), 1, RGB(255,255,255), RGB(255,0,0));
  }
}


void loop()
{

}
