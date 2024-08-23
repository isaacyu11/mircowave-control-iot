//CC:7B:5C:1E:1A:EC
#include <esp_now.h>
#include <WiFi.h>

// 微波爐ESP32的MAC地址（请用实际的MAC地址替换）
uint8_t microwaveMACAddress[] = {0xD0, 0xEF, 0x76, 0x44, 0xD4, 0x04};

// 极限开关连接的GPIO引脚
#define LIMIT_SWITCH_PIN 13

esp_now_peer_info_t peerInfo;

// 发送完成后的回调函数
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // 设置Wi-Fi模式为STA（Station）
  WiFi.mode(WIFI_STA);

  // 初始化ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // 注册发送完成回调函数
  esp_now_register_send_cb(onDataSent);

  // 添加对等设备（微波炉ESP32的MAC地址）
  memcpy(peerInfo.peer_addr, microwaveMACAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // 设置极限开关引脚为输入模式
  pinMode(LIMIT_SWITCH_PIN, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  int switchState = digitalRead(LIMIT_SWITCH_PIN);

  if (switchState == HIGH) {
    // 当开关打开（冰箱门打开）时发送信号
    const char *message = "DOOR_OPEN";
    esp_err_t result = esp_now_send(microwaveMACAddress, (uint8_t *)message, strlen(message));
    Serial.println("Door opened, message sent!");
    if (result == ESP_OK) {
    Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
    
    delay(1000);  // 防止重复发送太快
  }
}
