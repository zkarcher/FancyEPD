#include <stdint.h>
#include <Adafruit_GFX.h>
#include "FancyEPD.h"
//#include "FancyEPD_Demo_images.h"

#define DELAY_BETWEEN_IMAGES_MS       (6 * 1000)
#define DO_ROTATION                   (true)

// Pins for project: github.com/pdp7/kicad-teensy-epaper
//FancyEPD epd(k_epd_model_E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
FancyEPD epd(k_epd_model_E2215CS062, 17, 16, 14, 15);	// hardware SPI

//ESP12 Pinout
//FancyEPD epd(k_epd_model_E2215CS062, 15, 2, 5, 4, 14, 13);

void setup() {
	bool success = epd.init();

	if (!success) {
		// Panic and freak out
		return;
	}
}

// Animation
/*
int16_t ball_x = 0;
int16_t ball_y = 0;
void loop() {
	delay(1000);
}
*/

void loop() {
	if (DO_ROTATION) epd.setRotation(0);
	drawCircles();
	drawLabel("Update:\n builtin_refresh");
	epd.setBorderColor(0x00);	// white
	epd.updateScreen(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_ROTATION) epd.setRotation(1);
	drawTriangles();
	drawLabel("Update:\n  quick_refresh");
	epd.updateScreen(k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_ROTATION) epd.setRotation(2);
	drawCircles();
	drawLabel("Update:\n   no_blink");
	epd.updateScreen(k_update_no_blink);
	delay(DELAY_BETWEEN_IMAGES_MS);

	if (DO_ROTATION) epd.setRotation(3);
	drawTriangles();
	drawLabel("Update:\n    partial");
	epd.updateScreen(k_update_partial);
	delay(DELAY_BETWEEN_IMAGES_MS);

	/*
	// Angel
	if (DO_ROTATION) epd.setRotation(0);
	epd.setBorderColor(0xff);	// black
	epd.updateScreenWithImage(angel_4bit, k_image_4bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	// Angel
	epd.setBorderColor(0x00);	// white
	epd.updateScreenWithImage(angel2_8bit, k_image_8bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	// Doggy
	epd.setBorderColor(0x40);	// grey-ish
	epd.updateScreenWithImage(doggy_2bit, k_image_2bit_monochrome, k_update_quick_refresh);
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
