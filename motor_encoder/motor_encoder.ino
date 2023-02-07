//////////////////////////////////////////////////////////////////////////////////
// 4 wheeled Mecanum mobile robot. 2023. HyounjunOh. Jeonbuk National Universuty.
// ohj_980918@naver.com
// For OpenCR.
// [Motor Spec]
// Enoder : 13 PPR
// Gear Ratio : 51
// Pulse : 663
// 4 : 2652 Pulse
//////////////////////////////////////////////////////////////////////////////////

// motor control pin
const int motorDirPin_1 = 2; 
const int motorPWMPin_1 = 3; 
const int motorDirPin_2 = 4; 
const int motorPWMPin_2 = 5; 

//아두이노 메가 인터럽트 핀 2, 3, 18, 19, 20, 21
// encoder pin
const int encoderPinA_1 = 18;
const int encoderPinB_1 = 19;
const int encoderPinA_2 = 20;
const int encoderPinB_2 = 21;

// encoder pose
int encoderPos_1 = 0;
int encoderPos_2 = 0;

float sampling_time = 0.1;

const float ratio = 360./51./13.;

// P control
//float Kp = 30;
float targetrpm[4] = {14, 14, 14, 14};// serial로 받아온 rpm값


//안변해도 됨
void doEncoderA_1(){  encoderPos_1 += (digitalRead(encoderPinA)==digitalRead(encoderPinB))?1:-1;}
void doEncoderB_1(){  encoderPos_1 += (digitalRead(encoderPinA)==digitalRead(encoderPinB))?-1:1;}
void doEncoderA_2(){  encoderPos_2 += (digitalRead(encoderPinA)==digitalRead(encoderPinB))?1:-1;}
void doEncoderB_2(){  encoderPos_2 += (digitalRead(encoderPinA)==digitalRead(encoderPinB))?-1:1;}

void doMotor(bool dir, int vel){
  digitalWrite(motorDirPin, dir);
  analogWrite(motorPWMPin, dir?(255 - vel):vel);
}

void setup() {
  //1번 모터 
  pinMode(encoderPinA_1, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA_1, CHANGE);
  pinMode(encoderPinB_1, INPUT_PULLUP);
  attachInterrupt(1, doEncoderB_1, CHANGE);
  pinMode(encoderPinA_2, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA_2, CHANGE);
  pinMode(encoderPinB_2, INPUT_PULLUP);
  attachInterrupt(1, doEncoderB_2, CHANGE);
 
  pinMode(motorDirPin_1, OUTPUT);
  pinMode(motorDirPin_2, OUTPUT);

  Serial.begin(57600);
  Serial2.begin(57600); // 아두이노2로 목표 rpm 데이터 전달목적
}

void loop() {
  float motorDeg = float(encoderPos)*ratio;
  float error = targetDeg - motorDeg;
  float control = Kp*error;

  doMotor( (control>=0)?HIGH:LOW, min(abs(control), 255));

  Serial.print("encoderPos : ");
  Serial.print(encoderPos);
  Serial.print("   motorDeg : ");
  Serial.print(float(encoderPos)*ratio);
  Serial.print("   error : ");
    Serial.print(error);
  Serial.print("    control : ");
  Serial.print(control);
  Serial.print("    motorVel : ");
  Serial.println(min(abs(control), 255));
}