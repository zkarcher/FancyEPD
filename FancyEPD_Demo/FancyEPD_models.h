#ifndef FANCYEPD_MODELS_H
#define FANCYEPD_MODELS_H

epd_driver_t modelDriver(epd_model_t model) {
	switch (model) {
		case k_epd_CFAP122250A00213:
		case k_epd_E2215CS062:
			return k_driver_IL3895;

		case k_epd_CFAP128296C00290:
		case k_epd_CFAP128296D00290:
			return k_driver_CFAP128296;

		default: break;

	}

	return k_driver_unknown;
}

int16_t epdWidth(epd_model_t model)
{
	switch (model) {

		// Crystalfontz
		case k_epd_CFAP122250A00213:    return 122;
		case k_epd_CFAP128296C00290:    return 128;
		case k_epd_CFAP128296D00290:    return 128;

		// Pervasive Displays
		case k_epd_E2215CS062:          return 112;

		default:                        break;
	}

	return 0;	// not found
}

int16_t epdHeight(epd_model_t model)
{
	switch (model) {

		// Crystalfontz
		case k_epd_CFAP122250A00213:    return 250;
		case k_epd_CFAP128296C00290:    return 296;
		case k_epd_CFAP128296D00290:    return 296;

		// Pervasive Displays
		case k_epd_E2215CS062:          return 208;

		default:                        break;
	}

	return 0;	// not found
}

uint8_t colorChannelsForModel(epd_model_t model)
{
	switch (model) {
		case k_epd_CFAP128296D00290:    return 2;	// blk+red
		default: break;
	}

	return 1;
}

#endif
