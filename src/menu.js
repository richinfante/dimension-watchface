Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    // Load the remote config page
    Pebble.openURL("https://public.richinfante.com/dimension-watchface/pebble-dimension-config-v2.6.html");
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    console.log(e);
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
 Pebble.sendAppMessage(
   {
   "KEY_FOREGROUND" : configuration.KEY_FOREGROUND,
   "KEY_BACKGROUND" : configuration.KEY_BACKGROUND,
   "KEY_THICKNESS" : configuration.KEY_THICKNESS,
   "KEY_DIGIT_MARGIN" : configuration.KEY_DIGIT_MARGIN,
   "KEY_CORNER_RADIUS" : configuration.KEY_CORNER_RADIUS,
   "KEY_DIGIT_SPACING" : configuration.KEY_DIGIT_SPACING,
   },
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);