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
	k_epd_none = 0,
	k_epd_E2215CS062,	// Pervasive Displays 2.15" : http://www.pervasivedisplays.com/products/215
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
	// (Not recommended) k_update_no_blink usually looks better.
	k_update_partial,

	// Stronger than _partial. Best for general use.
	k_update_no_blink,

	// Quick inverted->normal transition.
	k_update_quick_refresh,

	// Manufacturer's default. Exciting blink and strobe effects.
	k_update_builtin_refresh,

	// INTERNAL (Don't use!)
	k_update_INTERNAL_image_layer,	// grayscale image layers

} epd_update_t;

typedef struct {
	int16_t xMin;
	int16_t yMin;
	int16_t xMax;
	int16_t yMax;
} window16;

typedef struct {
	const uint8_t * data;
	uint8_t mask;	// Start at MSB (0x80), shift right towards LSB (0x00)
	uint8_t word_size;
} vlq_decoder;

class FancyEPD : public Adafruit_GFX {
public:
	FancyEPD(epd_model_t model, uint32_t cs, uint32_t dc, uint32_t rs, uint32_t bs, uint32_t d0 = 0xffff, uint32_t d1 = 0xffff);
	bool init(uint8_t * optionalBuffer = NULL, epd_image_format_t bufferFormat = k_image_1bit);
	uint8_t * getBuffer();
	uint32_t getBufferSize();
	void clearBuffer(uint8_t color = 0);
	bool getAnimationMode();
	void setAnimationMode(bool isOn);
	void markDisplayDirty();
	void markDisplayClean();
	bool getPixel(int16_t x, int16_t y);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void setBorderColor(uint8_t color);

	// Waveform timing for all update types, except ones which
	// cannot be overriden, like:  k_update_builtin_refresh.
	//
	// Some updates have no inverted blink, and will ignore
	// the time_inverse value.
	void setCustomTiming(epd_update_t update_type, uint8_t time_normal = 0, uint8_t time_inverse = 0);
	void restoreDefaultTiming(epd_update_t);

	void update(epd_update_t update_type = k_update_auto);
	void updateWithImage(const uint8_t * data, epd_image_format_t format, epd_update_t update_type = k_update_auto);
	uint8_t updateWithCompressedImage(const uint8_t * data, epd_update_t update_type = k_update_auto);

	void setTemperature(uint8_t temperature);
	void freeBuffer();
	~FancyEPD();

protected:
	epd_model_t _model;
	uint32_t _d0, _d1, _cs, _dc, _rs, _bs;
	uint8_t _temperature;
	uint8_t _borderColor, _borderBit;
	bool _hardwareSPI;
	uint8_t * _buffer;
	bool _didMallocBuffer;
	epd_image_format_t _bufferFormat;
	uint8_t _updatesSinceRefresh;

	// Animation mode: Only redraw the pixels inside _window
	bool _isAnimationMode;
	window16 _window, _prevWindow;

	uint8_t _timingNormal[(uint8_t)k_update_builtin_refresh];
	uint8_t _timingInverse[(uint8_t)k_update_builtin_refresh];

	void _softwareSPI(uint8_t data);
	void _sendData(uint8_t index, uint8_t * data, uint16_t len);

	void _applyCustomTiming(epd_update_t update_type, uint8_t * timing_normal, uint8_t * timing_inverse);

	void _sendImageLayer(uint8_t layer_num, uint8_t layer_count, uint8_t newBorderBit);

	void _willUpdateWithImage(epd_update_t update_type);
	void _screenWillUpdate(epd_update_t update_type);

	void _sendDriverOutput();
	void _sendGateScanStart();
	void _sendDataEntryMode();
	void _sendGateDrivingVoltage();
	void _sendAnalogMode();
	void _sendTemperatureSensor();
	void _sendWaveforms(epd_update_t update_type, uint8_t time_normal = 0, uint8_t time_inverse = 0);
	void _sendBorderBit(epd_update_t update_type, uint8_t newBit);
	void _sendBufferData();
	void _sendUpdateActivation(epd_update_t update_type);
	void _sendWindow();

	void _applyRotationForBuffer(int16_t * x, int16_t * y);

	void _swapBufferBytes(int16_t xMinByte, int16_t yMin, int16_t xMaxByte, int16_t yMax, bool ascending);

	uint32_t _vlqDecode(vlq_decoder * decoder);
};

#endif
