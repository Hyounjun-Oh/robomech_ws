// serial2arduino.py 수신 테스트: 3번째 조인트 값이 58이면 LED를 켠다.
const int LED_PIN = 13;
const int JOINT_COUNT = 4;

int32_t joint[JOINT_COUNT];

// data를 separator로 나눠 최대 max_count개의 정수를 out에 저장하고, 저장한 개수를 반환
int splitInts(const String &data, char separator, int32_t *out, int max_count)
{
  int count = 0;
  int start = 0;
  while (count < max_count)
  {
    int index = data.indexOf(separator, start);
    if (index == -1)
    {
      out[count++] = data.substring(start).toInt();
      break;
    }
    out[count++] = data.substring(start, index).toInt();
    start = index + 1;
  }
  return count;
}

void setup() {
  Serial.begin(57600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  if (Serial.available())
  {
    splitInts(Serial.readStringUntil('\n'), ',', joint, JOINT_COUNT);
    digitalWrite(LED_PIN, joint[2] == 58 ? HIGH : LOW);
  }
}
