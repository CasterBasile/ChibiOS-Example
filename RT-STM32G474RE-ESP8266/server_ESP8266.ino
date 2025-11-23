#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <vector>
#include <algorithm> // for std::find

#define MAP_SIZE 200

ESP8266WebServer server(80);

std::vector<String> keys;
String valueMap[MAP_SIZE];

uint32_t hashString(String s) {
  uint32_t hashcode = 0;
  for (int i = 0; i < s.length(); i++) {
    hashcode += s.charAt(i);
  }
  return hashcode;
}

void handleRoot() {
  String response = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP8266 Control Panel</title>
<style>
  * { margin: 0; padding: 0; box-sizing: border-box; }
  body { background: #f8fafc; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; color: #1e293b; min-height: 100vh; line-height: 1.6; }
  header { background: white; color: #1e293b; padding: 2rem 1rem; text-align: center; border-bottom: 1px solid #e2e8f0; box-shadow: 0 1px 3px rgba(0,0,0,0.05); }
  header h1 { font-size: 2rem; font-weight: 600; margin-bottom: 0.5rem; color: #334155; }
  header p { font-size: 1rem; color: #64748b; font-weight: 400; }

  section { max-width: 1200px; margin: 0 auto; padding: 1.5rem 1rem 0; }
  section h2 { font-size: 1.1rem; font-weight: 700; color: #475569; margin: 0.5rem 0 0.25rem; text-transform: uppercase; letter-spacing: .08em; }
  .grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(320px, 1fr));
    gap: 1.25rem;
    padding: 1rem 0 2rem;
  }

  .ctrl {
    background: white;
    padding: 1.25rem;
    border-radius: 12px;
    box-shadow: 0 1px 3px rgba(0,0,0,0.1);
    border: 1px solid #e2e8f0;
    transition: all 0.2s ease;
    position: relative;
  }
  .ctrl:hover { box-shadow: 0 4px 12px rgba(0,0,0,0.1); border-color: #cbd5e1; }
  .ctrl h3 { font-size: 1.1rem; font-weight: 600; color: #2d3748; margin-bottom: 1rem; display: flex; align-items: center; gap: 0.5rem; }
  .delete-btn {
    position: absolute; top: 1rem; right: 1rem; background: #f8fafc; color: #64748b; border: 1px solid #e2e8f0;
    border-radius: 6px; width: 28px; height: 28px; cursor: pointer; font-size: 14px; display: flex; align-items: center;
    justify-content: center; transition: all 0.2s; font-weight: 500;
  }
  .delete-btn:hover { background: #fee2e2; border-color: #fecaca; color: #dc2626; }

  /* Inputs */
  button, input, select { font-size: 1rem; padding: 1rem 1.5rem; border-radius: 12px; border: 2px solid transparent; width: 100%; transition: all 0.3s ease; font-weight: 500; }
  button { background: #3b82f6; color: white; border: none; cursor: pointer; transition: all 0.2s ease; font-weight: 500; }
  button:hover { background: #2563eb; }
  button.off { background: #64748b; }
  button.off:hover { background: #475569; }
  input { background: #f8fafc; border: 1px solid #d1d5db; color: #374151; }
  input:focus { outline: none; border-color: #3b82f6; background: white; box-shadow: 0 0 0 3px rgba(59,130,246,0.1); }
  input[type=range] { -webkit-appearance: none; height: 8px; background: #e2e8f0; border-radius: 8px; outline: none; padding: 0; margin: 1rem 0; }
  input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 20px; height: 20px; border-radius: 50%; background: #3b82f6; cursor: pointer; border: 2px solid white; box-shadow: 0 1px 3px rgba(0,0,0,0.2); }
  .range-value { text-align: center; font-weight: 600; color: #3b82f6; margin-top: 0.5rem; }

  /* Output-only tile */
  .readonly .value {
    font-size: 1.75rem;
    font-weight: 700;
    color: #111827;
    letter-spacing: .02em;
  }
  .readonly .subtle { color: #6b7280; font-size: .85rem; margin-top: .25rem; }

  /* FAB */
  .fab { position: fixed; bottom: 2rem; right: 2rem; background: #10b981; color: white; border: none; border-radius: 12px; width: 56px; height: 56px; font-size: 24px; cursor: pointer; box-shadow: 0 4px 12px rgba(16,185,129,0.3); transition: all 0.2s ease; z-index: 1000; }
  .fab:hover { background: #059669; transform: translateY(-2px); box-shadow: 0 8px 20px rgba(16,185,129,0.4); }

  /* Modal */
  .modal { display: none; position: fixed; z-index: 2000; left: 0; top: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.6); backdrop-filter: blur(5px); }
  .modal-content { background: white; margin: 5% auto; padding: 0; border-radius: 20px; width: 90%; max-width: 500px; box-shadow: 0 20px 60px rgba(0,0,0,0.3); animation: modalSlideIn 0.3s ease; overflow: hidden; }
  @keyframes modalSlideIn { from { transform: translateY(-50px); opacity: 0; } to { transform: translateY(0); opacity: 1; } }
  .modal-header { background: #3b82f6; color: white; padding: 1.5rem 2rem; font-size: 1.2rem; font-weight: 600; }
  .modal-body { padding: 2rem; }
  .modal-body label { display: block; margin-bottom: 0.5rem; font-weight: 600; color: #374151; }
  .modal-body input, .modal-body select { margin-bottom: 1.5rem; }
  .modal-actions { display: flex; gap: 1rem; justify-content: flex-end; padding: 0 2rem 2rem 2rem; }
  .modal-actions button { width: auto; padding: 0.75rem 2rem; }
  .btn-secondary { background: #6b7280; }
  .btn-secondary:hover { background: #4b5563; }
  .close { color: white; float: right; font-size: 28px; font-weight: bold; cursor: pointer; line-height: 1; }
  .close:hover { opacity: 0.7; }

  @media (max-width: 768px) {
    header h1 { font-size: 2rem; }
    section { padding: 1rem; }
    .grid { grid-template-columns: 1fr; gap: 1rem; }
    .ctrl { padding: 1.25rem; }
    .modal-content { width: 95%; margin: 10% auto; }
  }
</style>
</head>
<body>
  <header>
    <h1>ESP8266 Control Panel</h1>
    <p>Device Management Interface</p>
  </header>

  <section>
    <h2>Outputs (from Serial)</h2>
    <div id="outputs" class="grid"></div>
  </section>

  <section>
    <h2>Controls</h2>
    <div id="buttons" class="grid"></div>
  </section>

  <button class="fab" onclick="openAddModal()">+</button>

  <!-- Add Control Modal -->
  <div id="addModal" class="modal">
    <div class="modal-content">
      <div class="modal-header">
        <span class="close" onclick="closeAddModal()">&times;</span>
        Add New Control
      </div>
      <div class="modal-body">
        <label for="controlName">Control Name:</label>
        <input type="text" id="controlName" placeholder="Enter control name...">
        <label for="controlType">Control Type:</label>
        <select id="controlType">
          <option value="1">ON/OFF Switch</option>
          <option value="2">Number Input</option>
          <option value="3">Slider</option>
          <option value="4">Text Input</option>
        </select>
      </div>
      <div class="modal-actions">
        <button class="btn-secondary" onclick="closeAddModal()">Cancel</button>
        <button onclick="createControlFromModal()">Create Control</button>
      </div>
    </div>
  </div>

<script>
function sendRequest(name, value){
  fetch(`/button?name=${encodeURIComponent(name)}&state=${encodeURIComponent(value)}`)
    .then(response => { if (!response.ok) console.error('Request failed'); })
    .catch(error => console.error('Error:', error));
}

function openAddModal(){ document.getElementById('addModal').style.display = 'block'; document.getElementById('controlName').focus(); }
function closeAddModal(){ document.getElementById('addModal').style.display = 'none'; document.getElementById('controlName').value=''; document.getElementById('controlType').value='1'; }

function createControlFromModal(){
  const name = document.getElementById('controlName').value.trim();
  const type = parseInt(document.getElementById('controlType').value);
  if (!name) { alert('Please enter a control name'); return; }
  const control = {name, type, value: getDefaultValue(type)};
  createControl(control);
  saveState();
  closeAddModal();
}

function getDefaultValue(type){ switch(type){ case 1:return "OFF"; case 2:return "0"; case 3:return "50"; case 4:return ""; default:return ""; } }

function createControl(ctrl){
  const container = document.createElement("div");
  container.className = "ctrl";
  container.innerHTML = `<button class="delete-btn" onclick="deleteControl(this)" title="Delete Control">×</button><h3>${ctrl.name}</h3>`;

  if(ctrl.type === 1){ // ON/OFF
    const btn = document.createElement("button");
    btn.innerText = ctrl.value === "ON" ? "ON" : "OFF";
    btn.className = ctrl.value === "ON" ? "" : "off";
    btn.onclick = () => {
      ctrl.value = ctrl.value === "ON" ? "OFF" : "ON";
      btn.innerText = ctrl.value;
      btn.className = ctrl.value === "ON" ? "" : "off";
      sendRequest(ctrl.name, ctrl.value);
      saveState();
    };
    container.appendChild(btn);

  } else if(ctrl.type === 2){ // Number
    const input = document.createElement("input");
    input.type = "number";
    input.value = ctrl.value || 0;
    input.placeholder = "Enter number...";
    input.onchange = () => { ctrl.value = input.value; sendRequest(ctrl.name, ctrl.value); saveState(); };
    container.appendChild(input);

  } else if(ctrl.type === 3){ // Slider
    const input = document.createElement("input");
    input.type = "range"; input.min = 0; input.max = 100; input.value = ctrl.value || 50;
    const valueDisplay = document.createElement("div");
    valueDisplay.className = "range-value"; valueDisplay.innerText = input.value + "%";
    input.oninput = () => { valueDisplay.innerText = input.value + "%"; };
    input.onchange = () => { ctrl.value = input.value; sendRequest(ctrl.name, ctrl.value); saveState(); };
    container.appendChild(input);
    container.appendChild(valueDisplay);

  } else if(ctrl.type === 4){ // Text
    const input = document.createElement("input");
    input.type = "text"; input.value = ctrl.value || ""; input.placeholder = "Enter text...";
    input.onchange = () => { ctrl.value = input.value; sendRequest(ctrl.name, ctrl.value); saveState(); };
    container.appendChild(input);
  }

  document.getElementById("buttons").appendChild(container);
}

function deleteControl(deleteBtn) {
  if (confirm('Are you sure you want to delete this control?')) {
    deleteBtn.parentElement.remove();
    saveState();
  }
}

function saveState(){
  const state = [];
  document.querySelectorAll("#buttons .ctrl").forEach(ctrl => {
    const name = ctrl.querySelector("h3").innerText;
    const input = ctrl.querySelector("input,button:not(.delete-btn)");
    let type, value;
    if(!input) return;
    if(input.tagName === "BUTTON") { type = 1; value = input.innerText; }
    else if(input.type === "number") { type = 2; value = input.value; }
    else if(input.type === "range") { type = 3; value = input.value; }
    else { type = 4; value = input.value; }
    state.push({name, type, value});
  });
  localStorage.setItem("controls", JSON.stringify(state));
}

function loadState(){
  const saved = JSON.parse(localStorage.getItem("controls") || "[]");
  saved.forEach(c => createControl(c));
}

/* -------- OUTPUT TILES (read-only) ---------- */

const outputMap = new Map(); // name -> DOM container

function upsertOutputTile(name, value) {
  let container = outputMap.get(name);
  if (!container) {
    container = document.createElement("div");
    container.className = "ctrl readonly";
    container.setAttribute("data-name", name);
    container.innerHTML = `
      <h3>${name}</h3>
      <div class="value" id="val-${cssSafe(name)}">${escapeHtml(value)}</div>
      <div class="subtle" id="ts-${cssSafe(name)}"></div>
    `;
    document.getElementById("outputs").appendChild(container);
    outputMap.set(name, container);
  } else {
    container.querySelector(".value").innerHTML = escapeHtml(value);
  }
  // timestamp
  const ts = new Date();
  const tsDiv = container.querySelector(".subtle");
  tsDiv.textContent = "Updated " + ts.toLocaleTimeString();
}

// Periodically fetch values from ESP
let pollTimer = null;
function startPolling(){
  const poll = () => {
    fetch('/values')
      .then(r => r.json())
      .then(arr => {
        if (!Array.isArray(arr)) return;
        arr.forEach(item => upsertOutputTile(item.name, item.value));
      })
      .catch(e => console.error(e));
  };
  poll();
  pollTimer = setInterval(poll, 1000); // 1s update
}

// helpers
function cssSafe(s){ return s.replace(/[^a-zA-Z0-9_-]/g, "_"); }
function escapeHtml(s){
  return String(s)
    .replace(/&/g,"&amp;")
    .replace(/</g,"&lt;")
    .replace(/>/g,"&gt;")
    .replace(/"/g,"&quot;")
    .replace(/'/g,"&#039;");
}

/* -------------------------------------------- */

window.onclick = function(event) {
  const modal = document.getElementById('addModal');
  if (event.target === modal) { closeAddModal(); }
}
document.addEventListener('keydown', function(event) {
  if (event.key === 'Escape') { closeAddModal(); }
  if (event.key === 'Enter' && document.getElementById('addModal').style.display === 'block') {
    createControlFromModal();
  }
});

document.addEventListener("DOMContentLoaded", function() {
  loadState();
  startPolling();
});
</script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", response);
}

void handleButton() {
  String name = server.arg("name");
  String state = server.arg("state");
  Serial.println(name + " " + state);
  server.send(200, "text/plain", "OK");
}

// New: JSON endpoint exposing Serial-fed outputs
void handleValues() {
  String json = "[";
  for (size_t i = 0; i < keys.size(); i++) {
    const String& k = keys[i];
    String v = valueMap[hashString(k) % MAP_SIZE];
    // Basic JSON escaping for quotes and backslashes
    String kEsc = k; kEsc.replace("\\","\\\\"); kEsc.replace("\"","\\\"");
    String vEsc = v; vEsc.replace("\\","\\\\"); vEsc.replace("\"","\\\"");
    json += "{\"name\":\"" + kEsc + "\",\"value\":\"" + vEsc + "\"}";
    if (i + 1 < keys.size()) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(38400);

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);

  String mac = WiFi.macAddress();
  mac.replace(":", "");
  String ssid = "ESP_32STServer-" + mac;

  if (!wifiManager.autoConnect(ssid.c_str())) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
    delay(5000);
  }

  Serial.println("Connesso al WiFi");

  server.on("/", handleRoot);
  server.on("/button", HTTP_GET, handleButton);
  server.on("/values", HTTP_GET, handleValues); // <-- new

  server.begin();
  Serial.println("Server avviato.");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();

  // Expect lines like: key:value\n
  if (Serial.available()) {
    String key = Serial.readStringUntil(':');
    key.trim();
    if (key.length() == 0) return;

    if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
      if (keys.size() < MAP_SIZE) { // simple guard
        keys.push_back(key);
      }
    }

    String value = Serial.readStringUntil('\n');
    value.trim();
    valueMap[hashString(key) % MAP_SIZE] = value;
  }
}
