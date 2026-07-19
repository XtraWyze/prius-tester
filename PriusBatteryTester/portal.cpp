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

.copy{
background:#7c3aed;
color:white;
}

.save-json{
background:#9333ea;
color:white;
}

.summary-card{
border:2px solid #333;
}

.summary-title{
color:#00ff66;
font-size:28px;
margin:0 0 10px;
}

.summary-timestamp{
font-size:14px;
color:#888;
margin-bottom:10px;
}

.grade-display{
font-size:72px;
font-weight:bold;
margin:10px 0 20px;
line-height:1;
}

.grade-A{
color:#16a34a;
}

.grade-B{
color:#86efac;
}

.grade-C{
color:#facc15;
}

.grade-D{
color:#fb923c;
}

.grade-F{
color:#ef4444;
}

.summary-actions{
margin-top:10px;
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

<div class="card summary-card" id="summaryCard" style="display:none;">

<h2 class="summary-title">✓ TEST COMPLETE</h2>

<div class="summary-timestamp" id="summaryTimestamp"></div>

<div class="grade-display" id="gradeDisplay">-</div>

<div class="row">
<span>Capacity</span>
<span class="value" id="summaryCapacity">0 mAh</span>
</div>

<div class="row">
<span>Energy</span>
<span class="value" id="summaryEnergy">0 Wh</span>
</div>

<div class="row">
<span>Elapsed Time</span>
<span class="value" id="summaryTime">0 s</span>
</div>

<div class="row">
<span>Start Voltage</span>
<span class="value" id="summaryStartVoltage">0 V</span>
</div>

<div class="row">
<span>End Voltage</span>
<span class="value" id="summaryEndVoltage">0 V</span>
</div>

<div class="row">
<span>Minimum Voltage</span>
<span class="value" id="summaryMinVoltage">0 V</span>
</div>

<div class="row">
<span>Maximum Voltage</span>
<span class="value" id="summaryMaxVoltage">0 V</span>
</div>

<div class="row">
<span>Average Voltage</span>
<span class="value" id="summaryAvgVoltage">0 V</span>
</div>

<div class="row">
<span>Average Current</span>
<span class="value" id="summaryAvgCurrent">0 A</span>
</div>

<div class="summary-actions">

<button class="copy" onclick="copyResults()">
📋 Copy Results
</button>

<button class="save-json" onclick="saveJson()">
💾 Save JSON
</button>

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
let lastSummary = null;

function formatElapsed(seconds){

const h=Math.floor(seconds/3600);
const m=Math.floor((seconds%3600)/60);

if(h>0){
return h+"h "+m+"m";
}

if(m>0){
return m+"m "+(seconds%60)+"s";
}

return seconds+"s";

}

function gradeClass(grade){

if(grade==="A") return "grade-A";
if(grade==="B") return "grade-B";
if(grade==="C") return "grade-C";
if(grade==="D") return "grade-D";
if(grade==="F") return "grade-F";

return "";

}

function updateSummaryCard(r){

lastSummary=r;

document.getElementById("summaryCard").style.display="block";

document.getElementById("summaryTimestamp").innerHTML=
"Completed: "+r.timestamp;

const gradeEl=document.getElementById("gradeDisplay");

gradeEl.innerHTML=r.grade;
gradeEl.className="grade-display "+gradeClass(r.grade);

document.getElementById("summaryCapacity").innerHTML=
Math.round(r.capacity_mAh)+" mAh";

document.getElementById("summaryEnergy").innerHTML=
r.energy_Wh.toFixed(2)+" Wh";

document.getElementById("summaryTime").innerHTML=
formatElapsed(r.elapsedSeconds);

document.getElementById("summaryStartVoltage").innerHTML=
r.startVoltage.toFixed(2)+" V";

document.getElementById("summaryEndVoltage").innerHTML=
r.endVoltage.toFixed(2)+" V";

document.getElementById("summaryMinVoltage").innerHTML=
r.minVoltage.toFixed(2)+" V";

document.getElementById("summaryMaxVoltage").innerHTML=
r.maxVoltage.toFixed(2)+" V";

document.getElementById("summaryAvgVoltage").innerHTML=
r.averageVoltage.toFixed(2)+" V";

document.getElementById("summaryAvgCurrent").innerHTML=
r.averageCurrent.toFixed(2)+" A";

}

function formatReport(r){

return [
"------------------------------------",
"Prius Battery Analyzer",
"------------------------------------",
"Grade: "+r.grade,
"Capacity: "+Math.round(r.capacity_mAh)+" mAh",
"Energy: "+r.energy_Wh.toFixed(1)+" Wh",
"Time: "+formatElapsed(r.elapsedSeconds),
"Start Voltage: "+r.startVoltage.toFixed(2)+"V",
"End Voltage: "+r.endVoltage.toFixed(2)+"V",
"Min Voltage: "+r.minVoltage.toFixed(2)+"V",
"Max Voltage: "+r.maxVoltage.toFixed(2)+"V",
"Average Voltage: "+r.averageVoltage.toFixed(2)+"V",
"Average Current: "+r.averageCurrent.toFixed(2)+"A",
"Timestamp: "+r.timestamp,
"------------------------------------"
].join("\n");

}

async function copyResults(){

if(!lastSummary){
alert("No test summary available.");
return;
}

const text=formatReport(lastSummary);

try{

await navigator.clipboard.writeText(text);
alert("Results copied to clipboard.");

}
catch(e){

const area=document.createElement("textarea");
area.value=text;
document.body.appendChild(area);
area.select();
document.execCommand("copy");
document.body.removeChild(area);
alert("Results copied to clipboard.");

}

}

function saveJson(){

window.location="/summary.json";

}

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

let results = await fetch("/results");
let r = await results.json();

if(r.completed){

updateSummaryCard(r);

}else{

document.getElementById("summaryCard").style.display="none";
lastSummary=null;

}

if(data.running){

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

document.getElementById("summaryCard").style.display="none";
lastSummary=null;

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
server.on("/summary.json", handleDownloadSummary);
server.begin();

Serial.println("Server Started");

}

// ------------------------------------
// Keep Server Running
// ------------------------------------

void updateWiFi() {

server.handleClient();



}
