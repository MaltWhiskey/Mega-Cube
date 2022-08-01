/**
* CREDIT: Example created by Bruno@DIYLAB.DE. Thanks a lot !
*
* SRC: https://github.com/DIYLAB-DE/DualDisplay_T4
*
* Example for Teensy 4/4.1 on using two ILI9341 screens 
* simultaneously (one on SPI0 and the other one on SPI1) 
**/


// the screen driver library
#include <ILI9341_T4.h>


//
// FIRST SCREEN IS WIRED TO SPI0 
//
#define PIN_SCK0        13  // mandatory 
#define PIN_MISO0       12  // mandatory
#define PIN_MOSI0       11  // mandatory
#define PIN_DC0         10  // mandatory, can be any pin but using pin 10 (or 36 or 37 on T4.1) provides greater performance

#define PIN_CS0          9  // optional (but recommended), can be any pin.  
#define PIN_RESET0       6  // optional (but recommended), can be any pin.  
#define PIN_BACKLIGHT0 255  // optional, set this only if the screen LED pin is connected directly to the Teensy. 
#define PIN_TOUCH_IRQ0 255  // optional, set this only if the touchscreen is connected on the same SPI bus
#define PIN_TOUCH_CS0  255  // optional, set this only if the touchscreen is connected on the same SPI bus


//
// SECOND SCREEN IS WIRED TO SPI1 
//
#define PIN_SCK1        27  // mandatory 
#define PIN_MISO1        1  // mandatory
#define PIN_MOSI1       26  // mandatory
#define PIN_DC1          0  // mandatory, can be any pin but using pin 0 (or 38 on T4.1) provides greater performance

#define PIN_CS1         30  // optional (but recommended), can be any pin.  
#define PIN_RESET1      29  // optional (but recommended), can be any pin.  
#define PIN_BACKLIGHT1 255  // optional, set this only if the screen LED pin is connected directly to the Teensy. 
#define PIN_TOUCH_IRQ1 255  // optional. set this only if the touchscreen is connected on the same SPI bus
#define PIN_TOUCH_CS1  255  // optional. set this only if the touchscreen is connected on the same SPI bus


#define SPI_SPEED 30000000


// screen size in portrait mode
#define LX 240
#define LY 320

// screen driver objects
ILI9341_T4::ILI9341Driver tft0(PIN_CS0, PIN_DC0, PIN_SCK0, PIN_MOSI0, PIN_MISO0, PIN_RESET0, PIN_TOUCH_CS0, PIN_TOUCH_IRQ0); // for screen on SPI0
ILI9341_T4::ILI9341Driver tft1(PIN_CS1, PIN_DC1, PIN_SCK1, PIN_MOSI1, PIN_MISO1, PIN_RESET1, PIN_TOUCH_CS1, PIN_TOUCH_IRQ1); // for screen on SPI1

// framebuffers
DMAMEM uint16_t internal_fb0[LX * LY]; // internal fb used by the library for the first screen
DMAMEM uint16_t internal_fb1[LX * LY]; // internal fb used by the library for the second screen
uint16_t fb[LX * LY];                  // main framebuffer we draw onto


// 4 diff buffers with about 6K memory each
ILI9341_T4::DiffBuffStatic<6000> diff1; // for the driver on SPI0
ILI9341_T4::DiffBuffStatic<6000> diff2; //

ILI9341_T4::DiffBuffStatic<6000> diff3; // for the driver on SPI1
ILI9341_T4::DiffBuffStatic<6000> diff4; //


/// <summary>
/// fill a framebuffer with a given color
/// </summary>
/// <param name="fb"></param>
/// <param name="color"></param>
void clear(uint16_t* fb, uint16_t color = 0) 
    {
    for (int i = 0; i < 240 * 320; i++) fb[i] = color;
    }

