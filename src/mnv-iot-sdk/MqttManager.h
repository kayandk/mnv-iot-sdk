/**
 * @file MqttManager.h
 * @brief MQTT connection and messaging handler for MNV-IoT SDK.
 */

#ifndef MNV_IOT_MQTT_MANAGER_H
#define MNV_IOT_MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "Config.h"

namespace mnv {

/**
 * @brief Typedef for message/command callback function.
 */
typedef std::function<void(char*, byte*, unsigned int)> CommandCallback;

/**
 * @class MqttManager
 * @brief Wraps PubSubClient to provide high-level IoT messaging patterns.
 */
class MqttManager {
public:
    /**
     * @brief Construct a new Mqtt Manager object.
     * @param wifiClient Reference to a WiFi client (e.g., WiFiClientSecure)
     */
    MqttManager(WiFiClient& wifiClient);

    /**
     * @brief Connect to the MQTT broker using provided JWT config.
     * @param config MQTT connection parameters from backend.
     * @return true if connected.
     */
    bool connect(const MQTTConfig& config);

    /**
     * @brief Maintain the MQTT connection. Must be called in loop().
     */
    void loop();

    /**
     * @brief Check if currently connected to broker.
     */
    bool isConnected();
    
    /**
     * @brief Publish sensor telemetry data (JSON).
     */
    bool publishTelemetry(const String& deviceId, const String& payload);

    /**
     * @brief Publish device state updates.
     */
    bool publishState(const String& deviceId, const String& payload);

    /**
     * @brief Respond to an RPC request.
     */
    bool publishRpcResponse(const String& deviceId, const String& payload);

    /**
     * @brief Set the handler for incoming commands/RPCs.
     */
    void setCommandCallback(CommandCallback callback);

private:
    PubSubClient _client;
    String _deviceId;
    CommandCallback _commandCallback;
    
    void _onMessage(char* topic, byte* payload, unsigned int length);
};

} // namespace mnv

#endif // MNV_IOT_MQTT_MANAGER_H
