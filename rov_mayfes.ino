#define BLYNK_TEMPLATE_ID "TMPLjvg-o1rH"
#define BLYNK_DEVICE_NAME "White"                           // Red or White
#define BLYNK_AUTH_TOKEN "XWgLBdtNfMilZn8TsdO0RgVIAzhL8XUU" // White
// #define BLYNK_AUTH_TOKEN "YKL7W_XpqUu_MIYGKqP1IrevpxR_eVgT" // Red
#define WIFI_SSID "Galaxy_5GMW_7641"
#define WIFI_PASSWORD "uvum8745"
#define BLYNK_PRINT Serial

#define SERIAL_PRINT_ON // いらなきゃCOすること

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "config.h" //wifiパスワードなんかを外部ファイルから読み込む際に使用

// ESP32の注意点として、書き込み時にBootボタンを押さないとコンパイルエラーが出ることがある

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

Servo servo1;
Servo servo2;
Servo servo3;

const int neutralUs = 1500;
const int maxUs = 1900;
const int minUs = 1100;

const int servo1Pin = 25; // 表記はGPIO12
const int servo1Period = 50;
const int servo2Pin = 26; // 表記は13
const int servo2Period = 50;
const int servo3Pin = 27; // 表記は13
const int servo3Period = 50;

int servo1Us = neutralUs;
int servo2Us = neutralUs;
int servo3Us = neutralUs;

float accelCoeff = 1.0f;
int servoInput1 = 0;
int servoInput2 = 0;
int servoInput3 = 0;
float Strength = 0.7;

// アプリ側でVirtual Pinに書き込みがあるたびに呼ばれる関数
// paramがV6に書き込まれたデータで、asInt()でInt型として処理 asFloatとかも色々ある

void myTimerEvent()
{
    Blynk.virtualWrite(V5, millis() / 1000); // 起動してからの経過時間
}

BLYNK_WRITE(V7)
{
    accelCoeff = param.asInt() / 100.0f;
}

BLYNK_WRITE(V1)
{
    servoInput1 = param.asInt();
}

BLYNK_WRITE(V2)
{
    servoInput2 = param.asInt();
}

BLYNK_WRITE(V3)
{
    servoInput3 = param.asInt();
}

BlynkTimer dataSendTimer;
BlynkTimer controlLogicTimer;

// スラスターの回転方向に合わせて＋round～か-round～に書き換え
int curve1(int input)
{
    int x = neutralUs + round(input * accelCoeff);
    return constrain(x, minUs, maxUs);
}

int curve2(int input)
{
    int x = neutralUs + round(input * accelCoeff);
    return constrain(x, minUs, maxUs);
}

int curve3(int input)
{
    int x = neutralUs + round(input * accelCoeff);
    return constrain(x, minUs, maxUs);
}

void servoLoop()
{
    servo1Us = curve1(servoInput1);
    servo2Us = curve2(servoInput2);
    servo3Us = curve3(servoInput3);

    servo1.writeMicroseconds(servo1Us);
    servo2.writeMicroseconds(servo2Us);
    servo3.writeMicroseconds(servo3Us);

#ifdef SERIAL_PRINT_ON
    Serial.print("Servo1: ");
    Serial.print(servo1Us - neutralUs);
    Serial.print(", Servo2: ");
    Serial.print(servo2Us - neutralUs);
    Serial.print(", Servo3: ");
    Serial.println(servo3Us - neutralUs);
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

    servo1.writeMicroseconds(1600);
    servo2.writeMicroseconds(1500);
    servo3.writeMicroseconds(1500);

    Serial.println("Servo Ready");

    delay(3000);

    Blynk.begin(auth, ssid, pass);
    dataSendTimer.setInterval(1000L, myTimerEvent);
    controlLogicTimer.setInterval(20L, servoLoop);
}

void loop()
{
    Serial.println("Running Now");
    Blynk.run();
    dataSendTimer.run();
    controlLogicTimer.run();
}
