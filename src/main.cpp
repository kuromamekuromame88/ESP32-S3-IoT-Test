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
      SerialUSB.println("[WS] Disconnected");
      break;

    case WStype_CONNECTED:
      SerialUSB.println("[WS] Connected to server");
      break;

    case WStype_TEXT:
      SerialUSB.println("[WS] Received TEXT:");
      SerialUSB.println((char*)payload);  // JSONをそのまま表示
      break;

    case WStype_ERROR:
      SerialUSB.println("[WS] Error");
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
  delay(1000);

  /* WiFi接続 */
  SerialUSB.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    SerialUSB.print(".");
  }

  SerialUSB.println("\nWiFi connected");
  SerialUSB.print("IP: ");
  SerialUSB.println(WiFi.localIP());

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
