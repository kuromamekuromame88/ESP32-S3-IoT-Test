#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#define DEVICE_TYPE "ONOFFLight"

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
   JSON送信（dataなし）
================================ */
void sendJson(const char* type) {
  StaticJsonDocument<300> doc;
  doc["app"]  = APP_NAME;
  doc["type"] = type;

  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(json);

  USBSerial.println(json);
}

/* ===============================
   JSON送信（通常通知）
   ※ controlフラグなし
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
   JSON送信（制御要求）
   ※ control=true
================================ */
void sendControl(const char* type, JsonDocument& dataDoc) {
  StaticJsonDocument<300> doc;
  doc["app"]     = APP_NAME;
  doc["type"]    = type;
  doc["control"] = true;
  doc["data"]    = dataDoc.as<JsonObject>();

  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(json);

  USBSerial.println(json);
}

/* ===============================
   受信JSON処理
================================ */
void handleJson(const char* app, const char* type, JsonDocument& doc) {

  bool isControl = doc["control"] | false;

  /* ---- 接続維持 ---- */
  if (strcmp(type, "ping") == 0) {
    sendJson("pong");
    return;
  }

  /* ---- 制御メッセージ ---- */
  if (isControl) {
    USBSerial.print("[CONTROL] ");
    USBSerial.println(type);

    if (strcmp(type, "onoff") == 0) {
      bool value = doc["data"]["value"] | false;
      USBSerial.println(value ? "ON" : "OFF");

      // 👉 GPIO制御をここに
    }
    return;
  }

  /* ---- 通常通知 ---- */
  USBSerial.print("[INFO] ");
  USBSerial.println(type);
}

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {

    case WStype_CONNECTED: {
      StaticJsonDocument<200> data;
      data["deviceID"]   = MACAddress;
      data["devicetype"] = DEVICE_TYPE;

      // 登録メッセージ（controlなし）
      sendJson("regist", data);
      break;
    }

    case WStype_TEXT: {
      StaticJsonDocument<512> doc;
      if (deserializeJson(doc, payload)) return;

      handleJson(
        doc["app"]  | "",
        doc["type"] | "",
        doc
      );
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
}
