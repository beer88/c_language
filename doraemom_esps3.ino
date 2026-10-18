
#include <SPI.h>
#include <GxEPD2_BW.h>
#include "Doraemon.h"

// =====================================
// XIAO ESP32-S3 E-PAPER PIN CONNECTIONS
// =====================================
#define EPD_CS    D3
#define EPD_DC    D4
#define EPD_RST   D5
#define EPD_BUSY  D7

// Waveshare 1.54-inch, 200x200 display
// Controller: GDEH0154D67
GxEPD2_BW<GxEPD2_154_D67,
          GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
);

// =====================================
// SETUP
// =====================================
void setup() {
  display.init(115200);

  display.setRotation(0);
  display.setFullWindow();

  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    drawDoraemon(display);
  } while (display.nextPage());

  // Put display into low-power sleep
  display.hibernate();
}

// =====================================
// LOOP
// =====================================
void loop() {
  // Nothing required.
  // The image remains visible on the e-paper.
}
