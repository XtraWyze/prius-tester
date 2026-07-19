// ======================================================
// portal.cpp
// ESP32 WiFi Web Server
// ======================================================
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

#include "portal.h"
#include "battery.h"
#include "FakeINA226.h"
#include "logger.h"
#include "web_api.h"

extern FakeINA226 sensor;
extern BatteryTest battery;
// ------------------------------------
// WiFi Settings
// ------------------------------------

const char* ssid = "PriusBatteryTester";
const char* password = "12345678";

WebServer server(80);

// ------------------------------------
// ------------------------------------
// Status API
// ------------------------------------


// ------------------------------------
// Home Page
// ------------------------------------

void handleRoot() {

server.send(200,"text/html",R"rawliteral(
<!DOCTYPE html>
<html>
<head>

<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Prius Battery Analyzer</title>

<style>

*{
box-sizing:border-box;
font-family:Arial,sans-serif;
}

body{
margin:0;
background:#111;
color:white;
text-align:center;
}

h1{
margin:0;
padding:18px;
background:#181818;
box-shadow:0 2px 10px rgba(0,0,0,.5);
}

.card{
background:#222;
margin:20px;
padding:20px;
border-radius:15px;
box-shadow:0 0 15px rgba(0,255,0,.15);
}

.status{
font-size:30px;
font-weight:bold;
margin-bottom:15px;
}

.row{
display:flex;
justify-content:space-between;
padding:10px 0;
font-size:22px;
border-bottom:1px solid #333;
}

.value{
color:#00ff66;
font-weight:bold;
}

button{

width:90%;
max-width:400px;

padding:16px;

margin:8px;

border:none;
border-radius:10px;

font-size:20px;

cursor:pointer;

transition:.2s;

}

button:hover{

transform:scale(1.02);

}

.start{
background:#16a34a;
color:white;
}

.stop{
background:#dc2626;
color:white;
}

.reset{
background:#2563eb;
color:white;
}

.download{
background:#0891b2;
color:white;
}

.delete{
background:#991b1b;
color:white;
}

footer{

margin-top:30px;
padding:20px;
font-size:14px;
color:#888;

}

canvas{
width:100% !important;
max-height:320px;
}

</style>

</head>

<body>

<h1>🔋 Prius Battery Analyzer</h1>

<div class="card">

<div class="status" id="status">
🔴 STOPPED
</div>

<div class="row">
<span>Voltage</span>
<span class="value" id="voltage">0.00 V</span>
</div>

<div class="row">
<span>Current</span>
<span class="value" id="current">0.00 A</span>
</div>

<div class="row">
<span>Power</span>
<span class="value" id="power">0.00 W</span>
</div>

<div class="row">
<span>Capacity</span>
<span class="value" id="capacity">0 mAh</span>
</div>

<div class="row">
<span>Energy</span>
<span class="value" id="energy">0 Wh</span>
</div>

<div class="row">
<span>SOC</span>
<span class="value" id="soc">0 %</span>
</div>

</div>

<div class="card">

<h2>📈 Live Voltage</h2>

<canvas id="voltageChart"></canvas>

</div>

<div class="card" id="resultsCard" style="display:none;">

<h2>✔ Test Complete</h2>

<div class="row">
<span>Grade</span>
<span class="value" id="grade">-</span>
</div>

<div class="row">
<span>Capacity</span>
<span class="value" id="resultCapacity">0 mAh</span>
</div>

<div class="row">
<span>Energy</span>
<span class="value" id="resultEnergy">0 Wh</span>
</div>

<div class="row">
<span>Time</span>
<span class="value" id="resultTime">0 s</span>
</div>

<div class="row">
<span>Start Voltage</span>
<span class="value" id="startVoltage">0 V</span>
</div>

<div class="row">
<span>End Voltage</span>
<span class="value" id="endVoltage">0 V</span>
</div>

</div>

<button class="start" onclick="startTest()">
▶ START TEST
</button>

<button class="stop" onclick="stopTest()">
■ STOP TEST
</button>

<button class="reset" onclick="resetTest()">
↺ RESET TEST
</button>

<hr style="width:90%;border-color:#333;">

<button class="download" onclick="downloadLog()">
⬇ DOWNLOAD CSV
</button>

<button class="delete" onclick="deleteLog()">
🗑 DELETE LOG
</button>

<footer>

ESP32 Prius Battery Analyzer

</footer>

<script>
const graph = document.getElementById("voltageChart");
const ctx = graph.getContext("2d");

graph.width = 700;
graph.height = 250;

const voltageHistory = [];

function drawGraph(){

    ctx.fillStyle="#222";
    ctx.fillRect(0,0,graph.width,graph.height);

    // Grid
    ctx.strokeStyle="#333";

    for(let i=0;i<=10;i++){

        let x=i*graph.width/10;

        ctx.beginPath();
        ctx.moveTo(x,0);
        ctx.lineTo(x,graph.height);
        ctx.stroke();

    }

    for(let i=0;i<=5;i++){

        let y=i*graph.height/5;

        ctx.beginPath();
        ctx.moveTo(0,y);
        ctx.lineTo(graph.width,y);
        ctx.stroke();

    }

    if(voltageHistory.length<2) return;

    let min=Math.min(...voltageHistory);
    let max=Math.max(...voltageHistory);

    if(max-min<0.2){

        max=min+0.2;

    }

    ctx.strokeStyle="#00ff66";
    ctx.lineWidth=3;

    ctx.beginPath();

    for(let i=0;i<voltageHistory.length;i++){

        let x=i*(graph.width/(120-1));

        let y=graph.height-
        ((voltageHistory[i]-min)/(max-min))*graph.height;

        if(i==0)
            ctx.moveTo(x,y);
        else
            ctx.lineTo(x,y);

    }

    ctx.stroke();

    ctx.fillStyle="white";
    ctx.font="16px Arial";

    ctx.fillText("Max: "+max.toFixed(2)+" V",10,20);
    ctx.fillText("Min: "+min.toFixed(2)+" V",10,40);

}
async function update(){
try{

let response=await fetch("/status");
let data=await response.json();

document.getElementById("voltage").innerHTML=data.voltage.toFixed(2)+" V";
document.getElementById("current").innerHTML=data.current.toFixed(2)+" A";
document.getElementById("power").innerHTML=data.power.toFixed(2)+" W";
document.getElementById("capacity").innerHTML=Math.round(data.capacity)+" mAh";
document.getElementById("energy").innerHTML=data.energy.toFixed(2)+" Wh";
document.getElementById("soc").innerHTML=Math.round(data.soc)+" %";
voltageHistory.push(data.voltage);

if(voltageHistory.length>120){

    voltageHistory.shift();

}

drawGraph();

if(data.running){

let results = await fetch("/results");
let r = await results.json();

if(r.completed){

document.getElementById("resultsCard").style.display="block";

document.getElementById("grade").innerHTML=r.grade;

document.getElementById("resultCapacity").innerHTML=
Math.round(r.capacity)+" mAh";

document.getElementById("resultEnergy").innerHTML=
r.energy.toFixed(2)+" Wh";

document.getElementById("resultTime").innerHTML=
r.time+" s";

document.getElementById("startVoltage").innerHTML=
r.startVoltage.toFixed(2)+" V";

document.getElementById("endVoltage").innerHTML=
r.endVoltage.toFixed(2)+" V";

}else{

document.getElementById("resultsCard").style.display="none";

}

document.getElementById("status").innerHTML="🟢 RUNNING";

}
else{

document.getElementById("status").innerHTML="🔴 STOPPED";

}

}
catch(e){

console.log(e);

document.getElementById("status").innerHTML =
"⚠ " + e;

}

}

async function startTest(){

document.getElementById("resultsCard").style.display="none";

await fetch("/start");

update();

}

async function stopTest(){

await fetch("/stop");
update();

}

async function resetTest(){

await fetch("/reset");
update();

}

function downloadLog(){

window.location="/download";

}

async function deleteLog(){

if(confirm("Delete battery log?")){

let r=await fetch("/delete");

alert(await r.text());

}

}

update();

setInterval(update,1000);

</script>

</body>
</html>
)rawliteral");

}

// ------------------------------------
// Initialize WiFi
// ------------------------------------

void initWiFi() {

Serial.println();
Serial.println("Starting Access Point...");

WiFi.mode(WIFI_AP);

if(!WiFi.softAP(ssid,password))
{
Serial.println("AP Failed");
return;
}

server.on("/", handleRoot);

server.on("/status", handleStatus);

server.on("/start", handleStart);
server.on("/stop", handleStop);
server.on("/reset", handleReset);

server.on("/download", handleDownload);
server.on("/delete", handleDelete);
server.on("/results", handleResults);
server.begin();

Serial.println("Server Started");

}

// ------------------------------------
// Keep Server Running
// ------------------------------------

void updateWiFi() {

server.handleClient();



}
