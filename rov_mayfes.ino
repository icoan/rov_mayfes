#define BLYNK_TEMPLATE_ID "TMPLjvg-o1rH"
#define BLYNK_DEVICE_NAME "Red"
#define BLYNK_AUTH_TOKEN "XWgLBdtNfMilZn8TsdO0RgVIAzhL8XUU"
#define WIFI_SSID "AiR-WiFi_0OT1A1"
#define WIFI_PASSWORD "80680466"
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

const int neutralUs = 1500;
const int maxUs = 1900;
const int minUs = 1100;

const int servo1Pin = 25;//表記はGPIO12
const int servo1Period = 50;
const int servo2Pin = 26;//表記は13
const int servo2Period = 50;
const int servo3Pin = 27;//表記は13
const int servo3Period = 50;

int servo1Us = neutralUs;
int servo2Us = neutralUs;
int servo3Us = neutralUs;

float accelAmount = 1.0f;
int turnClockAmount = 0;
int forwardAmount = 0;
float turnStrength = 0.7;
float forwardStrength = 0.7;

//アプリ側でVirtual Pinに書き込みがあるたびに呼ばれる関数
//paramがV6に書き込まれたデータで、asInt()でInt型として処理 asFloatとかも色々ある

void myTimerEvent()
{
  	Blynk.virtualWrite(V5, millis() / 1000);//起動してからの経過時間
}

BLYNK_WRITE(V7)
{
  	accelAmount = param.asInt() / 100.0f;
}

BLYNK_WRITE(V8)
{
  	turnClockAmount = param.asInt();
}

BLYNK_WRITE(V9)
{
  	forwardAmount = -param.asInt();
}

BlynkTimer dataSendTimer;
BlynkTimer controlLogicTimer;


int curve1(int input)
{
    int x = neutralUs + round(turnStrength * input * accelAmount);
    return constrain(x, minUs, maxUs);
}

int curve2(int input)
{
    int x = neutralUs + round(forwardStrength * input * accelAmount);
    return constrain(x, minUs, maxUs);
}

void servoLoop()
{
    servo1Us = curve1(turnClockAmount);
    servo2Us = curve2(-forwardAmount);
    servo3Us = curve1(turnClockAmount);

    servo1.writeMicroseconds(servo1Us);
    servo2.writeMicroseconds(servo2Us);
    servo3.writeMicroseconds(servo3Us);

    #ifdef SERIAL_PRINT_ON
        Serial.print(servo1Us - neutralUs);
        Serial.print(", ");
        Serial.print(servo2Us - neutralUs);
        Serial.print(", ");
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

    servo1.writeMicroseconds(1500);
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