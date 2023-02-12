const int motorDirPin = 8; // L298 Input 3, L9110 A-IA
const int motorPWMPin = 9; // L298 Input 4, L9110 A-IB

/* 
 *   모터 제어 함수
 *  아두이노와 L298의 연결을 위와 같이 했을 때
 *  방향은 dir이 high일 때 역방향, low일 때 순방향
 *  속도는 dir이 high일 때, 255는 정지, 0은 최고속도
 *  low일 때, 0은 정지, 255는 최고 속도
 */
void doMotor(bool dir, int vel){
  digitalWrite(motorDirPin, dir);
  analogWrite(motorPWMPin, (dir)?(255 - vel):vel); // dir이 high이면 255-vel, low이면 vel
}

void setup() {
  pinMode(motorDirPin, OUTPUT); // pinMode 설정은 digital만 필요하고 PWM을 사용할 analog는 필요 없음.
}

void loop() {
  // 시계방향으로 가속하다가 최고속도에서 다시 감속
  for(int i = 0; i < 255; i++){
    doMotor(HIGH, i); // (255-vel) 254 253 252 251 250 ... 1
    delay(10);
  }
  for(int i = 255; i >= 0; i--){
    doMotor(HIGH, i);  // (225-vel) 0 1 2 3 4 5 6 ... 254
    delay(10);
  }

  // 반시계방향으로 가속하다가 최고속도에서 다시 감속
  for(int i = 0; i < 255; i++){
    doMotor(LOW, i); // 1 2 3 4 5 6 ... 254
    delay(10);
  }
  for(int i = 255; i >= 0; i--){
    doMotor(LOW, i); // 255 254 253 252 251 ... 1
    delay(10);
  }
}


