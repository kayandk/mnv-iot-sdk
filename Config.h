/**
 * @file Config.h
 * @brief Configuration structures and topic definitions for MNV-IoT SDK.
 */

#ifndef MNV_IOT_CONFIG_H
#define MNV_IOT_CONFIG_H

#include <Arduino.h>

namespace mnv {

/**
 * @brief MQTT connection configuration returned from Backend.
 */
struct MQTTConfig {
    String host;      ///< MQTT Broker address
    int port;         ///< MQTT Broker port
    String clientId;  ///< Unique Client ID
    String username;  ///< Device ID or Username
    String token;     ///< JWT Token for password authentication
};

// MQTT Topic Patterns
#define TOPIC_TELEMETRY "devices/%s/telemetry" ///< Topic for sensor data
#define TOPIC_ATTRIBUTES "devices/%s/attributes" ///< Topic for device meta properties
#define TOPIC_STATE "devices/%s/state" ///< Topic for reporting device status (ON/OFF)
#define TOPIC_COMMANDS "commands/devices/%s" ///< Topic for receiving dashboard commands
#define TOPIC_STATUS "devices/%s/status" ///< Topic for LWT (Online/Offline)
#define TOPIC_RPC_REQ "devices/%s/rpc/request" ///< Topic for RPC requests
#define TOPIC_RPC_RES "devices/%s/rpc/response" ///< Topic for RPC responses

} // namespace mnv

#endif // MNV_IOT_CONFIG_H
