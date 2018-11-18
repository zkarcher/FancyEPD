function FancyEncoder(canvas, compression, bpc, channelCount, channelData)
{
	var self = this;
	self.output = null;
	self.byteCount = 0;

	var data = canvas.getContext('2d').getImageData(0, 0, canvas.width, canvas.height).data;

	var ar = [];

	// Image header (for compression)
	if (compression) {
		// Version
		ar.push(0x1);

		// BPC (bits per channel) of image
		ar.push(0x1);
		ar.push(bpc);

		// Color channels per pixel
		ar.push(0x2);
		ar.push(channelCount);

		// Canvas width & height
		ar.push(0x3);
		ar = ar.concat(VLQ([canvas.width, canvas.height], 8));

		// Image data header
		ar.push(0x4);
	}

	function appendImageDataToArray(binImg) {
		if (!compression) {
			var bitstream = new Bitstream();
			for (var i = 0; i < binImg.length; i++) {
				bitstream.append(binImg[i], 1);
			}
			var data = bitstream.finish(0x0);

			ar = ar.concat(data);

		} else {	// Apply compression
			// 0x0: raw
			var bitstream = new Bitstream();
			for (var i = 0; i < binImg.length; i++) {
				bitstream.append(binImg[i], 1);
			}
			var rawData = bitstream.finish(0x0);
			rawData.unshift(0x0);	// Prepend compression format

			// 0x1: RLE
			var rle = RLE(binImg);
			var rleData = VLQ(rle);	// 2 px types: black, white
			rleData.unshift(0x1);	// Prepend compression format

			// 0x2: RLE_XOR
			// Bitstream: RLE runs, same vs XOR pixels.
			// Store the deltas using VLQ.
			var rleXor = RLE_XOR(binImg, canvas.width);
			var xorData = VLQ(rleXor);	// 2 px types: same as above, XOR of above.
			xorData.unshift(0x2);	// Compression format

			// Choose the smallest data
			var bestData = null;
			var options = [rawData, rleData, xorData];

			_.each(options, function(data){
				if (!bestData || (data.length < bestData.length)) {
					bestData = data;
				}
			});

			// Prepend data length
			var len = bestData.length;
			bestData = VLQ([len], 8).concat(bestData);

			ar = ar.concat(bestData);
		}
	}

	// Start with the least-significant channel.
	// Store this either raw, or (if user specified) compressed.
	for (var i = bpc - 1; i >= 0; i--) {
		for (var c = 0; c < channelCount; c++) {
			var binImg = BinaryImageFromArray(channelData[c], 0x80 >> i, {invert:true});
			appendImageDataToArray(binImg);
		}
	}

	//console.log(format, ":: LENGTH:", ar.length);

	self.byteCount = ar.length;

	var out = "";
	while (ar.length) {
		out += "\t";

		var chunk = ar.splice(0, 16);

		while (chunk.length) {
			out += ("   " + chunk.shift()).substr(-3) + ",";
		}

		out += "\n";
	}

	//console.log(out);

	self.output = out;
}
