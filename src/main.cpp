#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

/* ===============================
   WiFi設定
================================ */
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

/* ===============================
   WebSocket設定（wss）
================================ */
const char* WS_HOST = "example.com";
const uint16_t WS_PORT = 443;
const char* WS_PATH = "/ws";

WebSocketsClient webSocket;

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {

    case WStype_CONNECTED:
      SerialUSB.println("[WebSocket] Connected (wss)");
      break;

    case WStype_DISCONNECTED:
      SerialUSB.println("[WebSocket] Disconnected");
      break;

    case WStype_TEXT:
      SerialUSB.println("[WebSocket] Received JSON:");
      SerialUSB.println((char*)payload);
      break;

    case WStype_ERROR:
      SerialUSB.println("[WebSocket] Error");
      break;

    default:
      break;
  }
}

/* ===============================
   セットアップ
================================ */
void setup() {
  SerialUSB.begin(115200);
  delay(500);

  SerialUSB.println("=== M5Stamp S3 WSS Logger ===");

  /* ---- WiFi接続 ---- */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  SerialUSB.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    SerialUSB.print(".");
  }

  SerialUSB.println("\n[WiFi] Connected");

  /* ---- MACアドレス表示 ---- */
  SerialUSB.print("[WiFi] MAC Address: ");
  SerialUSB.println(WiFi.macAddress());

  SerialUSB.print("[WiFi] IP Address: ");
  SerialUSB.println(WiFi.localIP());

  /* ---- WSS接続 ---- */
  webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);

  // 証明書検証を行わない（まずはこれでOK）
  webSocket.setInsecure();

  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

/* ===============================
   ループ
================================ */
void loop() {
  webSocket.loop();
}
