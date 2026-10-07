/*
 * Author: Automatic Addison
 * Website: https://automaticaddison.com
 * Description: Calculate the angular velocity in radians/second of a DC motor
 * with a built-in encoder (forward = positive; reverse = negative)
 *
 * [Serial 프로토콜] Python -> "cmd1,cmd2,cmd3,cmd4\n"
 *   앞의 2개는 이 보드의 모터 1, 2에 사용하고 나머지("cmd3,cmd4")는 Serial2로 slave에 전달한다.
 */
#include <MsTimer2.h>

#define MOTOR_COUNT 2

// Motor encoder output pulses per 360 degree revolution (measured manually)
const int ENC_COUNT_REV = 1326;

// Encoder output to Arduino Interrupt pin. Tracks the pulse count.
const int encoderPinA[MOTOR_COUNT] = {18, 20};
// Other encoder output to Arduino to keep track of wheel direction
const int encoderPinB[MOTOR_COUNT] = {19, 21};
const int motorDirPin[MOTOR_COUNT] = {2, 4};
const int motorPWMPin[MOTOR_COUNT] = {3, 5};

// RPM 측정 주기
const unsigned long RPM_INTERVAL_MS = 100;

const float rpm_to_radians = 0.10471975512;
const float rad_to_deg = 57.29578;

// Keep track of the number of wheel pulses (forward = +, reverse = -)
volatile long pulseCount[MOTOR_COUNT] = {0, 0};
// 타이머 인터럽트가 측정 주기마다 저장하는 펄스 수와 갱신 플래그
volatile long measuredPulses[MOTOR_COUNT] = {0, 0};
volatile bool rpmUpdated = false;

float targetRPM[MOTOR_COUNT] = {0, 0}; //serial로 받아온 rpm값

// B 채널이 LOW면 역방향, HIGH면 정방향
void countPulse(int m)
{
  pulseCount[m] += (digitalRead(encoderPinB[m]) == LOW) ? -1 : 1;
}
void motor_1_pulse() { countPulse(0); }
void motor_2_pulse() { countPulse(1); }

// 타이머 인터럽트: 측정 주기 동안의 펄스 수를 저장하고 카운터를 초기화
void sampleRPM()
{
  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    measuredPulses[m] = pulseCount[m];
    pulseCount[m] = 0;
  }
  rpmUpdated = true;
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

void printRPM(const long pulses[MOTOR_COUNT])
{
  float rpm[MOTOR_COUNT];
  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    rpm[m] = pulses[m] * (60000.0 / RPM_INTERVAL_MS) / ENC_COUNT_REV;
  }
  float ang_velocity_1 = rpm[0] * rpm_to_radians;
  float ang_velocity_1_deg = ang_velocity_1 * rad_to_deg;

  Serial.print(" Motor 1 Pulses: ");
  Serial.println(pulses[0]);
  Serial.print(" Motor 2 Pulses: ");
  Serial.println(pulses[1]);
  Serial.print(" Motor 1 Speed: ");
  Serial.print(rpm[0]);
  Serial.print(" Motor 2 Speed: ");
  Serial.print(rpm[1]);
  Serial.println(" RPM");
  Serial.print(" Angular Velocity: ");
  Serial.print(ang_velocity_1);
  Serial.print(" rad per second");
  Serial.print("\t");
  Serial.print(ang_velocity_1_deg);
  Serial.println(" deg per second");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);

  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    pinMode(encoderPinA[m], INPUT_PULLUP);
    pinMode(encoderPinB[m], INPUT);
    pinMode(motorDirPin[m], OUTPUT);
    pinMode(motorPWMPin[m], OUTPUT);
  }

  // Every time the pin goes high, this is a pulse
  attachInterrupt(digitalPinToInterrupt(encoderPinA[0]), motor_1_pulse, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderPinA[1]), motor_2_pulse, RISING);

  MsTimer2::set(RPM_INTERVAL_MS, sampleRPM);
  MsTimer2::start();
}

void loop() {
  if (Serial.available() > 0)
  {
    String slaveData = splitFloats(Serial.readStringUntil('\n'), ',', targetRPM, MOTOR_COUNT);
    if (slaveData.length() > 0)
    {
      Serial2.println(slaveData);
    }
  }

  if (rpmUpdated)
  {
    long pulses[MOTOR_COUNT];
    noInterrupts();
    for (int m = 0; m < MOTOR_COUNT; m++)
    {
      pulses[m] = measuredPulses[m];
    }
    rpmUpdated = false;
    interrupts();
    printRPM(pulses);
  }

  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    driveMotor(m, targetRPM[m]);
  }
}
