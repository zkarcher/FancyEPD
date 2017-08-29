#include <stdint.h>
#include <Adafruit_GFX.h>
#include "FancyEPD.h"
#include "FancyEPD_Demo_images.h"

#define DELAY_BETWEEN_IMAGES_MS       (6 * 1000)

// Pins set for project: github.com/pdp7/kicad-teensy-epaper
//FancyEPD epd(E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
FancyEPD epd(k_epd_model_E2215CS062, 17, 16, 14, 15);	// hardware SPI

void setup() {
	bool success = epd.init();

	if (!success) {
		// Panic and freak out
		return;
	}
}

void loop() {
	epd.updateScreenWithImage(angel_4bit, k_image_4bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	epd.updateScreenWithImage(doggy_2bit, k_image_2bit_monochrome, k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	drawCircles();
	drawLabel("Update:\n builtin_refresh");
	epd.updateScreen(k_update_builtin_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	drawLines();
	drawLabel("Update:\n  quick_refresh");
	epd.updateScreen(k_update_quick_refresh);
	delay(DELAY_BETWEEN_IMAGES_MS);

	drawCircles();
	drawLabel("Update:\n   no_blink");
	epd.updateScreen(k_update_no_blink);
	delay(DELAY_BETWEEN_IMAGES_MS);

	drawLines();
	drawLabel("Update:\n    partial");
	epd.updateScreen(k_update_partial);
	delay(DELAY_BETWEEN_IMAGES_MS);
}

void drawCircles()
{
	epd.clearBuffer();
	for (uint8_t i = 0; i < 5; i++) {
		uint8_t radius = random(1, 80);
		epd.drawCircle(random(epd.width()), random(epd.height()), radius, 0xff);
	}
}

void drawLines()
{
	epd.clearBuffer();
	for (uint8_t i = 0; i < 15; i++) {
		epd.drawLine(random(epd.width()), random(epd.height()), random(epd.width()), random(epd.height()), 0xff);
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
