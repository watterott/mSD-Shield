#ifndef mmc_h_
#define mmc_h_


#define SD_PINS_MSD_SHIELD
//#define SD_PINS_S65_SHIELD

//#define SD_SOFTWARE_SPI


//=== mSD-Shield Pins ===
#ifdef SD_PINS_MSD_SHIELD
#if (defined(__AVR_ATmega1280__) || \
     defined(__AVR_ATmega1281__) || \
     defined(__AVR_ATmega2560__) || \
     defined(__AVR_ATmega2561__))      //--- Arduino Mega ---

# define SD_CS_PIN           (4) //4 or 10
# if defined(SD_SOFTWARE_SPI)
#  define SD_MOSI_PIN        (11)
#  define SD_MISO_PIN        (12)
#  define SD_CLK_PIN         (13)
# else
#  define SD_MOSI_PIN        (51)
#  define SD_MISO_PIN        (50)
#  define SD_CLK_PIN         (52)
# endif

#elif (defined(__AVR_ATmega644__) || \
       defined(__AVR_ATmega644P__))    //--- Arduino 644 ---

# define SD_PWR_PIN          (12)
# define SD_CS_PIN           (3)
# define SD_MOSI_PIN         (5)
# define SD_MISO_PIN         (6)
# define SD_CLK_PIN          (7)

#else                                  //--- Arduino Uno ---

# define SD_CS_PIN           (4) //4 or 10
# define SD_MOSI_PIN         (11)
# define SD_MISO_PIN         (12)
# define SD_CLK_PIN          (13)

#endif
#endif //SD_PINS_MSD_SHIELD


//=== S65-Shield Pins ===
#ifdef SD_PINS_S65_SHIELD
#if (defined(__AVR_ATmega1280__) || \
     defined(__AVR_ATmega1281__) || \
     defined(__AVR_ATmega2560__) || \
     defined(__AVR_ATmega2561__))      //--- Arduino Mega ---

# define SD_SOFTWARE_SPI               //!!! use Software-SPI (default for Arduino Mega)

# define SD_PWR_PIN          (8)
# define SD_CS_PIN           (9)

# if defined(SD_SOFTWARE_SPI)
#  define SD_MOSI_PIN         (11)
#  define SD_MISO_PIN         (12)
#  define SD_CLK_PIN          (13)
# else
#  define SD_MOSI_PIN         (51)
#  define SD_MISO_PIN         (50)
#  define SD_CLK_PIN          (52)
# endif

#elif (defined(__AVR_ATmega644__) || \
       defined(__AVR_ATmega644P__))    //--- Arduino 644 ---

# define SD_PWR_PIN          (12)
# define SD_CS_PIN           (3)
# define SD_MOSI_PIN         (5)
# define SD_MISO_PIN         (6)
# define SD_CLK_PIN          (7)

#else                                  //--- Arduino Uno ---

# define SD_PWR_PIN          (8)
# define SD_CS_PIN           (9)
# define SD_MOSI_PIN         (11)
# define SD_MISO_PIN         (12)
# define SD_CLK_PIN          (13)

#endif
#endif //SD_PINS_S65_SHIELD


//MMC/SD command
#define CMD0                 (0x40+ 0) //GO_IDLE_STATE
#define CMD1                 (0x40+ 1) //SEND_OP_COND
#define	ACMD41               (0xC0+41) //SEND_OP_COND (SDC)
#define CMD8                 (0x40+ 8) //SEND_IF_COND
#define CMD9                 (0x40+ 9) //SEND_CSD
#define CMD10                (0x40+10) //SEND_CID
#define CMD12                (0x40+12) //STOP_TRANSMISSION
#define ACMD13               (0xC0+13) //SD_STATUS (SDC)
#define CMD16                (0x40+16) //SET_BLOCKLEN
#define CMD17                (0x40+17) //READ_SINGLE_BLOCK
#define CMD18                (0x40+18) //READ_MULTIPLE_BLOCK
#define CMD23                (0x40+23) //ET_BLOCK_COUNT
#define	ACMD23               (0xC0+23) //SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24                (0x40+24) //WRITE_BLOCK
#define CMD25                (0x40+25) //WRITE_MULTIPLE_BLOCK
#define CMD41                (0x40+41) //SEND_OP_COND (ACMD)
#define CMD55                (0x40+55) //APP_CMD
#define CMD58                (0x40+58) //READ_OCR

//Card type flags
#define CT_MMC               (0x01)
#define CT_SD1               (0x02)
#define CT_SD2               (0x04)
#define CT_SDC               (CT_SD1|CT_SD2)
#define CT_BLOCK             (0x08)

//Prototypes
//DSTATUS                      disk_initialize(UCHAR drv);
//DSTATUS                      disk_status (UCHAR drv);
//DRESULT                      disk_read (UCHAR drv, UCHAR *buff, DWORD sector, UCHAR count);
//DRESULT                      disk_write(UCHAR drv, const UCHAR *buff, DWORD sector, UCHAR count);
//DRESULT                      disk_ioctl(UCHAR drv, UCHAR ctrl, void *buff);
void                         disk_timerproc(void);
DWORD                        get_fattime(void);


#endif //mmc_h_
