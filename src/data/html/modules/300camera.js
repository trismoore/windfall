
var cameraIntervalTimer = 0;

var cameraWindow = $('<div id="cameraWindow">');

cameraWindow.append('<div id="movement" style="display:none;"/>'); // used for animating camera

cameraWindow.append('<div id="cameraPos">Position</div>');
cameraWindow.append('<div id="cameraLook">Look</div>');
cameraWindow.append('<div id="cameraUp">Up</div>');
//cameraWindow.append('<div id="cameraFOV">FOV</div>');
var cameraFOVslider = $('<input id="cameraFOV" style="width:300px"/>');
cameraWindow.append($('<div style="height:30px">FOV: </div>').append(cameraFOVslider));
cameraWindow.append('<br/>');
cameraWindow.append('<hr/>');

cameraWindow.kendoWindow({
	title: "Camera",
	width: "400px",
	height: "300px",
	resizable: false,
	appendTo: "#content",
	visible: false,
	close: function() { clearTimeout(cameraIntervalTimer); }
});

cameraFOVslider.kendoSlider({
	min: 1,
	max: 145,
	value: 60,
	smallStep: 1,
	largeStep: 20,
	tickPlacement: "bottomRight",
	tooltip: {format: "{0} &deg;"},
	showButtons: false,
//	change: function(e) { camera.setFOV(e.value); },
	slide: function(e) { camera.setFOV(e.value); }
});

UI.addMenu({text:"Camera"}, function() {
	$('#cameraWindow').data('kendoWindow').open();
	cameraIntervalTimer = setInterval(function() {
		$('#cameraPos').html(sprintf("Position: <span>(%.3f,%.3f,%.3f)</span>", camera.posx, camera.posy, camera.posz));
		$('#cameraLook').html(sprintf("Look: <span>(%.3f,%.3f,%.3f)</span>", camera.lookx, camera.looky, camera.lookz));
		$('#cameraUp').html(sprintf("Up: <span>(%.3f,%.3f,%.3f)</span>", camera.upx, camera.upy, camera.upz));
//		$('#cameraFOV').html(sprintf("FOV: %.3f", camera.fov));
		$('#cameraFOV').data('kendoSlider').value(camera.fov);
	}, 1000/30 ); // 30fps
});

var cameraMoveX, cameraMoveY, cameraMoveZ;

window.moveCamera = function(x,y,z, lookx,looky,lookz, upx,upy,upz, fov, duration, easing)
{
  if ('undefined' == typeof upx) { upx=camera.upx;upy=camera.upy;upz=camera.upz; }
  if ('undefined' == typeof fov) { fov=camera.fov; }
  if ('undefined' == typeof duration) { duration = 1000; }
  if ('undefined' == typeof easing) { easing="easeOutQuad"; }
  
  var camOX=camera.posx, camOY=camera.posy, camOZ=camera.posz;
  var camLX=camera.lookx,camLY=camera.looky,camLZ=camera.lookz;
  var camUX=camera.upx,camUY=camera.upy,camUZ=camera.upz;
  var camFOV=camera.fov;

  $('#cameraWindow #movement')
	.stop()
	.css({left:"0px"})
	.animate({left:1000},{duration:duration,easing:easing,step:function(f) {
		var t=f/1000.0;
		camera.setPos( camOX+(x-camOX)*t, camOY+(y-camOY)*t, camOZ+(z-camOZ)*t );
		camera.setLook(camLX+(lookx-camLX)*t, camLY+(looky-camLY)*t, camLZ+(lookz-camLZ)*t );
		camera.setUp(camUX+(upx-camUX)*t, camUY+(upy-camUY)*t, camUZ+(upz-camUZ)*t );
		camera.setFOV( camFOV+(fov-camFOV)*t );
	}});
};

var buttons = {
	AboveAndWide: "moveCamera(+0.650,+0.490,+0.393, -0.404,-0.866,+0.294, -0.577,+0.613,+0.538, 100, 3000, 'easeOutElastic');",
	InDipWideish: "moveCamera(+0.612,+0.171,+0.861, -0.300,-0.590,-0.750, +0.000,+1.000,+0.000,  70, 3000, 'easeInOutQuart');",
	OnHillTele:   "moveCamera(+0.342,+0.282,+0.725, +0.772,-0.250,-0.584, -0.307,+0.901,+0.291,  20, 3000, 'easeInOutQuart');",
	UpsideTele:   "moveCamera(+0.867,+0.359,+0.887, -0.364,-0.164,-0.917, -0.241,-0.881,+0.406,   5, 3000, 'easeInOutQuart');",
	NausiaInducinglyWide: "moveCamera(+0.339,+0.163,+0.342, +0.425,-0.885,+0.194, +0.340,+0.894,+0.292, 145, 3000, 'easeInOutQuart');",
	Patches56Overview: "moveCamera(+4.471,+3.043,+5.687, -0.220,-0.702,-0.677, -0.212,+0.802,-0.556,  72, 3000, 'easeInOutQuart');",
	At200: "moveCamera(+199.618,+2.199,+199.751, -1.213,-0.220,-1.217, +0.330,+0.883,+0.333,  87, 3000, 'easeInOutQuart');",
	Overview: "moveCamera(-1.130,+7.750,-1.266, +0.573,-0.698,+0.429, +0.470,+0.807,+0.358,  90, 3000, 'easeInOutQuart');",
};
function addButton(text, strToEval) {
	$('<button class="k-button">'+text+'</button>').click(function(){eval(strToEval);}).appendTo('#cameraWindow');
};
addButton("Output Position", "console.log(sprintf(\"moveCamera(%+0.3f,%+0.3f,%+0.3f, %+0.3f,%+0.3f,%+0.3f, %+0.3f,%+0.3f,%+0.3f, %3d, 3000, 'easeInOutQuart');\", camera.posx,camera.posy,camera.posz, camera.lookx,camera.looky,camera.lookz, camera.upx,camera.upy,camera.upz, camera.fov));");
$('#cameraWindow').append('<hr/>');
$.each(buttons, function(k,v) { addButton(k,v); });
