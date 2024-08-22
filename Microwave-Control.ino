#define BLYNK_TEMPLATE_ID           "TMPL6Hbt2JWWS"
#define BLYNK_TEMPLATE_NAME         "Microwave Control IOT"
#define BLYNK_AUTH_TOKEN            "j7Js-LI0KwhnbSf-VC-wgmhg6Qb3-4bn"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "mmslab_dev";
char pass[] = "mmslab406";

bool relayOn = false;
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

void setup() {
  // Debug console
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(2, OUTPUT); // 假设GPIO 2为输出引脚
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
    digitalWrite(2, HIGH);
    Serial.println("relay on");
  }
  else if(!relayOn && millis() - relayStartTime > relayTimer)
  {
    digitalWrite(2,LOW);
    Serial.println("relay off");
  }
  else
  {
    relayOn = false;
  }
}
