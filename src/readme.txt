mSD-Shield Software Package
===========================
  Visit github.com/watterott/msd-shield for updates.


Installation
------------
  Copy the content of /libraries/ to your Arduino lib folder 
  /arduino/libraries/ or to your user documents folder
  /My Documents/Arduino/libraries/
  If there are existing folders from a previous installation,
  please delete them before copying.
  Further infos: http://www.arduino.cc/en/Hacking/Libraries


Documentation
-------------
  See docu.htm


Known issues
------------
  If using the Ethernet-Shield together with the mSD-Shield, 
  this must be initialized before using the SD-Card.

  Normal mSD-Shield, not mSD Mega-Edition:

    For Hardware-SPI support on Mega boards connect the mSD-Shield
    as follows. No Software changes are required.
           Mega   mSD-Shield
      SCK   52   ->   13
      MOSI  51   ->   11
      MISO  50   ->   12

    Or if you dont want to make any pin changes, but use the
    mSD-Shield on the Mega, then Software-SPI has to be enabled in
    the Arduino libs. Uncomment the following lines:
      libraries/ADS7846/ADS7846.cpp:40 (#define SOFTWARE_SPI)
      libraries/MIO283QT2/MIO283QT2.cpp:29 (#define SOFTWARE_SPI)
      libraries/SDcard/mmc.h:8 (#define SD_SOFTWARE_SPI)


Third party software
--------------------
  ChaN's FatFs
    http://elm-chan.org/fsw/ff/00index_e.html

  digitalWriteFast
    http://code.google.com/p/digitalwritefast/


License
-------
  See license.txt


History
-------
         2012  v0.21  New directory struct.

  Mar 09 2012  v0.20  Compatible with Arduino 1.0.
                      Support for ATmega644 added.

  Oct 30 2011  v0.13  TP calibration routine added.
                      Updated examples.

  Sep 04 2011  v0.12  Added drawMLText().
                      Added standard print().
                      Updated FatFS to 0.08b.
  
  Aug 23 2011  v0.11  Docu updated.
                      Minor changes.

  Apr 11 2011  v0.10  Bug in ADS7846 lib fixed.

  Mar 12 2011  v0.09  Added DS1307 lib.

  Jan 24 2011  v0.08  Using digitalWriteFast for
                      pin/port access.

  Jan 08 2011  v0.07  Updated FatFS to 0.08a.
                      Demo2 with LFN support.
                      New fonts.

  Nov 28 2010  v0.06  New example: BMPDemo

  Nov 21 2010  v0.05  Bugfix release.

  Nov 20 2010  v0.04  String support added.

  Nov 07 2010  v0.03  Updated docu and examples.

  Oct 24 2010  v0.02  Arduino Mega support added.
                      More examples.

  Oct 14 2010  v0.01  First release.
