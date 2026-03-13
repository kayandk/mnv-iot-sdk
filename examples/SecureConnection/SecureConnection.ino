#include <WiFi.h>
#include <ArduinoJson.h>
#include <mnv-iot-sdk/IoTClient.h>

// Credentials (In production, load these from secure storage)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* deviceId = "ESP32_SECURE_001";
const char* deviceSecret = "YOUR_DEVICE_SECRET";
const char* apiBaseUrl = "http://api.yourdomain.com/api/v1";

mnv::IoTClient client(apiBaseUrl);

void setup() {
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected");

    // Initialize MNV IoT Client
    client.begin(deviceId, deviceSecret);

    // Setup command callback (Standard & RPC)
    client.onCommand([](char* topic, byte* payload, unsigned int length) {
        String topicStr = String(topic);
        String message = "";
        for (int i = 0; i < length; i++) message += (char)payload[i];
        
        Serial.printf("[IoT SDK] Message on %s: %s\n", topic, message.c_str());

        // Simple RPC handler: if method is "toggle", send response
        if (topicStr.indexOf("rpc/request") > -1) {
            StaticJsonDocument<256> resDoc;
            resDoc["status"] = "success";
            resDoc["info"] = "Command processed by Smart SDK";
            
            String response;
            serializeJson(resDoc, response);
            client.publishRpcResponse(response);
            Serial.println("[IoT SDK] Sent RPC Response");
        }
    });

    // Initial connection (includes HTTPS Auth + MQTT Connect)
    if (!client.connect()) {
        Serial.print("Connection failed: ");
        Serial.println(client.getLastError());
    }
}

void loop() {
    client.loop();

    // Send telemetry every 10 seconds
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 10000) {
        lastMsg = millis();
        
        StaticJsonDocument<128> doc;
        doc["temperature"] = random(20, 30);
        doc["humidity"] = random(40, 60);
        
        String jsonBuffer;
        serializeJson(doc, jsonBuffer);
        
        if (client.publishTelemetry(jsonBuffer)) {
            Serial.println("Telemetry sent: " + jsonBuffer);
        } else {
            Serial.println("Telemetry failed");
        }
    }
}
