#include <stdint.h>
#include <Adafruit_GFX.h>
#include "FancyEPD.h"
//#include "FancyEPD_Demo_images.h"
#include "compression_test.h"

#define DELAY_BETWEEN_IMAGES_MS       (6 * 1000)
#define DO_ROTATION                   (true)

// Pins for project: github.com/pdp7/kicad-teensy-epaper
//FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
FancyEPD epd(k_epd_E2215CS062, 17, 16, 14, 15);	// hardware SPI

//ESP12 Pinout
//FancyEPD epd(k_epd_E2215CS062, 15, 2, 5, 4, 14, 13);

void setup() {
	bool success = epd.init();

	if (!success) {
		// Panic and freak out
		return;
	}
}

void loop() {
	//loop_boxes();
	//loop_anim();
	//loop_images();

	loop_compression_test();
}

void loop_compression_test() {
	epd.setRotation(0);
	epd.setBorderColor(0xff);	// black

	uint16_t err = epd.updateWithCompressedImage(angel);

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

void loop_images() {
	if (DO_ROTATION) epd.setRotation(0);
	drawCircles();
	drawLabel("Update:\n builtin_refresh");
	epd.setBorderColor(0x00);	// white
	epd.update(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_ROTATION) epd.setRotation(1);
	drawTriangles();
	drawLabel("Update:\n  quick_refresh");
	epd.update(k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_ROTATION) epd.setRotation(2);
	drawCircles();
	drawLabel("Update:\n   no_blink");
	epd.update(k_update_no_blink);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_ROTATION) epd.setRotation(3);
	drawTriangles();
	drawLabel("Update:\n    partial");
	epd.update(k_update_partial);
	delay(DELAY_BETWEEN_IMAGES_MS);

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

void drawCircles()
{
	epd.clearBuffer();
	for (uint8_t i = 0; i < 5; i++) {
		uint8_t radius = random(1, 80);
		epd.drawCircle(random(epd.width()), random(epd.height()), radius, 0xff);
	}
}

void drawTriangles()
{
	epd.clearBuffer();

	const float TRI = 3.1415926f * (2.0f / 3.0f);

	for (uint8_t i = 0; i < 6; i++) {
		int16_t x = random(epd.width());
		int16_t y = random(epd.height());
		int16_t r = random(2, 80);
		float theta = random(0xffffff) * (TRI / 0xffffff);

		for (uint8_t p = 0; p < 3; p++) {
			epd.drawLine(
				x + r * cosf(theta + TRI * p),
				y + r * sinf(theta + TRI * p),
				x + r * cosf(theta + TRI * (p + 1)),
				y + r * sinf(theta + TRI * (p + 1)),
				0xff
			);
		}
	}
}

void drawLabel(String str)
{
	// Background box
	const uint8_t box_height = 20;
	epd.fillRect(0, 0, epd.width(), box_height, 0x0);
	epd.drawFastHLine(0, box_height, epd.width(), 0xff);

	epd.setCursor(0, 0);
	epd.print(str);
}
