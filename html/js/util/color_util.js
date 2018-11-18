function hex2rgb8( hex ) {
	return {
		r:	((hex&0xff0000)>>16),
		g:	((hex&0x00ff00)>> 8),
		b:	((hex&0x0000ff)    )
	};
}

function hex2rgb_f( hex ) {
	return {
		r:	((hex&0xff0000)>>16) * (1.0/0xff),
		g:	((hex&0x00ff00)>> 8) * (1.0/0xff),
		b:	((hex&0x0000ff)    ) * (1.0/0xff)
	};
}

// Fixes problems with dat.GUI, '#123456' is converted to int.
// Known problems: Does not assert anything about the incoming argument.
function colorToInt( thing ) {
	var str = String(thing);
	if( str.substr(0,1) == '#' ) {
		return parseInt( '0x' + substr(1) );	// Drop '#', assume the rest is hex
	}
	return parseInt(thing);
}

// Expects h,s,v in range 0..1
function hsv2rgb(h, s, v) {
	// Expand hue by 6 times for the 6 color regions.
	// Wrap the remainder inside 0..1
	var hWrap = h * 6.0;
	hWrap -= Math.floor(hWrap);

	var p = v * (1.0 - s);	// bottom (min) value
	var q = v * (1.0 - (hWrap * s));	// falling (yellow->green: the red channel falls)
	var t = v * (1.0 - ((1.0 - hWrap) * s));	// rising (red->yellow: the green channel rises)

	// Color region
	var hPos = h;
	if (hPos < 0.0) hPos += Math.ceil(-hPos);
	var hue_i = Math.floor(hPos * 6.0) % 6;

	switch (hue_i) {
		case 0:	return {r:v, g:t, b:p};	// red -> yellow
		case 1: return {r:q, g:v, b:p};	// yellow -> green
		case 2: return {r:p, g:v, b:t};	// green -> cyan
		case 3: return {r:p, g:q, b:v};	// cyan -> blue
		case 4: return {r:t, g:p, b:v};	// blue -> magenta
		case 5: return {r:v, g:p, b:q};	// magenta -> red
	}

	return {r:0, g:0, b:0};	// sanity escape
}

// Expects r,g,b in range 0..1
function rgb2hsv(r, g, b)
{
	var _min = Math.min(r, g, b);
	var _max = Math.max(r, g, b);
	var delta = _max - _min;

	// Prevent div by 0 later: Return grayscale/black/white
	if (delta == 0.0) {
		return {h:0, s:0, v:_max};
	}

	var h6 = 0.0;	// hue in range 0..6
	var s = (delta / _max);	// Saturation
	var v = _max;	// Value

	if (r == _max) {
		h6 = (g - b) / delta;	// Between yellow & magenta
	} else if (g == _max) {
		h6 = 2.0 + (b - r) / delta;	// Between cyan & yellow
	} else {	// Between magenta & cyan
		h6 = 4.0 + (r - g) / delta;
	}

	var h = h6 / 6.0;	// range: 0..1

	if (h < 0.0) {
		h += 1.0;	// Wrap around
	}

	return {h:h, s:s, v:v};
}
