#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

/* ===============================
   WiFi設定
================================ */
const char* ssid     = "aterm-e26bca-g";
const char* password = "05a7282157314";

/* ===============================
   WebSocket設定
================================ */
const char* websocket_host = "tool-webs.onrender.com";
const uint16_t websocket_port = 443;
const char* websocket_path = "/ws/wmqtt";

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
      USBSerial.println("[WS] Disconnected");
      break;

    case WStype_CONNECTED:
      USBSerial.println("[WS] Connected to server");
      break;

    case WStype_TEXT:
      USBSerial.println("[WS] Received TEXT:");
      USBSerial.println((char*)payload);  // JSONをそのまま表示
      break;

    case WStype_ERROR:
      USBSerial.println("[WS] Error");
      break;

    default:
      break;
  }
}

/* ===============================
   セットアップ
================================ */
void setup() {
  USBSerial.begin(115200);
  delay(1000);

  /* WiFi接続 */
  USBSerial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USBSerial.print(".");
  }

  USBSerial.println("\nWiFi connected");
  USBSerial.print("IP: ");
  USBSerial.println(WiFi.localIP());

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
