# 파이썬 -> OpenCR 매니풀레이터 조인트 position 시리얼 통신
# rad값의 조인트를 모터 비를 곱하여 전송.
# Hyounjun Oh
# E-mail : ohj_980918@naver.com

import serial
import time
import csv
import numpy as np

ser = serial.Serial('COM3', 57600) #OpenCR Port

J = 2 #조인트의 개수

pi = np.pi
ratio = (1/(pi*2))*(4092)
while True:
    if ser.readable():
        publish_data = csv.reader(open('manipulator_code\joint_value.csv', 'r'))
        joint_val = np.array([])
        for line in publish_data:
            converted_value = ratio*np.array([list(map(float, line))])
            #joint_val = np.append(joint_val, converted_value)
            trans_data = str(converted_value[0][0]) + ',' + str(converted_value[0][1]) #아두이노가 받기 위해서 문자열로 데이터 변환. 구분자 : ','
            print(trans_data)
            time.sleep(0.01)


        #joint_val = (ratio*joint_val).reshape(len(joint_val)//J,J)
        #print(joint_val)


        #ser.write(joint_val)
        #print("Value Publishig...")
        #time.sleep(0.5)