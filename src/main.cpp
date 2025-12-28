#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Websocket.h>  // Arduino-Websocket-Fast

/* ===== WiFi設定 ===== */
const char* WIFI_SSID     = "aterm-e26bca-g";
const char* WIFI_PASSWORD = "05a7282157314";

/* ===== 接続先 ===== */
static const char* WS_HOST = "tool-webs.onrender.com";
static const int   WS_PORT = 443;
static const char* WS_PATH = "/ws/wmqtt";

/* ===== WebSocket ===== */
Websocket websocket;
WiFiClientSecure secureClient;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== WebSocket Fast Client (wss) ===");

  // Wi-Fi接続
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Connected");
  Serial.print("[WiFi] MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  // 証明書検証オフ（まずは動作優先）
  secureClient.setInsecure();

  // WebSocket 初期化
  websocket.setClient(&secureClient);
  websocket.onEvent([](WebsocketEvent event, String str) {
    if (event == WS_EVT_CONNECT) {
      Serial.println("[WS] Connected");
    } else if (event == WS_EVT_DISCONNECT) {
      Serial.println("[WS] Disconnected");
    } else if (event == WS_EVT_TEXT) {
      Serial.println("[WS] Received JSON:");
      Serial.println(str);
    }
  });

  // 実際に接続（wss://example.com/ws）
  String url = String("wss://") + WS_HOST + WS_PATH;
  websocket.begin(url, WS_PORT);
}

void loop() {
  // 受信処理
  websocket.poll();
}
