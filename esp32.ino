#include <WiFi.h>
#include <WebSocketsServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <U8g2lib.h>

#define OLED_SDA 21    // 设置OLED的SDA接口引脚
#define OLED_SCL 22    // 设置OLED的SCL接口引脚

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA);

#include <ArduinoJson.h>

const char* ssid = "lzsdiv";
const char* password = "xxxxxxxx";
const int webSocketPort = 5246;

#include <String.h>

void putlog(String logdata) {
  u8g2.clearBuffer();   // 清空缓冲区
  u8g2.setFont(u8g2_font_ncenB08_tr);   // 设置字体
  u8g2.drawStr(0, 16, logdata);   // 在坐标(0, 16)处显示字符串"hello"
  u8g2.sendBuffer();   // 发送缓冲区内容到显示器
}

WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

void controlSerial(int pin, bool isHigh) {
  digitalWrite(pin, isHigh ? HIGH : LOW);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("[%u] 连接成功\n", num);
      break;
    case WStype_TEXT:
      Serial.printf("[%u] 收到消息: %s\n", num, payload);
      
      // 解析 JSON 消息
      StaticJsonDocument<200> jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, payload);
      
      if (error) {
        Serial.print("解析 JSON 失败：");
        Serial.println(error.c_str());
        return;
      }
      
      // 读取 post 字段值
      int postValue = jsonDoc["post"];
      
      // 检查是否与串口引脚号相匹配
      if (postValue == 12) {
        controlSerial(12, true);
        putlog("12");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(12, false);
      }
      if (postValue == 14) {
        controlSerial(14, true);
        putlog("14");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(14, false);
      }
      if (postValue == 27) {
        controlSerial(27, true);
        putlog("27");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(27, false);
      }
      if (postValue == 26) {
        controlSerial(26, true);
        putlog("26");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(26, false);
      }
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u] 断开连接\n", num);
      break;
  }
}

void setup() {
  Serial.begin(115200);

  u8g2.begin(); // 显示器
  putlog("is runing...");
  delay(200); // 高电平持续 200 ms
  
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);
  pinMode(26, OUTPUT);
  digitalWrite(26, LOW);
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("访问点名称: ");
  Serial.println(ssid);
  putlog(ssid);
  delay(500);
  Serial.print("IP 地址: ");
  Serial.println(myIP);
  putlog(myIP);
  delay(500);
  
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {
  webSocket.loop();
}
