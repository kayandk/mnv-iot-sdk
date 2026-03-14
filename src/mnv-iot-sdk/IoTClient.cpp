#include "IoTClient.h"

#include "ConfigPortal.h"
#include <Preferences.h>

namespace mnv {

IoTClient::IoTClient(const String& apiBaseUrl) 
    : _apiBaseUrl(apiBaseUrl), _auth(apiBaseUrl), _mqtt(_wifiClient) {}

void IoTClient::begin(const String& deviceId, const String& secret) {
    _deviceId = deviceId;
    _secret = secret;
    _isInitialized = true;
}

bool IoTClient::connect() {
    if (!_isInitialized) {
        // Try to load from flash if not initialized
        Preferences prefs;
        prefs.begin("mnv-iot", true);
        _deviceId = prefs.getString("dev_id", "");
        _secret = prefs.getString("dev_sec", "");
        String savedSsid = prefs.getString("w_ssid", "");
        String savedPass = prefs.getString("w_pass", "");
        prefs.end();

        if (_deviceId != "" && savedSsid != "") {
             WiFi.begin(savedSsid.c_str(), savedPass.c_str());
             _isInitialized = true;
        } else {
            _lastError = "Not initialized and no saved config found.";
            return false;
        }
    }

    // 1. WiFi check
    if (WiFi.status() != WL_CONNECTED) {
        _lastError = "WiFi not connected";
        return false;
    }

    // 2. Authenticate with Backend
    Serial.println("[IoT SDK] Authenticating with backend...");
    if (!_auth.authenticate(_deviceId, _secret, _mqttConfig)) {
        _lastError = "Auth failed: " + _auth.getLastError();
        return false;
    }

    // 3. Connect to MQTT
    Serial.println("[IoT SDK] Connecting to MQTT Broker...");
    if (!_mqtt.connect(_mqttConfig)) {
        _lastError = "MQTT connection failed";
        return false;
    }

    Serial.println("[IoT SDK] Successfully connected and authenticated.");
    return true;
}

void IoTClient::loop() {
    if (_mqtt.isConnected()) {
        _mqtt.loop();
    } else {
        // Smart auto-reconnect with exponential backoff
        static unsigned long lastRetry = 0;
        static unsigned long retryInterval = 5000;
        
        if (millis() - lastRetry > retryInterval) {
            lastRetry = millis();
            Serial.println("[IoT SDK] Attempting to reconnect...");
            if (connect()) {
                retryInterval = 5000; // Reset on success
            } else {
                retryInterval = min((unsigned long)60000, retryInterval * 2); // Max 1 minute
                Serial.printf("[IoT SDK] Reconnect failed. Retrying in %lu s\n", retryInterval / 1000);
            }
        }
    }
}

bool IoTClient::publishTelemetry(const String& jsonData) {
    return _mqtt.publishTelemetry(_deviceId, jsonData);
}

bool IoTClient::publishState(const String& stateData) {
    return _mqtt.publishState(_deviceId, stateData);
}

bool IoTClient::publishRpcResponse(const String& responseData) {
    return _mqtt.publishRpcResponse(_deviceId, responseData);
}

bool IoTClient::publishAttributes(const String& jsonAttributes) {
    return _mqtt.publishAttributes(_deviceId, jsonAttributes);
}

void IoTClient::onCommand(CommandCallback callback) {
    _mqtt.setCommandCallback(callback);
}

bool IoTClient::isConnected() {
    return _mqtt.isConnected();
}

String IoTClient::getLastError() const {
    return _lastError;
}

void IoTClient::startSmartConfig() {
    ConfigPortal portal;
    String apSsid = "MNV-IoT-" + (_deviceId != "" ? _deviceId : String((uint32_t)ESP.getEfuseMac(), HEX));
    portal.start(apSsid, _deviceId);

    while (!portal.isConfigDone()) {
        portal.loop();
        delay(1);
    }

    // Save to flash
    Preferences prefs;
    prefs.begin("mnv-iot", false);
    prefs.putString("w_ssid", portal.getSsid());
    prefs.putString("w_pass", portal.getPassword());
    prefs.putString("dev_id", portal.getDeviceId());
    prefs.putString("dev_sec", portal.getSecret());
    prefs.end();

    Serial.println("[IoT SDK] Config saved to flash. Restarting...");
    ESP.restart();
}

} // namespace mnv
