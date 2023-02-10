/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Taehun Lim (Darby) */

/* Editor: Hyounjun Oh 
   E-mail : ohj_980918@naver.com
   Jeonbuk-National University*/
/* [수정사항]
2023. 01. 28. 다이나믹셀 ID1 과 ID2 동시제어 
2023. 02. 05. 다이나믹셀 ID1 ~ ID4 사다리꼴 프로파일 적용. 제어완료.
2023. 02. 06. 조인트 값 파이썬으로부터 Serial2 포트로 받아오기.*/


#include <DynamixelWorkbench.h>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif   
#define BAUDRATE  57600
#define JOINT_1    1 //DXL_ID
#define JOINT_2    2
#define JOINT_3    3
#define JOINT_4    4

DynamixelWorkbench dxl_wb;

int joint[3]; //조인트 값 담을 배열 선언.
int32_t get_position[3];
int32_t get_desired_position[3];
int home_position[4] = {2048,2400,1300,2500};
// int home_position[4] = {2048,2048,2048,2048};
int desired_position[4] = {3000,2300,1500,2300};
int desired_position_2[4] = {3000,2048,2048,1024};

void setup() 
{
  Serial.begin(57600); //다이나믹셀 제어용 UART
  Serial2.begin(57600);
  
  //Serial.begin(9600); //Joint값 받는 UART

  // while(!Serial); // Wait for Opening Serial Monitor

  const char *log;
  bool result = false;
  bool joint_1_check = false; //조인트 상태 체크
  bool joint_2_check = false;
  bool joint_3_check = false;
  bool joint_4_check = false;

  uint8_t joint_1 = JOINT_1;
  uint8_t joint_2 = JOINT_2;
  uint8_t joint_3 = JOINT_3;
  uint8_t joint_4 = JOINT_4;
  uint16_t model_number = 0;

  // 모터 상태 초기화.

  result = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to init");
  }
  else
  {
    Serial.print("Succeeded to init : ");
    Serial.println(BAUDRATE);  
  }

  joint_1_check = dxl_wb.ping(joint_1, &model_number, &log);
  joint_2_check = dxl_wb.ping(joint_2, &model_number, &log);
  joint_3_check = dxl_wb.ping(joint_3, &model_number, &log);
  joint_4_check = dxl_wb.ping(joint_4, &model_number, &log);
  if (joint_1_check == false || joint_2_check == false || joint_3_check == false || joint_4_check == false)
  {
    Serial.println(log);
    Serial.println("Failed to ping");
  }
  else
  {
    Serial.println("Succeeded to ping");
  }

  joint_1_check = dxl_wb.jointMode(joint_1, 0, 0, &log);
  joint_2_check = dxl_wb.jointMode(joint_2, 0, 0, &log);
  joint_3_check = dxl_wb.jointMode(joint_3, 0, 0, &log);
  joint_4_check = dxl_wb.jointMode(joint_4, 0, 0, &log);
  if (joint_1_check == false || joint_2_check == false || joint_3_check == false || joint_4_check == false)
  {
    Serial.println(log);
    Serial.println("Failed to change joint mode");
  }
  else
  {
    Serial.println("Succeed to change joint mode");
    Serial.println("Dynamixel is moving...");
  }
  dxl_wb.itemWrite(JOINT_1, "Drive_Mode", 4); //DriveMode Profile Configuration 정의 
  dxl_wb.itemWrite(JOINT_1, "Profile_Acceleration", 4989);
  dxl_wb.itemWrite(JOINT_1, "Profile_Velocity", 3506);

  dxl_wb.itemWrite(JOINT_2, "Drive_Mode", 4); //DriveMode Profile Configuration 정의 
  dxl_wb.itemWrite(JOINT_2, "Profile_Acceleration", 4989);
  dxl_wb.itemWrite(JOINT_2, "Profile_Velocity", 3506); 

  dxl_wb.itemWrite(JOINT_3, "Drive_Mode", 4); //DriveMode Profile Configuration 정의 
  dxl_wb.itemWrite(JOINT_3, "Profile_Acceleration", 4989);
  dxl_wb.itemWrite(JOINT_3, "Profile_Velocity", 3506); 

  dxl_wb.itemWrite(JOINT_4, "Drive_Mode", 4); //DriveMode Profile Configuration 정의 
  dxl_wb.itemWrite(JOINT_4, "Profile_Acceleration", 4989);
  dxl_wb.itemWrite(JOINT_4, "Profile_Velocity", 3506); 
}

