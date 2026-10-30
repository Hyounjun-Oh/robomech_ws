# 파이썬 -> OpenCR 매니풀레이터 조인트 position 시리얼 통신
# rad값의 조인트를 모터 비를 곱하여 전송.
# Hyounjun Oh
# E-mail : ohj_980918@naver.com

import argparse
import csv
import time
from pathlib import Path

import numpy as np
import serial

JOINT_CSV = Path(__file__).resolve().parent / 'joint_value.csv'
RAD_TO_PULSE = (1 / (2 * np.pi)) * 4092  # 다이나믹셀 1회전 펄스 수 / 2π
SEND_INTERVAL = 5  # [s] 다음 조인트 값 전송까지 대기 시간


def load_joint_pulses(path=JOINT_CSV):
    """CSV(행마다 조인트 rad값)를 읽어 정수 펄스값 리스트로 변환한다."""
    with open(path, newline='') as f:
        return [[int(RAD_TO_PULSE * float(v)) for v in row] for row in csv.reader(f) if row]


def encode(pulses):
    """아두이노가 받기 위해서 문자열로 데이터 변환. 구분자 : ','"""
    return ','.join(map(str, pulses)).encode()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', default='COM4', help='OpenCR Port COM3, MEGA Port COM4')
    parser.add_argument('--baudrate', type=int, default=57600)
    parser.add_argument('--csv', type=Path, default=JOINT_CSV)
    args = parser.parse_args()

    ser = serial.Serial(args.port, args.baudrate)
    sent = 0
    while ser.readable():
        for pulses in load_joint_pulses(args.csv):
            data = encode(pulses)
            print(data.decode())
            ser.write(data)
            time.sleep(SEND_INTERVAL)
            print(sent)
            sent += 1


if __name__ == '__main__':
    main()
