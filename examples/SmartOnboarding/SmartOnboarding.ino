#include <WiFi.h>
#include <mnv-iot-sdk/IoTClient.h>

// Full URL to your MNV-IoT Backend API
const char* apiBaseUrl = "http://api.yourdomain.com/api/v1";

mnv::IoTClient client(apiBaseUrl);

const int CONFIG_BUTTON_PIN = 0; // Usually Boot button on ESP32

void setup() {
    Serial.begin(115200);
    pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);

    Serial.println("\n--- MNV-IoT Smart Onboarding ---");
    
    // Check if we should enter Smart Config mode (e.g. at first boot or button held)
    // Here we auto-enter config if no WiFi is saved in flash internally by the SDK.
    if (!client.connect()) {
        Serial.println("No saved configuration or WiFi fail. Starting Smart Config Portal...");
        client.startSmartConfig(); // Blocks and restarts device upon success
    }

    Serial.println("Device connected successfully!");
    
    // Setup message handler
    client.onCommand([](char* topic, byte* payload, unsigned int length) {
        Serial.println("Command received!");
    });
}

void loop() {
    client.loop();

    // Long press Boot button to reset configuration
    if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
        unsigned long startTime = millis();
        while(digitalRead(CONFIG_BUTTON_PIN) == LOW) { delay(10); }
        if (millis() - startTime > 3000) {
            Serial.println("Resetting configuration...");
            client.startSmartConfig();
        }
    }

    // Send simple heartbeat
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 30000) {
        lastMsg = millis();
        client.publishTelemetry("{\"uptime\":" + String(millis()/1000) + "}");
    }
}
