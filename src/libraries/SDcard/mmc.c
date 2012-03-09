/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN                 */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/


#include <avr/io.h>
#include "integer.h"
#include "ff.h"
#include "diskio.h"
#include "mmc.h"
#include "../digitalWriteFast/digitalWriteFast.h"


#ifdef SD_PWR_PIN
# define SD_POWER_OFF()      digitalWriteFast(SD_PWR_PIN, 1)
# define SD_POWER_ON()       digitalWriteFast(SD_PWR_PIN, 0)
#endif

#define SD_DESELECT()        digitalWriteFast(SD_CS_PIN, 1)
#define SD_SELECT()          digitalWriteFast(SD_CS_PIN, 0)

#define SD_MOSI_HIGH()       digitalWriteFast(SD_MOSI_PIN, 1)
#define SD_MOSI_LOW()        digitalWriteFast(SD_MOSI_PIN, 0)

#define SD_MISO_READ()       digitalReadFast(SD_MISO_PIN)

#define SD_CLK_HIGH()        digitalWriteFast(SD_CLK_PIN, 1)
#define SD_CLK_LOW()         digitalWriteFast(SD_CLK_PIN, 0)


static volatile DSTATUS Stat = STA_NOINIT; //Disk status 

static volatile BYTE Timer1, Timer2; //100Hz decrement timer

static BYTE CardType; //b0:MMC, b1:SDC, b2:Block addressing


/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

#if defined(SD_SOFTWARE_SPI)
static void xmit_spi(BYTE data)
{
  BYTE mask;

  for(mask=0x80; mask!=0; mask>>=1)
  {
    SD_CLK_LOW();
    if(mask & data)
    {
      SD_MOSI_HIGH();
    }
    else
    {
      SD_MOSI_LOW();
    }
    SD_CLK_HIGH();
  }
  SD_CLK_LOW();
  return;
}
#else
# define xmit_spi(dat) SPDR=(dat); loop_until_bit_is_set(SPSR,SPIF)
#endif


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static BYTE rcvr_spi(void)
{
#if defined(SD_SOFTWARE_SPI)
  BYTE bit, data;

  SD_MOSI_HIGH();
  for(data=0, bit=8; bit!=0; bit--)
  {
    SD_CLK_HIGH();
    data <<= 1;
    if(SD_MISO_READ())
    {
    data |= 1;
    }
    else
    {
      //data |= 0;
    }
    SD_CLK_LOW();
  }
  return data;
#else
  SPDR = 0xFF;
  loop_until_bit_is_set(SPSR, SPIF);
  return SPDR;
#endif
}


#if defined(SD_SOFTWARE_SPI)
static void rcvr_spi_m(BYTE *dst)
{
  BYTE bit, data;

  SD_MOSI_HIGH();
  for(data=0, bit=8; bit!=0; bit--)
  {
    SD_CLK_HIGH();
    data <<= 1;
    if(SD_MISO_READ())
    {
      data |= 1;
    }
    else
    {
      //data |= 0;
    }
    SD_CLK_LOW();
  }
  *dst = data;
  return;
}
#else
# define rcvr_spi_m(dst) SPDR=0xFF; loop_until_bit_is_set(SPSR,SPIF); *(dst)=SPDR
#endif


/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static BYTE wait_ready(void)
{
  BYTE res;

  Timer2 = 50; //Wait for ready in timeout of 500ms
  rcvr_spi();
  do
  {
    res = rcvr_spi();
  }while((res != 0xFF) && Timer2);

  return res;
}


/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static void release_spi(void)
{
  SD_DESELECT();
  rcvr_spi();

  return;
}


/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static void power_on(void)
{
#ifdef SD_PWR_PIN
	SD_POWER_ON();
#endif
	SD_DESELECT();
	for (Timer1 = 5; Timer1; );	//Wait for 50ms

  return;
}


static void power_off(void)
{
  SD_SELECT();
  wait_ready();
  release_spi();
/*
#ifdef SD_PWR_PIN
  SPCR = 0x00;                  //spi off
  SD_CLK_LOW();
  SD_MOSI_LOW();
  SD_SELECT();                  //CS -> low
  SD_POWER_OFF();               //power off
#endif
*/
  Stat |= STA_NOINIT;

  return;
}


