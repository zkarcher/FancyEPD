$(document).ready(function(){

	var img = null;

	const screens = {
		"E2215CS062": {dims:[112, 208]},
	};

	_.each(Object.keys(screens), function(key){
		var dims = screens[key].dims;

		// Add an <option> to the dropdown
		$('#screen').append($("<option></option>")
			.attr("value", key)
			.text(key + " [" + dims.join(",") + "]"));
	});

	function dropzoneError(err, file) {
		alert("dropzoneError: " + err);
	}

	function onDropzoneData(file) {
		console.log("onDropzoneData:", file);

		img = new Image;
		img.src = URL.createObjectURL(file);

		redrawImage();
	}

	function resizeDropzone() {
		var screen = screens[$('#screen').val()];
		var dims = screen['dims'];
		var w = dims[0];
		var h = dims[1];

		$("#dropzone").css({"width": w+"px", "height": h+"px"});
		$("#drop_canvas").attr({"width": w, "height": h});
	}

	function redrawImage() {
		if (!img) return;


	}

	$("#dropzone").filedrop({
		fallback_id: 'upload_button',
		fallback_dropzoneClick : true,
		error: dropzoneError,
		allowedfiletypes: ['image/jpeg','image/png','image/gif'],
		allowedfileextensions: ['.jpg','.jpeg','.png','.gif'],
		maxfiles: 1,
		beforeEach: onDropzoneData
	});

	// Init
	resizeDropzone();



	var img = document.getElementById('image');

	$(img).attr("src", "img/angel2.png");
	$(img).on("load", function(){
		console.log("loaded");

		var canvas = document.createElement('canvas');
		canvas.width = img.width;
		canvas.height = img.height;
		canvas.getContext('2d').drawImage(img, 0, 0, img.width, img.height);

		var enc = new FancyEncoder(canvas, "1bpp_monochrome_rle_xor_vlq");
	});

});
