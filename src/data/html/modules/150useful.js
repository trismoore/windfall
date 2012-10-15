console.log("Setting up useful functions");

window.smooth = function(from, to, callback, duration, easing)
{
	// use JQuery to perform a smooth transition (because we can use JQuery's nice easing functions)

	if ('undefined' == typeof easing) easing="easeOutQuad";
	if ('undefined' == typeof duration) duration=1000;

	$('<div>').css({left:from+"px"}).animate({left:to},{duration:duration,easing:easing,step:callback});
}

window.splitAndPreLog = function(str)
{
	$.each(str.split('\n'), function(k,v) { 
		console.log(sprintf("<pre>%03d: %s</pre>",k,v));
	});	
}

window.cat = function(f)
{
	UI.loadStringFromFile(f, "splitAndPreLog");
}

