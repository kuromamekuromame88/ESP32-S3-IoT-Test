#include <Arduino.h>

#define PWM_PIN 5
#define FREQUENCY 1000

/*
int state = false;

void ledon(int level){
  state = true;
  level = level?level:255;
  ledcWrite(PWM_PIN, level);
}

void ledoff(int level){
  state = false;
  ledcWrite(PWM_PIN, 0);
}

bool getstate(){
  return state;
}*/

void setup(){
  //ピンの設定
  pinMode(PWM_PIN, OUTPUT);

  //PWM設定（ピン番号, 周波数, 分解能）
  //ledcAttach(PWM_PIN, FREQUENCY, 8);
}

void loop(){
  digitalWrite(PWM_PIN, LOW);
  //ledcWrite(PWM_PIN, 0);
  delay(1000);
  digitalWrite(PWM_PIN, HIGH);
  //ledcWrite(PWM_PIN, 255);
  delay(1000);
}