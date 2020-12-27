This is a library for the touch screen.
This library support these touch IC:
HR2046
XPT2046
 
Check out the file of LCDWIKI TOUCH lib Requirements for our tutorials and wiring diagrams.

These touch screen use 4-wire spi to communicate, 5 pins are required to interface (Need to add the TP_IRQ pin).
Run the touch screen calibration program and put the calibration parameter into the file of cali_para.h after the calibration has passed.

MIT license, all text above must be included in any redistribution

To download. click the DOWNLOADS button in the top right corner, rename the uncompressed folder LCDWIKI_TOUCH. Check that the LCDWIKI_TOUCH folder contains LCDWIKI_TOUCH.cpp and LCDWIKI_TOUCH.h

Place the LCDWIKI_TOUCH library folder your <arduinosketchfolder>/libraries/ folder. You may need to create the libraries subfolder if its your first library. Restart the IDE

Also requires the LCDWIKI_GUI and LCDWIKI_KBV library for Arduino. 
https://github.com/lcdwiki/LCDWIKI_gui
https://github.com/lcdwiki/LCDWIKI_kbv


