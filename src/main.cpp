#include <WiFi.h>
#include <WebsocketsClient.h>

using namespace websockets;

/* ===============================
   WiFi設定
================================ */
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

/* ===============================
   WebSocket設定
================================ */
const char* WS_URL = "wss://example.com/ws";

/* ===============================
   WebSocketクライアント
================================ */
WebsocketsClient wsClient;

/* ===============================
   setup
================================ */
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== ESP32-Websockets WSS Logger ===");

  /* ---- WiFi接続 ---- */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Connected");

  /* ---- MACアドレス表示 ---- */
  Serial.print("[WiFi] MAC Address: ");
  Serial.println(WiFi.macAddress());

  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  /* ---- WebSocketイベント ---- */
  wsClient.onEvent([](WebsocketsEvent event, String data) {
    switch (event) {

      case WebsocketsEvent::ConnectionOpened:
        Serial.println("[WebSocket] Connected (wss)");
        break;

      case WebsocketsEvent::ConnectionClosed:
        Serial.println("[WebSocket] Disconnected");
        break;

      case WebsocketsEvent::GotPing:
        Serial.println("[WebSocket] Ping");
        break;

      case WebsocketsEvent::GotPong:
        Serial.println("[WebSocket] Pong");
        break;
    }
  });

  wsClient.onMessage([](WebsocketsMessage message) {
    Serial.println("[WebSocket] Received JSON:");
    Serial.println(message.data());
  });

  /* ---- 証明書検証なし（まずは動作優先） ---- */
  wsClient.setInsecure();

  /* ---- wss接続 ---- */
  Serial.print("[WebSocket] Connecting to ");
  Serial.println(WS_URL);

  wsClient.connect(WS_URL);
}

/* ===============================
   loop
================================ */
void loop() {
  wsClient.poll();
}
