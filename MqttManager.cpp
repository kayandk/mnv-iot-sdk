#include "MqttManager.h"

namespace mnv {

MqttManager::MqttManager(WiFiClient& wifiClient) : _client(wifiClient) {
    _client.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->_onMessage(topic, payload, length);
    });
}

bool MqttManager::connect(const MQTTConfig& config) {
    _deviceId = config.clientId;
    _client.setServer(config.host.c_str(), config.port);
    _client.setBufferSize(2048); // Support larger JSON payloads

    char statusTopic[128];
    snprintf(statusTopic, sizeof(statusTopic), TOPIC_STATUS, _deviceId.c_str());
    
    // LWT Payload (JSON)
    String lwtPayload = "{\"online\":false}";

    if (_client.connect(config.clientId.c_str(), 
                        config.username.c_str(), 
                        config.token.c_str(),
                        statusTopic, 
                        1, // QOS
                        true, // Retain
                        lwtPayload.c_str())) {
        
        // Subscribe to command topic
        char cmdTopic[128];
        snprintf(cmdTopic, sizeof(cmdTopic), TOPIC_COMMANDS, _deviceId.c_str());
        _client.subscribe(cmdTopic);

        // Subscribe to RPC request topic
        char rpcTopic[128];
        snprintf(rpcTopic, sizeof(rpcTopic), TOPIC_RPC_REQ, _deviceId.c_str());
        _client.subscribe(rpcTopic);

        // Send Online status
        publishState(_deviceId, "{\"online\":true}");
        
        return true;
    }
    return false;
}

void MqttManager::loop() {
    _client.loop();
}

bool MqttManager::isConnected() {
    return _client.connected();
}

bool MqttManager::publishTelemetry(const String& deviceId, const String& payload) {
    char topic[128];
    snprintf(topic, sizeof(topic), TOPIC_TELEMETRY, deviceId.c_str());
    return _client.publish(topic, payload.c_str());
}

bool MqttManager::publishState(const String& deviceId, const String& payload) {
    char topic[128];
    snprintf(topic, sizeof(topic), TOPIC_STATUS, deviceId.c_str());
    return _client.publish(topic, payload.c_str(), true); // Retained
}

bool MqttManager::publishRpcResponse(const String& deviceId, const String& payload) {
    char topic[128];
    snprintf(topic, sizeof(topic), TOPIC_RPC_RES, deviceId.c_str());
    return _client.publish(topic, payload.c_str());
}

void MqttManager::setCommandCallback(CommandCallback callback) {
    _commandCallback = callback;
}

void MqttManager::_onMessage(char* topic, byte* payload, unsigned int length) {
    if (_commandCallback) {
        _commandCallback(topic, payload, length);
    }
}

} // namespace mnv
