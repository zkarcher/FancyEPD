#include <stdint.h>
#include <Adafruit_GFX.h>
#include "FancyEPD.h"
#include "FancyEPD_Demo_images.h"
//#include "compression_test.h"
//#include "crystal_fontz_test.h"

#define DELAY_BETWEEN_IMAGES_MS       (15 * 1000)
#define DO_ROTATION                   (true)
#define BLINK_PIN                     (13)
#define DO_SERIAL                     (true)

// Pins for project: github.com/pdp7/kicad-teensy-epaper
//FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
//FancyEPD epd(k_epd_CFAP122250A00213, 17, 16, 14, 15);//, 13, 11);	// software SPI
//FancyEPD epd(k_epd_CFAP128296C00290, 17, 16, 14, 15);//, 13, 11);	// software SPI
FancyEPD epd(k_epd_CFAP128296D00290, 17, 16, 14, 15);//, 13, 11);	// software SPI

//FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15);	// hardware SPI

//ESP12 Pinout
//FancyEPD epd(k_epd_E2215CS062, 15, 2, 5, 4, 14, 13);

void setup() {
	if (DO_SERIAL) {
		Serial.begin(115200);
	}

	while (!Serial) {};

	// reset?
	const uint8_t RESET_PIN = 6;
	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, LOW);
	delay(100);
	digitalWrite(RESET_PIN, HIGH);
	delay(100);

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
	loop_shapes();
	//loop_compression_test();
}

void loop_compression_test() {
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
	// Longer timing == probably not needed
	epd.setCustomTiming(k_update_quick_refresh, 50, 20);
	epd.setCustomTiming(k_update_no_blink, 60, 20);
	//epd.setCustomTiming(k_update_partial, 50, 50);

	/*
	if (DO_SERIAL) Serial.println("next: builtin");
	if (DO_ROTATION) epd.setRotation(0);
	epd.clearBuffer();
	drawCircles(0x1);
	drawCircles(0x2);
	drawLabel("Update:\n builtin_refresh");
	epd.setBorderColor(0x00);	// white
	epd.update(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_SERIAL) Serial.println("next: quick refresh");
	if (DO_ROTATION) epd.setRotation(1);
	epd.clearBuffer();
	drawTriangles(0x1);
	drawTriangles(0x2);
	drawLabel("Update:\n  quick_refresh");
	epd.update(k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	if (DO_SERIAL) Serial.println("next: no_blink");
	if (DO_ROTATION) epd.setRotation(2);

	// 0b00==white, 0b01=="muddy red", 0b11==black
	epd.setBorderColor(0b01);

	epd.clearBuffer();
	drawCircles(0x1, true);
	drawCircles(0x2, false);
	drawLabel("Update:\n   no_blink");

	epd.update(k_update_no_blink); // best timing is: 90
	//epd.update(k_update_quick_refresh);	// best timing is: 50

	delay(DELAY_BETWEEN_IMAGES_MS);

	/*
	if (DO_SERIAL) Serial.println("next: partial");
	if (DO_ROTATION) epd.setRotation(3);
	epd.clearBuffer();
	drawTriangles(0x1);
	drawTriangles(0x2);
	drawLabel("Update:\n    partial");
	epd.update(k_update_partial);
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/

	/*
	// Angel
	if (DO_ROTATION) epd.setRotation(0);
	epd.setBorderColor(0xff);	// black
	epd.updateWithImage(angel_4bit, k_image_4bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	// Angel
	epd.setBorderColor(0x00);	// white
	epd.updateWithImage(angel2_8bit, k_image_8bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	// Doggy
	epd.setBorderColor(0x40);	// grey-ish
	epd.updateWithImage(doggy_2bit, k_image_2bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);
	*/
}

void drawCircles(uint16_t color, bool on_top)
{
	for (uint8_t i = 0; i < 10; i++) {
		uint8_t radius = random(1, 30);
		uint8_t stroke = random(1, 10);

		int16_t cx = random(epd.width());
		int16_t cy = random(epd.height() / 2);

		if (!on_top) {
			cy += epd.height() / 2;
		}

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
