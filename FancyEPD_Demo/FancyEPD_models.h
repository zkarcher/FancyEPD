#ifndef FANCYEPD_MODELS_H
#define FANCYEPD_MODELS_H

int16_t epdWidth(epd_model_t model)
{
	switch (model) {
		case k_epd_CFAP122250A00213:    return 122;
		case k_epd_E2215CS062:          return 112;
		default:                        break;
	}

	return 0;	// not found
}

int16_t epdHeight(epd_model_t model)
{
	switch (model) {
		case k_epd_CFAP122250A00213:    return 250 / 8;
		case k_epd_E2215CS062:          return 208;
		default:                        break;
	}

	return 0;	// not found
}

#endif
