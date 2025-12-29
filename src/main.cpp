#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#define DEVICE_TYPE "ONOFFLight"
#define LED_PIN 5

/* ===============================
   WiFi設定
================================ */
const char* ssid     = "aterm-e26bca-g";
const char* password = "05a7282157314";

/* ===============================
   WebSocket設定
================================ */
const char* WS_HOST = "tool-webs.onrender.com";
const uint16_t WS_PORT = 443;
const char* WS_PATH = "/ws/wmqtt";

/* ===============================
   アプリ名
================================ */
const char* APP_NAME = "wmqtt";

/* ===============================
   WifiのMACアドレス
=============================== */
String MACAddress;

/* ===============================
   WebSocket
================================ */
WebSocketsClient webSocket;

/* ===============================
   グローバル状態
================================ */
bool Light_flag = false;

/* ===============================
   JSON送信（dataなし）
================================ */
void sendJson(const char* type) {
  StaticJsonDocument<200> doc;
  doc["app"]  = APP_NAME;
  doc["type"] = type;

  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(json);

  USBSerial.println(json);
}

/* ===============================
   JSON送信（dataあり）
   ※ controlは含めない
================================ */
void sendJson(const char* type, JsonDocument& dataDoc) {
  StaticJsonDocument<300> doc;
  doc["app"]  = APP_NAME;
  doc["type"] = type;
  doc["data"] = dataDoc.as<JsonObject>();

  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(json);

  USBSerial.println(json);
}

/* ===============================
   受信JSON処理
================================ */
void handleJson(const char* type, JsonDocument& doc) {
  if(strcmp(app, "wmqtt") !== 0) return;
  if (strcmp(type, "ping") == 0) {
    sendJson("pong");
    return;
  }

  if (strcmp(type, "onoff") == 0) {
    bool value = doc["data"]["value"] | false;
    Light_flag = value;

    USBSerial.print("[ONOFF] ");
    USBSerial.println(value ? "ON" : "OFF");
    return;
  }

  USBSerial.print("[UNKNOWN] ");
  USBSerial.println(type);
}

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t event, uint8_t* payload, size_t length) {

  switch (event) {

    case WStype_CONNECTED: {
      StaticJsonDocument<200> data;
      data["deviceID"]   = MACAddress;
      data["devicetype"] = DEVICE_TYPE;
      data["control"]    = false;   // ★ 登録時のみの識別フラグ

      sendJson("regist", data);
      break;
    }

    case WStype_TEXT: {
      StaticJsonDocument<512> doc;
      if (deserializeJson(doc, payload)) return;

      handleJson(doc["type"] | "", doc);
      break;
    }

    default:
      break;
  }
}

/* ===============================
   setup
================================ */
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  USBSerial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  MACAddress = WiFi.macAddress();

  webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

/* ===============================
   loop
================================ */
void loop() {
  webSocket.loop();
  digitalWrite(LED_PIN, Light_flag ? HIGH : LOW);
}
