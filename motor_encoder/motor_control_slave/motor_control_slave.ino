//////////////////////////////////////////////////////////////////////////////////
// 4 wheeled Mecanum mobile robot. 2023. HyounjunOh. Jeonbuk National Universuty.
// ohj_980918@naver.com
// For OpenCR.
// [Motor Spec]
// Enoder : 13 PPR
// Gear Ratio : 51
// Pulse : 663
// 4 : 2652 Pulse
// slave Arduino
// [Serial 프로토콜] master -> Serial2 "rpm1,rpm2\n"
//////////////////////////////////////////////////////////////////////////////////
#include <MsTimer2.h>

#define MOTOR_COUNT 2

// motor control pin
const int motorDirPin[MOTOR_COUNT] = {2, 4};
const int motorPWMPin[MOTOR_COUNT] = {3, 5};

// encoder pin (아두이노 메가 인터럽트 핀 2, 3, 18, 19, 20, 21)
const int encoderPinA[MOTOR_COUNT] = {18, 20};
const int encoderPinB[MOTOR_COUNT] = {19, 21};

// encoder pose / count (ISR에서 갱신)
volatile long encoderPos[MOTOR_COUNT] = {0, 0};
volatile int encoderCount[MOTOR_COUNT] = {0, 0};

// RPM 계산
const unsigned long SAMPLING_MS = 100;
const float SAMPLING_TIME = SAMPLING_MS / 1000.0;
const int CHEBAE = 2;
const int PPR = 13;
volatile float motorRPM[MOTOR_COUNT] = {0, 0};

// P control
const float Kp = 30;

//Target RPM
float targetRPM[MOTOR_COUNT] = {0, 0}; //serial로 받아온 rpm값

// Inturrupt Function 체배 감지되면 카운트 업.
void onEncoderEdge(int m, bool channelA)
{
  bool same = digitalRead(encoderPinA[m]) == digitalRead(encoderPinB[m]);
  encoderPos[m] += (same == channelA) ? 1 : -1;
  encoderCount[m] += 1;
}
void doEncoderA_1() { onEncoderEdge(0, true); }
void doEncoderB_1() { onEncoderEdge(0, false); }
void doEncoderA_2() { onEncoderEdge(1, true); }
void doEncoderB_2() { onEncoderEdge(1, false); }

// 타이머 인터럽트: SAMPLING_MS 동안 들어온 엣지 수로 RPM 계산
void updateRPM()
{
  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    motorRPM[m] = (60.0 * encoderCount[m]) / (SAMPLING_TIME * PPR * CHEBAE);
    encoderCount[m] = 0;
  }
}

// command의 부호로 방향, 크기(최대 255)로 PWM을 정한다.
void driveMotor(int m, float command)
{
  bool dir = command >= 0;
  int vel = min(abs(command), 255);
  digitalWrite(motorDirPin[m], dir);
  analogWrite(motorPWMPin[m], dir ? (255 - vel) : vel);
}

// data를 separator로 나눠 앞의 최대 max_count개를 out에 저장하고, 남은 문자열을 반환
String splitFloats(const String &data, char separator, float *out, int max_count)
{
  int start = 0;
  for (int count = 0; count < max_count; count++)
  {
    int index = data.indexOf(separator, start);
    if (index == -1)
    {
      out[count] = data.substring(start).toFloat();
      return "";
    }
    out[count] = data.substring(start, index).toFloat();
    start = index + 1;
  }
  return data.substring(start);
}

void setup() {
  Serial2.begin(38400); // From Master Arduino

  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    pinMode(encoderPinA[m], INPUT_PULLUP);
    pinMode(encoderPinB[m], INPUT_PULLUP);
    pinMode(motorDirPin[m], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(encoderPinA[0]), doEncoderA_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB[0]), doEncoderB_1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinA[1]), doEncoderA_2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB[1]), doEncoderB_2, CHANGE);

  MsTimer2::set(SAMPLING_MS, updateRPM);
  MsTimer2::start();
}

void loop() {
  if (Serial2.available() > 0)
  {
    String inputStr = Serial2.readStringUntil('\n');
    splitFloats(inputStr, ',', targetRPM, MOTOR_COUNT);
  }

  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    noInterrupts();
    float rpm = motorRPM[m];
    interrupts();
    driveMotor(m, Kp * (targetRPM[m] - rpm));
  }
}
