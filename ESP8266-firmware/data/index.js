var sw = document.getElementById("switch");
var point = document.getElementById("points");
var label = document.getElementById("label");
var reset = document.getElementById("reset");

sw.addEventListener("change", swchange, false);
point.addEventListener("input", pointchange, false);
reset.addEventListener("click", setup, false);

function init() {
  let WebURL = new URL(window.location.href);
  WebURL.pathname = "sw";
  var requist = new XMLHttpRequest();
  requist.open("get", WebURL.href.toString(), false);
  requist.send();
  var stat = requist.responseText;
  sw.checked = stat == "on" ? true : false;
  WebURL.pathname = "br";
  requist.open("get", WebURL.href.toString(), false);
  requist.send();
  var val = requist.responseText;
  if (Number.isInteger(val)) {
    label.innerHTML = val;
    point.value = val;
  }
}
function swchange() {
  let WebURL = new URL(window.location.href);
  WebURL.pathname += "ledit";
  var requist = new XMLHttpRequest();
  requist.open("POST", WebURL.href.toString(), false);
  requist.setRequestHeader("Content-Type", "application/json");
  var json = {
    led: {
      power: sw.checked ? "on" : "off",
    },
  };
  requist.send(JSON.stringify(json));
}
function pointchange() {
  label.innerHTML = point.value;
  let WebURL = new URL(window.location.href);
  WebURL.pathname += "ledit";
  var requist = new XMLHttpRequest();
  requist.open("POST", WebURL.href.toString(), false);
  requist.setRequestHeader("Content-Type", "application/json");
  var json = {
    led: {
      brightness: point.value,
    },
  };
  requist.send(JSON.stringify(json));
}
function setup() {
  let WebURL = new URL("http://192.168.1.1/setup");
  var requist = new XMLHttpRequest();
  requist.open("POST", WebURL.href.toString(), false);
  requist.setRequestHeader("Content-Type", "application/json");
  var json = {
    mode: "",
    wifi: {
      ssid: "",
      pwd: "",
    },
    led: {
      power: "off",
      num: 30,
      brightness: 255,
      same: "true",
      color: 0xffffff,
    },
  };
  requist.send(JSON.stringify(json));
  setTimeout(function () {
    window.location.reload();
  }, 5000);
}
