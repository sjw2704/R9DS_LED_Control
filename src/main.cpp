#include <Arduino.h>
#include "PinChangeInterrupt.h"

// — ISR 프로토타입 —
void isrPowerSwitch();
void isrColorControl();
void isrBrightness();

// - 함수 -
void powerOnOff();
void colorControl();
void brightnessControl();
void serialPrint();

// — 수신기 채널 입력 핀 정의 —
// A2: CH5(AUX1) ← SwA 스위치 (전원 ON/OFF)
// A0: CH3 ← 스로틀 스틱 (색상 제어, 서서히 변화)
// A1: CH2 ← 엘리베이터 스틱 (밝기 조절)
const int pinPowerSwitch = A2;
const int pinColorCtrl   = A0;
const int pinBrightness  = A1;

// — PWM 신호 저장 변수 —
// PWM 폭 (0~2000us)
volatile int           powerPulseWidth   = 1500; // PWM 미신호시 기본값
volatile unsigned long powerStartMicros  = 0;
volatile bool          newPowerPulse     = false;

volatile int           colorPulseWidth   = 1500; // PWM 미신호시 기본값
volatile unsigned long colorStartMicros  = 0;
volatile bool          newColorPulse     = false;

volatile int           brightPulseWidth  = 1500; // PWM 미신호시 기본값
volatile unsigned long brightStartMicros = 0;
volatile bool          newBrightPulse    = false;

int pwmPower;
int pwmColor;
int pwmBright;

// — RGB LED 핀 (common-cathode) —
const int pinR = 9;
const int pinG = 10;
const int pinB = 11;
const int OnOffLED = 6;
const int BrightnessLED = 5;
int r, g, b;

void setup() {
  // SwA (CH5) ISR 등록 → 전원 ON/OFF
  pinMode(pinPowerSwitch, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(pinPowerSwitch), isrPowerSwitch, CHANGE);
  
  // CH3 (색상 제어) ISR 등록 → hue 제어
  pinMode(pinColorCtrl, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(pinColorCtrl), isrColorControl, CHANGE);

  // CH2 (밝기) ISR 등록
  pinMode(pinBrightness, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(pinBrightness), isrBrightness, CHANGE);

  // RGB LED 핀 출력 설정
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(OnOffLED, OUTPUT);
  pinMode(BrightnessLED, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  if (newPowerPulse) {
    pwmPower  = powerPulseWidth; 
    powerOnOff();
    newPowerPulse = false;
  }
  if (newColorPulse) {
    pwmColor  = colorPulseWidth;   
    colorControl();
    newColorPulse = false;
  }
  if (newBrightPulse) {
    pwmBright = brightPulseWidth; 
    brightnessControl();
    newBrightPulse = false;
  }
  // 시리얼 디버그
  serialPrint();
}

// — ISR 정의 —

// SwA (CH5) → 전원 제어
void isrPowerSwitch() {
  if (digitalRead(pinPowerSwitch) == HIGH) { // HIGH: 일 때, 시간 측정
    powerStartMicros = micros();
  } else {  // LOW: 일 때, PWM 폭 측정
    powerPulseWidth   = micros() - powerStartMicros;
    powerStartMicros  = 0;  // PWM 폭 초기화
    newPowerPulse     = true; // 시리얼 디버그용
  }
}

// CH3 → 색상 제어 (hue)
void isrColorControl() {
  if (digitalRead(pinColorCtrl) == HIGH) {
    colorStartMicros = micros();
  } else if (colorStartMicros && !newColorPulse) {
    colorPulseWidth  = micros() - colorStartMicros;
    colorStartMicros = 0;
    newColorPulse    = true;
  }
}

// CH2 → 밝기
void isrBrightness() {
  if (digitalRead(pinBrightness) == HIGH) {
    brightStartMicros = micros();
  } else if (brightStartMicros && !newBrightPulse) {
    brightPulseWidth  = micros() - brightStartMicros;
    brightStartMicros = 0;
    newBrightPulse    = true;
  }
} 

//- 함수 정의 -
void brightnessControl(){
  // CH2 → 밝기(0~255)
  int brightness = map(pwmBright, 1000, 2000, 0, 255);
  brightness = constrain(brightness, 0, 255); // 만약 0~255 범위를 벗어나면 0 또는 255로 고정
  analogWrite(BrightnessLED, brightness); // 밝기 LED 제어
  return;
}

void powerOnOff() {
  if (pwmPower < 1500) { // 스위치가 내려가면 LED OFF
    digitalWrite(OnOffLED, LOW);
  }
  else{// LED 출력
    digitalWrite(OnOffLED, HIGH);
  }
  return;
}

void colorControl() {
    // CH3 → hue (0~255)
    int hue = map(pwmColor, 1000, 2000, 0, 255);
    hue = constrain(hue, 0, 255);
  
    // HSV → RGB 변환 (V=255)
    if (hue < 85) {
      // 0..84 : red→green
      r = 255 - hue * 3;
      g = hue * 3;
      b = 0;
    } else if (hue < 170) {
      // 85..169 : green→blue
      int h2 = hue - 85;
      r = 0;
      g = 255 - h2 * 3;
      b = h2 * 3;
    } else {
      // 170..255 : blue→red
      int h2 = hue - 170;
      r = h2 * 3;
      g = 0;
      b = 255 - h2 * 3;
    }
  
    analogWrite(pinR, r); // R
    analogWrite(pinG, g); // G
    analogWrite(pinB, b); // B
}  

void serialPrint(){
    // (선택) 시리얼 디버그
    if (newPowerPulse) {
      Serial.print("Power PWM: ");  Serial.println(pwmPower);
    }
    if (newColorPulse) {
      Serial.print("Color PWM: ");  Serial.println(pwmColor);
    }
    if (newBrightPulse) {
      Serial.print("Bright PWM: "); Serial.println(pwmBright);
    }
}
