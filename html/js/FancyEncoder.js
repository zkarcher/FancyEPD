function FancyEncoder(canvas, format)
{
	var self = this;

	var data = canvas.getContext('2d').getImageData(0, 0, img.width, img.height).data;

	var ar = [];

	switch (format) {
		case "8bpp_monochrome_raw":
		{
			// 8-bit color, 1 pixel per byte
			for (var i = 0; i < data.length; i += 4) {
				ar.push(0xff - data[i]);
			}
		}
		break;

		case "4bpp_monochrome_raw":
		{
			// 4-bit color, 2 pixels per byte
			for (var i = 0; i < data.length; i += 8) {
				var byte = (data[i] & 0xf0) | ((data[i + 4] & 0xf0) >> 4);
				byte = 0xff - byte;
				ar.push(byte);
			}
		}
		break;

		case "2bpp_monochrome_raw":
		{
			// 2-bit color, 4 pixels per byte
			for (var i = 0; i < data.length; i += 16) {
				var byte = (data[i] & 0xc0) | ((data[i + 4] & 0xc0) >> 2) | ((data[i + 8] & 0xc0) >> 4) | ((data[i + 12] & 0xc0) >> 6);
				byte = 0xff - byte;
				ar.push(byte);
			}
		}
		break;

		case "1bpp_monochrome_raw":
		{
			// 1-bit color, 8 pixels per byte
			for (var i = 0; i < data.length; i += 32) {
				var byte = ((data[i     ] & 0x80)     ) |
				           ((data[i +  4] & 0x80) >> 1) |
				           ((data[i +  8] & 0x80) >> 2) |
				           ((data[i + 12] & 0x80) >> 3) |
				           ((data[i + 16] & 0x80) >> 4) |
				           ((data[i + 20] & 0x80) >> 5) |
				           ((data[i + 24] & 0x80) >> 6) |
				           ((data[i + 28] & 0x80) >> 7);
				byte = 0xff - byte;
				ar.push(byte);
			}
		}
		break;

		case "1bpp_monochrome_rle_vql":
		{
			var offset = 0;
			var mask = 0x80;

			// RLE encoding. Count consecutive "runs" of
			// same-colored pixels. This is just a tiny state machine.
			var rle = [];
			var runIsWhite = false;
			var run = 0;

			for (var y = 0; y < img.height; y++) {
				for (var x = 0; x < img.width; x++) {
					// [offset * 4]: Read the red component of each pixel.
					// Skip green, blue, alpha.
					var isWhite = (data[offset * 4] & mask);

					if (isWhite == runIsWhite) {	// No change.
						run++;

					} else {	// Pixel changed state. Store the run.
						rle.push(run);
						runIsWhite = isWhite;
						run = 1;
					}

					// Advance to next pixel
					offset++;
				}
			}

			// Store the final run (if any).
			if (run) rle.push(run);

			ar = rle;	// debuggin'

		}
		break;

		default:
		{
			console.log("** FancyEncoder: unrecognized format:", format);
		}
		break;

	}

	console.log(format, ":: LENGTH:", ar.length);

	var str = "";
	while (ar.length) {
		str += "\t";

		var chunk = ar.splice(0, 16);

		while (chunk.length) {
			str += ("   " + chunk.shift()).substr(-3) + ",";
		}

		str += "\n";
	}

	console.log(str);

}
