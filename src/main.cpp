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

  USBSerial.print("[WS] Sent: ");
  USBSerial.println(json);
}

/* ===============================
   JSON送信（dataあり）
================================ */
void sendJson(const char* type, JsonDocument& dataDoc) {
  StaticJsonDocument<300> doc;

  doc["app"]  = APP_NAME;
  doc["type"] = type;
  doc["data"] = dataDoc.as<JsonObject>();

  String json;
  serializeJson(doc, json);

  webSocket.sendTXT(json);

  USBSerial.print("[WS] Sent: ");
  USBSerial.println(json);
}

/* ===============================
   受信JSON処理
================================ */
void handleJson(const char* app, const char* type, JsonDocument& doc) {

  if (strcmp(type, "ping") == 0) {
    USBSerial.println("[APP] ping received");
    sendJson("pong");
  }

  else if (strcmp(type, "chat") == 0 && strcmp(app, "webchat") == 0) {
    const char* data = doc["data"] | "(no data)";
    USBSerial.print("[APP] webchat chat data: ");
    USBSerial.println(data);
  }

  else {
    USBSerial.print("[APP] unknown type: ");
    USBSerial.println(type);
  }
}

/* ===============================
   WebSocketイベント
================================ */
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {

    case WStype_CONNECTED: {
      USBSerial.println("[WS] Connected");

      StaticJsonDocument<200> data;
      data["deviceID"]   = MACAddress;
      data["devicetype"] = DEVICE_TYPE;

      sendJson("regist", data);
      break;
    }

    case WStype_DISCONNECTED:
      USBSerial.println("[WS] Disconnected");
      break;

    case WStype_TEXT: {
      USBSerial.println("[WS] Received JSON:");

      String jsonStr = String((char*)payload);
      USBSerial.println(jsonStr);

      StaticJsonDocument<512> doc;
      DeserializationError err = deserializeJson(doc, jsonStr);

      if (err) {
        USBSerial.print("[JSON] Parse error: ");
        USBSerial.println(err.c_str());
        return;
      }

      const char* app  = doc["app"]  | "";
      const char* msgType = doc["type"] | "";

      handleJson(app, msgType, doc);
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
  delay(1000);

  WiFi.begin(ssid, password);
  USBSerial.print("WiFi connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    USBSerial.print(".");
  }

  MACAddress = WiFi.macAddress();
  USBSerial.print("\nMACアドレス: ");
  USBSerial.println(MACAddress);

  USBSerial.println("WiFi connected");
  USBSerial.println(WiFi.localIP());

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
