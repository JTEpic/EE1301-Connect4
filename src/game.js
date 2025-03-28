// global variables
var deviceID = "";
//expires:
var accessToken = "";
var baseURL = "https://api.particle.io/v1/devices/"

document.getElementById("data1").innerHTML = "REFRESH";

const xLength=7,yLength=6;
//create array and set all to 0, (0=open, 1=player 1, 2=player 2)
let array=Array(xLength).fill(Array(yLength).fill(0));

function start(objButton) {
    var varName = "temperature"; // your cloud variable name goes here
    $.ajax({
      url: baseURL + deviceID + '/' + varName,
      headers: { 'Authorization': 'Bearer ' + accessToken },
      method: 'GET',
      success: function (resp) {
        document.getElementById("data1").innerText = resp.result;
      },
    });
}