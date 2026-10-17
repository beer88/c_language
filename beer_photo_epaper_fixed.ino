#include <SPI.h>
#include <GxEPD2_BW.h>
#include "beer_photo_200x200.h"

// XIAO ESP32-S3 + Waveshare 1.54-inch 200x200 e-paper
// Display controller: GDEH0154D67

#define EPD_CS    D3
#define EPD_DC    D4
#define EPD_RST   D5
#define EPD_BUSY  D7

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
);

void setup() {
  display.init(115200);
  display.setRotation(0);
  display.setFullWindow();

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    // Header bitmap is already 200x200; no scaling is required.
    display.drawImage(
      epd_bitmap_beer_photo,
      0, 0,
      BEER_PHOTO_WIDTH,
      BEER_PHOTO_HEIGHT
    );
  } while (display.nextPage());

  display.hibernate();
}

void loop() {
  // E-paper retains the image without continuous refreshing.
}
