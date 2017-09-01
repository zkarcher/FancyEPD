function RLE(ar)
{
	// RLE encoding. Count consecutive "runs" of
	// same-colored pixels. This is just a tiny state machine.
	var out = [];
	var runIsOn = false;
	var run = 0;

	for (var i = 0; i < ar.length; i++) {
		var pxOn = (ar[i] ? true : false);

		if (pxOn == runIsOn) {	// No change
			run++;

		} else {	// Pixel changed state. Store the run.
			out.push(run);
			runIsOn = pxOn;
			run = 1;	// Reset run count
		}
	}

	// Store the final run (if any).
	if (run) out.push(run);

	return out;
}
