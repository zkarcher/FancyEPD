#include <stdint.h>
#include <Adafruit_GFX.h>
#include "FancyEPD.h"
#include "FancyEPD_Demo_images.h"
//#include "compression_test.h"
//#include "crystal_fontz_test.h"

#define DELAY_BETWEEN_IMAGES_MS       (10 * 1000)
#define DO_ROTATION                   (true)
#define BLINK_PIN                     (13)
#define DO_SERIAL                     (true)

// Pins for project: github.com/pdp7/kicad-teensy-epaper
//FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
//FancyEPD epd(k_epd_CFAP122250A00213, 17, 16, 14, 15);//, 13, 11);	// software SPI

//FancyEPD epd(k_epd_CFAP128296C00290, 17, 16, 14, 15, 13, 11);	// software SPI

// CrystalFontz: 128 x 296 black + red
//FancyEPD epd(k_epd_CFAP128296D00290, 17, 16, 14, 15);	// software SPI

// Crystalfontz 152x152 color
//FancyEPD epd(k_epd_CFAP152152B00154, 17, 16, 14, 15);

// Crystalfontz 104x212 flexible
FancyEPD epd(k_epd_CFAP104212D00213, 17, 16, 14, 15);

//FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15);	// hardware SPI

//ESP12 Pinout
//FancyEPD epd(k_epd_E2215CS062, 15, 2, 5, 4, 14, 13);

void setup() {
	if (DO_SERIAL) {
		Serial.begin(115200);
	}

	while (!Serial) {};

	Serial.println("Starting setup...");

	// Random seed: Combine 8 samples of 4 LSB's, reading unused pin
	pinMode(0, INPUT);
	uint32_t seed = 0;
	for (uint8_t i = 0; i < 8; i++) {
		seed = (seed << 4) | (analogRead(0) & 0xf);
	}
	randomSeed(seed);

	Serial.println("Reset...");

	// reset?
	const uint8_t RESET_PIN = 6;
	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, LOW);
	delay(100);
	digitalWrite(RESET_PIN, HIGH);
	delay(100);

	Serial.println("Will init...");

	/*
	while (true) {
		int result = digitalRead(15);
		if (result == HIGH) {
			Serial.println("hi");
		} else {
			Serial.println("lo");
		}
		delay(1000);
	}
	*/

	// If onboard LED is blinking at 2Hz: EPD could not init.
	// Probably not enough RAM on the microcontroller.
	bool success = epd.init();

	if (!success) {
		// Panic and freak out.
		pinMode(BLINK_PIN, OUTPUT);

		while (true) {
			digitalWrite(BLINK_PIN, HIGH);
			delay(250);
			digitalWrite(BLINK_PIN, LOW);
			delay(250);
		}
	}

	Serial.println("setup success!");
}

void loop() {
	//loop_boxes();
	//loop_anim();
	//loop_shapes();
	//loop_compression_test();

	// vv Hmm, this is drawing red & black circles :P I don't get it?
	//test_grayscale_on_black_and_red_panel();

	//test_cellular_automata();
  test_cellular_automata_grayscale();

	//test_box_refresh();
	//test_stripe_refresh();
	//test_checkerboard_refresh();
	//test_fast_color_redraw();

	//test_breakout_board();

	//test_image_layers();
}

void test_image_layers() {
	epd.setRotation(1);

	// Framebuffer
	int16_t w = epd.width();
	int16_t h = epd.height();
	uint8_t buf[w * h];

	// Nice XOR pattern
	/*
	const uint8_t bits = 8;
	uint8_t mask = 0x1;
	int16_t rx = random(w);
	int16_t ry = random(h);
	for (int16_t x = 0; x < w; x++) {
		for (int16_t y = 0; y < h; y++) {
			buf[y * w + x] = (x + rx) ^ (y + ry);
		}
	}
	*/

	// Mandelbrot
	const uint8_t bits = 5;
	uint8_t mask = 0x80 >> (bits - 1);

	float xMult = 4.0 / w;
	float yMult = xMult;

	for (int16_t xi = 0; xi < w; xi++) {
		for (int16_t yi = 0; yi < h; yi++) {
			float xf = (xi - (w >> 1)) * xMult;
			float yf = (yi - (h >> 1)) * yMult;

			float real = xf;
			float imag = yf;

			// Mandelbrot. Iterations: z' = z*z + c
			uint8_t i = 0;
			for (; i < (0x1 << bits); i++) {
				float re = (real * real) - (imag * imag) + xf;
				float im = 2.0 * real * imag + yf;

				if ((re < -2.0) || (2.0 < re)) {
					break;
				}

				real = re;
				imag = im;
			}

			buf[yi * w + xi] = ((~i) << (8 - bits));
		}
	}

	epd.clearBuffer();
	epd.update(k_update_quick_refresh);

	epd.setCustomTiming(k_update_no_blink, 6);

	// Send each image layer
	for (uint8_t i = 0; i < bits; i++) {
		for (int16_t x = 0; x < w; x++) {
			for (int16_t y = 0; y < h; y++) {
				epd.drawPixel(x, y, ((buf[y * w + x] & mask) ? 1 : 0));
			}
		}

		epd.update(k_update_no_blink);

		mask <<= 1;
	}

	delay(DELAY_BETWEEN_IMAGES_MS);
}

