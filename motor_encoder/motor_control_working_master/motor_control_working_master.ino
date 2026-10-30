// 엔코더 모터 2개 개루프 PWM 구동 (master Arduino 동작 확인용)
#define MOTOR_COUNT 2

// true: Python에서 "cmd1,cmd2\n"을 받아 구동, false: DEFAULT_COMMAND로 구동
const bool USE_SERIAL_COMMAND = false;
const float DEFAULT_COMMAND[MOTOR_COUNT] = {100.0, 100.0};
const unsigned long LOOP_DELAY_MS = 1000;

// motor control pin
const int motorDirPin[MOTOR_COUNT] = {4, 6};
const int motorPWMPin[MOTOR_COUNT] = {5, 7};

// encoder pin (아두이노 메가 인터럽트 핀 2, 3, 18, 19, 20, 21)
const int encoderPinA[MOTOR_COUNT] = {18, 20};
const int encoderPinB[MOTOR_COUNT] = {19, 21};

// encoder pose / count (ISR에서 갱신)
volatile long encoderPos[MOTOR_COUNT] = {0, 0};
volatile int encoderCount[MOTOR_COUNT] = {0, 0};

//Target RPM
float targetRPM[MOTOR_COUNT] = {DEFAULT_COMMAND[0], DEFAULT_COMMAND[1]}; //serial로 받아온 rpm값

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
  Serial.begin(38400); // From Python
  Serial2.begin(38400); // To slave Arduino

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
}

void loop() {
  if (USE_SERIAL_COMMAND && Serial.available() > 0)
  {
    splitFloats(Serial.readStringUntil('\n'), ',', targetRPM, MOTOR_COUNT);
  }

  for (int m = 0; m < MOTOR_COUNT; m++)
  {
    driveMotor(m, targetRPM[m]);
  }
  delay(LOOP_DELAY_MS);
}
