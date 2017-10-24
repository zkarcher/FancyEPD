function BinaryImage(canvas, mask, params)
{
	if (!params) params = {};

	var data = canvas.getContext('2d').getImageData(0, 0, canvas.width, canvas.height).data;

	var buf = new ArrayBuffer(canvas.width * canvas.height);
	var out = new Uint8Array(buf);

	// (data.length / 4): Read red pixel data.
	// Skip green, blue, alpha.
	for (var i = 0; i < data.length / 4; i++) {
		if (params.invert) {
			out[i] = (data[i * 4] & mask) ? 0 : 1;
		} else {
			out[i] = (data[i * 4] & mask) ? 1 : 0;
		}
	}

	return out;
}
