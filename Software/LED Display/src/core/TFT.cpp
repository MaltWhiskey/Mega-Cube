#include "tft.h"

#include <Arduino.h>
#include <ILI9341_t3.h>
#include <JPEGDecoder.h>
#include <SD.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <font_Arial.h>
/*------------------------------------------------------------------------------
 * TFT CLASS
 *----------------------------------------------------------------------------*/
//#define TIRQ_PIN TFT::TOUCH_IRQ
XPT2046_Touchscreen ts(TFT::TOUCH_CS);
ILI9341_t3 tft = ILI9341_t3(TFT::TFT_CS, TFT::TFT_DC);
boolean wastouched = true;
Sd2Card card;
SdVolume volume;
SdFile root;
File root2;
void setupcard();
void drawSdJpeg(const char *, int, int);
void showTime(uint32_t msTime);

void TFT::loop() {
  boolean istouched = (ts.touched() && ts.getPoint().z > 1100);
  if (istouched) {
    TS_Point p = ts.getPoint();
    if (!wastouched) {
      tft.fillScreen(ILI9341_BLACK);
      tft.setTextColor(ILI9341_YELLOW);
      tft.setFont(Arial_60);
      tft.setCursor(60, 80);
      tft.print("Touch");
    }
    tft.fillRect(100, 150, 140, 60, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setFont(Arial_24);
    tft.setCursor(100, 150);
    tft.print("X = ");
    tft.print(p.x);
    tft.setCursor(100, 180);
    tft.print("Y = ");
    tft.print(p.y);
    Serial.print(", x = ");
    Serial.print(p.x);
    Serial.print(", y = ");
    Serial.print(p.y);
    Serial.print(", z = ");
    Serial.println(p.z);
    drawSdJpeg("Cat.jpg", 0, 0);
  } else {
    if (wastouched) {
      tft.fillScreen(ILI9341_BLACK);
      tft.setTextColor(ILI9341_RED);
      tft.setFont(Arial_48);
      tft.setCursor(120, 50);
      tft.print("No");
      tft.setCursor(80, 120);
      tft.print("Touch");
      drawSdJpeg("Cute.jpg", 0, 0);
    }
    // Serial.println("no touch");
  }
  wastouched = istouched;
  delay(100);
}

void printDirectory(File dir, int numTabs);

void TFT::begin() {
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, LOW);
  pinMode(TFT_RESET, OUTPUT);
  digitalWrite(TFT_RESET, LOW);
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  pinMode(TOUCH_CS, OUTPUT);
  digitalWrite(TOUCH_CS, HIGH);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  // setupcard();

  // if (!SD.begin(SD_CS)) {
  //  Serial.println("initialization failed.");
  // }
  //  root2 = SD.open("/");
  //  printDirectory(root2, 0);..

  digitalWrite(TFT_RESET, HIGH);
  delay(100);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);

  ts.begin();
  ts.setRotation(1);
  digitalWrite(TFT_LED, HIGH);
  digitalWrite(TFT_RESET, HIGH);

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_PINK);
  tft.setFont(Arial_48);
  tft.setCursor(60, 50);
  tft.print("Calista");
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(60, 120);
  tft.print("Jarig!");
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

/*******************/
// set up variables using the SD utility library functions:

void setupcard() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(TFT::SD_CS, OUTPUT);

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  while (!card.init(SPI_FULL_SPEED,
                    TFT::SD_CS)) {  // this alternate init WORKS OK
    // while (!card.init(SPI_HALF_SPEED, TFT::SD_CS)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println(
        "* did you change the chipSelect pin to match your shield or "
        "module?");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or
  // FAT32
  if (!volume.init(card)) {
    Serial.println(
        "Could not find FAT16/FAT32 partition.\nMake sure you've formatted "
        "the "
        "card");
    return;
  }

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();  // clusters are collections of blocks
  volumesize *= volume.clusterCount();     // we'll have a lot of clusters
  volumesize *= 512;  // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if
// they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For
// convenience images that do not fit totally on the screen are cropped to the
// nearest MCU size and may leave right/bottom borders.
void jpegRender(int xpos, int ypos) {
  // jpegInfo(); // Print information from the JPEG file (could comment this
  // line out)

  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  uint32_t min_w = min(mcu_w, max_x % mcu_w);
  uint32_t min_h = min(mcu_h, max_y % mcu_h);

  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  uint32_t drawTime = millis();

  max_x += xpos;
  max_y += ypos;

  // Fetch data from the file, decode and display
  while (JpegDec.read()) {  // While there is more data in the file
    pImg = JpegDec.pImage;  // Decode a MCU (Minimum Coding Unit, typically a
                            // 8x8 or 16x16 pixel block)

    int mcu_x =
        JpegDec.MCUx * mcu_w +
        xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    if (mcu_x + mcu_w <= max_x)
      win_w = mcu_w;
    else
      win_w = min_w;
    if (mcu_y + mcu_h <= max_y)
      win_h = mcu_h;
    else
      win_h = min_h;

    uint32_t mcu_pixels = win_w * win_h;

    if ((mcu_x + win_w) <= tft.width() && (mcu_y + win_h) <= tft.height()) {
      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x
      // + width - 1, y + height - 1)
      // tft.setWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);

      // Push all MCU pixels to the TFT window
      while (mcu_pixels--) {
        // Push each pixel to the TFT MCU area
        tft.pushColor(*pImg++);
      }

      // Push all MCU pixels to the TFT window, ~18% faster to pass an array
      // pointer and length to the library tft.pushColor16(pImg, mcu_pixels);
      // // To be supported in HX8357 library at a future date

    } else if ((mcu_y + win_h) >= tft.height())
      JpegDec.abort();  // Image has run off bottom of screen so abort decoding
  }

  showTime(millis() - drawTime);  // These lines are for sketch testing only
  // Serial.print(" Draw count:");
  // Serial.println(icount++);
}