/// <summary>
/// draw a disk centered at (x,y) with radius r and color col
/// </summary>
/// <param name="fb"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="r"></param>
/// <param name="col"></param>
void drawDisk(uint16_t* fb, double x, double y, double r, uint16_t col) {
    int xmin = (int)(x - r);
    int xmax = (int)(x + r);
    int ymin = (int)(y - r);
    int ymax = (int)(y + r);
    if (xmin < 0) xmin = 0;
    if (xmax >= LX) xmax = LX - 1;
    if (ymin < 0) ymin = 0;
    if (ymax >= LY) ymax = LY - 1;
    const double r2 = r * r;
    for (int j = ymin; j <= ymax; j++) {
        double dy2 = (y - j) * (y - j);
        for (int i = xmin; i <= xmax; i++) {
            const double dx2 = (x - i) * (x - i);
            if (dx2 + dy2 <= r2) fb[i + (j * LX)] = col;
        }
    }
}

/// <summary>
/// draw a pixel
/// </summary>
/// <param name="fb"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="color"></param>
void drawPixel(uint16_t* fb, int x, int y, uint16_t color) {
    if ((x < 0) || (y < 0) || (x >= LX) || (y >= LY)) return;
    fb[x + LX * y] = color;
}

/// <summary>
/// draw a line
/// </summary>
/// <param name="fb"></param>
/// <param name="x0"></param>
/// <param name="y0"></param>
/// <param name="x1"></param>
/// <param name="y1"></param>
/// <param name="color"></param>
void drawLine(uint16_t* fb, int x0, int y0, int x1, int y1, uint16_t color) {
    bool yLonger = false;
    int shortLen = y1 - y0;
    int longLen = x1 - x0;
    if (abs(shortLen) > abs(longLen)) {
        int swap = shortLen;
        shortLen = longLen;
        longLen = swap;
        yLonger = true;
    }
    int decInc;
    if (longLen == 0) decInc = 0;
    else decInc = (shortLen << 16) / longLen;

    if (yLonger) {
        if (longLen > 0) {
            longLen += y0;
            for (int j = 0x8000 + (x0 << 16); y0 <= longLen; ++y0) {
                drawPixel(fb, j >> 16, y0, color);
                j += decInc;
            }
            return;
        }
        longLen += y0;
        for (int j = 0x8000 + (x0 << 16); y0 >= longLen; --y0) {
            drawPixel(fb, j >> 16, y0, color);
            j -= decInc;
        }
        return;
    }

    if (longLen > 0) {
        longLen += x0;
        for (int j = 0x8000 + (y0 << 16); x0 <= longLen; ++x0) {
            drawPixel(fb, x0, j >> 16, color);
            j += decInc;
        }
        return;
    }
    longLen += x0;
    for (int j = 0x8000 + (y0 << 16); x0 >= longLen; --x0) {
        drawPixel(fb, x0, j >> 16, color);
        j -= decInc;
    }
}

/// <summary>
/// return a uniform in [0,1)
/// </summary>
/// <returns></returns>
double unif() {
    return random(2147483647) / 2147483647.0;
}

/// <summary>
/// a bouncing ball
/// </summary>
struct Ball {
    double x, y, dirx, diry, r; // position, direction, radius. 
    uint16_t color;

    Ball() {
        r = unif() * 25; // random radius
        x = r; // start at the corner
        y = r; //
        dirx = unif() * 5; // direction and speed are random...
        diry = unif() * 5; // ...but not isotropic !
        color = random(65536); // random color
    }

    void move() {
        // move
        x += dirx;
        y += diry;
        // and bounce against border
        if (x - r < 0) { x = r;  dirx = -dirx; }
        if (y - r < 0) { y = r;  diry = -diry; }
        if (x > LX - r) { x = LX - r;  dirx = -dirx; }
        if (y > LY - r) { y = LY - r;  diry = -diry; }
    }

    void draw(uint16_t* fb) {
        drawDisk(fb, x, y, r, color);
    }
};

// vars
Ball balls[99];
int nFrames = 120;

