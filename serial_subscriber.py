import serial

ser = serial.Serial(port='COM4',baudrate=9600)

while ser.readable():
    line = ser.readline()
    splited_first = line[:len(line)-2].decode().split('/')
    data_1 = list(map(float, splited_first[0].split(' ')))
    data_2 = list(map(float, splited_first[1].split(' ')))
    print(data_1)
    print(data_2)