//####################################################################################################
// Print image information to the serial port (optional)
//####################################################################################################
// JpegDec.decodeFile(...) or JpegDec.decodeArray(...) must be called before
// this info is available!
void jpegInfo() {
  // Print information extracted from the JPEG file
  Serial.println("JPEG image info");
  Serial.println("===============");
  Serial.print("Width      :");
  Serial.println(JpegDec.width);
  Serial.print("Height     :");
  Serial.println(JpegDec.height);
  Serial.print("Components :");
  Serial.println(JpegDec.comps);
  Serial.print("MCU / row  :");
  Serial.println(JpegDec.MCUSPerRow);
  Serial.print("MCU / col  :");
  Serial.println(JpegDec.MCUSPerCol);
  Serial.print("Scan type  :");
  Serial.println(JpegDec.scanType);
  Serial.print("MCU width  :");
  Serial.println(JpegDec.MCUWidth);
  Serial.print("MCU height :");
  Serial.println(JpegDec.MCUHeight);
  Serial.println("===============");
  Serial.println("");
}

//####################################################################################################
// Show the execution time (optional)
//####################################################################################################
// WARNING: for UNO/AVR legacy reasons printing text to the screen with the
// Mega might not work for sketch sizes greater than ~70KBytes because 16 bit
// address pointers are used in some libraries.

// The Due will work fine with the HX8357_Due library.

void showTime(uint32_t msTime) {
  // tft.setCursor(0, 0);
  // tft.setTextFont(1);
  // tft.setTextSize(2);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // tft.print(F(" JPEG drawn in "));
  // tft.print(msTime);
  // tft.println(F(" ms "));
  Serial.print(F(" JPEG drawn in "));
  Serial.print(msTime);
  Serial.println(F(" ms "));
}

//####################################################################################################
// Pull a jpeg file off the SD Card and send it as a "C" formatted as an array
// to the serial port
//####################################################################################################
// The array can be cut and pasted from the Serial Monitor window into jpegX.h
// attached to this sketch
void createArray(const char *filename) {
  File jpgFile;
  uint8_t sdbuffer[32];  // SD read pixel buffer (16 bits per pixel)

  // Check file exists and open it
  if ((jpgFile = SD.open(filename)) == NULL) {
    Serial.println(F("JPEG file not found"));
    return;
  }

  uint8_t data;
  byte line_len = 0;

  Serial.print("const uint8_t ");
  // Make the array the same as the file name with the .(file extension)
  // removed
  while (*filename != '.') Serial.print(*filename++);

  Serial.println("[] PROGMEM = {");  // PROGMEM added for AVR processors, it
                                     // is ignored by Due
  // Pull all data falues from file and print in the array format
  while (jpgFile.available()) {
    data = jpgFile.read();
    Serial.print("0x");  // Add hexadecimal prefix
    if (abs(data) < 16)
      Serial.print("0");  // Add a leading zero to create a neater array
    Serial.print(data, HEX);
    Serial.print(",");  // Add value and comma
    line_len++;
    // Add a newline every 32 bytes
    if (line_len >= 32) {
      line_len = 0;
      Serial.println();
    }
  }
  Serial.println("};");
  Serial.println();
  // close the file:
  jpgFile.close();
}

//####################################################################################################
// Draw a JPEG on the TFT pulled from SD Card
//####################################################################################################
// xpos, ypos is top left corner of plotted image
void drawSdJpeg(const char *filename, int xpos, int ypos) {
  // Open the named file (the Jpeg decoder library will close it)
  File jpegFile =
      SD.open(filename, FILE_READ);  // or, file handle reference for SD library

  if (!jpegFile) {
    Serial.print("ERROR: File \"");
    Serial.print(filename);
    Serial.println("\" not found!");
    return;
  }

  Serial.println("===========================");
  Serial.print("Drawing file: ");
  Serial.println(filename);
  Serial.println("===========================");

  // Use one of the following methods to initialise the decoder:
  boolean decoded = JpegDec.decodeSdFile(
      jpegFile);  // Pass the SD file handle to the decoder,
  // boolean decoded = JpegDec.decodeSdFile(filename);  // or pass the
  // filename (String or character array)

  if (decoded) {
    // print information about the image to the serial port
    jpegInfo();
    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  } else {
    Serial.println("Jpeg file format not supported!");
  }
}
