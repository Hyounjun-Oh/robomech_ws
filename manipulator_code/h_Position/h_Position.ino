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

/* Editor: Hyounjun Oh */
/* [수정사항]
2023. 01. 28. 다이나믹셀 ID1 과 ID2 동시제어 */

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

void setup() 
{
  Serial.begin(57600);
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
  if (joint_1_check == false || joint_2_check == false)
  {
    Serial.println(log);
    Serial.println("Failed to ping");
  }
  else
  {
    Serial.println("Succeeded to ping");
    Serial.print("joint_1 : ");
    Serial.println(joint_1);
    Serial.print("joint_2 : ");
    Serial.print(joint_2);
    Serial.print(" model_number : ");
    Serial.println(model_number);
  }

  joint_1_check = dxl_wb.jointMode(joint_1, 0, 0, &log);
  joint_2_check = dxl_wb.jointMode(joint_2, 0, 0, &log);
  if (joint_1_check == false || joint_2_check == false)
  {
    Serial.println(log);
    Serial.println("Failed to change joint mode");
  }
  else
  {
    Serial.println("Succeed to change joint mode");
    Serial.println("Dynamixel is moving...");

    for (int count = 0; count < 3; count++)
    {
      dxl_wb.goalPosition(joint_1, (int32_t)0);
      dxl_wb.goalPosition(joint_2, (int32_t)0);
      delay(3000);

      dxl_wb.goalPosition(joint_1, (int32_t)2022);
      dxl_wb.goalPosition(joint_2, (int32_t)2022);
      delay(3000);
    }
  }
}

void loop() 
{

}