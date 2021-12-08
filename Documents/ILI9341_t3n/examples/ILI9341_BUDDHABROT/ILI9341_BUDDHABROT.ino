// Buddhabrot
// j.tarbell   January, 2004
// Albuquerque, New Mexico
// complexification.net
//  http://www.complexification.net/gallery/machines/buddhabrot/appletl/index.html
// based on code by Paul Bourke
// astronomy.swin.edu.au/~pbourke/
// j.tarbell   April, 2005
//
/*
WRITE BMP TO SD CARD
Jeff Thompson
Summer 2012
Writes pixel data to an SD card, saved as a BMP file.  Lots of code
via the following...
BMP header and pixel format:
   http://stackoverflow.com/a/2654860
SD save:
   http://arduino.cc/forum/index.php?topic=112733 (lots of thanks!)
... and the SdFat example files too
www.jeffreythompson.org
*/
// Modified to run on Teensy
// MJS     March, 2019

#include <ILI9341_t3n.h> // Hardware-specific library
#include <SD.h>
#include <SPI.h>
#include <ili9341_t3n_font_Arial.h>

#define ILI9341_RST 23 //23 for t4
#define ILI9341_DC 9
#define ILI9341_CS 10
ILI9341_t3n tft = ILI9341_t3n(ILI9341_CS, ILI9341_DC, ILI9341_RST);


const int dim = 240;             // screen dimensions (square window)
int bailout = 200;         // number of iterations before bail
int plots = 10000;        // number of plots to execute per frame (x30 = plots per second)

// 2D array to hold exposure values
byte exposure[dim*dim];
int maxexposure;           // maximum exposure value
int time = 0;
int exposures = 0;

boolean drawing;

//bmp save
char name[] = "9px_0000.bmp";       // filename convention (will auto-increment)
const int w = dim;                   // image width in pixels
const int h = dim;                    // " height
const boolean debugPrint = true;    // print details of process over serial?

const int imgSize = w*h;
// set fileSize (used in bmp header)
int rowSize = 4 * ((3*w + 3)/4);      // how many bytes in the row (used to create padding)
int fileSize = 54 + h*rowSize;        // headers (54 bytes) + pixel data

unsigned char *img = NULL;

File file;
const int cardPin = 18;          // pin that the SD is connected to (d8 for SparkFun MicroSD shield)

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.setFont(Arial_9);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextDatum(TL_DATUM);

  if ( ARM_DWT_CYCCNT == ARM_DWT_CYCCNT ) {
    ARM_DEMCR |= ARM_DEMCR_TRCENA; // T_3.x only needs this
    ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
    Serial.println("CycleCnt Started.");
  }
  
  // SD setup
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(cardPin)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

}

uint32_t CCdiff;
void loop() {
  plotPlots();
  static int Dexposures = 0;
  time++;
  if (time % 30 == 0) {
    // show progress every 2 seconds or so...
    CCdiff = ARM_DWT_CYCCNT;
    findMaxExposure();
    CCdiff = ARM_DWT_CYCCNT - CCdiff;
    renderBrot();
    saveBMP();
    // show exposure value
    // show exposure value
    tft.drawString("bailout:  ", 0, 0);
    tft.drawNumber(bailout, 0, 25);
    tft.drawString("exposures: ", 0, 40);
    tft.drawNumber(exposures, 0, 60);
    tft.drawNumber(exposures - Dexposures, 0, 80);
    tft.drawString("Cycles: ", 0, 100);
    tft.drawNumber(CCdiff, 0, 120);
    Dexposures = exposures;
  }

  if ( exposures > 10000000 ) {
    exposures = 0;
    memset(exposure, 0, sizeof( exposure ));
  }
}

void plotPlots() {
  float x, y;
  // iterate through some plots
  for (int n=0;n<plots;n++) {
    // Choose a random point in same range
    x = randomFloat(-2.0,1.0);
    y = randomFloat(-1.5,1.5);
    if (iterate(x,y,false)) {
      iterate(x,y,true);
      exposures++;
    }
  }
}


//   Iterate the Mandelbrot and return TRUE if the point exits
//   Also handle the drawing of the exit points
boolean iterate(float x0, float y0, boolean drawIt) {
  float x = 0;
  float y = 0;
  float xnew, ynew;
  int ix,iy;

  for (int i=0;i<bailout;i++) {
    xnew = x * x - y * y + x0;
    ynew = 2 * x * y + y0;
    if (drawIt && (i > 3)) {
      ix = int(dim * (xnew + 2.0) / 3.0);
      iy = int(dim * (ynew + 1.5) / 3.0);
      if (ix >= 0 && iy >= 0 && ix < dim && iy < dim) {
        // rotate and expose point
        exposure[ix*dim+iy]++;
      }
    }
    if ((xnew*xnew + ynew*ynew) > 4) {
      // escapes
      return true;
    }
    x = xnew;
    y = ynew;
  }
  // does not escape
  return false;
}

