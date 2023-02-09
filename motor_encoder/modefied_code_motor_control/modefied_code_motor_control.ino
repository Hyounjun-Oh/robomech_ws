/*
 * Author: Automatic Addison
 * Website: https://automaticaddison.com
 * Description: Calculate the angular velocity in radians/second of a DC motor
 * with a built-in encoder (forward = positive; reverse = negative) 
 */
 
// Motor encoder output pulses per 360 degree revolution (measured manually)
#include <MsTimer2.h>
#define ENC_COUNT_REV 1326
 
// Encoder output to Arduino Interrupt pin. Tracks the pulse count.
#define ENC_IN_1_A 18 //5
#define ENC_IN_2_A 20 //3
// Other encoder output to Arduino to keep track of wheel direction
// Tracks the direction of rotation.
#define ENC_IN_1_B 19 //4
#define ENC_IN_2_B 21 //2
#define MOT_DIR_PIN_1 4
#define MOT_PWM_PIN_1 5
#define MOT_DIR_PIN_2 6
#define MOT_PWM_PIN_2 7
 
// True = Forward; False = Reverse
boolean Direction_motor_1 = true;
boolean Direction_motor_2 = true;
// Keep track of the number of right wheel pulses
volatile long motor_1_pulse_count = 0;
volatile long motor_2_pulse_count = 0;
// One-second interval for measurements
int interval = 100;
  
// Counters for milliseconds during interval
long previousMillis = 0;
long currentMillis = 0;
 
// Variable for RPM measuerment
float rpm_motor_1 = 0;
float rpm_motor_2 = 0;
// Variable for angular velocity measurement
float ang_velocity_1 = 0;
float ang_velocity_1_deg = 0;
 
const float rpm_to_radians = 0.10471975512;
const float rad_to_deg = 57.29578;

float targetRPM[2] = {0,0};//serial로 받아온 rpm값
String slaveData;

void setup() {
 
  // Open the serial port at 9600 bps
  Serial.begin(115200);
  Serial2.begin(38400);
 
  // Set pin states of the encoder
  pinMode(ENC_IN_1_A , INPUT_PULLUP);
  pinMode(ENC_IN_1_B , INPUT);
  pinMode(ENC_IN_2_A , INPUT_PULLUP);
  pinMode(ENC_IN_2_B , INPUT);
 
  // Every time the pin goes high, this is a pulse
  attachInterrupt(digitalPinToInterrupt(ENC_IN_1_A), motor_1_pulse, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_IN_2_A), motor_2_pulse, RISING);

  MsTimer2::set(100, getRPM); //getRPM함수를 0.1초마다 실행
  MsTimer2::start();// 타이머 인터럽트 start
}
 
void loop() {
  if(Serial.available() > 0)
  {
    String inputStr = Serial.readStringUntil('\n');
    Split(inputStr,',');
  }
  float control_1 = 0;
  float control_2 = 0;

  doMotor(motorDirPin_1, motorPWMPin_1,(control_1>=0)?HIGH:LOW, min(abs(control_1), 255));
  doMotor(motorDirPin_2, motorPWMPin_2,(control_2>=0)?HIGH:LOW, min(abs(control_2), 255));
}
 
// Increment the number of pulses by 1
void motor_1_pulse() {
   
  // Read the value for the encoder for the right wheel
  int val = digitalRead(ENC_IN_1_B);
 
  if(val == LOW) {
    Direction_motor_1 = false; // Reverse
  }
  else {
    Direction_motor_1 = true; // Forward
  }
   
  if (Direction_motor_1) {
    motor_1_pulse_count++;
  }
  else {
    motor_1_pulse_count--;
  }
}
void motor_2_pulse() {
   
  // Read the value for the encoder for the right wheel
  int val = digitalRead(ENC_IN_2_B);
 
  if(val == LOW) {
    Direction_motor_2 = false; // Reverse
  }
  else {
    Direction_motor_2 = true; // Forward
  }
   
  if (Direction_motor_2) {
    motor_2_pulse_count++;
  }
  else {
    motor_2_pulse_count--;
  }
}
void doMotor(int motor_dir_pin, int motor_rpm_pin ,bool dir, int vel){
  digitalWrite(motor_dir_pin, dir);
  analogWrite(motor_rpm_pin, dir?(255 - vel):vel);
}
void getRPM(){
  rpm_motor_1 = (float)(motor_1_pulse_count * 60 / ENC_COUNT_REV);
  rpm_motor_2 = (float)(motor_2_pulse_count * 60 / ENC_COUNT_REV);
  ang_velocity_1 = rpm_motor_1 * rpm_to_radians;   
  ang_velocity_1_deg = ang_velocity_1 * rad_to_deg;
  ang_velocity_2 = rpm_motor_2 * rpm_to_radians;   
  ang_velocity_2_deg = ang_velocity_2 * rad_to_deg;
    
  Serial.print(" Motor 1 Pulses: ");
  Serial.println(motor_1_pulse_count);
  Serial.print(" Motor 2 Pulses: ");
  Serial.println(motor_2_pulse_count);
  Serial.print(" Motor 1 Speed: ");
  Serial.print(rpm_motor_1);
  Serial.print(" Motor 2 Speed: ");
  Serial.print(rpm_motor_2);
  Serial.println(" RPM");
  Serial.print(" Angular Velocity: ");
  Serial.print(rpm_motor_1);
  Serial.print(" rad per second");
  Serial.print("\t");
  Serial.print(ang_velocity_1_deg);
  Serial.println(" deg per second");
  Serial.println();

  motor_1_pulse_count = 0;
  motor_2_pulse_count = 0;
}
void Split(String sData, char cSeparator){	
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