void loop() 
{
  // if (Serial2.available())
  // {
  //   String inputStr = Serial2.readStringUntil('\n');
  //   Split(inputStr,',');
  //   if (joint[2] == 58){
  //     digitalWrite(13, HIGH);
  //   }else{
  //     digitalWrite(13, LOW);
  //   };

  char input_data;
  while(Serial.available())
  {
    input_data = Serial.read();
  }

  // Serial.println("----------------------------------------"); 
  // Serial.println("Mode : From Python");    
  // dxl_wb.goalPosition(JOINT_1, joint[0]);
  // dxl_wb.goalPosition(JOINT_2, joint[1]);
  // dxl_wb.goalPosition(JOINT_3, joint[2]);
  // dxl_wb.goalPosition(JOINT_4, joint[3]);
  // int delay_joint = maximum_calculation();
  // delay_motor(delay_joint);
  // Serial.println("complete");
  if(input_data == '3')
  {
    Serial.println("----------------------------------------"); 
    Serial.println("Mode : 1");    
    dxl_wb.goalPosition(JOINT_1, desired_position_2[0]);
    dxl_wb.goalPosition(JOINT_2, desired_position_2[1]);
    dxl_wb.goalPosition(JOINT_3, desired_position_2[2]);
    dxl_wb.goalPosition(JOINT_4, desired_position_2[3]);
    //int delay_joint = maximum_calculation();
    //delay_motor(delay_joint);
    delay(5000);
    Serial.println("complete");
  }
  if(input_data == '2')
  {
    Serial.println("----------------------------------------"); 
    Serial.println("Mode : 1");    
    dxl_wb.goalPosition(JOINT_1, desired_position[0]);
    dxl_wb.goalPosition(JOINT_2, desired_position[1]);
    dxl_wb.goalPosition(JOINT_3, desired_position[2]);
    dxl_wb.goalPosition(JOINT_4, desired_position[3]);
    //int delay_joint = maximum_calculation();
    //delay_motor(delay_joint);
    delay(5000);
    Serial.println("complete");
  }
  if(input_data == '1')
  {
    Serial.println("----------------------------------------"); 
    Serial.println("Mode : Home Position");     
    
    dxl_wb.goalPosition(JOINT_2, home_position[1]);
    dxl_wb.goalPosition(JOINT_1, home_position[0]);
    dxl_wb.goalPosition(JOINT_3, home_position[2]);
    dxl_wb.goalPosition(JOINT_4, home_position[3]);
    //int delay_joint = maximum_calculation();
    //delay_motor(delay_joint);
    delay(5000);
    Serial.println("complete");
  }
  //}
}

void delay_motor(int joint_ID){
  while ((abs(get_desired_position[joint_ID-1] - get_position[joint_ID-1])) > 1){
    dxl_wb.itemRead(joint_ID, "Goal_Position", &get_desired_position[joint_ID-1]);
    dxl_wb.itemRead(joint_ID, "Present_Position", &get_position[joint_ID-1]);
    delay(1);
  };
};

int maximum_calculation(){
  dxl_wb.itemRead(JOINT_1, "Goal_Position", &get_desired_position[0]);
  dxl_wb.itemRead(JOINT_1, "Present_Position", &get_position[0]);
  dxl_wb.itemRead(JOINT_2, "Goal_Position", &get_desired_position[1]);
  dxl_wb.itemRead(JOINT_2, "Present_Position", &get_position[1]);
  dxl_wb.itemRead(JOINT_3, "Goal_Position", &get_desired_position[2]);
  dxl_wb.itemRead(JOINT_3, "Present_Position", &get_position[2]);
  dxl_wb.itemRead(JOINT_4, "Goal_Position", &get_desired_position[3]);
  dxl_wb.itemRead(JOINT_4, "Present_Position", &get_position[3]);
  Serial.print("desired position : [");
  Serial.print(get_desired_position[0]);
  Serial.print(", ");
  Serial.print(get_desired_position[1]);
  Serial.print(", ");
  Serial.print(get_desired_position[2]);
  Serial.print(", ");
  Serial.print(get_desired_position[3]);
  Serial.println("]");

  Serial.print("present position : [");
  Serial.print(get_position[0]);
  Serial.print(", ");
  Serial.print(get_position[1]);
  Serial.print(", ");
  Serial.print(get_position[2]);
  Serial.print(", ");
  Serial.print(get_position[3]);
  Serial.println("]");
  int arr[4] = {abs(get_desired_position[0]-get_position[0]),abs(get_desired_position[1]-get_position[1]),abs(get_desired_position[2]-get_position[2]),abs(get_desired_position[3]-get_position[3])};
  int max = 0;
  int index = 0;
  for(int i = 0;i<4;i++)
  {
    if(arr[i] > max ){
      max = arr[i];
      index = i+1;
    };

  };
  if (index == 0){index = 1;};
  Serial.println(max);
  Serial.println(index);
  return index;
};

void Split(String sData, char cSeparator)
{	
	int nCount = 0;
	int nGetIndex = 0 ;
 
	//임시저장
	String sTemp = "";
 
	//원본 복사
	String sCopy = sData;
  int i = 0;
	while(true)
	{
		//구분자 찾기
		nGetIndex = sCopy.indexOf(cSeparator);
 
		//리턴된 인덱스가 있나?
		if(-1 != nGetIndex)
		{
			//있다.
 
			//데이터 넣고
			sTemp = sCopy.substring(0, nGetIndex);
      joint[i] = sTemp.toInt();
		
			//뺀 데이터 만큼 잘라낸다.
			sCopy = sCopy.substring(nGetIndex + 1);
		}
		else
		{
			//없으면 마무리 한다.
      joint[i] = sCopy.toInt();
			break;
		}
 
		//다음 문자로~
		++nCount;
    ++i;
	}
 
}