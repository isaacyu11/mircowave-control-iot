//D0:EF:76:44:D4:04
#define BLYNK_TEMPLATE_ID           "TMPL6Hbt2JWWS"
#define BLYNK_TEMPLATE_NAME         "Microwave Control IOT"
#define BLYNK_AUTH_TOKEN            "j7Js-LI0KwhnbSf-VC-wgmhg6Qb3-4bn"

#include <WiFi.h>
#include <WiFiClient.h>
#include <esp_now.h>
#include <BlynkSimpleEsp32.h>

#define RELAY_PIN 2

char ssid[] = "mmslab_dev";
char pass[] = "mmslab406";

bool relayOn = false;
bool microwaveRunning = false;
unsigned long relayTimer = 900000;
unsigned long relayStartTime = 0;

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  int value = param.asInt();

  if(value == 1){
    relayOn = true;
  }
  else{
    relayOn = false;
  }
}

BLYNK_WRITE(V1)
{
  int value = param.asInt();

  relayTimer = value*60000;
}

void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  // 处理接收到的数据
  char message[len + 1];
  memcpy(message, incomingData, len);
  message[len] = '\0';
  
  Serial.print("Received message: ");
  Serial.println(message);

  if (strcmp(message, "DOOR_OPEN") == 0) {
    // 冰箱门打开，启动微波炉
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Microwave started");
    
    relayStartTime = millis();
    microwaveRunning = true;
  }
}


void setup() {
  // Debug console
  Serial.begin(115200);

  // 设置Wi-Fi模式为STA（Station）
  WiFi.mode(WIFI_STA);

  // 延迟以确保Wi-Fi模块初始化
  delay(100); // 添加一个短暂的延迟
  
  // 获取MAC地址
  String macAddress = WiFi.macAddress();
  
  Serial.print("MAC Address: ");
  Serial.println(macAddress);

  // 初始化ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 注册接收回调函数
  esp_now_register_recv_cb(onDataRecv);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(2, OUTPUT); // 假设GPIO 2为输出引脚
  digitalWrite(RELAY_PIN, LOW);  // 初始化为低电平（关闭继电器）
}

void loop() {
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
  if(relayOn)
  {
    relayStartTime = millis();
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Microwave on");
    microwaveRunning = true;
  }

  if(microwaveRunning && millis() - relayStartTime >= relayTimer)
  {
    digitalWrite(RELAY_PIN,LOW);
    Serial.println("Microwave off");

    microwaveRunning = false;
  }

}
