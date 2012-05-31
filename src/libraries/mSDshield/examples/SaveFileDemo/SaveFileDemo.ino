/*
  SaveFileDemo (Display + SD-Card)
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


void WriteFile(char *file)
{
  FIL fdst;     //file objects
  FRESULT res;  //result code
  UINT wr;      //file R/W count
  char data[16];

  res = f_open(&fdst, file, FA_CREATE_ALWAYS | FA_WRITE);
  if(res == FR_OK)
  {
    strcpy(data, "test 123");
    res = f_write(&fdst, data, strlen(data), &wr);
    f_close(&fdst);
    return;
  }

  Serial.println("Error!");
  lcd.drawTextPGM(5, 5, PSTR("Error!"), 1, RGB(0,0,0), RGB(255,255,255));
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
    Serial.println("Write file...");
    lcd.clear(RGB(0,255,0));
    lcd.drawTextPGM(5, 5, PSTR("Write file..."), 1, RGB(255,255,255), RGB(0,255,0));
    WriteFile("test_wr.txt");
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
