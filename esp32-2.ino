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

const char* ssid = "lzsdiy";
const char* password = "xxxxxxxx";
const int webSocketPort = 5246;

// WEB网页
#include <ESPAsyncWebSrv.h>
const int httpServerPort = 80;
AsyncWebServer server(httpServerPort);

#include <String.h>

void putlog(const int x, const String& logdata) {
  // 清空屏幕
  display.clearBuffer();

  // 设置字体和位置
  display.setFont(u8g2_font_profont12_tf);
  
  display.setCursor(x, 8); // 在坐标(x, y)
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
        Serial.printf("post %d\n", postValue);
      } else {
        Serial.println("post 字段不是一个有效的数字");
        return;
      }
      
      // 根据 post 值控制串口引脚
      if (postValue == 12) {
        controlSerial(12, true);
        putlog(0,"port 12 on");
        delay(200); // 高电平持续 200 ms
        putlog(0,"port 12 off");
        controlSerial(12, false);
      }
      else if (postValue == 14) {
        controlSerial(14, true);
        putlog(0,"port 14 on");
        delay(200); // 高电平持续 200 ms
        putlog(0,"port 14 off");
        controlSerial(14, false);
      }
      else if (postValue == 27) {
        controlSerial(27, true);
        putlog(0,"port 27 on");
        delay(200); // 高电平持续 200 ms
        putlog(0,"port 27 off");
        controlSerial(27, false);
      }
      else if (postValue == 26) {
        controlSerial(26, true);
        putlog(0,"port 26 on");
        delay(200); // 高电平持续 200 ms
        putlog(0,"port 26 off");
        controlSerial(26, false);
      }
      else if (postValue == 25) {
        controlSerial(25, true);
        putlog(0,"port 25 on");
        delay(200); // 高电平持续 200 ms
        putlog(0,"port 25 off");
        controlSerial(25, false);
      }
      else {
        Serial.println("未知的 post 值");
        putlog(0,"port errorf");
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
  
  putlog(0,"ESP32 is running...");
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
  Serial.print("IP 地址: ");
  Serial.println(myIP.toString());
  String wifidata;
  wifidata += ssid;
  wifidata += " ";
  wifidata += myIP.toString();
  putlog(0, wifidata);
  delay(500);
  
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){

String html = R"html(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP32 Web控制台</title>
</head>
<body>
  
  <h1>ESP32 Web控制台</h1>
  
  <div>
    <button id="port12">控制引脚12</button>
  </div>
  <div>
    <button id="port14">控制引脚14</button>
  </div>
  <div>
    <button id="port25">控制引脚25</button>
  </div>
  <div>
    <button id="port26">控制引脚26</button>
  </div>
  <div>
    <button id="port27">控制引脚27</button>
  </div>

  <script>
    const serverAddress = 'ws://192.168.4.1:5246';
    const socket = new WebSocket(serverAddress);
    
    socket.onopen = function(event) {
      console.log('连接成功');
    };
    
    socket.onmessage = function(event) {
      console.log(event.data);
    };
    
    socket.onerror = function(error) {
      console.error(error);
    };
    
    socket.onclose = function(event) {
      console.log('连接已关闭');
    };
    
    const buttons = document.getElementsByTagName('button');
    
    for (let i = 0; i < buttons.length; i++) {
      const button = buttons[i];
      
      button.addEventListener('click', function(event) {
        const port = this.id.substring(4); // 获取端口号（12、14、25、26或27）
        const message = JSON.stringify({post: parseInt(port)});
        socket.send(message);
      });
    }
  </script>
  
</body>
</html>
)html";

    request->send(200, "text/html", html);
  });
  
  server.begin();
}

void loop() {
  webSocket.loop();
  delay(5); // 添加延迟，释放CPU资源
}
