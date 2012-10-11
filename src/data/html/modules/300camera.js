
var cameraIntervalTimer = 0;

var cameraWindow = $('<div id="cameraWindow">');

cameraWindow.append('<div id="cameraPos">Position</div>');
cameraWindow.append('<div id="cameraLook">Look</div>');
cameraWindow.append('<div id="cameraUp">Up</div>');
//cameraWindow.append('<div id="cameraFOV">FOV</div>');
var cameraFOVslider = $('<input id="cameraFOV" style="width:340px"/>');
cameraWindow.append($('<div>FOV: </div>').append(cameraFOVslider));

cameraWindow.kendoWindow({
	title: "Camera",
	width: "400px",
	height: "200px",
	resizable: false,
	appendTo: "#content",
	visible: false,
	close: function() { clearTimeout(cameraIntervalTimer); }
});

cameraFOVslider.kendoSlider({
	min: 5,
	max: 145,
	value: 60,
	smallStep: 5,
	largeStep: 20,
	tickPlacement: "bottomRight",
	tooltip: {format: "{0} &deg;"},
	change: function(e) { camera.setFOV(e.value); },
	slide: function(e) { camera.setFOV(e.value); }
});

UI.addMenu({text:"Camera"}, function() {
	$('#cameraWindow').data('kendoWindow').open();
	cameraIntervalTimer = setInterval(function() {
		$('#cameraPos').html(sprintf("Position: <span>(%.3f,%.3f,%.3f)</span>", camera.posx, camera.posy, camera.posz));
		$('#cameraLook').html(sprintf("Look: <span>(%.3f,%.3f,%.3f)</span>", camera.lookx, camera.looky, camera.lookz));
		$('#cameraUp').html(sprintf("Up: <span>(%.3f,%.3f,%.3f)</span>", camera.upx, camera.upy, camera.upz));
//		$('#cameraFOV').html(sprintf("FOV: %.3f", camera.fov));
	//	$('#cameraFOV').data('kendoSlider').value(camera.fov);
	}, 100);
});

