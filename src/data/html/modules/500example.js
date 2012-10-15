
UI.addMenu({text:"Example menu item"}, function(t) {
	console.log("Example function called from " + t);
});

UI.addMenu({
	text: "Multi level menu test",
	items: [
		{ text: "item one" },
		{ text: "<span style='color: blue'>item two</span>", encoded:false },
		{ text: "item three" },
		{ text: "item <b>four</b> <i>here</i>", encoded: false }
	]
}, function(t) { console.log("multi level test, pressed: "+t); } );

console.log("Loaded <b>example</b> plugin.");

