#include <WiFi.h>
#include <WebSocketsClient.h>

/* ===============================
   WiFi設定
================================ */
const char* WIFI_SSID     = "aterm-e26bca-g";
const char* WIFI_PASSWORD = "05a7282157314";

/* ===============================
   WebSocket（wss）
================================ */
const char* WS_HOST = "tool-webs.onrender.com";
const uint16_t WS_PORT = 443;
const char* WS_PATH = "/ws/wmqtt";

WebSocketsClient webSocket;

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {

    case WStype_CONNECTED:
      Serial.println("[WebSocket] Connected (wss)");
      break;

    case WStype_DISCONNECTED:
      Serial.println("[WebSocket] Disconnected");
      break;

    case WStype_TEXT:
      Serial.println("[WebSocket] Received JSON:");
      Serial.println((char*)payload);
      break;

    case WStype_ERROR:
      Serial.println("[WebSocket] Error");
      break;

    default:
      break;
  }
}

/* ===============================
   setup
================================ */
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== M5Stamp S3 WSS Logger (PlatformIO) ===");

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

  /* ---- WSS接続（証明書検証なし） ---- */
  webSocket.beginSSL(
    WS_HOST,
    WS_PORT,
    WS_PATH,
    "",     // サブプロトコルなし
    false   // ← これが「insecure」
  );

  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

/* ===============================
   loop
================================ */
void loop() {
  webSocket.loop();
}
