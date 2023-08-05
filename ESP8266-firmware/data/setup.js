var ap_mode = document.getElementById("ap_mode");
var lan_mode = document.getElementById("lan_mode");
var radio = document.getElementsByName("radio");
var infoForm = document.getElementById("info");
var submit = document.getElementById("submit");

var mode_str = "";

ap_mode.addEventListener("click", swAP, false);
lan_mode.addEventListener("click", swLan, false);
submit.addEventListener("click", setup, false);

function swAP() {
  radio[0].checked = true;
  infoForm.style.display = "none";
  mode_str = "ap";
}

function swLan() {
  radio[1].checked = true;
  infoForm.style.display = "flex";
  mode_str = "lan";
}

function setup() {
  var requist = new XMLHttpRequest();
  requist.open("POST", "http://192.168.1.1/setup", false);
  requist.setRequestHeader("Content-Type", "application/json");
  var json = {
    mode: mode_str,
    wifi: {
      ssid: document.getElementById("ssid").value.toString(),
      pwd: document.getElementById("pwd").value.toString(),
    },
  };
  requist.send(JSON.stringify(json));
  setTimeout(function () {
    window.location.reload();
  }, 5000);
}
