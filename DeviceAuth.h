/**
 * @file DeviceAuth.h
 * @brief Secure HTTPS authentication handler for MNV-IoT SDK.
 */

#ifndef MNV_IOT_DEVICE_AUTH_H
#define MNV_IOT_DEVICE_AUTH_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Config.h"

namespace mnv {

/**
 * @class DeviceAuth
 * @brief Handles secure exchange of device credentials for MQTT JWT tokens.
 */
class DeviceAuth {
public:
    /**
     * @brief Construct a new Device Auth object.
     * @param apiBaseUrl The server base URL (e.g., https://api.mnv.vn)
     */
    DeviceAuth(const String& apiBaseUrl);

    /**
     * @brief Authenticate device and retrieve MQTT configuration.
     * @param deviceId Unique identifier of the device.
     * @param secret Pre-shared secret key.
     * @param config Reference to MQTTConfig struct to store results.
     * @return true if authentication succeeded.
     */
    bool authenticate(const String& deviceId, const String& secret, MQTTConfig& config);

    /**
     * @brief Get the last error message.
     */
    String getLastError() const { return _lastError; }

private:
    String _apiBaseUrl;
    String _lastError;
};

} // namespace mnv

#endif // MNV_IOT_DEVICE_AUTH_H
