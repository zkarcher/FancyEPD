function Bitstream()
{
	var self = this;
	self.data = [];

	var byte = 0x0;
	var mask = 0x80;

	self.append = function(value, bitCount) {
		for (var b = bitCount - 1; b >= 0; b--) {
			// Start a new byte?
			if (!mask) {
				byte = 0x0;
				mask = 0x80;
			}

			if (value & (0x1 << b)) {
				byte |= mask;
			}

			// Advance to next bit
			mask >>= 1;
			if (!mask) {
				self.data.push(byte);
			}
		}
	}

	self.finish = function(appendByte) {
		if (mask) {
			while (mask) {
				byte |= (appendByte & mask);
				mask >>= 1;
			}

			self.data.push(byte);
		}

		return self.data;
	}
}
