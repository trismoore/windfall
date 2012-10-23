
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

  x*=landscape.numPatchesX; y*=landscape.heightScale; z*=landscape.numPatchesZ;

  $('#cameraWindow #movement')
	.stop()
	.css({left:"0px"})
	.animate({left:1000},{duration:duration,easing:easing,step:function(f) {
		var t=f/1000.0;
		camera.setPos( (camOX+(x-camOX)*t), (camOY+(y-camOY)*t), (camOZ+(z-camOZ)*t));
		camera.setLook(camLX+(lookx-camLX)*t, camLY+(looky-camLY)*t, camLZ+(lookz-camLZ)*t );
		camera.setUp(camUX+(upx-camUX)*t, camUY+(upy-camUY)*t, camUZ+(upz-camUZ)*t );
		camera.setFOV( camFOV+(fov-camFOV)*t );
	}});
};

var buttons = {
 AboveAndWideElastic: "moveCamera(+0.5  ,+2.5  ,+0.5  , +0.1  ,-1.   ,+0.1  , +0.976,+0.201,+0.065,  60, 5000, 'easeOutElastic');",
             DipWide: "moveCamera(+0.338,+0.415,+0.281, +0.325,-0.246,+0.913, +0.034,+0.993,+0.101, 109, 3000, 'easeInOutQuart');",
           Telephoto: "moveCamera(+0.876,+1.126,+0.916, -0.708,-0.260,-0.658, -0.089,+0.991,-0.082,   6, 3000, 'easeInOutQuart');",
            Overview: "moveCamera(+0.108,+2.491,+0.064, +0.549,-1.330,+0.648, +0.334,+0.856,+0.394,  90, 3000, 'easeInOutQuart');",
               Circ1: "moveCamera(+0.804,+0.918,+0.746, -0.863,-0.495,-0.103, -0.363,+0.930,-0.056,  76, 3000, 'easeInOutQuart');",
               Circ2: "moveCamera(+0.901,+1.055,+0.393, -0.686,-0.551,+0.475, -0.349,+0.904,+0.247,  76, 3000, 'easeInOutQuart');",
               Circ3: "moveCamera(+0.632,+1.076,+0.143, +0.059,-0.582,+0.811, +0.020,+0.887,+0.461,  76, 3000, 'easeInOutQuart');",
               Circ4: "moveCamera(-0.033,+0.874,+0.088, +0.811,-0.513,+0.281, +0.366,+0.922,+0.124,  76, 3000, 'easeInOutQuart');",
             UKOgwen: "moveCamera(+0.524,+0.998,+0.679, -0.239,-0.540,-0.807, -0.032,+0.994,-0.106,  71, 3000, 'easeInOutQuart');",
         UKGlenNevis: "moveCamera(+0.475,+0.805,+0.317, -0.988,-0.142,+0.068, +0.311,+0.950,-0.022,  18, 3000, 'easeInOutQuart');",
          UKOverview: "moveCamera(+0.5,+1056.518,+0.493, +0.000,-1.000,+0.010, +0.000,+1.000,-0.020,  68, 1500, 'easeInOutQuart');",
};
function addButton(text, strToEval) {
	$('<button class="k-button">'+text+'</button>').click(function(){eval(strToEval);}).appendTo('#cameraWindow');
};
addButton("Output Position", "console.log(sprintf(\"moveCamera(%+0.3f,%+0.3f,%+0.3f, %+0.3f,%+0.3f,%+0.3f, %+0.3f,%+0.3f,%+0.3f, %3d, 3000, 'easeInOutQuart');\", "
  + "camera.posx/landscape.numPatchesX,camera.posy/landscape.heightScale,camera.posz/landscape.numPatchesZ, camera.lookx,camera.looky,camera.lookz, camera.upx,camera.upy,camera.upz, camera.fov));");
$('#cameraWindow').append('<hr/>');
$.each(buttons, function(k,v) { addButton(k,v); });

window.cameraGoToStartPosition = function() { eval(buttons.Overview); }
