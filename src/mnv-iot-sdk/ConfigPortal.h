#ifndef MNV_CONFIG_PORTAL_H
#define MNV_CONFIG_PORTAL_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

namespace mnv {

class ConfigPortal {
public:
    ConfigPortal();
    void start(const String& ssid, const String& deviceId = "");
    void loop();
    bool isConfigDone() const { return _configDone; }
    
    String getSsid() const { return _ssid; }
    String getPassword() const { return _password; }
    String getDeviceId() const { return _deviceId; }
    String getSecret() const { return _secret; }

private:
    void handleRoot();
    void handleSave();
    void handleNotFound();

    WebServer _server;
    DNSServer _dnsServer;
    
    String _ssid;
    String _password;
    String _deviceId;
    String _secret;
    bool _configDone = false;
};

} // namespace mnv

#endif // MNV_CONFIG_PORTAL_H
