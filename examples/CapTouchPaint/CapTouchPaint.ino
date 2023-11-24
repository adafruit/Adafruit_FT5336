/***************************************************
  This is our touchscreen painting example for the Adafruit HX8357
  with FT5336 captouch breakout
  ----> http://www.adafruit.com/products/5846

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_HX8357.h>
#include <Adafruit_FT5336.h>

// The FT5336 uses hardware I2C (SCL/SDA)
Adafruit_FT5336 ctp = Adafruit_FT5336();
#define FT5336_MAXTOUCHES  5

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS         9
#define TFT_RST        -1
#define TFT_DC         10
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC);

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
  Serial.begin(115200);
  //while (!Serial) delay(10);     // pause the serial port
 
  Serial.println("3.5 inch Cap Touch Paint!");
  
  tft.begin();

  if (! ctp.begin(FT53XX_DEFAULT_ADDR, &Wire)) {  // pass in 'sensitivity' coefficient and I2C bus
    Serial.println("Couldn't start FT5336 touchscreen controller");
    while (1) delay(10);
  }

  Serial.println("Capacitive touchscreen started");
  
  tft.fillScreen(HX8357_BLACK);
  
  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, HX8357_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_MAGENTA);
  tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
  tft.fillRect(BOXSIZE*7, 0, BOXSIZE, BOXSIZE, HX8357_BLACK);
   
  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
  currentcolor = HX8357_RED;
}

void loop() {
  uint8_t touches = ctp.touched();
  // Wait for a touch
  if (! touches) {
    return;
  }

  // Retrieve the points, up to 5!
  TS_Point ps[FT5336_MAXTOUCHES];
  ctp.getPoints(ps, FT5336_MAXTOUCHES);
  
  for (int i=0; i<FT5336_MAXTOUCHES; i++) {
    // Check if z (pressure) is zero, skip if so
    if (ps[i].z == 0) continue;
    /*
    ps[i].x = map(ps[i].x, 0, 320, 0, 320);
    ps[i].y = map(ps[i].y, 0, 480, 0, 480);
    */
    // Print out the remapped/rotated coordinates
    Serial.print("("); Serial.print(ps[i].x);
    Serial.print(", "); Serial.print(ps[i].y);
    Serial.print(")\t");  
  }
  Serial.println();
  
  if (ps[0].y < BOXSIZE) {
     oldcolor = currentcolor;

     if (ps[0].x < BOXSIZE) { 
       currentcolor = HX8357_RED; 
       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     } else if (ps[0].x < BOXSIZE*2) {
       currentcolor = HX8357_YELLOW;
       tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     } else if (ps[0].x < BOXSIZE*3) {
       currentcolor = HX8357_GREEN;
       tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     } else if (ps[0].x < BOXSIZE*4) {
       currentcolor = HX8357_CYAN;
       tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     } else if (ps[0].x < BOXSIZE*5) {
       currentcolor = HX8357_BLUE;
       tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     } else if (ps[0].x <= BOXSIZE*6) {
       currentcolor = HX8357_MAGENTA;
       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     } else if (ps[0].x <= BOXSIZE*7) {
       currentcolor = HX8357_WHITE;
       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_RED);
     } else {
        // erase
        tft.fillScreen(HX8357_BLACK);
        
        // make the color selection boxes
        tft.fillRect(0, 0, BOXSIZE, BOXSIZE, HX8357_RED);
        tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_YELLOW);
        tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_GREEN);
        tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_CYAN);
        tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_BLUE);
        tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_MAGENTA);
        tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     }

     if (oldcolor != currentcolor) {
        if (oldcolor == HX8357_RED) 
          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, HX8357_RED);
        if (oldcolor == HX8357_YELLOW) 
          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, HX8357_YELLOW);
        if (oldcolor == HX8357_GREEN) 
          tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, HX8357_GREEN);
        if (oldcolor == HX8357_CYAN) 
          tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, HX8357_CYAN);
        if (oldcolor == HX8357_BLUE) 
          tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, HX8357_BLUE);
        if (oldcolor == HX8357_MAGENTA) 
          tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, HX8357_MAGENTA);
        if (oldcolor == HX8357_WHITE) 
          tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, HX8357_WHITE);
     }
  }
  for (int i=0; i<FT5336_MAXTOUCHES; i++) {
    // Check if z (pressure) is zero, skip if so
    if (ps[i].z == 0) continue;
    
    if (((ps[i].y-PENRADIUS) > BOXSIZE) && ((ps[i].y+PENRADIUS) < tft.height())) {
      tft.fillCircle(ps[i].x, ps[i].y, PENRADIUS, currentcolor);
    }
  }
}