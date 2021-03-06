#ifndef DISPLAYHELPERS_HPP
#define DISPLAYHELPERS_HPP

#include <Adafruit_GFX.h>

void drawCircle(Adafruit_GFX &display, int16_t x0, int16_t y0, int radius, uint8_t color) { // draws the outline of a circle
  // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    display.drawPixel(x0 + x, y0 + y, color);
    display.drawPixel(x0 + y, y0 + x, color);
    display.drawPixel(x0 - y, y0 + x, color);
    display.drawPixel(x0 - x, y0 + y, color);
    display.drawPixel(x0 - x, y0 - y, color);
    display.drawPixel(x0 - y, y0 - x, color);
    display.drawPixel(x0 + y, y0 - x, color);
    display.drawPixel(x0 + x, y0 - y, color);

    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}

void fillCircle(Adafruit_GFX &display, int16_t x0, int16_t y0, uint16_t radius, uint16_t color) { // draws a solid, filled circle
  // https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    display.drawFastHLine(x0 - x, y0 + y, 2*x, color);
    // display.drawLine(x0 + x, y0 + y, x0 - x, y0 + y, color);
    display.drawFastHLine(x0 - y, y0 + x, 2*y, color);
    // display.drawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
    display.drawFastHLine(x0 - y, y0 - x, 2*y, color);
    // display.drawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    display.drawFastHLine(x0 - x, y0 - y, 2*x, color);
    // display.drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    }
    if (err > 0) {
      x--;
      err -= 2 * x + 1;
    }
  }
}

#endif // DISPLAYHELPERS_HPP