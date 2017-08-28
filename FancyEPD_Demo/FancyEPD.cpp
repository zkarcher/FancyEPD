#include <stdlib.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "FancyEPD.h"
#include "FancyEPD_models.h"

void FancyEPD::FancyEPD(epd_model_t model, uint32_t cs, uint32_t dc, uint32_t rs, uint32_t bs, int32_t d0, int32_t d1)
{
	_model = model;
	_cs = cs;
	_dc = dc;
	_rs = rs;
	_bs = bs;
	_d0 = d0;
	_d1 = d1;
}

bool FancyEPD::init()
{
	switch (model) {
		case k_epd_model_E2215CS062:
		{
			_width = 112;
			_height = 208;
		}
		break;

		default:
		{
			return false;
		}
		break;
	}

	_buffer = calloc(getBufferSize(), sizeof(uint8_t));

	return (bool)_buffer;
}

void FancyEPD::softwareSpi(uint8_t data) {
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
	if ((x < 0) || (y < 0) || (x >= _width || (y >= _height)) return;

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

void updateScreen(epd_update_t update_type)
{
	if (update_type == k_update_auto) {
		update_type = k_update_partial;
	}


}

void updateScreenWithImage(uint8_t * data, epd_image_format_t format, epd_update_t update_type)
{

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

void FancyEPD::_sendData(uint8_t command, uint8_t * data, uint16_t len) {
  digitalWrite( _cs, LOW );
  digitalWrite( _dc, LOW );

  delay(1);

	if ( _spiMode ) {
		SPI.transfer(command);
	} else {
		softwareSpi(command);
	}
  delay(1);	// FIXME ZKA

	digitalWrite(_dc, HIGH);
	delay(1);

  for (uint16_t i = 0; i < len; i++) {
    if (_spiMode) {
			SPI.transfer(data[i]);
		} else {
    	softwareSpi(data[i]);
		}
  }
  delay(1);	// FIXME ZKA

  digitalWrite(_cs, HIGH);
}

void FancyEPD::_set_xy_window(uint8_t xs, uint8_t xe, uint8_t ys, uint8_t ye)
{
	_sendData(0x44, (uint8_t *){xs, xe}, 2);
	_sendData(0x45, (uint8_t *){ys, ye}, 2);
}

void FancyEPD::_set_xy_counter(uint8_t x, uint8_t y)
{
	_sendData(0x4E, (uint8_t *){x}, 1);
	_sendData(0x4F, (uint8_t *){y}, 1);
}
