#ifndef FANCY_EPD_H
#define FANCY_EPD_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <SPI.h>
#include <Adafruit_GFX.h>

typedef enum epd_model_t {
	k_epd_model_none = 0,
	k_epd_model_E2215CS062,	// Pervasive Displays 2.15" : http://www.pervasivedisplays.com/products/215
} epd_model_t;

typedef enum epd_image_format_t {
	k_image_none = 0,
	k_image_1bit,             // 1 byte == 8 pixels
	k_image_2bit_monochrome,  // 1 byte == 4 pixels
	k_image_4bit_monochrome,  // 1 byte == 2 pixels
	k_image_8bit_monochrome,  // 1 byte == 1 pixel
} epd_image_format_t;

typedef enum epd_update_t {
	k_update_none = 0,

	// FancyEPD chooses what it feels is best.
	k_update_auto,

	// Only applies voltage to changed pixels.
	k_update_partial,

	// Stronger than _partial. Best for general use.
	k_update_no_blink,

	// Quick inverted->normal transition.
	k_update_quick_refresh,

	// Manufacturer's default. Exciting blink and strobe effects.
	k_update_builtin_refresh,

} epd_update_t;

class FancyEPD : public Adafruit_GFX {
public:
	FancyEPD(epd_model_t model, uint32_t cs, uint32_t dc, uint32_t rs, uint32_t bs, uint32_t d0 = 0xffff, uint32_t d1 = 0xffff);
	bool init();
	int16_t width();
	int16_t height();
	uint8_t * getBuffer();
	uint32_t getBufferSize();
	void clearBuffer(uint8_t color = 0);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void updateScreen(epd_update_t update_type = k_update_auto);
	void updateScreenWithImage(uint8_t * data, epd_image_format_t format, epd_update_t update_type = k_update_auto);
	void setTemperature(uint8_t temperature);

	void destroy();

private:
	epd_model_t _model;
	uint32_t _d0, _d1, _cs, _dc, _rs, _bs;
	uint8_t _temperature;
	bool _spiMode;
	uint8_t * _buffer;
	int16_t _width, _height;

	void _waitUntilNotBusy();
	void _softwareSPI(uint8_t data);
	void _sendData(uint8_t index, uint8_t * data, uint16_t len);

	void _prepareForScreenUpdate(epd_update_t update_type);
	void _sendDriverOutput();
	void _sendGateScanStart();
	void _sendDataEntryMode();
	void _sendGateDrivingVoltage();
	void _sendAnalogMode();
	void _sendTemperatureSensor();
	void _sendWaveforms(epd_update_t update_type);
	void _sendImageData();
	void _sendUpdateActivation(epd_update_t update_type);

	void _reset_xy();
	void _send_xy_window(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye);
	void _send_xy_counter(uint8_t x, uint8_t y);
};

#endif
