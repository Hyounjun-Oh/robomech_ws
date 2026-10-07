# 아두이노(serial_publish)가 보내는 "a b c/d e f" 형식의 데이터를 읽어 출력한다.
import argparse

import serial


def parse_line(line):
    """b'1 2.5 3/4 5.5 6\\r\\n' -> [[1.0, 2.5, 3.0], [4.0, 5.5, 6.0]]"""
    groups = line.decode().strip().split('/')
    return [list(map(float, group.split())) for group in groups]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', default='COM4')
    parser.add_argument('--baudrate', type=int, default=9600)
    args = parser.parse_args()

    ser = serial.Serial(port=args.port, baudrate=args.baudrate)
    while ser.readable():
        for data in parse_line(ser.readline()):
            print(data)


if __name__ == '__main__':
    main()
