#include "ConfigPortal.h"

namespace mnv {

ConfigPortal::ConfigPortal() : _server(80) {}

void ConfigPortal::start(const String& ssid, const String& deviceId) {
    _deviceId = deviceId;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str());

    Serial.println("[IoT SDK] Config Portal started.");
    Serial.print("[IoT SDK] AP SSID: "); Serial.println(ssid);
    Serial.print("[IoT SDK] IP: "); Serial.println(WiFi.softAPIP());

    _dnsServer.start(53, "*", WiFi.softAPIP());

    _server.on("/", HTTP_GET, std::bind(&ConfigPortal::handleRoot, this));
    _server.on("/save", HTTP_POST, std::bind(&ConfigPortal::handleSave, this));
    _server.onNotFound(std::bind(&ConfigPortal::handleNotFound, this));
    
    _server.begin();
}

void ConfigPortal::loop() {
    _dnsServer.processNextRequest();
    _server.handleClient();
}

void ConfigPortal::handleRoot() {
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style>";
    html += "body { font-family: sans-serif; background: #0f172a; color: white; text-align: center; padding: 20px; }";
    html += "input { width: 100%; padding: 12px; margin: 10px 0; border: 1px solid #1e293b; border-radius: 8px; background: #1e293b; color: white; box-sizing: border-box; }";
    html += "button { width: 100%; padding: 15px; background: #3b82f6; border: none; border-radius: 8px; color: white; font-weight: bold; cursor: pointer; }";
    html += "h1 { font-size: 24px; margin-bottom: 30px; }";
    html += ".card { background: #1e293b66; border-radius: 20px; padding: 30px; border: 1px solid #ffffff14; }";
    html += "</style></head><body>";
    html += "<div class='card'><h1>MNV-IoT Setup</h1>";
    html += "<form action='/save' method='POST'>";
    html += "<input type='text' name='s' placeholder='WiFi SSID' required>";
    html += "<input type='password' name='p' placeholder='WiFi Password'>";
    html += "<input type='text' name='d' placeholder='Device ID' value='" + _deviceId + "'>";
    html += "<input type='text' name='k' placeholder='Device Secret'>";
    html += "<button type='submit'>SAVE & CONNECT</button>";
    html += "</form></div></body></html>";
    _server.send(200, "text/html", html);
}

void ConfigPortal::handleSave() {
    _ssid = _server.arg("s");
    _password = _server.arg("p");
    _deviceId = _server.arg("d");
    _secret = _server.arg("k");
    
    String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'><style>";
    html += "body { font-family: sans-serif; background: #0f172a; color: white; text-align: center; padding: 50px 20px; }";
    html += "h2 { color: #10b981; }";
    html += "</style></head><body>";
    html += "<h2>Configuration Saved!</h2>";
    html += "<p>The device will now restart and attempt to connect to <b>" + _ssid + "</b>.</p>";
    html += "<p>Please return to the Dashboard.</p></body></html>";
    
    _server.send(200, "text/html", html);
    delay(2000);
    _configDone = true;
}

void ConfigPortal::handleNotFound() {
    _server.sendHeader("Location", "/", true);
    _server.send(302, "text/plain", "");
}

} // namespace mnv