void findMaxExposure() {
  // assume no exposure
  maxexposure=0;
  // find the largest density value
  for (int i=0;i<dim;i++) {
    for (int j=0;j<dim;j++) {
      maxexposure = max(maxexposure,exposure[i*dim+j]);
    }
  }
}


void renderBrot() {
  // draw to screen
  for (int i=0;i<dim;i++) {
    for (int j=0;j<dim;j++) {
      float ramp = exposure[i*dim+j] / (maxexposure / 2.5);
      
      // blow out ultra bright regions
      if (ramp > 3)  {
        ramp = 1;
      }
      uint16_t color = tft.color565(int(ramp*128), int(ramp*128), int(ramp*255));
      tft.drawPixel(j+80, i, color);

      //color c = color(int(ramp*255), int(ramp*255), int(ramp*255));
      //set(j,i,c);
    }
  }
}

double randomFloat(float minf, float maxf)
{
  return minf + random(1UL << 31) * (maxf - minf) / (1UL << 31);  // use 1ULL<<63 for max double values)
}

void saveBMP(){
  // if name exists, create new filename
  for (int i=0; i<10000; i++) {
    name[4] = (i/1000)%10 + '0';    // thousands place
    name[5] = (i/100)%10 + '0';     // hundreds
    name[6] = (i/10)%10 + '0';      // tens
    name[7] = i%10 + '0';           // ones
    file = SD.open(name, O_CREAT | O_EXCL | O_WRITE);
    if (file) {
      break;
    }
  }

  // set fileSize (used in bmp header)
  int rowSize = 4 * ((3*w + 3)/4);      // how many bytes in the row (used to create padding)
  int fileSize = 54 + h*rowSize;        // headers (54 bytes) + pixel data

  img = (unsigned char *)malloc(3*w*h);
  
  for (int i=0;i<dim;i++) {
    for (int j=0;j<dim;j++) {
      float ramp = exposure[i*dim+j] / (maxexposure / 2.5);
      
      // blow out ultra bright regions
      if (ramp > 3)  {
        ramp = 1;
      }
      //uint16_t color = tft.color565(int(ramp*128), int(ramp*128), int(ramp*255));
      //tft.drawPixel(j+80, i, color);
      img[(j*w + i)*3+0] = (unsigned char)(int(ramp*255));    // B
      img[(j*w + i)*3+1] = (unsigned char)(int(ramp*128));    // G
      img[(j*w + i)*3+2] = (unsigned char)(int(ramp*128));    // R
      
      // padding (the 4th byte) will be added later as needed...
      //color c = color(int(ramp*255), int(ramp*255), int(ramp*255));
      //set(j,i,c);
    }
  }

  // create padding (based on the number of pixels in a row
  unsigned char bmpPad[rowSize - 3*w];
  for (int i=0; i<sizeof(bmpPad); i++) {         // fill with 0s
    bmpPad[i] = 0;
  }

  // create file headers (also taken from StackOverflow example)
  unsigned char bmpFileHeader[14] = {            // file header (always starts with BM!)
    'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0   };
  unsigned char bmpInfoHeader[40] = {            // info about the file (size, etc)
    40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0   };

  bmpFileHeader[ 2] = (unsigned char)(fileSize      );
  bmpFileHeader[ 3] = (unsigned char)(fileSize >>  8);
  bmpFileHeader[ 4] = (unsigned char)(fileSize >> 16);
  bmpFileHeader[ 5] = (unsigned char)(fileSize >> 24);

  bmpInfoHeader[ 4] = (unsigned char)(       dim      );
  bmpInfoHeader[ 5] = (unsigned char)(       dim >>  8);
  bmpInfoHeader[ 6] = (unsigned char)(       dim >> 16);
  bmpInfoHeader[ 7] = (unsigned char)(       w >> 24);
  bmpInfoHeader[ 8] = (unsigned char)(       h      );
  bmpInfoHeader[ 9] = (unsigned char)(       h >>  8);
  bmpInfoHeader[10] = (unsigned char)(       h >> 16);
  bmpInfoHeader[11] = (unsigned char)(       h >> 24);

  // write the file (thanks forum!)
  file.write(bmpFileHeader, sizeof(bmpFileHeader));    // write file header
  file.write(bmpInfoHeader, sizeof(bmpInfoHeader));    // " info header

  for (int i=0; i<h; i++) {                            // iterate image array
    file.write(img+(w*(h-i-1)*3), 3*w);                // write px data
    file.write(bmpPad, (4-(w*3)%4)%4);                 // and padding as needed
  }
  free(img);
  file.close();                                        // close file when done writing
}
