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

//Motor Control
const int motor1_PWMPinA = 2;
const int motor1_PWMPinB = 3;
const int motor2_PWMPinA = 4;
const int motor2_PWMPinB = 5;
const int motor3_PWMPinA = 6;
const int motor3_PWMPinB = 7;
const int motor4_PWMPinA = 8;
const int motor4_PWMPinB = 9;
//Encoder Control
const int encoder_1_pose = 0;
const int encoder_2_pose = 0;
const int encoder_3_pose = 0;
const int encoder_4_pose = 0;

const int motor1_Ecd_A = 10;
const int motor1_Ecd_B = 11;

const int motor2_Ecd_A = 12;
const int motor2_Ecd_B = 13;

const int motor3_Ecd_A = 14;
const int motor3_Ecd_B = 15;

const int motor4_Ecd_A = 16;
const int motor4_Ecd_B = 17;

const float ratio = 360./51./13.;

// P control
float Kp = 30;

// Interrupt Function
void doEncoderA1(){encoderPos += (digitalRead(motor1_Ecd_A)==digitalRead(motor1_Ecd_B))?1:-1;}
void doEncoderB1(){encoderPos += (digitalRead(motor1_Ecd_A)==digitalRead(motor1_Ecd_B))?-1:1;}

void doEncoderA2(){encoderPos += (digitalRead(motor2_Ecd_A)==digitalRead(motor2_Ecd_B))?1:-1;}
void doEncoderB2(){encoderPos += (digitalRead(motor2_Ecd_A)==digitalRead(motor2_Ecd_B))?-1:1;}

void doEncoderA3(){encoderPos += (digitalRead(motor3_Ecd_A)==digitalRead(motor3_Ecd_B))?1:-1;}
void doEncoderB3(){encoderPos += (digitalRead(motor3_Ecd_A)==digitalRead(motor3_Ecd_B))?-1:1;}

void doEncoderA4(){encoderPos += (digitalRead(motor4_Ecd_A)==digitalRead(motor4_Ecd_B))?1:-1;}
void doEncoderB4(){encoderPos += (digitalRead(motor4_Ecd_A)==digitalRead(motor4_Ecd_B))?-1:1;}

void doMotor(bool dir, int vel){
  digitalWrite(motorDirPin, dir);
  analogWrite(motorPWMPin, dir?(255 - vel):vel);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1_Ecd_A, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA1, CHANGE);
  pinMode(motor1_Ecd_B, INPUT_PULLUP);
  attachInterrupt(1,  doEncoderB1, CHANGE);
  pinMode(motor1_DirPin, OUTPUT);

  pinMode(motor2_Ecd_A, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA1, CHANGE);
  pinMode(motor2_Ecd_B, INPUT_PULLUP);
  attachInterrupt(1,  doEncoderB1, CHANGE);
  pinMode(motor2_DirPin, OUTPUT);

  pinMode(motor3_Ecd_A, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA1, CHANGE);
  pinMode(motor3_Ecd_B, INPUT_PULLUP);
  attachInterrupt(1,  doEncoderB1, CHANGE);
  pinMode(motor3_DirPin, OUTPUT);

  pinMode(motor4_Ecd_A, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA1, CHANGE);
  pinMode(motor4_Ecd_B, INPUT_PULLUP);
  attachInterrupt(1,  doEncoderB1, CHANGE);
  pinMode(motor4_DirPin, OUTPUT);

  Serial.begin(9600);

}
// 수정 필요!! 23.01.15
void loop() {
  // put your main code here, to run repeatedly:
  float targetDeg1 = 360;

  float motorDeg1 = float(encoder_1_pose)*ratio;
  float motorDeg2 = float(encoder_2_pose)*ratio;
  float motorDeg3 = float(encoder_3_pose)*ratio;
  float motorDeg4 = float(encoder_4_pose)*ratio;      
 
  float error1 = targetDeg1 - motorDeg1;
  float control = Kp*error1;

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
