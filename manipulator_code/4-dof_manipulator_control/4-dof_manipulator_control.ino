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
#define JOINT_COUNT 4

const uint8_t JOINT_IDS[JOINT_COUNT] = {1, 2, 3, 4}; //DXL_ID

// 사다리꼴 속도 프로파일 설정
const int32_t DRIVE_MODE_TIME_BASED = 4; //DriveMode Profile Configuration 정의
const int32_t PROFILE_ACCELERATION = 4989;
const int32_t PROFILE_VELOCITY = 3506;
const unsigned long MOVE_WAIT_MS = 5000;

const int32_t home_position[JOINT_COUNT] = {2048, 2400, 1300, 2500};
const int32_t desired_position[JOINT_COUNT] = {3000, 2300, 1500, 2300};
const int32_t desired_position_2[JOINT_COUNT] = {3000, 2048, 2048, 1024};

DynamixelWorkbench dxl_wb;

int32_t joint[JOINT_COUNT]; //파이썬으로부터 받은 조인트 값
int32_t get_position[JOINT_COUNT];
int32_t get_desired_position[JOINT_COUNT];

void printResult(bool ok, const char *log, const char *fail_msg, const char *success_msg)
{
  if (ok)
  {
    Serial.println(success_msg);
  }
  else
  {
    Serial.println(log);
    Serial.println(fail_msg);
  }
}

void setup()
{
  Serial.begin(57600); //다이나믹셀 제어용 UART
  Serial2.begin(57600); //Joint값 받는 UART

  const char *log = NULL;
  uint16_t model_number = 0;

  // 모터 상태 초기화.
  bool result = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log);
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

  result = true;
  for (int i = 0; i < JOINT_COUNT; i++)
    result &= dxl_wb.ping(JOINT_IDS[i], &model_number, &log);
  printResult(result, log, "Failed to ping", "Succeeded to ping");

  result = true;
  for (int i = 0; i < JOINT_COUNT; i++)
    result &= dxl_wb.jointMode(JOINT_IDS[i], 0, 0, &log);
  printResult(result, log, "Failed to change joint mode", "Succeed to change joint mode\nDynamixel is moving...");

  for (int i = 0; i < JOINT_COUNT; i++)
  {
    dxl_wb.itemWrite(JOINT_IDS[i], "Drive_Mode", DRIVE_MODE_TIME_BASED);
    dxl_wb.itemWrite(JOINT_IDS[i], "Profile_Acceleration", PROFILE_ACCELERATION);
    dxl_wb.itemWrite(JOINT_IDS[i], "Profile_Velocity", PROFILE_VELOCITY);
  }
}

void moveJoints(const char *mode_name, const int32_t positions[JOINT_COUNT])
{
  Serial.println("----------------------------------------");
  Serial.print("Mode : ");
  Serial.println(mode_name);
  for (int i = 0; i < JOINT_COUNT; i++)
    dxl_wb.goalPosition(JOINT_IDS[i], positions[i]);
  delay(MOVE_WAIT_MS);
  Serial.println("complete");
}

void loop()
{
  // 수신 버퍼의 마지막 문자를 명령으로 사용한다.
  char input_data = 0;
  while (Serial.available())
  {
    input_data = Serial.read();
  }

  switch (input_data)
  {
    case '1': moveJoints("Home Position", home_position); break;
    case '2': moveJoints("2", desired_position); break;
    case '3': moveJoints("3", desired_position_2); break;
  }

  // 파이썬(serial2arduino.py)으로부터 조인트 값 받아 이동하기:
  // if (Serial2.available())
  // {
  //   splitInts(Serial2.readStringUntil('\n'), ',', joint, JOINT_COUNT);
  //   for (int i = 0; i < JOINT_COUNT; i++)
  //     dxl_wb.goalPosition(JOINT_IDS[i], joint[i]);
  //   waitForJoint(farthestJoint());
  // }
}

void readPositions()
{
  for (int i = 0; i < JOINT_COUNT; i++)
  {
    dxl_wb.itemRead(JOINT_IDS[i], "Goal_Position", &get_desired_position[i]);
    dxl_wb.itemRead(JOINT_IDS[i], "Present_Position", &get_position[i]);
  }
}

void printPositions(const char *label, const int32_t positions[JOINT_COUNT])
{
  Serial.print(label);
  Serial.print(" : [");
  for (int i = 0; i < JOINT_COUNT; i++)
  {
    if (i > 0) Serial.print(", ");
    Serial.print(positions[i]);
  }
  Serial.println("]");
}

// 목표 위치에 도달할 때까지 대기 (joint_index: 0 ~ JOINT_COUNT-1)
void waitForJoint(int joint_index)
{
  uint8_t id = JOINT_IDS[joint_index];
  while (abs(get_desired_position[joint_index] - get_position[joint_index]) > 1)
  {
    dxl_wb.itemRead(id, "Goal_Position", &get_desired_position[joint_index]);
    dxl_wb.itemRead(id, "Present_Position", &get_position[joint_index]);
    delay(1);
  }
}

// 목표 위치까지 남은 거리가 가장 큰 조인트의 인덱스를 반환
int farthestJoint()
{
  readPositions();
  printPositions("desired position", get_desired_position);
  printPositions("present position", get_position);

  int32_t max_error = 0;
  int index = 0;
  for (int i = 0; i < JOINT_COUNT; i++)
  {
    int32_t error = abs(get_desired_position[i] - get_position[i]);
    if (error > max_error)
    {
      max_error = error;
      index = i;
    }
  }
  Serial.println(max_error);
  Serial.println(index);
  return index;
}

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
