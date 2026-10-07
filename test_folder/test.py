# 입력한 명령을 아두이노로 보내고 응답 한 줄을 출력하는 시리얼 테스트 스크립트
import argparse
import time

import serial


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', default='COM3')
    parser.add_argument('--baudrate', type=int, default=57600)
    args = parser.parse_args()

    py_serial = serial.Serial(port=args.port, baudrate=args.baudrate)
    while True:
        command = input('아두이노에게 내릴 명령:')
        py_serial.write(command.encode())
        time.sleep(1)

        if py_serial.readable():
            # BYTE 단위로 받은 응답(예: b'\xec\x97\x86\xec\x9d\x8c\r\n')을 디코딩 후 출력
            response = py_serial.readline()
            print(response.decode().rstrip('\r\n'))


if __name__ == '__main__':
    main()
