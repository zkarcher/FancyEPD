// Interpolates between fromVal (progress==0) up to toVal (progress==1.0).
function lerp( fromVal, toVal, progress ) {
	return fromVal + (toVal-fromVal)*progress;
}

// ThreeJS: Vector interpolation is a destructive operation, and/or some functions (.lerpVectors) are buggy!
function lerpVector3( v0, v1, progress ) {
	return new THREE.Vector3( v0.x+(v1.x-v0.x)*progress, v0.y+(v1.y-v0.y)*progress, v0.z+(v1.z-v0.z)*progress );
}

// Convenient randomness functions
function maybe() {
	return Math.random() < 0.5;
}

// Returns a random number between 0..maxValue
function rand( maxValue ) {
	return Math.random() * (maxValue || 1.0);
}

// Returns a random number between fromValue..toValue
function randRange( fromValue, toValue ) {
	return Math.random() * (toValue-fromValue) + fromValue;
}

// Returns a random number between -value..value ("bi" == bipolar)
function randBi( value ) {
	return Math.random() * (2*(value||1.0)) - (value||1.0);
}

function randRangeBi( v0, v1 ) {
	return (Math.random() * (v1-v0)+v0) * ((Math.random()<0.5) ? 1 : -1);
}

function randInt( maxValue ) {
	return Math.floor( Math.random()*maxValue );
}

function randRangeInt( fromValue, toValue ) {	// exclusive!
	return fromValue + randInt( toValue - fromValue );
}

function randFromArray( ar ) {
	return ar[ randInt(ar.length) ];
}

// Constrains (clips) the value between min..max
function clamp( value, min, max ) {
	return Math.min( max, Math.max( min, value ));
}

// Wrap-around functions. Similar to modulus (%) but smarter handling of negative values.
function wrap( value, height ) {
	return value - (Math.floor( value / height ) * height);
}

// Wraps value as close to target as possible.
function wrapCloseToTarget( value, height, target ) {
	var diff = target - value;
	return value + (Math.round(diff/height) * height);
}

function getTouchX( event ) {
	if( event.hasOwnProperty('clientX') ) return event['clientX'];
	if( event.hasOwnProperty('touches') ) return event['touches'][0]['clientX'];
	if( event.hasOwnProperty('originalEvent') ) return event['originalEvent']['touches'][0]['clientX'];
	console.log("** getTouchX: error", event);
}
function getTouchY( event ) {
	if( event.hasOwnProperty('clientY') ) return event['clientY'];
	if( event.hasOwnProperty('touches') ) return event['touches'][0]['clientY'];
	if( event.hasOwnProperty('originalEvent') ) return event['originalEvent']['touches'][0]['clientY'];
	console.log("** getTouchY: error", event);
}

function rotateVec2( v2, addAngle ){
	var len = v2.length();
	var angle = Math.atan2( v2.y, v2.x );
	v2.set( len * Math.cos(angle+addAngle), len * Math.sin(angle+addAngle) );
}
