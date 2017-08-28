#include <stdint.h>
#include <Adafruit_GFX.h>
#include "FancyEPD.h"
//#include "FancyEPD_Demo_images.h"

#define DELAY_BETWEEN_IMAGES_MS       (3000)

// Pins set for project: github.com/pdp7/kicad-teensy-epaper
//FancyEPD epd(E2215CS062, 17, 16, 14, 15, 13, 11);	// software SPI
FancyEPD epd(k_epd_model_E2215CS062, 17, 16, 14, 15);	// hardware SPI

void setup() {
	bool success = epd.init();

	if (!success) {
		// Panic and freak out
		return;
	}

	epd.updateScreen();
}

void loop() {

	// Simple test:

	/*
	epd.updateScreenWithImage(zach_photo, k_image_4bit_monochrome);

	delay(DELAY_BETWEEN_IMAGES_MS);
	*/



	//delay(DELAY_BETWEEN_IMAGES_MS);
}
