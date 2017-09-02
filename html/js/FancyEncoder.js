function FancyEncoder(canvas, format)
{
	var self = this;

	var data = canvas.getContext('2d').getImageData(0, 0, canvas.width, canvas.height).data;

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

		case "1bpp_monochrome_rle_vlq":
		{
			var binImg = BinaryImage(canvas, 0x10);
			var rle = RLE(binImg);
			var vlq = VLQ(rle, 2);	// 2: black, and white.

			ar = vlq;
		}
		break;

		case "1bpp_monochrome_terrain_vlq":
		{
			var binImg = BinaryImage(canvas, 0x10);
			var terr = new TerrainEncoding(binImg, canvas.width);

			// Make a VLQ array
			var data = [];
			var slope = 0;
			var distance = 0;
			for (var i = 0; i < terr.length; i++) {
				var step = terr[i];

				// Differences
				var slopeDiff = wrap(step.slope - slope, 256);
				if (slopeDiff >= 128) {
					slopeDiff -= 256;
				}
				data.push(slopeDiff);

				var stepDiff = step.distance - distance;
				data.push(stepDiff);

				/*
				var slope = step.slope;
				if (slope >= 128) slope -= 256;
				data.push(slope);
				data.push(step.distance);
				*/
			}

			console.log("data", data);

			var vlq = VLQ(data, 2);
			ar = vlq;

			// Debug: Show bin img
			var cvs = document.createElement('canvas');
			cvs.width = canvas.width;
			cvs.height = canvas.height;

			var ctx = cvs.getContext('2d');
			var data = ctx.getImageData(0, 0, cvs.width, cvs.height).data;

			var d = 0;
			for (var y = 0; y < cvs.height; y++) {
				for (var x = 0; x < cvs.width; x++) {
					data[d] = data[d + 1] = data[d + 2] = (binImg[d / 4] ? 0xff : 0x0);	// red, green, blue
					data[d + 3] = 0xff;	// alpha

					d += 4;
				}
			}

			var imgData = new ImageData(data, cvs.width, cvs.height);

			ctx.putImageData(imgData, 0, 0, 0, 0, cvs.width, cvs.height);

			document.body.appendChild(cvs);
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
