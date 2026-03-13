#include "DeviceAuth.h"

namespace mnv {

DeviceAuth::DeviceAuth(const String& apiBaseUrl) : _apiBaseUrl(apiBaseUrl) {}

bool DeviceAuth::authenticate(const String& deviceId, const String& secret, MQTTConfig& config) {
    if (WiFi.status() != WL_CONNECTED) {
        _lastError = "WiFi not connected";
        return false;
    }

    HTTPClient http;
    String url = _apiBaseUrl + "/provision/auth";
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> reqDoc;
    reqDoc["deviceId"] = deviceId;
    reqDoc["secret"] = secret;

    String requestBody;
    serializeJson(reqDoc, requestBody);

    int httpCode = http.POST(requestBody);

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<1024> resDoc;
        DeserializationError error = deserializeJson(resDoc, payload);

        if (error) {
            _lastError = "JSON parse error: " + String(error.c_str());
            http.end();
            return false;
        }

        config.host = resDoc["mqttHost"].as<String>();
        config.port = resDoc["mqttPort"].as<int>();
        config.clientId = resDoc["clientId"].as<String>();
        config.username = resDoc["username"].as<String>();
        config.token = resDoc["password"].as<String>();

        http.end();
        return true;
    } else {
        _lastError = "HTTP error: " + String(httpCode);
        if (httpCode > 0) {
            _lastError += " " + http.getString();
        }
        http.end();
        return false;
    }
}

} // namespace mnv
