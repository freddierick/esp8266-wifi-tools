#include <ESP8266WiFi.h>
#include <SimpleTimer.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include "index.h"

SimpleTimer networkScaner_Timer(5000);
bool networkScaner_active = false;
bool networkScaner_paused = false;
DynamicJsonDocument networkScaner_json(1024);
char networkScaner_jsonTemplate[] = "{\"data\":[]}";

ESP8266WebServer apiServer(80);

// Function protrypes for api handler
void handleRoot(); 
void handleGetNetworks();
void handleGetState();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  const char* ssid     = "ESP8266-Access-Point";
  const char* password = "123456789";

  WiFi.softAP(ssid, password);
  

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  apiServer.on("/", HTTP_GET, handleRoot);

  apiServer.on("/state", HTTP_GET, handleGetState);
  apiServer.on("/networks", HTTP_GET, handleGetNetworks);
  
  apiServer.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}

void loop() {
  apiServer.handleClient();
  long currentMillis = millis();
  // put your main code here, to run repeatedly:

  // 
  if (!networkScaner_paused && !networkScaner_active) {
    WiFi.scanNetworks(true);
    Serial.print("\nScan start ... ");
    networkScaner_active = true;
  }
  
  if (networkScaner_Timer.isReady() && networkScaner_active && WiFi.scanComplete() >= 0) {
    Serial.printf("%d network(s) found\n", WiFi.scanComplete());
    // deserializeJson(networkScaner_jsonTemplate, networkScaner_json);
    // memset(networkScaner_array, 0, sizeof(networkScaner_array));

    for (int i = 0; i < WiFi.scanComplete(); i++)
    {
      networkScaner_json[i]["SSID"] = WiFi.SSID(i);
      networkScaner_json[i]["channel"] = WiFi.channel(i);
      networkScaner_json[i]["RSSI"] = WiFi.RSSI(i);
      networkScaner_json[i]["encryptionType"] = WiFi.encryptionType(i);
      networkScaner_json[i]["index"] = i; 
      // Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : "");
    }
    serializeJson(networkScaner_json, Serial);
    
    networkScaner_active = false;
    WiFi.scanDelete();
  }



}


void handleRoot() {
  apiServer.send(200, "text/html", indexHtml);
}

void handleGetState() {
  String json_string;
  DynamicJsonDocument state_json(1024);
  state_json["time"] = millis();
  state_json["isNetworkDiscoveryPaused"] = networkScaner_paused;
  serializeJson(state_json, json_string);
  apiServer.send(200, "application/json", json_string);
}

void handleGetNetworks() {
  String networkScaner_json_string;
  serializeJson(networkScaner_json, networkScaner_json_string);
  apiServer.send(200, "application/json", networkScaner_json_string);
}