static BYTE chk_power(void) //Socket power state: 0=off, 1=on
{
  return 1;
}


/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static BYTE rcvr_datablock(BYTE *buff, UINT btr)
{
  BYTE token;

  Timer1 = 20; //Wait for data packet in timeout of 200ms
  do
  {
    token = rcvr_spi();
  }while((token == 0xFF) && Timer1);
  //If not valid data token, retutn with error
  if(token != 0xFE)
  {
    return 0;
  }

  //Receive the data block into buffer
  do
  {
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
    rcvr_spi_m(buff++);
  }while(btr -= 4);

  //Discard CRC
  rcvr_spi();
  rcvr_spi();

  return 1;  //Return with success
}


/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
static BYTE xmit_datablock(const BYTE *buff, BYTE token)
{
  BYTE resp, wc;

  if(wait_ready() != 0xFF)
  {
    return 0;
  }

  xmit_spi(token); //Xmit data token
  if(token != 0xFD) //Is data token
  {  
    wc = 0;
    //Xmit the 512 byte data block to MMC
    do
    {
      xmit_spi(*buff++);
      xmit_spi(*buff++);
    }while (--wc);
    //CRC (Dummy)
    xmit_spi(0xFF);
    xmit_spi(0xFF);
    //Receive data response
    resp = rcvr_spi(); 
    if((resp & 0x1F) != 0x05)
    {
      return 0;
    }
  }

  return 1;
}
#endif /* _READONLY */


/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static BYTE send_cmd(BYTE cmd, DWORD arg)
{
  BYTE n, res;

  //ACMD<n> is the command sequense of CMD55-CMD<n>
  if(cmd & 0x80)
  {
    cmd &= 0x7F;
    res = send_cmd(CMD55, 0);
    if(res > 1){ return res; }
  }

  //Select the card and wait for ready
  SD_DESELECT();
  SD_SELECT();
  if(wait_ready() != 0xFF)
  {
    SD_DESELECT();
    return 0xFF;
  }

  //Send command packet
  xmit_spi(cmd);                //Start + Command index
  xmit_spi((BYTE)(arg >> 24)); //Argument[31..24]
  xmit_spi((BYTE)(arg >> 16)); //Argument[23..16]
  xmit_spi((BYTE)(arg >> 8));  //Argument[15..8]
  xmit_spi((BYTE)arg);         //Argument[7..0]
  n = 0x01;                     //Dummy CRC + Stop
  if(cmd == CMD0){ n = 0x95; }  //Valid CRC for CMD0(0)
  if(cmd == CMD8){ n = 0x87; }  //Valid CRC for CMD8(0x1AA)
  xmit_spi(n);

  //Receive command response
  if(cmd == CMD12){ rcvr_spi(); } //Skip a stuff byte when stop reading
  n = 10; //Wait for a valid response in timeout of 10 attempts
  do
  {
    res = rcvr_spi();
  }while((res & 0x80) && --n);

  return res;  //Return with the response value
}


/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE drv)
{
  BYTE n, cmd, ty, ocr[4], init_try;
#if !defined(SD_SOFTWARE_SPI)
  BYTE spcr, spsr;
#endif

  if(drv)              { return STA_NOINIT; } //Supports only single drive
  if(Stat & STA_NODISK){ return Stat;       } //No card in the socket

#if defined(SD_SOFTWARE_SPI)
# ifdef SD_PWR_PIN
  SD_CLK_LOW();
  SD_MOSI_LOW();
  SD_SELECT();                  //CS -> low
	SD_POWER_OFF();               //power off
	for (Timer1 = 10; Timer1; );	//Wait for 100ms
# endif
  power_on();                   //Force socket power on
#else
  //save spi settings
  spcr = SPCR;
  spsr = SPSR;

# ifdef SD_PWR_PIN
  SPCR = 0x00;                  //spi off
  SD_CLK_LOW();
  SD_MOSI_LOW();
  SD_SELECT();                  //CS -> low
  SD_POWER_OFF();               //power off
  for (Timer1 = 10; Timer1; );  //Wait for 100ms
# endif
  power_on();                   //Force socket power on

  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1); //SPI mode 0, clk=Fcpu/128
  SPSR = (0<<SPI2X);
