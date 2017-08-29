#include <stdlib.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "FancyEPD.h"

static int16_t _modelWidth(epd_model_t model)
{
	switch (model) {
		case k_epd_model_E2215CS062:    return 112;
		default:                        break;
	}

	return 0;	// not found
}

static int16_t _modelHeight(epd_model_t model)
{
	switch (model) {
		case k_epd_model_E2215CS062:    return 208;
		default:                        break;
	}

	return 0;	// not found
}

FancyEPD::FancyEPD(epd_model_t model, uint32_t cs, uint32_t dc, uint32_t rs, uint32_t bs, uint32_t d0, uint32_t d1) : Adafruit_GFX(_modelWidth(model), _modelHeight(model))
{
	_model = model;
	_cs = cs;	// Chip select
	_dc = dc;	// Data/command
	_rs = rs;	// Register select
	_bs = bs;	// Busy signal
	_d0 = d0;
	_d1 = d1;
	_spiMode = (d0 == 0xffff);
	_width = _modelWidth(model);
	_height = _modelHeight(model);
	_temperature = 0x1A;
}

bool FancyEPD::init()
{
	_buffer = (uint8_t *)calloc(getBufferSize(), sizeof(uint8_t));
	if (!_buffer) return false;

	// SPI
	if (_spiMode) {
		SPI.begin();

	} else {
		// Software SPI
		pinMode(_d0, OUTPUT);
		pinMode(_d1, OUTPUT);
	}

	pinMode(_cs, OUTPUT);
  pinMode(_dc, OUTPUT);
  pinMode(_rs, OUTPUT);
  pinMode(_bs, INPUT_PULLUP);

	// Reset the screen
	digitalWrite(_rs, HIGH);
	delay( 1 );                       //Delay 1ms	// FIXME ZKA
	digitalWrite(_cs, HIGH);

	return true;
}

int16_t FancyEPD::width()
{
	return _width;
}

int16_t FancyEPD::height()
{
	return _height;
}

uint8_t * FancyEPD::getBuffer()
{
	return _buffer;
}

uint32_t FancyEPD::getBufferSize()
{
	// Assumes 1-bit buffer
	return (_width * _height) / 8;
}

// Override Adafruit_GFX basic function for setting pixels
void FancyEPD::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

	int16_t temp;
	switch (getRotation()) {
		case 1:
		{
			temp = x;
			x = (_width - 1) - y;
			y = temp;
		}
		break;

		case 2:
		{
			x = (_width - 1) - x;
			y = (_height - 1) - y;
		}
		break;

		case 3:
		{
			temp = x;
			x = y;
			y = (_height - 1) - temp;
		}
		break;

		default:
			break;
	}

	uint8_t *ptr = &_buffer[(x / 8) + y * ((_width + 7) / 8)];

	if (color) {
		*ptr |= 0x80 >> (x & 7);
	} else {
		*ptr &= ~(0x80 >> (x & 7));
	}
}

void FancyEPD::setTemperature(uint8_t temperature)
{
	// TODO: Temperature is a 12-bit value, so we're
	//       losing some resolution here.
	_temperature = temperature;
}

void FancyEPD::updateScreen(epd_update_t update_type)
{
	if (update_type == k_update_auto) {
		update_type = k_update_quick_refresh;
	}

	_waitForBusySignal();
	_prepareForScreenUpdate();
	_sendImageData();
	_sendUpdateActivation(update_type);
}

void FancyEPD::updateScreenWithImage(uint8_t * data, epd_image_format_t format, epd_update_t update_type)
{
	// FIXME ZKA
}

void FancyEPD::destroy()
{
	if (_buffer) {
		free(_buffer);
		_buffer = NULL;
	}
}

//
//  PRIVATE
//

void FancyEPD::_waitForBusySignal()
{
	// Ensure the busy pin is LOW
	while (digitalRead( _bs ) == HIGH);
}

void FancyEPD::_softwareSPI(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		if (data & (0x80 >> i)) {
			digitalWrite( _d1, HIGH );
		} else {
			digitalWrite( _d1, LOW );
		}

		digitalWrite( _d0, HIGH );
		digitalWrite( _d0, LOW );
	}
}

void FancyEPD::_sendData(uint8_t command, uint8_t * data, uint16_t len) {
  digitalWrite( _cs, LOW );
  digitalWrite( _dc, LOW );

  delay(1);	// FIXME ZKA

	if ( _spiMode ) {
		SPI.transfer(command);
	} else {
		_softwareSPI(command);
	}

  delay(1);	// FIXME ZKA

	digitalWrite(_dc, HIGH);

	delay(1);	// FIXME ZKA

  for (uint16_t i = 0; i < len; i++) {
    if (_spiMode) {
			SPI.transfer(data[i]);
		} else {
    	_softwareSPI(data[i]);
		}
  }

  delay(1);	// FIXME ZKA

  digitalWrite(_cs, HIGH);
}

void FancyEPD::_prepareForScreenUpdate()
{
	_sendDriverOutput();
	_sendGateScanStart();
	_sendDataEntryMode();
	_sendGateDrivingVoltage();
	_sendAnalogMode();
	_sendTemperatureSensor();
	_reset_xy();
}

void FancyEPD::_sendDriverOutput()
{
	uint8_t data[] = {0xCF, 0x00};
	_sendData(0x01, data, 2);
}

void FancyEPD::_sendGateScanStart()
{
	uint8_t data[] = {0x0};
	_sendData(0x0F, data, 1);
}

void FancyEPD::_sendDataEntryMode()
{
	uint8_t data[] = {0x03};
	_sendData(0x11, data, 1);
}

void FancyEPD::_sendGateDrivingVoltage()
{
	uint8_t data[] = {0x10, 0x0A};
	_sendData(0x03, data, 2);
}

void FancyEPD::_sendAnalogMode()
{
	uint8_t data[] = {0, 0, 0};
	_sendData(0x05, data, 1);
	_sendData(0x75, data, 3);
}

void FancyEPD::_sendTemperatureSensor()
{
	uint8_t data[] = {_temperature, 0x0};
	_sendData(0x1A, data, 2);
}

void FancyEPD::_sendImageData()
{
	_sendData(0x24, _buffer, getBufferSize());
}

void FancyEPD::_sendUpdateActivation(epd_update_t update_type)
{
	uint8_t sequence = 0xF7;
	_sendData(0x22, &sequence, 1);	// Display Update type

	_sendData(0x20, NULL, 0);	// Master activation
}

void FancyEPD::_reset_xy()
{
	_send_xy_window(0, (width() >> 3) - 1, 0, height() - 1);
	_send_xy_counter(0, 0);
}

void FancyEPD::_send_xy_window(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye)
{
	uint8_t data_x[] = {xs, xe};
	_sendData(0x44, data_x, 2);
	uint8_t data_y[] = {ys, ye};
	_sendData(0x45, data_y, 2);
}

void FancyEPD::_send_xy_counter(uint8_t x, uint8_t y)
{
	_sendData(0x4E, &x, 1);
	_sendData(0x4F, &y, 1);
}
