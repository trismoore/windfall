$(document).ready(function() {

	window.loadingText = function(t) { $('#loadingText').html("Loading <i>"+t+"</i>"); };
	window.loadingTextAndFraction = function(t,f) { var f100=f*100; loadingText(t); $('#loadingBar').css({borderLeftWidth:f100+"px",width:100-f100}); }

	window.runningInBrowser = "undefined" == typeof(UI);

	if (runningInBrowser) {
		UI = {};
		UI.addToLog = function(m,c) { };
		UI.quit = function() { console.info("Would be quitting now."); };
		UI.loadStringFromFile = function(f,c) { 
			eval(c+"(\"{\\\"showConsole\\\":true}\");");
		}
		UI.saveStringToFile = function(f,str) { 
			alert("Not saving to file " + f + "\n" + str);
		}
		UI.modules = ["100console.js", "150useful.js", "300camera.js"];
		camera = {};
		camera.posx=camera.posy=camera.posz=1;
		camera.lookx=camera.looky=camera.lookz=0;
		camera.upx=camera.upz=0; camera.upy=1;
		camera.fov=60;
		camera.setPos=camera.setLook=camera.setUp=function(f){};
		camera.setFOV=function(f){camera.fov=f;}
	}

	window.quit = function() {
		if ('object' == typeof UI.static.autostop) {
			console.log("Running autostop objects");
			$.each(UI.static.autostop, function(k,v) {
				if (typeof v == 'function') v.call();
				else eval(v);
			});
		}
		console.log("Saving static object to file");
		saveStaticObject();
		console.log("Quitting");
		UI.quit();
	}

	UI.menuCallbacks = {};

	UI.addMenu = function(menu, callback, appendTo) {
		UI.menuCallbacks[menu.text] = callback;
		$('#topMenuBar').data('kendoMenu').append(menu, appendTo);
	}

	var onMenuItemSelected = function(e) {
		var t = e.item.firstChild.textContent;
		if (UI.menuCallbacks[t]) {
			UI.menuCallbacks[t](t,e);
		} else {
			tp = e.item.parentNode.parentNode.parentNode.firstChild.textContent;
			if (UI.menuCallbacks[tp]) 
				UI.menuCallbacks[tp](t,e);
		}
	}

	$("#topMenuBar").kendoMenu({
		select: onMenuItemSelected
	});

	UI.addMenu({text:"Quit"},function(){quit();});
			

	window.scrollbarSize = 0;
	var $div = $('<div />')
		.css({ width: 100, height: 100, overflow: 'auto', position: 'absolute', top: -1000, left: -1000 })
		.prependTo('body').append('<div />').find('div')
		.css({ width: '100%', height: 200 });
	scrollbarSize = 100 - $div.width();
	$div.parent().remove();

	window.isElementScrolledToBottom = function(l) {
		// check if the height of the element + the scroll posistion (- any scrollbar that may be there) is
		// the same as the full size of the element.
		return l.scrollTop() + l.height() - (l[0].scrollWidth>l[0].offsetWidth?scrollbarSize:0) == l[0].scrollHeight;
	}

	window.loadStaticObject = function(s) { 
		try {
			UI.static = JSONfn.parse(s);
			if (UI.static != undefined && UI.static.autorun != undefined && UI.static.autorun.length > 0) {
				$.each(UI.static.autorun,function(k,v) {
					if (typeof v == 'function') {
						console.log("Autorunning " + k + ", \"" + v.toString() + "\"");
						v.call();
					}
					else {
						window.eval(v);
						console.log("Autorunning " + k + ", \"" + v.entityify() + "\"");
					}
				});
			}
		} catch(e) {
			console.error("Uncaught error trying to load UI.static: " + e);
		}
		onStaticObjectFinished();
	}

	window.saveStaticObject = function() {
		UI.saveStringToFile("static.json", JSONfn.stringify(UI.static));
	}

	// load modules (this happens before autorun, incase we want to use one of the modules)
	console.log("Loading modules");
	UI.modules = UI.modules.sort();

	// load the first in the array, wait for it, and then recurse
	function loadModule() {
		if (UI.modules.length > 0) {
			var m = "modules/" + UI.modules.shift();
			loadingText(m);
			$.getScript(m)
				.done(function(script, textStatus) {
					console.log(m + " loaded OK: " + textStatus);
					loadModule();
				})
				.fail(function(jqxhr, settings, exception) {
					console.error("Module " + m + " failed to load!");
					console.error(exception.message);
					loadModule(); // try next anyway
				});
		} else {
			delete UI.modules;
			onModulesFinished();
		}
	}
	loadModule();

	console.log("Modules loaded, loading static object");
	loadingText("static object");

	// default values:
	UI.static = {};
	UI.static.showConsole = false;
	UI.static.autoExpandObjectsInConsole = false;
	UI.static.scrollOnEnter = true;
	UI.static.startConsoleMaximized = false;
	UI.static.autorun = [];
	UI.static.autostop = [];

	function onModulesFinished() {
		// load static file and parse (this also executes all autorun objects)
		UI.loadStringFromFile("static.json","loadStaticObject");
	}

	function onStaticObjectFinished() {
		console.log("UI all done!");
		loadingText("engine");
		UI.loaded(); // signal engine that it can go ahead with it's loading
	}

	window.onEngineFinishedLoading = function() {
		$('#loading').fadeOut();
	}

	// due to the nature of javascript asynchronisity, code here will be executed BEFORE modules have finished (and before static and autorun etc)
});
