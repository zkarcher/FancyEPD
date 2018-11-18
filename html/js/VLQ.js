// https://en.wikipedia.org/wiki/Variable-length_quantity

function VLQ(ar, forceWordBits)
{
	// Try different vlq sizes.
	var bestLen = Infinity;
	var bestStream = null;
	var bestWordBits = 0;

	// wordBits _includes_ the initial bit indicating whether
	// the value continues.
	for (var wordBits = 2; wordBits <= 8; wordBits++) {
		if (forceWordBits && (wordBits != forceWordBits)) continue;

		var streamData = vlqBitstream(ar, wordBits);

		console.log("stream :: wordBits:", wordBits, "len:", streamData.length);

		if (streamData.length < bestLen) {
			bestLen = streamData.length;
			bestStream = streamData;
			bestWordBits = wordBits;
		}
	}

	if (!forceWordBits) {
		bestStream.unshift(bestWordBits);
	}

	return bestStream;
}

//   Store in a bitstream format. Values are encoded using
//   zig-zag encoding.
function VLQ_Delta(ar, accumCount, forceWordBits)
{
	// Init accums array (typically there are 2:
	// black vs white, or same vs XOR pixels)
	var buf = new ArrayBuffer(accumCount * 4);
	var accums = new Int32Array(buf);
	for (var i = 0; i < accumCount; i++) {
		accums[i] = 0;
	}

	// Determine VLQ values to be encoded
	var diffs = [];
	for (var i = 0; i < ar.length; i++) {
		var difference = ar[i] - accums[i % accumCount];
		diffs.push(difference);

		// Cache the value in the accumulator
		accums[i % accumCount] = ar[i];
	}

	// Try different vlq sizes.
	var bestLen = Infinity;
	var bestStream = null;
	var bestWordBits = 0;

	// wordBits _includes_ the initial bit indicating whether
	// the value continues.
	for (var wordBits = 2; wordBits <= 8; wordBits++) {
		if (forceWordBits && (wordBits != forceWordBits)) continue;

		var streamData = vlqBitstream_deltas(diffs, wordBits);

		console.log("delta stream :: wordBits:", wordBits, "len:", streamData.length);

		if (streamData.length < bestLen) {
			bestLen = streamData.length;
			bestStream = streamData;
			bestWordBits = wordBits;
		}
	}

	if (!forceWordBits) {
		bestStream.unshift(bestWordBits);
	}

	return bestStream;
}

// https://en.wikipedia.org/wiki/Zigzag_code
// 0->0, -1->1, 1->2, -2->3, 2->4, etc
function zigZagEncode(v)
{
	// Positive, and zero
	if (v >= 0) {
		return (v << 1);
	}

	// Negative
	return (-v << 1) - 1;
}

function vlqBitstream(ar, wordBits)
{
	var stream = new Bitstream();

	// Words: Initial bit indicates whether the word continues.
	// wordBits _includes_ the initial bit indicating whether
	// the value continues.
	var valueBits = wordBits - 1;

	for (var i = 0; i < ar.length; i++) {
		var v = ar[i];

		// How many bits are needed to encode this value?
		var vTemp = v;
		var bitCount = 1;
		while (vTemp) {
			vTemp >>= 1;
			if (!vTemp) break;
			bitCount++;
		}

		var wordCount = Math.ceil(bitCount / valueBits);

		// Value mask: 3 -> 0b111
		var valueMask = (0x1 << valueBits) - 1;

		for (var w = wordCount - 1; w >= 0; w--) {
			// Flag indicating whether value continues
			var doesContinue = (w > 0);
			stream.append((doesContinue ? 1 : 0), 1);

			stream.append((v >> (w * valueBits)) & valueMask, valueBits);
		}
	}

	// The stream can be terminated by ending with all 1's.
	// (This just inflates a value endlessly, until stream ends.)
	var data = stream.finish(0xff);
	return data;
}

// FIXME ZKA: Is there a bug in the values generated here?
function vlqBitstream_deltas(ar, wordBits)
{
	var stream = new Bitstream();

	// Words: Initial bit indicates whether the word continues.
	// wordBits _includes_ the initial bit indicating whether
	// the value continues.
	var valueBits = wordBits - 1;

	for (var i = 0; i < ar.length; i++) {
		var z = zigZagEncode(ar[i]);

		// How many bits are needed to encode this value?
		var zTemp = z;
		var bitCount = 1;
		while (zTemp) {
			zTemp >>= 1;
			if (!zTemp) break;
			bitCount++;
		}

		var wordCount = Math.ceil(bitCount / valueBits);

		// Value mask: 3 -> 0b111
		var valueMask = (0x1 << valueBits) - 1;

		for (var w = wordCount - 1; w >= 0; w--) {
			// Flag indicating whether value continues
			var doesContinue = (w > 0);
			stream.append((doesContinue ? 1 : 0), 1);

			stream.append((z >> (w * valueBits)) & valueMask, valueBits);
		}
	}

	// The stream can be terminated by ending with all 1's.
	// (This just inflates a value endlessly, until stream ends.)
	var data = stream.finish(0xff);
	return data;
}
