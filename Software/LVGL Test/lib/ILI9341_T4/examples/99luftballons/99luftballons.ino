/********************************************************************
*
* ILI9341_T4 library example: displaying moving sprites...
*
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
ILI9341_T4::DiffBuffStatic<6000> diff1;
ILI9341_T4::DiffBuffStatic<6000> diff2;

// screen size in portrait mode
#define LX  240
#define LY  320

// our framebuffers
uint16_t internal_fb[LX * LY];     // used by the library for buffering
uint16_t fb[LX * LY];              // the main framebuffer we draw onto.





/********************************************************************
* code for drawing colored balls.
********************************************************************/


/** fill a framebuffer with a given color*/
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

/** return a uniform in [0,1) */
double unif()
    {
    return random(2147483647) / 2147483647.0;
    }

/** a bouncing ball */
struct Ball
    {
    double x, y, dirx, diry, r; // position, direction, radius. 
    uint16_t color;

    Ball()
        {
        r = unif() * 25; // random radius
        x = r; // start at the corner
        y = r; //
        dirx = unif() * 5; // direction and speed are random...
        diry = unif() * 5; // ...but not isotropic !
        color = random(65536); // random color
        }

    void move()
        {
        // move
        x += dirx;
        y += diry;
        // and bounce against border
        if (x - r < 0) { x = r;  dirx = -dirx; }
        if (y - r < 0) { y = r;  diry = -diry; }
        if (x > LX - r) { x = LX - r;  dirx = -dirx; }
        if (y > LY - r) { y = LY - r;  diry = -diry; }
        }

    void draw(uint16_t* fb)
        {
        drawDisk(fb, x, y, r, color);
        }
    };








// our 99 luftballons
Ball balls[99];



/********************************************************************
* Main display code.
*
* With 30MHz SPI, the theoretical maximum framerate when doing full
* framebuffer redraw is 24 FPS.
*
* Here, we get around 100 FPS (without vsync) and a stable 60 FPS with
* vsync (screen tearing free).
********************************************************************/


void setup()
    {
    Serial.begin(9600);

    tft.output(&Serial);                // output debug infos to serial port.     
    while (!tft.begin(SPI_SPEED));      // init the display
    tft.setRotation(0);                 // portrait mode 240 x320
    tft.setFramebuffer(internal_fb);    // set the internal framebuffer (enables double buffering)
    tft.setDiffBuffers(&diff1, &diff2); // set the 2 diff buffers => activate differential updates. 
    tft.setDiffGap(4);                  // use a small gap for the diff buffers

    // Below, vsync_spacing = 2 means we want 120/2=60Hz fixed framerate with vsync enabled.
    // Here, at 30mhz spi, we could even choose vsync_spacing = 1 and refresh rate = 90hz
    // to get a solid 90fps and it would still works. We can also try setting vsync_spacing = 0 
    // to find out the maximum framerate without vsync (which will be over 100fps). 

    tft.setRefreshRate(120);            // around 120hz for the display refresh rate. 
    tft.setVSyncSpacing(2);             // set framerate = refreshrate/2 (and enable vsync at the same time). 

    if (PIN_BACKLIGHT != 255)
        { // make sure backlight is on
        pinMode(PIN_BACKLIGHT, OUTPUT);
        digitalWrite(PIN_BACKLIGHT, HIGH);
        }
    }



int nbf = 0; // count the number of frames drawn. 

void loop()
    {
    clear(fb, 0); // erase the framebuffer, black background. 

    for (auto& b : balls)
        { // move and then draw all the balls onto the framebuffer
        b.move();
        b.draw(fb);
        }

    tft.overlayFPS(fb, 63488, 0, 0.2f); // draw the FPS counter on the top left, in red on a semi-transparent white background
    tft.update(fb); // push the framebuffer to be displayed

    if (++nbf % 2000 == 500)
        { // prints stats every 2000 frames. 
        tft.printStats();
        diff1.printStats();
        diff2.printStats();
        }
    }


/** end of file */