#endif

  //80 dummy clocks
  for(n = 10; n; n--){ rcvr_spi(); }
  
  init_try = 3;
  do
  {
    ty = 0;
    if(send_cmd(CMD0, 0) == 1) //Enter Idle state
    {
      Timer1 = 100; //Initialization timeout of 1000ms
      if(send_cmd(CMD8, 0x1AA) == 1) //SD v2 ?
      {
        for(n=0; n < 4; n++){ ocr[n] = rcvr_spi(); } //Get trailing return value of R7 resp
        //if((ocr[2] == 0x01) && (ocr[3] == 0xAA)) //The card can work at vdd range of 2.7-3.6V
        //{
          while(Timer1 && send_cmd(ACMD41, 1UL<<30)); // Wait for leaving idle state (ACMD41 with HCS bit)
          if(Timer1 && (send_cmd(CMD58, 0) == 0)) //Check CCS bit in the OCR
          {
            for(n=0; n < 4; n++){ ocr[n] = rcvr_spi(); }
            ty = (ocr[0]&0x40) ? CT_SD2|CT_BLOCK : CT_SD2; //SD v2
          }
        //}
      }
      else //SD v1 or MMC v3
      {
        if(send_cmd(ACMD41, 0) <= 1)
        {
          ty = CT_SD1; cmd = ACMD41; //SD v1
        }
        else
        {
          ty = CT_MMC; cmd = CMD1; //MMC v3
        }
        while(Timer1 && send_cmd(cmd, 0)); //Wait for leaving idle state
        if(!Timer1 || send_cmd(CMD16, 512) != 0) //Set R/W block length to 512 
        {
          ty = 0;
        }
      }
    }
  }while((ty == 0) && --init_try);

  CardType = ty;
  release_spi();

#if !defined(SD_SOFTWARE_SPI)
  //restore spi settings
  SPCR = spcr;
  if(spsr & (1<<SPI2X))
  {
    SPSR = (1<<SPI2X);
  }
