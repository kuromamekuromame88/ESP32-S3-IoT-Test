#include <WiFi.h>
#include <WebSocketsClient.h>

/* ===============================
   WiFi設定
================================ */
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

/* ===============================
   WebSocket設定
================================ */
const char* WS_HOST = "example.com";   // サーバーのドメイン or IP
const uint16_t WS_PORT = 80;            // wsなら80, wssなら443
const char* WS_PATH = "/ws";            // WebSocketのパス

WebSocketsClient webSocket;

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {

    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected");
      break;

    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      break;

    case WStype_TEXT:
      Serial.println("[WebSocket] Received message:");
      Serial.println((char*)payload);   // JSON文字列をそのまま表示
      break;

    case WStype_ERROR:
      Serial.println("[WebSocket] Error");
      break;

    default:
      break;
  }
}

/* ===============================
   セットアップ
================================ */
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== M5Stamp S3 WebSocket Logger ===");

  /* ---- WiFi接続 ---- */
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WiFi] Connected");

  /* ---- MACアドレス表示 ---- */
  String mac = WiFi.macAddress();
  Serial.print("[WiFi] MAC Address: ");
  Serial.println(mac);

  /* ---- IP表示（おまけ） ---- */
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  /* ---- WebSocket接続 ---- */
  webSocket.begin(WS_HOST, WS_PORT, WS_PATH);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000); // 再接続間隔(ms)
}

/* ===============================
   ループ
================================ */
void loop() {
  webSocket.loop();
}
