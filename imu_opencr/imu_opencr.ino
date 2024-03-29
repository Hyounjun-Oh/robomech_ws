/* AccRAW*/
/*
  Range   : +/- 2 g
  Scale   : 16384 = 1 g
 */
 /*GyroRAW*/
/*
  Range   : +/- 2000 deg/sec
  Scale   : 16.4 = 1 deg/sec
 */ 
 /*R P Y*/
 /*
  Range   : Roll  : +/- 180 deg/sec
            Pitch : +/- 180 deg/sec
            Yaw   : +/- 180 deg/sec
  Scale   : Roll  : 1 = 1 deg/sec
            Pitch : 1 = 1 deg/sec
            Yaw   : 1 = 1 deg/sec
 */
#include <IMU.h>


cIMU    IMU;

uint8_t   err_code;
uint8_t   led_tog = 0;
uint8_t   led_pin = 13;

void setup()
{
  Serial.begin(115200);

  IMU.begin();

  pinMode( led_pin, OUTPUT );
}


void loop() {
  static uint32_t tTime[3];
  static uint32_t imu_time = 0;


  if( (millis()-tTime[0]) >= 500 )
  {
    tTime[0] = millis();

    digitalWrite( led_pin, led_tog );
    led_tog ^= 1;
  }

  tTime[2] = micros();
  if( IMU.update() > 0 ) imu_time = micros()-tTime[2];


  if( (millis()-tTime[1]) >= 50 )
  {
    tTime[1] = millis();

    Serial.print(imu_time);
    Serial.print("\t");
    Serial.print(IMU.accRaw[0]);    // ACC X
    Serial.print("\t");
    Serial.print(IMU.accRaw[1]);    // ACC Y
    Serial.print("\t");
    Serial.print(IMU.accRaw[2]);    // ACC Z
    Serial.print("\t");
    Serial.print(IMU.gyroRaw[0]);    // GYRO X
    Serial.print("\t");
    Serial.print(IMU.gyroRaw[1]);    // GYRO Y
    Serial.print("\t");
    Serial.print(IMU.gyroRaw[2]);    // GYRO Z
    Serial.print("\t");
    Serial.print(IMU.rpy[0]);
    Serial.print("\t");
    Serial.print(IMU.rpy[1]);
    Serial.print("\t");
    Serial.println(IMU.rpy[2]);
  }
  
  if( Serial.available() )
  {
    char Ch = Serial.read();

    if( Ch == '1' )
    {
      Serial.println("ACC Cali Start");

      IMU.SEN.acc_cali_start();
      while( IMU.SEN.acc_cali_get_done() == false )
      {
        IMU.update();
      }

      Serial.print("ACC Cali End ");
    }
  }
}
