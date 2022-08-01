/********************************************************************
*
* ILI9341_T4 library example. vsync and screen tearing demo.
*
* This example demonstrates the effect of vsync on screen tearing.
* At each frame, a disk is drawn, alternatingly in red and blue.
*
* When vsync is activated and the framerate becomes high enough, the
* disk appear to be solid violet ! When vsync is disabled, screen
* tearing becomes apparent...
********************************************************************/

#include <Arduino.h>
#include <ILI9341_T4.h>



//
// DEFAULT WIRING USING SPI 0 ON TEENSY 4/4.1
//
#define PIN_SCK     13      // mandatory
#define PIN_MISO    12      // mandatory
#define PIN_MOSI    11      // mandatory
#define PIN_DC      10      // mandatory, can be any pin but using pin 10 (or 36 or 37 on T4.1) provides greater performance

#define PIN_CS      9       // optional (but recommended), can be any pin.  
#define PIN_RESET   6       // optional (but recommended), can be any pin. 
#define PIN_BACKLIGHT 255   // optional, set this only if the screen LED pin is connected directly to the Teensy.
#define PIN_TOUCH_IRQ 255   // optional. set this only if the touchscreen is connected on the same SPI bus
#define PIN_TOUCH_CS  255   // optional. set this only if the touchscreen is connected on the same spi bus


//
// ALTERNATE WIRING USING SPI 1 ON TEENSY 4/4.1 
//
//#define PIN_SCK     27      // mandatory 
//#define PIN_MISO    1       // mandatory
//#define PIN_MOSI    26      // mandatory
//#define PIN_DC      0       // mandatory, can be any pin but using pin 0 (or 38 on T4.1) provides greater performance

//#define PIN_CS      30      // optional (but recommended), can be any pin.  
//#define PIN_RESET   29      // optional (but recommended), can be any pin.  
//#define PIN_BACKLIGHT 255   // optional, set this only if the screen LED pin is connected directly to the Teensy. 
//#define PIN_TOUCH_IRQ 255   // optional. set this only if the touchscreen is connected on the same SPI bus
//#define PIN_TOUCH_CS  255   // optional. set this only if the touchscreen is connected on the same spi bus



// 30MHz SPI. Can do much better with short wires
#define SPI_SPEED       30000000


// the screen driver object
ILI9341_T4::ILI9341Driver tft(PIN_CS, PIN_DC, PIN_SCK, PIN_MOSI, PIN_MISO, PIN_RESET, PIN_TOUCH_CS, PIN_TOUCH_IRQ);


// 2 diff buffers with about 6K memory each
// (in this simple case, only 1K memory buffer would be enough). 
ILI9341_T4::DiffBuffStatic<6000> diff1;
ILI9341_T4::DiffBuffStatic<6000> diff2;


// framebuffers
uint16_t internal_fb[320 * 240];     // used for buffering
uint16_t fb[320 * 240];              // main framebuffer we draw onto.



// screen dimension, depending on the mode. 
int LX, LY;


/********************************************************************
********************************************************************/


/** fill a framebuffer with a given color */
void clear(uint16_t* fb, uint16_t color = 0)
    {
    for (int i = 0; i < LX * LY; i++) fb[i] = color;
    }


/** draw a disk centered at (x,y) with radius r and color col on the framebuffer fb */
void drawDisk(uint16_t* fb, double x, double y, double r, uint16_t col)
    {
    int xmin = (int)(x - r);
    int xmax = (int)(x + r);
    int ymin = (int)(y - r);
    int ymax = (int)(y + r);
    if (xmin < 0) xmin = 0;
    if (xmax >= LX) xmax = LX - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= LY) ymax = LY - 1;
    const double r2 = r * r;
    for (int j = ymin; j <= ymax; j++)
        {
        double dy2 = (y - j) * (y - j);
        for (int i = xmin; i <= xmax; i++)
            {
            const double dx2 = (x - i) * (x - i);
            if (dx2 + dy2 <= r2) fb[i + (j * LX)] = col;
            }
        }
    }



/********************************************************************
********************************************************************/




void setup()
    {
    Serial.begin(9600);

    tft.output(&Serial);                // output debug infos to serial port.  
    while (!tft.begin(SPI_SPEED));      // init the display
    
    LX = tft.width();                   // save screen dimension for the drawing methods. 
    LY = tft.height();                  //
    
    tft.setRotation(0);                 // start in portrait mode 240x320
    tft.setFramebuffer(internal_fb);    // set the internal framebuffer (enables double buffering)
    tft.setDiffBuffers(&diff1, &diff2); // set 2 diff buffers -> enables diffenrential updates.
        
    if (PIN_BACKLIGHT != 255)
        { // make sure backlight is on
        pinMode(PIN_BACKLIGHT, OUTPUT);
        digitalWrite(PIN_BACKLIGHT, HIGH);
        }

    tft.setRefreshRate(40); // start with a screen refresh rate around 40hz
    tft.setVSyncSpacing(8); // and a frame rate around 40/8 = 5Hz (very slow !!!)

    }



const uint16_t BLUE = 31;           // colors
const uint16_t RED = (31 << 11);    // there is no violet here :-)

int n = 0; // counter
int a = 0; // angle

void loop()
    {
    clear(fb, 65535); // draw a white background 

    int x = LX / 2 + 50 * cos(a * 0.02);  // the center of the disk 
    int y = LY / 2 + 50 * sin(a * 0.02);  // move around a bit
    drawDisk(fb, x, y, 60, ((++a & 1) ? RED : BLUE)); // alternate blue and red color at each frame. 

    tft.overlayFPS(fb, 3, 63488, 0,  0.3f); // draw the FPS counter on the top left, in red on a semi-transparent white background
    tft.update(fb); // push the framebuffer to be displayed

    adjustMode(); // change the framerate / operation mode / orientation  
    }



void adjustMode()
    {
    switch (n++)
        {
    default:
        return;
    case 0:
        break; // we start around 5Hz
    case 15:
        tft.setVSyncSpacing(4);  // around 10hz (framerate = refreshrate/4)
        break;
    case 45:
        tft.setVSyncSpacing(2);  // around 20 Hz (framerate = refreshrate/2)
        break;
    case 100:
        tft.setVSyncSpacing(1);  // around 40 Hz (framerate = refreshrate)
        break;
    case 200:
        tft.setRefreshRate(60); // around 60 Hz (changing framerate will pause the animation for a short time). 
        break;
    case 350:
        tft.setRefreshRate(90); // around 90 Hz (changing framerate will pause the animation for a short time)
        break;
    case 700:
        tft.setVSyncSpacing(0);  // disable vsync => create screen tearing :-(
        break;
    case 1400:
        tft.setVSyncSpacing(1);  // vsync back on => prevent screen tearing :-)
        tft.setRotation((tft.getRotation() + 1) & 3); // cycle over the possible orientations 
        LX = tft.width();  // update the screen dimension
        LY = tft.height(); //
        n = 351; // loop. 
        }
    if (tft.getVSyncSpacing() > 0)
        {
        Serial.printf("- Alternating red / blue circles at %u fps. in orientation %d\n  VSYNC ENABLED => no screen tearing.\n\n",
            (int)round(tft.getRefreshRate() / tft.getVSyncSpacing()), tft.getRotation());
        }
    else
        {
        Serial.printf("- Alternating red / blue circles at maximum fps. in orientation %d\n  VSYNC DISABLED => ", tft.getRotation());
        Serial.printf((tft.getRotation() & 1) ? "diagonal screen tearing\n\n" : "straight screen tearing\n\n");
        }
    }


/** end of file */