#endif

  if(ty) //Initialization succeded
  {
    Stat &= ~STA_NOINIT; //Clear STA_NOINIT
  }
  else //Initialization failed
  {
    power_off();
  }

  return Stat;
}


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE drv)
{
  if(drv){ return STA_NOINIT; } //Supports only single drive

  return Stat;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
  if(drv || !count)    { return RES_PARERR; }
  if(Stat & STA_NOINIT){ return RES_NOTRDY; }

  if(!(CardType & CT_BLOCK)){ sector *= 512; } //Convert to byte address if needed

  if(count == 1) //Single block read
  {
    if((send_cmd(CMD17, sector) == 0) && rcvr_datablock(buff, 512))
    {
      count = 0;
    }
  }
  else //Multiple block read
  {
    if(send_cmd(CMD18, sector) == 0)
    {
      do
      {
        if(!rcvr_datablock(buff, 512))
        {
          break;
        }
        buff += 512;
      }while(--count);
      send_cmd(CMD12, 0); //STOP_TRANSMISSION
    }
  }
  release_spi();

  return count ? RES_ERROR : RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
  if(drv || !count)     { return RES_PARERR; }
  if(Stat & STA_NOINIT) { return RES_NOTRDY; }
  if(Stat & STA_PROTECT){ return RES_WRPRT;  }

  if(!(CardType & CT_BLOCK)){ sector *= 512; }//Convert to byte address if needed

  if(count == 1) //Single block write
  {
    if((send_cmd(CMD24, sector) == 0) && xmit_datablock(buff, 0xFE))
    {
      count = 0;
    }
  }
  else //Multiple block write
  {
    if(CardType & CT_SDC)
    {
      send_cmd(ACMD23, count);
    }
    if(send_cmd(CMD25, sector) == 0)
    {
      do
      {
        if(!xmit_datablock(buff, 0xFC))
        {
          break;
        }
        buff += 512;
      }while(--count);
      if(!xmit_datablock(0, 0xFD)) //STOP_TRAN token
      {
        count = 1;
      }
    }
  }
  release_spi();

  return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY == 0 */


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
  DRESULT res;
  BYTE n, csd[16], *ptr = buff;
  WORD csize;

  if(drv){ return RES_PARERR; }

  res = RES_ERROR;

  if(ctrl == CTRL_POWER)
  {
    switch(ptr[0])
    {
      case 0: //Sub control code (POWER_OFF)
        if(chk_power())
        {
          power_off();
        }
        res = RES_OK;
        break;
      case 1: //Sub control code (POWER_ON)
        power_on();
        res = RES_OK;
        break;
      case 2: //Sub control code (POWER_GET)
        ptr[1] = (BYTE)chk_power();
        res = RES_OK;
        break;
      default :
        res = RES_PARERR;
    }
  }
  else
  {
    if(Stat & STA_NOINIT){ return RES_NOTRDY; }
    switch (ctrl)
    {
      case CTRL_SYNC: //Make sure that no pending write process
        SD_SELECT();
        if(wait_ready() == 0xFF)
        {
          res = RES_OK;
        }
        else
        {
          release_spi();
        }
        break;

      case GET_SECTOR_COUNT: //Get number of sectors on the disk (DWORD)
        if((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16))
        {
          if((csd[0] >> 6) == 1) //SDC ver 2.00
          {
            csize = csd[9] + ((WORD)csd[8] << 8) + 1;
            *(DWORD*)buff = (DWORD)csize << 10;
          }
          else //SDC ver 1.XX or MMC
          {
            n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
            csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
            *(DWORD*)buff = (DWORD)csize << (n - 9);
          }
          res = RES_OK;
        }
        break;

      case GET_SECTOR_SIZE: //Get R/W sector size (WORD)
        *(WORD*)buff = 512;
        res = RES_OK;
        break;

      case GET_BLOCK_SIZE: //Get erase block size in unit of sector (DWORD)
        if(CardType & CT_SD2) //SDC ver 2.00
        {
          if(send_cmd(ACMD13, 0) == 0) //Read SD status
          {
            rcvr_spi();
            if(rcvr_datablock(csd, 16)) //Read partial block
            {
              for(n=64 - 16; n; n--){ rcvr_spi(); } //Purge trailing data
              *(DWORD*)buff = 16UL << (csd[10] >> 4);
              res = RES_OK;
            }
          }
        }
        else //SDC ver 1.XX or MMC
        {
          if((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) //Read CSD
          {
            if(CardType & CT_SD1) //SDC ver 1.XX
            {
              *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
            }
            else //MMC
            {
              *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
            }
            res = RES_OK;
          }
        }
        break;

      case MMC_GET_TYPE: //Get card type flags (1 byte)
        *ptr = CardType;
        res = RES_OK;
        break;

      case MMC_GET_CSD: //Receive CSD as a data block (16 bytes)
        if((send_cmd(CMD9, 0) == 0) && rcvr_datablock(ptr, 16))
        {
          res = RES_OK;
        }
        break;

      case MMC_GET_CID: //Receive CID as a data block (16 bytes)
        if((send_cmd(CMD10, 0) == 0) && rcvr_datablock(ptr, 16))
        {
          res = RES_OK;
        }
        break;

      case MMC_GET_OCR: //Receive OCR as an R3 resp (4 bytes)
        if(send_cmd(CMD58, 0) == 0)
        {
          for(n=4; n; n--){ *ptr++ = rcvr_spi(); }
          res = RES_OK;
        }
        break;

      case MMC_GET_SDSTAT: //Receive SD status as a data block (64 bytes)
        if(send_cmd(ACMD13, 0) == 0)
        {
          rcvr_spi();
          if(rcvr_datablock(ptr, 64))
          {
            res = RES_OK;
          }
        }
        break;

      default:
        res = RES_PARERR;
    }

    release_spi();
  }

  return res;
}
#endif /* _USE_IOCTL != 0 */


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc(void)
{
  BYTE n;

  n = Timer1;
  if(n){ Timer1 = --n; }
  n = Timer2;
  if(n){ Timer2 = --n; }

  return;
}


/*-----------------------------------------------------------------------*/
/* User Provided Timer Function for FatFs module                         */
/*-----------------------------------------------------------------------*/

DWORD get_fattime(void)
{
  return (((DWORD)(2009-1980)) << 25) | // Year
         (((DWORD)          6) << 21) | // Month
         (((DWORD)         27) << 16) | // Day
         (((DWORD)         12) << 11) | // Hour
         (((DWORD)          0) <<  5) | // Min
         (((DWORD)          0) >>  1);  // Sec
}
