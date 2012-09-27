$(document).ready(function() {

	window.runningInBrowser = "undefined" == typeof(UI);

	if (runningInBrowser) {
		UI = {};
		UI.addToLog = function(m,c) { };
		UI.Quit = function() { alert("Would be quitting now."); };
	}

	var onMenuItemSelected = function(e) {
		var t = $(e.item).children(".k-link").text();
		//console.log("Top Menu Item Selected: " + t);
		switch(t) {
			case "Quit":
				console.log("Quitting");
				UI.Quit();
			break;
			case "Console":
				if ($('#consoleWindow').parent().css('display')!='block') {
					$('#consoleWindow').data("kendoWindow").open();
					setTimeout(scrollToBottomOfConsole, 100);
					$('#consoleInputBar input').focus();
				} else {
					$('#consoleWindow').data("kendoWindow").close();
				}
			break;
			default:
				console.log("Unknown menu item: " + t);
			break;
		}
	}

	$("#topMenuBar").kendoMenu({
		dataSource: [
			{ text: "Test menu",
				items: [
					{ text: "test1" },
					{ text: "test two" }
				]
			},
			{ text: "Console" },
			{ text: "Quit" }
		],
		select: onMenuItemSelected
	});

	var consoleWindow = $('#consoleWindow');
	if (!consoleWindow.data("kendoWindow")) {
		consoleWindow.kendoWindow({
			title: "Debug console",
			width: "900px",
			height: "500px",
			minWidth: 220,
			minHeight: 120,
			actions: ["Maximize","Close"],
			appendTo: "#content"
		});
		$('#consoleInputBar input').focus();
	}
	consoleWindow.data("kendoWindow").maximize();

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

	window.scrollToBottomOfConsole = function() {
		var cw = $('#consoleWindow ul.log');
		cw.scrollTop(cw[0].scrollHeight);
	}

	window.scrollToBottomOfConsoleSlow = function(speed) {
		if ('undefined' == typeof speed) speed='slow';
		var cw = $('#consoleWindow ul.log');
		cw.stop().animate({scrollTop: cw[0].scrollHeight}, speed);
	}

	// take a javascript type and pretty-print it (colour it, wrap quotes etc)	
	window.syntaxSpan = function(obj,name) {
		var val = 'syntaxSpan error';
		var cls = typeof(obj);
		switch(cls) {
			case 'number': val=obj.toString(); break;
			case 'undefined': val='undefined'; break;
			case 'string': val=obj.entityify().quote(); break;
			case 'function': val=obj.toString().entityify(); break;
			case 'boolean': val=obj?'true':'false'; break;
			case 'object': 
				if (obj===null) { 
					val='[null]'; 
					cls='null'; 
				} else if (obj instanceof Array) {
					val='[' + obj.toString().entityify() + ']';
					//val=preAndSyntax(obj);
					cls='array';
				} else if (Object.keys(obj).length == 0) {
					val=Object.prototype.toString.call(obj);
					if (val=='[object Object]') val+=' (empty)';
				} else {
					hasChildren=true;
					val='';//Object.prototype.toString.call(obj) + ' (with children)';
				}
				break;
			default:
				val=obj;
				console.warn('Should not be here in syntaxSpan. obj=',obj);
			break;
		}
		if ('undefined' == typeof name)
			return ('<span class="hl-'+cls+'">'+val+'</span>');
		else if (val != '')
			return ('<span class="hl-'+cls+'">'+name+' : '+val+'</span>');
		else 
			return ('<span class="hl-'+cls+'">'+name+'</span>');
	}

	elementTreeReferences = {};

	window.elementTreeRecurse = function(obj, tree, appendTo) {
		$.each(obj, function(k,v) {
			var hasChildren = (typeof v == 'object') && (v!==null) && (Object.keys(v).length > 0);
			var r = tree.data('kendoTreeView').append({
					text: syntaxSpan(v,k),
					encoded: false,
					hasChildren: hasChildren
				},appendTo);
			elementTreeReferences[r.data('uid')] = v;
		});
	}

	window.elementTree = function(obj, logType) {
		if ('undefined' == typeof logType) logType = "log";
		var typ = typeof obj;
		// if it's an object (not null) and has children then we should treeView it to allow exploring
		if ('object' == typ && null !== obj && Object.keys(obj).length > 0) {
			var elementTreeView = $('<div class="elementTreeView">').kendoTreeView();
			elementTreeView.data('kendoTreeView').bind('expand',function (e) {
				var scroll = isElementScrolledToBottom($('#consoleWindow ul.log'));
				var n = $(e.node);
				if (n.find('ul').length != 0) return; // already have children
				var r = elementTreeReferences[n.data('uid')];
				if ('undefined' == typeof(r))
					console.error("this node's data ref is undefined! event:",e);
				else 
					elementTreeRecurse(r, elementTreeView, n);
				if (scroll) scrollToBottomOfConsole();
			});
			var tmp = {};
			tmp[Object.prototype.toString.call(obj)] = obj;
			elementTreeRecurse(tmp, elementTreeView);
			var scroll = isElementScrolledToBottom($('#consoleWindow ul.log'));
			$('#consoleWindow ul.log').append($('<li class="'+logType+'">').append(elementTreeView));
			UI.addToLog(syntaxSpan(obj),logType);
			if (scroll) scrollToBottomOfConsole();
		} else { // not a complicated object, just print it without a tree
			addToConsole(syntaxSpan(obj),logType);
		}
	};
	
	var addToConsoleRaw = function(m) {
		var ulLog = $('#consoleWindow ul.log');
		var scroll = isElementScrolledToBottom(ulLog);
		ulLog.append(m);
		if (scroll) scrollToBottomOfConsole(); 
	};

	window.addToConsole = function(m,c) {
		//addToConsoleRaw("<li class='"+c+"'>"+$('<div/>').text(m).html()+"</li>"); // escape it (so that < and > show up)
		addToConsoleRaw("<li class='"+c+"'>"+m+"</li>"); // escape it (so that < and > show up)
	}
	window.addToConsoleAndLog = function(m,c) {
		if ("undefined" == typeof(c)) c='log';
		if (typeof m != 'string') m = syntaxSpan(m);
		UI.addToLog(m,c);   // send it back to the engine, so we can log it
		addToConsole(m,c);
	};

	// Hijack javascript console to also print to ours
	(function(){
		var oldLog = console.log;
		console.log = function (message) {
			addToConsoleAndLog(message,"log");
			oldLog.apply(console, arguments);
		};
		var oldError = console.error;
		console.error = function(message) {
			addToConsoleAndLog(message,"error");
			oldError.apply(console, arguments);
		};
		var oldWarning = console.warn;
		console.warn = function(message) {
			addToConsoleAndLog(message,"warning");
			oldWarning.apply(console, arguments);
		};
		var oldInfo = console.info;
		console.info = function(message) {
			addToConsoleAndLog(message,"debug");
			oldInfo.apply(console, arguments);
		};
	})();

	// neat history function (press UP in input box to get previous commands)
	UI.consoleInputBuffer = [];
	UI.consoleInputBufferPos = 0;

	$('#consoleInputBar input').keyup(function(e) {
		if (e.which == 13) { // 13==ENTER
			var t = $('#consoleInputBar input').val();
			UI.consoleInputBufferPos = UI.consoleInputBuffer.length;
			UI.consoleInputBuffer[UI.consoleInputBufferPos++] = t;
			console.log("> " + t.entityify());
			$('#consoleInputBar input').val("");
			//console.log("Evaling " + t);
			try {
				r = window.eval(t);
			//	if ("undefined" != typeof(r)) 
					//console.log(r);
					//console.log(preAndSyntax(r));
					elementTree(r);
			} catch (e) {
				console.error(e.message);
			}
			//eval(t);
		} else if (e.which == 38) { // 38 == UP
			if (UI.consoleInputBufferPos > 0) UI.consoleInputBufferPos--;
			$('#consoleInputBar input').val(UI.consoleInputBuffer[UI.consoleInputBufferPos]);
		} else if (e.which == 40) { // 40 == DOWN
			if (UI.consoleInputBufferPos < UI.consoleInputBuffer.length)
				UI.consoleInputBufferPos++;
			// you can go 'off the end' of the array (and get a blank line)
			if (UI.consoleInputBufferPos < UI.consoleInputBuffer.length) {
				$('#consoleInputBar input').val(UI.consoleInputBuffer[UI.consoleInputBufferPos]);
			} else 
				$('#consoleInputBar input').val("");
		}
	});

	// might make this a bit prettier at some point
	UI.history = function() { console.log(UI.consoleInputBuffer); }
	
	// if we're running in the engine, grab all saved console messages and display them
	// (the engine saves all messages while we're loading)
	if (runningInBrowser) {
		console.log("We're running in a browser, adding some dummy text here...");
		for (var i=0;i<100;++i) console.log("blah " + i);
		console.log("See also <a href='../../windfall_log.html'>log file</a>");
	} else
		UI.popBackBuffer();
});
