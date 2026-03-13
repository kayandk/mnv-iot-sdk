/**
 * @file IoTClient.h
 * @brief Main Facade for MNV-IoT SDK, providing a simple interface for IoT tasks.
 */

#ifndef MNV_IOT_CLIENT_H
#define MNV_IOT_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include "Config.h"
#include "DeviceAuth.h"
#include "MqttManager.h"

namespace mnv {

/**
 * @class IoTClient
 * @brief The main entry point for the MNV-IoT SDK. 
 * Manages Auth + MQTT lifecycle automatically.
 */
class IoTClient {
public:
    /**
     * @brief Construct a new IoT Client object.
     * @param apiBaseUrl Full URL to the MNV-IoT Backend API.
     */
    IoTClient(const String& apiBaseUrl);
    
    /**
     * @brief Initialize client with device credentials.
     * @param deviceId System-assigned device ID.
     * @param secret Device-specific secret key.
     */
    void begin(const String& deviceId, const String& secret);

    /**
     * @brief Authenticate and connect to MQTT broker.
     * @return true if fully connected.
     */
    bool connect();

    /**
     * @brief Keep the SDK running. Call this in Arduino loop().
     */
    void loop();
    
    /**
     * @brief Publish telemetry (sensor) data to the cloud.
     * @param jsonData Stringified JSON payload.
     * @return true if published successfully.
     */
    bool publishTelemetry(const String& jsonData);

    /**
     * @brief Report device state manually.
     * @param stateData JSON or raw string describing state.
     */
    bool publishState(const String& stateData);

    /**
     * @brief Publish binary/JSON response to an RPC request.
     */
    bool publishRpcResponse(const String& responseData);

    /**
     * @brief Set the Command handler.
     * @param callback Function to call when a command or RPC is received.
     */
    void onCommand(CommandCallback callback);

    /**
     * @brief Check connectivity status.
     */
    bool isConnected();

    /**
     * @brief Retrieve detailed error message if a task fails.
     */
    String getLastError() const;

private:
    String _apiBaseUrl;
    String _deviceId;
    String _secret;
    String _lastError;
    
    MQTTConfig _mqttConfig;
    DeviceAuth _auth;
    WiFiClient _wifiClient;
    MqttManager _mqtt;
    
    bool _isInitialized = false;
};

} // namespace mnv

#endif // MNV_IOT_CLIENT_H