/// <summary>
/// setup
/// </summary>
void setup() {
        
        
    Serial.begin(9600);

    // use USB serial for debug information / troubleshooting.  
    tft0.output(&Serial);
    tft1.output(&Serial);
    
    // init the screens
    while (!tft0.begin(SPI_SPEED)) 
        {
        Serial.print("\n\n\n ****** ERROR CONNECTING TO ILI9341 SCREEN ON SPI 0 ****** \n\n");
        delay(1000);
        }
    while (!tft1.begin(SPI_SPEED))
        {
        Serial.print("\n\n\n ****** ERROR CONNECTING TO ILI9341 SCREEN ON SPI 1 ****** \n\n");
        delay(1000);
        }

    // set parameters for the first display
    tft0.setRotation(0);                 // portrait mode 240 x320
    tft0.setFramebuffer(internal_fb0);   // set the internal framebuffer (enables double buffering)
    tft0.setDiffBuffers(&diff1, &diff2); // set the 2 diff buffers => activate differential updates. 
    tft0.setDiffGap(4);                  // use a small gap for the diff buffers
    tft0.setRefreshRate(150);            // max refresh rate. 
    tft0.setVSyncSpacing(2);             // set framerate = refreshrate/2 (and enable vsync at the same time). 

    // set parameters for the second display
    tft1.setRotation(0);                 // portrait mode 240 x320
    tft1.setFramebuffer(internal_fb1);   // set the internal framebuffer (enables double buffering)
    tft1.setDiffBuffers(&diff3, &diff4); // set the 2 diff buffers => activate differential updates. 
    tft1.setDiffGap(4);                  // use a small gap for the diff buffers
    tft1.setRefreshRate(150);            // max refresh rate. 
    tft1.setVSyncSpacing(2);             // set framerate = refreshrate/2 (and enable vsync at the same time). 

     // make sure backlight is on
    if (PIN_BACKLIGHT0 != 255) {
        pinMode(PIN_BACKLIGHT0, OUTPUT);
        digitalWrite(PIN_BACKLIGHT0, HIGH);
    }
    if (PIN_BACKLIGHT1 != 255) {
        pinMode(PIN_BACKLIGHT1, OUTPUT);
        digitalWrite(PIN_BACKLIGHT1, HIGH);
    }
}

/// <summary>
/// main loop
/// </summary>

elapsedMillis em = 0; 
void loop() 
    {
    for (int frame = 0; frame < nFrames; frame++) draw(frame);
    for (int frame = (nFrames); frame >= 0; frame--) draw(frame);
    
    // output some info about the drivers every 5 seconds.
    if (em > 5000)
        { 
        em = 0;  // reset counter        
        Serial.println("\n\n\n ******* INFO ABOUT THE FIRST SCREEN (ON SPI0) ******* \n");
        tft0.printStats();
        diff1.printStats();
        diff2.printStats();        
        Serial.println("\n\n\n ******* INFO ABOUT THE FIRST SCREEN (ON SPI1) ******* \n");
        tft1.printStats();
        diff3.printStats();
        diff4.printStats();        
        }
    }

/// <summary>
/// draw on both displays
/// </summary>
/// <param name="frame"></param>
void draw(int frame) {
    
    // draw on display 0
    clear(fb, 0); // clear the framebuffer 
    // draw the balls
    for (auto& b : balls) 
        {
        b.move();
        b.draw(fb);
        }
        
    tft0.overlayFPS(fb); // draw the FPS counter on the top right corner of the framebuffer
    tft0.update(fb, false); // update screen 1 on SPI0 async. via DMA. 

    // NOTE: here the update on screen 1 is still in progress but we can
    // reuse fb because all changes have been saved in the internal framebuffer. 

    // draw on display 1
    clear(fb, 0); // clear the framebuffer 
    
    // draw the animation
    int n = 9;
    float rot = frame * 2 * PI / nFrames;

    for (int i = 0; i < (n); i++) {
        float a = rot + i * 2 * PI / n;
        int x1 = 120 + cosf(a) * frame;
        int y1 = 160 + sinf(a) * frame;
        for (int j = i + 1; j < n; j++) {
            a = rot + j * 2 * PI / n;
            int x2 = 120 + cosf(a) * frame;
            int y2 = 160 + sinf(a) * frame;
            drawLine(fb, x1, y1, x2, y2, 0xFFF);
        }
    }
    
    tft1.overlayFPS(fb); // draw the FPS counter on the top right corner of the framebuffer
    tft1.update(fb, false); // update screen 2 on SPI1 async. via DMA

    // NOTE: here the update on screen 2 is still in progress but we can
    // reuse fb because all changes have been saved in the internal framebuffer. 
    // ... so we can directly start the next iteration without waiting ! 
}

/** end of file */

