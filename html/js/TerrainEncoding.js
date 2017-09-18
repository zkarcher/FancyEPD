function TerrainEncoding(binImg, width)
{
	var wellBuf = new ArrayBuffer(width);
	var wells = new Uint8Array(wellBuf);
	for (var i = 0; i < width; i++) {
		wells[i] = 0;
	}

	var offset = -1;
	var accum = 0;
	var terr = [];	// {.slope = (uint8), .distance = (uint32), .wells = (Uint8Array)}

	while (offset < binImg.length) {
		var result = findBestTerrain(binImg, wellBuf, offset, accum);

		if (result.distance == 0) {
			console.log("** result.distance is 0. This is bug.");
			break;
		}

		terr.push(result);

		for (var d = 0; d < result.distance; d++) {
			accum += result.slope;
			offset++;
		}

		for (var w = 0; w < wells.length; w++) {
			wells[w] = result.wells[w];
		}
	}

	console.log("Terrain:", terr.length);
	console.log(terr);

	return terr;
}

// Do not modify wellBuf directly. Make a deep copy slice().
function findBestTerrain(binImg, wellBuf, offset, accum)
{
	var bestSlope = 0;
	var bestAccel = 0;
	var bestDistance = 0;
	var bestWells = null;

	for (var startSlope = 0; startSlope <= 0xff; startSlope++) {

		//for (var accel = -4; accel <= 3; accel++) {
			var slope = startSlope;
			var wb = wellBuf.slice(0);
			var wells = new Uint8Array(wb);
			var os = offset + 1;
			var ac = accum;
			var dist = 0;

			while (os < binImg.length) {
				ac += slope;
				//slope += accel;
				wells[os % wells.length] += ac;

				var isWellOn = wells[os % wells.length] >= 128;
				var isPxOn = binImg[os] ? true : false;

				// Mismatched pixel? Then the slope stops here.
				if (isWellOn != isPxOn) {
					break;
				}

				dist++;
				os++;
			}

			if (dist > bestDistance) {
				bestDistance = dist;
				bestSlope = startSlope;
				bestAccel = accel;
				bestWells = wells;
			}
		//}
	}

	return {
		"slope": bestSlope,
		"accel": bestAccel,
		"distance": bestDistance,
		"wells": bestWells,
	};
}
