//////////////////////////////////////////////////////////////////////////////////
// 4 wheeled Mecanum mobile robot. 2023. HyounjunOh. Jeonbuk National Universuty.
// ohj_980918@naver.com
// For OpenCR.
// [Motor Spec]
// Enoder : 13 PPR
// Gear Ratio : 51
// Pulse : 663
// 4 : 2652 Pulse
// Master Arduino
//////////////////////////////////////////////////////////////////////////////////
#include <MsTimer2.h>

// motor control pin
const int motorDirPin_1 = 2; 
const int motorPWMPin_1 = 3; 
const int motorDirPin_2 = 4; 
const int motorPWMPin_2 = 5; 

//아두이노 메가 인터럽트 핀 2, 3, 18, 19, 20, 21
// encoder pin
const int encoderPin_count_1 = 2;
const int encoderPin_count_2 = 3;
const int encoderPinA_1 = 18;
const int encoderPinB_1 = 19;
const int encoderPinA_2 = 20;
const int encoderPinB_2 = 21;

// encoder pose
int encoderPos_1 = 0;
int encoderPos_2 = 0;

// encoder count
int encoderCount_1 = 0;
int encoderCount_2 = 0;

//intitalization
float sampling_time = 0.1;
int chebae = 2;
int PPR = 13;
const float ratio = 360./51./13.;

// P control
float Kp = 30;

//Target RPM
float targetRPM[1] = {0};//serial로 받아온 rpm값
String slaveData;

// Inturrupt Function 체배 감지되면 카운트 업.
void doEncoderA_1(){  encoderPos_1 += (digitalRead(encoderPinA_1)==digitalRead(encoderPinB_1))?1:-1;encoderCount_1 += 1;}
void doEncoderB_1(){  encoderPos_1 += (digitalRead(encoderPinA_1)==digitalRead(encoderPinB_1))?-1:1;encoderCount_1 += 1;}
void doEncoderA_2(){  encoderPos_2 += (digitalRead(encoderPinA_2)==digitalRead(encoderPinB_2))?1:-1;encoderCount_2 += 1;}
void doEncoderB_2(){  encoderPos_2 += (digitalRead(encoderPinA_2)==digitalRead(encoderPinB_2))?-1:1;encoderCount_2 += 1;}

void doMotor(int motor_dir_pin, int motor_rpm_pin ,bool dir, int vel){
  digitalWrite(motor_dir_pin, dir);
  analogWrite(motor_rpm_pin, dir?(255 - vel):vel);
}

// getPRM1과 2로 나누어서 각각 타이머인터럽트 활성화 하기.
float getRPM_1(){
  int edge = encoderCount_1;
  float RPM = (60*edge)/(sampling_time * PPR * chebae);
  encoderCount_1 = 0;  
  return RPM;
}

float getRPM_2(){
  int edge = encoderCount_2;
  float RPM = (60*edge)/(sampling_time * PPR * chebae);
  encoderCount_2 = 0;
  return RPM;
}

void setup() {
  Serial.begin(38400); // From Python
  Serial2.begin(38400); // To slave Arduino
  //1번 모터 
  pinMode(encoderPinA_1, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA_1, CHANGE);
  pinMode(encoderPinB_1, INPUT_PULLUP);
  attachInterrupt(1, doEncoderB_1, CHANGE);
  //2번 모터
  pinMode(encoderPinA_2, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA_2, CHANGE);
  pinMode(encoderPinB_2, INPUT_PULLUP);
  attachInterrupt(1, doEncoderB_2, CHANGE);
  //모터1, 모터2 direction핀
  pinMode(motorDirPin_1, OUTPUT);
  pinMode(motorDirPin_2, OUTPUT);
  //타이머 인터럽트
  MsTimer2::set(100, getRPM_1); //getRPM함수를 0.1초마다 실행
  MsTimer2::start();// 타이머 인터럽트 start
  MsTimer2::set(100, getRPM_2); //getRPM함수를 0.1초마다 실행
  MsTimer2::start();// 타이머 인터럽트 start
}

void loop() {

  if(Serial.available() > 0)
  {
    String inputStr = Serial.readStringUntil('\n');
    Split(inputStr,',');
  }

  float motorRPM_1 = getRPM_1();
  float motorRPM_2 = getRPM_2();
  float rpm_error_1 = targetRPM[0] - motorRPM_1;
  float rpm_error_2 = targetRPM[1] - motorRPM_2;
  float control_1 = Kp*rpm_error_1;
  float control_2 = Kp*rpm_error_2;

  doMotor(motorDirPin_1, motorPWMPin_1,(control_1>=0)?HIGH:LOW, min(abs(control_1), 255));
  doMotor(motorDirPin_2, motorPWMPin_2,(control_2>=0)?HIGH:LOW, min(abs(control_2), 255));
  //Serial.print("encoderPos : ");
  //Serial.print(encoderPos);
  //Serial.print("   motorDeg : ");
  //Serial.print(float(encoderPos)*ratio);
  //Serial.print("   error : ");
  //Serial.print(error);
  //Serial.print("    control : ");
  //Serial.print(control);
  //Serial.print("    motorVel : ");
  //Serial.println(min(abs(control), 255));
}

void Split(String sData, char cSeparator)
{	
	int nCount = 0;
	int nGetIndex = 0 ;
 
	//임시저장
	String sTemp = "";
 
	//원본 복사
	String sCopy = sData;
  int i = 0;
  int j = 0;
	while(true)
	{
		//구분자 찾기
		nGetIndex = sCopy.indexOf(cSeparator);
 
		//리턴된 인덱스가 있나?
		if((-1 != nGetIndex) || (j>1))
		{
			//있다.
      j+=1;
			//데이터 넣고
			sTemp = sCopy.substring(0, nGetIndex);
      targetRPM[i] = sTemp.toFloat();
		
			//뺀 데이터 만큼 잘라낸다.
			sCopy = sCopy.substring(nGetIndex + 1);
		}
		else
		{
			//없으면 마무리 한다.
      slaveData = sCopy;
      Serial2.print(slaveData);
			break;
		}
 
		//다음 문자로~
		++nCount;
    ++i;
	}
 
}