void test_breakout_board() {
	/*
	// Hmm this is not working
	// Compressed image drew 3 layers ?! rrr
	epd.setRotation(2);
	epd.updateWithImage(angel_crop, k_image_4bit_monochrome);
	*/

	epd.clearBuffer();
	drawCircles(0x1, true);
	epd.update(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	epd.clearBuffer();
	drawTriangles(0x1);
	epd.update(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);
}

void test_checkerboard_refresh() {
	int16_t width = epd.width();
	int16_t height = epd.height();

  // Draw smaller and smaller checkerboards
  uint8_t mask = 0x04;
  while (mask >= 0x04) {

  	for (int16_t w = 0; w < width; w++) {
  		for (int16_t h = 0; h < height; h++) {
  			epd.drawPixel(w, h, ((w ^ h) & mask) ? 1 : 0);
  		}
  	}

    epd.waitUntilNotBusy();
  	epd.update(k_update_INTERNAL_blink_like_crazy);

    mask >>= 1;
  }

  draw_color_boxes_art();

	//epd.update(k_update_no_blink);
  epd.waitUntilNotBusy();
	epd.update(k_update_INTERNAL_then_show_coherent_image);
  epd.waitUntilNotBusy();

	delay(DELAY_BETWEEN_IMAGES_MS);
}

void test_fast_color_redraw() {
	draw_color_boxes_art();

	epd.waitUntilNotBusy();
	epd.update(k_update_INTERNAL_fast_color_redraw);
  epd.waitUntilNotBusy();

	delay(DELAY_BETWEEN_IMAGES_MS);
}

void test_circles() {
	epd.clearBuffer();

	drawCircles(0x1, true);
	drawCircles(0x2, true);

	epd.update(k_update_builtin_refresh);
	epd.waitUntilNotBusy();
	delay(DELAY_BETWEEN_IMAGES_MS);
}

void draw_color_boxes_art() {
  int16_t width = epd.width();
  int16_t height = epd.height();

  epd.setRotation(2);
  epd.clearBuffer();

  // Draw random little boxes
  int16_t boxSize = random(8, 14);

  for (int16_t x = 0; x < width; x += boxSize) {
    for (int16_t y = 0; y < height; y += boxSize) {
      epd.fillRect(x, y, boxSize, boxSize, random(0, 3));
    }
  }

  // Draw a row of big boxes
  int16_t bigSize = (width + 2) / 3;
  int16_t atY = random(height - bigSize);
  uint16_t color = 0;
  for (int16_t x = 0; x < width; x += bigSize) {
    epd.fillRect(x, atY, bigSize, bigSize, color);
    color++;
  }
}

void test_box_refresh() {
  draw_color_boxes_art();
	epd.update(k_update_builtin_refresh);
	epd.waitUntilNotBusy();
	delay(DELAY_BETWEEN_IMAGES_MS);
}

void test_stripe_refresh() {
	int16_t width = epd.width();
	int16_t height = epd.height();

	//const uint8_t PATTERN = 0b11001100;
	const uint8_t PATTERN = 0b1111;

	//epd.setCustomTiming(k_update_no_blink, 2, 2);

	for (uint8_t r = 0; r < 3; r++) { // refresh count

		for (int16_t w = 0; w < width; w++) {
			for (int16_t h = 0; h < height; h++) {

				int16_t idx = r + w + h;

				uint8_t mask = 0b1 << (idx & 0b111);

				epd.drawPixel(w, h, (PATTERN & mask) ? 1 : 0);
			}
		}

		epd.update(k_update_no_blink);
	}

	delay(DELAY_BETWEEN_IMAGES_MS);
}

uint16_t minutes = 0;
uint16_t hours = 0;

void draw_digit(uint8_t d, int16_t sz, int16_t x, int16_t y, uint16_t color) {
	// Draw digits in the least-used color, so they contrast
	const uint8_t DIGIT_DATA[] = {
		0b11111, 0b10001, 0b11111,	// 0
		0b10000, 0b11111, 0b00000,	// 1
		0b10111, 0b10101, 0b11101,	// 2
		0b10101, 0b10101, 0b11111,	// 3
		0b11100, 0b00100, 0b11111,	// 4
		0b11101, 0b10101, 0b10111,	// 5
		0b11111, 0b10101, 0b10111,	// 6
		0b10000, 0b10000, 0b11111,	// 7
		0b11111, 0b10101, 0b11111,	// 8
		0b11101, 0b10101, 0b11111,	// 9
		0b00000, 0b01010, 0b00000		// :
	};

	for (uint8_t i = 0; i < 3; i++) {
		uint8_t art = DIGIT_DATA[d * 3 + i];

		int16_t drawX = x + i * sz;

		for (uint8_t j = 0; j < 5; j++) {
			uint8_t mask = 0x10 >> j;

			if (art & mask) {
				int16_t drawY = y + j * sz;

				epd.fillRect(drawX, drawY, sz, sz, color);
			}
		}
	}
}

void draw_clock_digits(uint8_t sz, int16_t atX, int16_t atY, uint8_t color) {
  draw_digit(hours / 10,   sz, atX,           atY, color);
  draw_digit(hours % 10,   sz, atX + sz * 4,  atY, color);
  draw_digit(10,           sz, atX + sz * 7,  atY, color);
  draw_digit(minutes / 10, sz, atX + sz * 10,  atY, color);
  draw_digit(minutes % 10, sz, atX + sz * 14, atY, color);
}

void advance_the_clock() {
  minutes++;
  if (minutes >= 60) {
    minutes = 0;
    hours = (hours + 1) % 24;
  }
}

void test_cellular_automata_grayscale() {
  Serial.println("Drawing...");

  epd.setRotation(2);

  int16_t w = epd.width();
  int16_t h = epd.height();

  // 3 color states (white, black, red): Choose 27 rules (3^3)
  uint8_t rules[27];

  for (uint8_t r = 0; r < 27; r++) {
    rules[r] = (uint8_t)(random(3));
  }

  // Pixel data is stored here:
  uint8_t px[h][w];

  // Total number of pixel each color
  uint16_t pxTotals[3] = {0, 0, 0};	// white, black, red

  // Fill the first row with random noise
  for (int16_t x = 0; x < w; x++) {
    px[0][x] = random(3);
  }

  // Derive the next rows from the row above.
  for (int16_t y = 1; y < h; y++) {
    for (int16_t x = 0; x < w; x++) {
      uint8_t a0 = px[y - 1][((x - 1) + w) % w];
      uint8_t a1 = px[y - 1][x];
      uint8_t a2 = px[y - 1][(x + 1) % w];

      uint8_t rule_index = (a0 * 9) + (a1 * 3) + a2;
      px[y][x] = rules[rule_index];

      // Count the number of pixels in each color.
      pxTotals[px[y][x]]++;
    }
  }

  uint16_t pxMax = max(pxTotals[0], max(pxTotals[1], pxTotals[2]));
  uint16_t pxMin = min(pxTotals[0], min(pxTotals[1], pxTotals[2]));

  uint8_t colorMax = 0;
  uint8_t colorMin = 0;
  for (uint8_t c = 0; c < 3; c++) {
    if (pxMax == pxTotals[c]) {
      colorMax = c;
    }
    if (pxMin == pxTotals[c]) {
      colorMin = c;
    }
  }

  // Set the border color to match the most pixels
  //epd.setBorderColor(colorMax);

  // Clear
  epd.clearBuffer();
  epd.waitUntilNotBusy();
  epd.update(k_update_builtin_refresh);

  // Clock params:
  uint8_t sz = 5;
  int16_t atX = random(w - sz * 18) + sz;
  int16_t atY = h - (6 * sz) - random(sz * 5);

  // Draw as grayscale
  for (uint8_t b = 0; b < 2; b++) {
    // LSB first
    uint8_t mask = (0x1 << b);

    for (int16_t y = 0; y < h; y++) {
      for (int16_t x = 0; x < w; x++) {
        epd.drawPixel(x, y, (px[y][x] & mask) ? 0 : 1);
      }
    }

    draw_clock_digits(sz, atX, atY, 0);

    epd.setCustomTiming(k_update_INTERNAL_image_layer, 50);

    epd.waitUntilNotBusy();
    epd.update(k_update_INTERNAL_image_layer);
    epd.waitUntilNotBusy();
  }

  advance_the_clock();

  delay(DELAY_BETWEEN_IMAGES_MS);

  Serial.println("  Draw complete.");
}

void test_cellular_automata() {
	Serial.println("Drawing...");

	epd.setRotation(2);

	int16_t w = epd.width();
	int16_t h = epd.height();

  // Trying to reset/unburn the colors, here
  for (uint8_t c = 0; c <= 2; c++) {
    epd.waitUntilNotBusy();
    epd.fillRect(0, 0, w, h, c);
    epd.setBorderColor(c);
    epd.update(k_update_builtin_refresh);
    epd.waitUntilNotBusy();
    delay(2000);
  }

	// 3 color states (white, black, red): Choose 27 rules (3^3)
	uint8_t rules[27];

	for (uint8_t r = 0; r < 27; r++) {
		rules[r] = (uint8_t)(random(3));
	}

	// Pixel data is stored here:
	uint8_t row0[w];	// Previous row
	uint8_t row1[w];	// Active row being created, based on row0

	// Total number of pixel each color
	uint16_t pxTotals[3] = {0, 0, 0};	// white, black, red

	// Fill the first row with random noise
	for (int16_t x = 0; x < w; x++) {
		row0[x] = random(3);
		epd.drawPixel(x, 0, row0[x]);
	}

	// Derive the next rows from the row above.
	for (int16_t y = 1; y < h; y++) {
		for (int16_t x = 0; x < w; x++) {
			uint8_t a0 = row0[((x - 1) + w) % w];
			uint8_t a1 = row0[x];
			uint8_t a2 = row0[(x + 1) % w];

			uint8_t rule_index = (a0 * 9) + (a1 * 3) + a2;
			row1[x] = rules[rule_index];
			epd.drawPixel(x, y, row1[x]);

			// Count the number of pixels in each color.
			pxTotals[row1[x]]++;
		}

		// Copy row1 values to row0, so they can generate a new row
		for (int16_t x = 0; x < w; x++) {
			row0[x] = row1[x];
		}
	}

	uint16_t pxMax = max(pxTotals[0], max(pxTotals[1], pxTotals[2]));
	uint16_t pxMin = min(pxTotals[0], min(pxTotals[1], pxTotals[2]));

	uint8_t colorMax = 0;
	uint8_t colorMin = 0;
	for (uint8_t c = 0; c < 3; c++) {
		if (pxMax == pxTotals[c]) {
			colorMax = c;
		}
		if (pxMin == pxTotals[c]) {
			colorMin = c;
		}
	}

	// Set the border color to match the most pixels
	epd.setBorderColor(colorMax);

  uint8_t sz = 5;
  int16_t atX = random(w - sz * 18) + sz;
  int16_t atY = h - (6 * sz) - random(sz * 5);
  draw_clock_digits(sz, atX, atY, colorMin);
  advance_the_clock();

	epd.waitUntilNotBusy();
	epd.update(k_update_builtin_refresh);
	//epd.update(k_update_INTERNAL_fast_color_redraw);
	epd.waitUntilNotBusy();

	delay(DELAY_BETWEEN_IMAGES_MS);

	Serial.println("  Draw complete.");
}

void test_grayscale_on_black_and_red_panel() {
	epd.setRotation(3);

	epd.fillRect(0, 0, epd.width(), epd.height(), 0x0);
	drawCircles(0x1, true);
	drawCircles(0x1, false);
	drawLabel("Hello world!");
	epd.update(k_update_builtin_refresh);
	epd.waitUntilNotBusy();

	delay(DELAY_BETWEEN_IMAGES_MS);
}

void loop_compression_test() {
	/*
	epd.setRotation(0);
	epd.setBorderColor(0xff);	// black

	//uint16_t err = epd.updateWithCompressedImage(michael);

	uint16_t err = epd.updateWithCompressedImage(sunset, k_update_quick_refresh);

	if (err) {
		String str = "err: " + String(err);
		drawLabel(str);
		epd.update();
	}

	delay(DELAY_BETWEEN_IMAGES_MS);
	*/
}

int8_t drawsUntilModeSwitch = 0;

void loop_boxes() {
	epd_update_t updateType = k_update_no_blink;

	drawsUntilModeSwitch--;

	if (drawsUntilModeSwitch < 0) {
		// Toggle animation mode
		epd.setAnimationMode(!epd.getAnimationMode());

		// Reset draw count
		drawsUntilModeSwitch = (epd.getAnimationMode() ? 40 : 12);

		// Draw label
		if (epd.getAnimationMode()) {
			drawLabel("Animation mode:\n         ON!");
		} else {
			drawLabel("Animation mode:\n    Off.");
		}

		// Flash the screen, clear all pixels
		updateType = k_update_builtin_refresh;
		epd.markDisplayDirty();
	}

	uint8_t BOX_X = (epd.width() - 50) / 2;
	uint8_t BOX_Y = (epd.height() - 50) / 2;

	// Erase old boxes
	epd.fillRect(BOX_X, BOX_Y, 50, 50, 0x0);

	uint32_t rando = (uint32_t)(random(0xffffffff));

	for (uint8_t x = 0; x < 5; x++) {
		for (uint8_t y = 0; y < 5; y++) {
			rando >>= 1;
			if (rando & 0x1) continue;

			epd.fillRect(BOX_X + x * 10, BOX_Y + y * 10, 10, 10, 0xff);
		}
	}

	epd.setBorderColor(0x0);

	epd.update(updateType);
}

// Animation
const int8_t SPEED_X = 5;
int16_t ball_x = 0;
int16_t dir_x = SPEED_X;
const int8_t SPEED_Y = 2;
int16_t ball_y = 0;
int16_t dir_y = SPEED_Y;
const int16_t BALL_SZ = 32;

void loop_anim() {
	// Erase old position
	epd.fillCircle(ball_x + BALL_SZ / 2, ball_y + BALL_SZ / 2, BALL_SZ / 2, 0x0);

	ball_x += dir_x;
	ball_y += dir_y;

	if ((ball_x + BALL_SZ) >= epd.width()) {
		dir_x = -SPEED_X;
	} else if (ball_x <= 0) {
		dir_x = SPEED_X;
	}

	if ((ball_y + BALL_SZ) >= epd.height()) {
		dir_y = -SPEED_Y;
	} else if (ball_y <= 0) {
		dir_y = SPEED_Y;
	}

	// Draw new position
	epd.fillCircle(ball_x + BALL_SZ / 2, ball_y + BALL_SZ / 2, BALL_SZ / 2, 0xff);

	// FIXME ZKA: Remove this
	//epd.markDisplayDirty();

	epd.setBorderColor(0xff);

	epd.update(k_update_no_blink);
}

void loop_shapes() {
	// FIXME ZKA black+red needs special timing to look
	//       correct.
	/*
	epd.setCustomTiming(k_update_quick_refresh, 20, 80);
	epd.setCustomTiming(k_update_no_blink, 70);
	epd.setCustomTiming(k_update_partial, 70);
	*/



	// black+red border color:
	// 0b00==white, 0b01=="muddy red", 0b11==black



	/*
	if (DO_SERIAL) Serial.println("next: builtin");
	if (DO_ROTATION) epd.setRotation(0);
	epd.setBorderColor(0b0);
	epd.clearBuffer();
	drawCircles(0x1, true);
	drawCircles(0x2, false);
	drawLabel("Update:\n builtin_refresh");
	epd.setBorderColor(0x00);	// white
	epd.update(k_update_builtin_refresh);
	epd.waitUntilNotBusy();
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	/*
	if (DO_SERIAL) Serial.println("next: quick refresh");
	if (DO_ROTATION) epd.setRotation(3);
	*/

	/*
	epd.clearBuffer();
	drawCircles(0x1, true);
	drawCircles(0x2, false);
	drawLabel("Update:\n  quick_refresh");
	epd.update(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);
	epd.waitUntilNotBusy();
	*/

	// Fix burn-in
	/*
	for (uint8_t c = 0; c <= 0x2; c++) {
		epd.fillRect(0, 0, epd.width(), epd.height(), c);
		epd.setBorderColor(c);
		epd.update(k_update_builtin_refresh);
		epd.waitUntilNotBusy();
		delay(DELAY_BETWEEN_IMAGES_MS);
	}
	*/

/*
epd.setCustomTiming(k_update_quick_refresh, 20, 80);
epd.setCustomTiming(k_update_no_blink, 70);
epd.setCustomTiming(k_update_partial, 70);
*/

	/*
	// Clear
	epd.fillRect(0, 0, epd.width(), epd.height(), 0x0);
	epd.update(k_update_builtin_refresh);
	epd.waitUntilNotBusy();

	// Quick and dirty grayscale?
	for (uint8_t b = 0; b < 4; b++) {
		for (int16_t x = 0; x < epd.width(); x++) {
			epd.drawFastVLine(x, 0, epd.height(), (x >> (b+1)) & 0x1);
		}

		const uint8_t TIMINGS[] = {200, 200, 200, 200};

		epd.waitUntilNotBusy();
		epd.setCustomTiming(k_update_no_blink, TIMINGS[b]);
		epd.update(k_update_quick_refresh);
		epd.waitUntilNotBusy();
	}

	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	/*
	if (DO_SERIAL) Serial.println("next: no_blink");
	if (DO_ROTATION) epd.setRotation(2);
	epd.setBorderColor(0b01);
	epd.clearBuffer();
	drawCircles(0x1, true);
	drawCircles(0x2, false);
	drawLabel("Update:\n   no_blink");
	epd.update(k_update_no_blink);
	epd.waitUntilNotBusy();
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	/*
	if (DO_SERIAL) Serial.println("next: partial");
	if (DO_ROTATION) epd.setRotation(3);
	epd.clearBuffer();
	drawTriangles(0x1);
	drawTriangles(0x2);
	drawLabel("Update:\n    partial");
	epd.update(k_update_partial);
	epd.waitUntilNotBusy();
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	/*
	if (DO_ROTATION) epd.setRotation(2);
	//epd.updateWithCompressedImage(angel_flexible_1bit, k_update_builtin_refresh);
	epd.updateWithImage(angel_flexible_1bit_no_compression, k_image_1bit, k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	//epd.updateWithCompressedImage(zka_1bit_bug_maybe, k_update_builtin_refresh);
	epd.updateWithCompressedImage(zka_1bit_bug_maybe, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	epd.updateWithImage(angel_flexible_1bit_no_compression, k_image_1bit, k_update_no_blink);
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	// Angel
	/*
	if (DO_ROTATION) epd.setRotation(2);
	epd.setBorderColor(0xff);	// black
	epd.updateWithCompressedImage(angel_flexible, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	// Angel 2
	epd.setBorderColor(0x00);	// white
	epd.updateWithCompressedImage(angel2_flexible, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	/*
	// Doggy
	epd.setBorderColor(0x40);	// grey-ish
	epd.updateWithImage(doggy_2bit, k_image_2bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/
}

void drawCircles(uint16_t color, bool on_top)
{
	for (uint8_t i = 0; i < 20; i++) {
		uint8_t radius = random(1, 30);
		uint8_t stroke = random(1, 10);

		int16_t cx = random(epd.width());
		int16_t cy = random(epd.height());// / 2);

		/*
		if (!on_top) {
			cy += epd.height() / 2;
		}
		*/

		for (uint8_t sx = 0; sx < stroke; sx++) {
			for (uint8_t sy = 0; sy < stroke; sy++) {
				epd.drawCircle(cx + sx, cy + sy, radius, color);
			}
		}
	}
}

void drawTriangles(uint16_t color)
{
	const float TRI = 3.1415926f * (2.0f / 3.0f);

	for (uint8_t i = 0; i < 6; i++) {
		int16_t x = random(epd.width());
		int16_t y = random(epd.height());
		int16_t r = random(2, 80);
		uint8_t stroke = random(1, 4);
		float theta = random(0xffffff) * (TRI / 0xffffff);

		for (uint8_t sx = 0; sx < stroke; sx++) {
			for (uint8_t sy = 0; sy < stroke; sy++) {
				for (uint8_t p = 0; p < 3; p++) {
					epd.drawLine(
						sx + x + r * cosf(theta + TRI * p),
						sy + y + r * sinf(theta + TRI * p),
						sx + x + r * cosf(theta + TRI * (p + 1)),
						sy + y + r * sinf(theta + TRI * (p + 1)),
						color
					);
				}
			}
		}
	}
}

void drawLabel(String str)
{
	// Background box
	const uint8_t box_height = 20;
	epd.fillRect(0, 0, epd.width(), box_height, 0x0);
	epd.drawFastHLine(0, box_height, epd.width(), 0x1);

	epd.setCursor(0, 0);
	epd.setTextColor(0x01);
	epd.print(str);
}
