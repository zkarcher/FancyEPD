function BinaryImage(canvas, mask)
{
	var data = canvas.getContext('2d').getImageData(0, 0, canvas.width, canvas.height).data;

	var buf = new ArrayBuffer(canvas.width * canvas.height);
	var out = new Uint8Array(buf);

	// (data.length / 4): Read red pixel data.
	// Skip green, blue, alpha.
	for (var i = 0; i < data.length / 4; i++) {
		out[i] = (data[i * 4] & mask) ? 1 : 0;
	}

	return out;
}
