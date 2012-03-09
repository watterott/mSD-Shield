#ifndef SDcard_h
#define SDcard_h


#ifdef __cplusplus
extern "C" {
#endif
  #include <inttypes.h>
  #include "ff.h"
#ifdef __cplusplus
}
#endif


//for pin defination see  mmc.h


class SDcard
{
  public:
    SDcard();
    void init(uint8_t clock_div);
    void service(void);
    uint8_t mount(void);
    void unmount(void);

  private:
    FATFS fatfs;
};


#endif //SDcard_h
