#define BLYNK_TEMPLATE_ID "TMPLjvg-o1rH"
#define BLYNK_DEVICE_NAME "Red"
#define BLYNK_AUTH_TOKEN "XWgLBdtNfMilZn8TsdO0RgVIAzhL8XUU"
#define WIFI_SSID "Xperia YHf41b"
#define WIFI_PASSWORD "f49a25e5a2f7q"
#define BLYNK_PRINT Serial

#define SERIAL_PRINT_ON//いらなきゃCOすること

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "config.h" //wifiパスワードなんかを外部ファイルから読み込む際に使用

//ESP32の注意点として、書き込み時にBootボタンを押さないとコンパイルエラーが出ることがある

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

Servo servo1;
Servo servo2;
Servo servo3;

const int maxUs = 1900;
const int minUs = 1100;

const int servo1Pin = 19;//表記はGPIO12
const int servo1Period = 50;
const int servo2Pin = 18;//表記は13
const int servo2Period = 50;
const int servo3Pin = 23;//表記は13
const int servo3Period = 50;

int servo1Us = 1500;
int servo2Us = 1500;
int servo3Us = 1500;

int turnClockAmount = 0;
int forwardAmount = 0;
float turnStrength = 0.5;
float forwardStrength = 0.7;

//アプリ側でVirtual Pinに書き込みがあるたびに呼ばれる関数
//paramがV6に書き込まれたデータで、asInt()でInt型として処理 asFloatとかも色々ある
//BLYNK_WRITE(V6)
//{
//  int dutycycle = param.asInt();
//  ledcWrite(led_channel, dutycycle);
//}

void myTimerEvent()
{
  Blynk.virtualWrite(V5, millis() / 1000);//起動してからの経過時間
}

BLYNK_WRITE(V8)
{
  turnClockAmount = param.asInt();
}

BLYNK_WRITE(V9)
{
  forwardAmount = param.asInt();
}

BlynkTimer timer1;
BlynkTimer timer2;


int curve1(int input)
{
  int x = 1500 + round(turnStrength * input);
  return constrain(x, minUs, maxUs);
}

int curve2(int input)
{
  int x = 1500 + round(forwardStrength * input);
  return constrain(x, minUs, maxUs);
}

void servoLoop()
{
  servo1Us = curve1(turnClockAmount);
  servo2Us = curve1(-turnClockAmount);
  servo3Us = curve2(forwardAmount);

  servo1.writeMicroseconds(servo1Us);
  servo2.writeMicroseconds(servo2Us);
  servo3.writeMicroseconds(servo3Us);

  #ifdef SERIAL_PRINT_ON
    Serial.print(servo1Us);
    Serial.print(", ");
    Serial.print(servo2Us);
    Serial.print(", ");
    Serial.println(servo3Us);
  #endif
} 

void setup()
{
  delay(100);
  // Debug console
  Serial.begin(9600);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo1.setPeriodHertz(servo1Period);
  servo1.attach(servo1Pin, minUs, maxUs);
  servo2.setPeriodHertz(servo2Period);
  servo2.attach(servo2Pin, minUs, maxUs);
  servo3.setPeriodHertz(servo3Period);
  servo3.attach(servo3Pin, minUs, maxUs);

  Serial.println("Servo Attached");

  delay(1000);

  servo1.writeMicroseconds(1500);
  servo2.writeMicroseconds(1500);
  servo3.writeMicroseconds(1500);

  Serial.println("Servo Ready");

  delay(3000);

  Blynk.begin(auth, ssid, pass);

  timer1.setInterval(1000L, myTimerEvent);
  timer2.setInterval(20L, servoLoop);
}

void loop()
{
  Serial.println("Running Now");
  Blynk.run();
  timer1.run();
  timer2.run();
}