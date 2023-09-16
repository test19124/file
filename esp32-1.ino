#include <WiFi.h>
#include <WebSocketsServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <U8g2lib.h>

#define SCREEN_WIDTH 128   // OLED屏幕宽度
#define SCREEN_HEIGHT 32   // OLED屏幕高度

// OLED屏幕对象
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#include <ArduinoJson.h>

const char* ssid = "lzsdiv";
const char* password = "xxxxxxxx";
const int webSocketPort = 5246;

#include <String.h>

void putlog(const String& logdata) {
  // 清空屏幕
  display.clearBuffer();

  // 设置字体和位置
  display.setFont(u8g2_font_profont12_tf);
  display.setCursor(0, 8); // 在坐标(0, 8)

  display.print(logdata.c_str());

  // 更新屏幕显示
  display.sendBuffer();
}

WebSocketsServer webSocket = WebSocketsServer(webSocketPort);

void controlSerial(int pin, bool isHigh) {
  digitalWrite(pin, isHigh ? HIGH : LOW);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  
  StaticJsonDocument<200> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, payload);

  switch(type) {
    case WStype_CONNECTED:
      Serial.printf("[%u] 连接成功\n", num);
      break;
    case WStype_TEXT:
      Serial.printf("[%u] 收到消息: %s\n", num, payload);
      
      // 解析 JSON 消息
      if (error) {
        Serial.print("解析 JSON 失败：");
        Serial.println(error.c_str());
        return;
      }
      
      // 读取 post 字段值
      int postValue;

      // 检查是否为有效的数字
      if (jsonDoc["post"].is<int>()) {
        postValue = jsonDoc["post"];
      } else {
        Serial.println("post 字段不是一个有效的数字");
        return;
      }
      
      // 根据 post 值控制串口引脚
      if (postValue == 12) {
        controlSerial(12, true);
        putlog("port 12");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(12, false);
      }
      else if (postValue == 14) {
        controlSerial(14, true);
        putlog("port 14");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(14, false);
      }
      else if (postValue == 27) {
        controlSerial(27, true);
        putlog("port 27");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(27, false);
      }
      else if (postValue == 26) {
        controlSerial(26, true);
        putlog("port 26");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(26, false);
      }
      else if (postValue == 25) {
        controlSerial(25, true);
        putlog("port 25");
        delay(200); // 高电平持续 200 ms
        putlog("");
        controlSerial(25, false);
      }
      else {
        Serial.println("未知的 post 值");
      }
      break;
    case WStype_DISCONNECTED:
      Serial.printf("[%u] 断开连接\n", num);
      break;
  }
}

void setup() {
  // 初始化串口连接
  Serial.begin(115200);

  // 初始化I2C总线和OLED屏幕
  Wire.begin();
  
  // 初始化 OLED 屏幕
  display.begin();
  
  putlog("is running...");
  delay(200);
  
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  pinMode(27, OUTPUT);
  digitalWrite(27, LOW);
  pinMode(26, OUTPUT);
  digitalWrite(26, LOW);
  pinMode(25, OUTPUT);
  digitalWrite(25, LOW);
  
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("访问点名称: ");
  Serial.println(ssid);
  putlog(ssid);
  delay(500);
  Serial.print("IP 地址: ");
  Serial.println(myIP.toString());
  putlog(myIP.toString());
  delay(500);
  
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {
  webSocket.loop();
  delay(5); // 添加延迟，释放CPU资源
}
