# R9DS를 이용한 LED 제어

## **1. 📝개요**

- AT9S 조종기의 신호를 R9DS를 통해 받은 후, 이를 통해 아두이노에 연결된 RGB LED를 제어한다.
- AT9S는 FHSS로 R9DS에게 PWM 신호를 보낸다.
- PWM 신호를 받으면 아두이노는 PinChangeInterrupt를 통해 등록된 ISR를 실행한다.

## **2. 🎥 시연 영상**

[201901975 서종원 R9DS LED Control 과제](https://youtu.be/reNCDT40KNs)

## **3. 시스템 플로우**

<img src= "https://github.com/sjw2704/R9DS_LED_Control/blob/main/images/flow.png" width="700" />

## **4. 🕹️회로 구성**

<img src= "https://github.com/sjw2704/R9DS_LED_Control/blob/main/images/IMG_0626.jpg" width="500" />

| **PIN** | **아두이노 연결부** |
| --- | --- |
| **CH2_signal** | A1 |
| **CH3_signal** | A0 |
| **CH5_signal** | A2 |
| **R9DS VCC** | 5V |
| **R9DS GND** | GND |
| **3Colors_LED_R** | 9 |
| **3Colors_LED_G** | 10 |
| **3Colors_LED_B** | 11 |
| **3Colors_LED_GND** | GND |
| **On/Off LED** | 5 |
| **Level LED** | 6 |

## **5. 💻소프트웨어 개요**

### ISR

`void isrPowerSwitch()`  : CH5(SWA 스위치)의 PWM 폭을 계산한다.

`void isrColorControl()`  : CH3(쓰로틀 스틱)의 PWM 폭을 계산한다.

`void isrBrightness()`  : CH2(엘레베이터 스틱)의 PWM 폭을 계산한다.

### 사용자 정의 함수

`void powerOnOff(int pwmPower)`  : pwmPower가 기준값(1500)이상이면 LED를 켜고, 이하이면 끈다.

`void colorControl(int pwmColor, int brightness)`  : hue값을 RGB로 변환하여 출력한다.

`int brightnessControl(int pwmBright)`  : pwmBright값(1000~2000)을 0~255의 값으로 맵핑한다.

`void serialPrint(int pwmPower, int pwmColor, int pwmBright)`  : PWM값을 확인하기 위해 serial로 결과값을 출력한다. 

## **6. 주의 사항**

- R9DS에서 사용하는 모든 채널에 VCC,GND를 연결할 필요는 없습니다.
    - 원하는 채널 하나에만 연결하면 정상 작동합니다.
- SwA 스위치를 CH5로 사용하기 위해서는 menu의 Aux-CH에서 맵핑을 해야합니다.
