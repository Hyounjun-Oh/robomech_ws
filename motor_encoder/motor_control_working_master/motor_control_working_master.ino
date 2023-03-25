#include <MsTimer2.h>

// motor control pin
const int motorDirPin_1 = 4; 
const int motorPWMPin_1 = 5; 
const int motorDirPin_2 = 6; 
const int motorPWMPin_2 = 7; 

//아두이노 메가 인터럽트 핀 2, 3, 18, 19, 20, 21
// encoder pin
const int encoderPinA_1 = 18; //5
const int encoderPinB_1 = 19; //4
const int encoderPinA_2 = 20; //3
const int encoderPinB_2 = 21; //2

// encoder pose
int encoderPos_1 = 0;
int encoderPos_2 = 0;

// encoder count
int encoderCount_1 = 0;
int encoderCount_2 = 0;

//intitalization
float sampling_time = 0.1;
int PPR = 1326;
const float ratio = 360./51./13.;

// P control
float Kp = 30;

float RPM_1 = 0.0;
float RPM_2 = 0.0;
int edge_1 = 0;
int edge_2 = 0;
float motorRPM_1 = 0;
float motorRPM_2 = 0;
//Target RPM
float targetRPM[2] = {100.0, 100.0};//serial로 받아온 rpm값
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
//void getRPM(){
//  edge_1 = encoderCount_1;
//  motorRPM_1 = (60*edge_1)/132.6;
//  
//  edge_2 = encoderCount_2;
//  motorRPM_2 = (60*edge_2)/132.6;
//
//  Serial.println("------------------------------");  
//  Serial.print("1 : ");
//  Serial.println(motorRPM_1);
//  Serial.print("2 : ");
//  Serial.println(motorRPM_2);
//  Serial.println("------------------------------");
//  encoderCount_1 = 0;
//  encoderCount_2 = 0;
//}


// void motorPID(){    //10ms마다 호출 

//     speedActualLeft = (leftMotorEncoder - leftMotorEncoderPrevPID)*60*(1000/LOOPTIME)/4200; 

// //속도를 RPM으로 측정하는 코드 : Encoder의 현재값에서 이전값을 뺀후, 측정 시간으로 나누고 Pulse 수 등을 사용해서 RPM 으로 변환한다.   LOOPTIME이 PID 제어가 동작하는 주기이다 (나의 경우는 10ms)  

//     leftMotorEncoderPrevPID = leftMotorEncoder;     //현재 엔코더 값을 다음에 사용하도록 저장
//     error = (speedRequestLeft) - (speedActualLeft);    //에러량을 계산
//     iTermLeft += (Ki * error);                                 //I 항의 경우 누적해야 하므로 별도로 저장
//     pidTerm = (Kp * error) + (Kd * (error - lastErrorLeft)) + iTermLeft; //전체 PID 값을 계산

//     lastErrorLeft = error;                                      //현재 에러 값을 다음에 사용하도록 저장
//     leftControl = constrain(abs(pidTerm), 0, 255) * ((pidTerm <0)?-1:1);   //PID 텀을 모터 제어에 사용되는 값의 범위인 0~255의 값으로 변환하는 함수
//     LeftMotor_PID( (leftControl>=0)? 1:0, abs(leftControl));   //실제 모터의 PWM 값으로 출력하는 함수

// }

void setup() {
  Serial.begin(38400); // From Python
  Serial2.begin(38400); // To slave Arduino
  //1번 모터 
  pinMode(encoderPinA_1, INPUT_PULLUP);
  attachInterrupt(5, doEncoderA_1, CHANGE);
  pinMode(encoderPinB_1, INPUT_PULLUP);
  attachInterrupt(4, doEncoderB_1, CHANGE);
  //2번 모터
  pinMode(encoderPinA_2, INPUT_PULLUP);
  attachInterrupt(3, doEncoderA_2, CHANGE);
  pinMode(encoderPinB_2, INPUT_PULLUP); 
  attachInterrupt(2, doEncoderB_2, CHANGE);
  //모터1, 모터2 direction핀
  pinMode(motorDirPin_1, OUTPUT);
  pinMode(motorDirPin_2, OUTPUT);

//  MsTimer2::set(100, getRPM); //getRPM함수를 0.1초마다 실행
//  MsTimer2::start();// 타이머 인터럽트 start
}

void loop() {
//  if(Serial.available() > 0)
//  {
//    String inputStr = Serial.readStringUntil('\n');
//    Split(inputStr,',');
//  }
  //float rpm_error_1 = targetRPM[0] - motorRPM_1;
  //float rpm_error_2 = targetRPM[1] - motorRPM_2;
  float control_1 = targetRPM[0];//Kp*rpm_error_1;
  float control_2 = targetRPM[1];//Kp*rpm_error_2;

  doMotor(motorDirPin_1, motorPWMPin_1,(control_1>=0)?HIGH:LOW, min(abs(control_1), 255));
  doMotor(motorDirPin_2, motorPWMPin_2,(control_2>=0)?HIGH:LOW, min(abs(control_2), 255));
  delay(1000);
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
      targetRPM[i] = sCopy.toFloat();
			break;
		}
 
		//다음 문자로~
		++nCount;
    ++i;
	}
 
}
