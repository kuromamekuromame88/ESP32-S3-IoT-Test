#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

/* ===============================
   WiFi設定
================================ */
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

/* ===============================
   WebSocket設定
================================ */
const char* websocket_host = "example.com";
const uint16_t websocket_port = 443;
const char* websocket_path = "/ws";

/* ===============================
   オブジェクト
================================ */
WebSocketsClient webSocket;

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {

    case WStype_DISCONNECTED:
      Serial.println("[WS] Disconnected");
      break;

    case WStype_CONNECTED:
      Serial.println("[WS] Connected to server");
      break;

    case WStype_TEXT:
      Serial.println("[WS] Received TEXT:");
      Serial.println((char*)payload);  // JSONをそのまま表示
      break;

    case WStype_ERROR:
      Serial.println("[WS] Error");
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
  delay(1000);

  /* WiFi接続 */
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  /* WebSocket設定 */
  webSocket.beginSSL(websocket_host, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);

  webSocket.setReconnectInterval(5000); // 5秒ごとに再接続
}

/* ===============================
   ループ
================================ */
void loop() {
  webSocket.loop();
}
