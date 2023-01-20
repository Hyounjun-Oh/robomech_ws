# 메카넘휠 역기구학 솔버
# Hyounjun Oh
# mail : ohj_980918@naver.com
# [reference]
# 2015 International Seminar on Intelligent Technology and Its Applications 
# Inverse Kinematic Implementation of F our-Wheels Mecanum Drive Mobile Robot Using Stepper Motors
import numpy as np

width = 170 # 중심점으로부터 모터의 가로 위치
length = 170 # 중심점으로부터 모터의 세로 위치
radius = 0.05 # 메카넘휠 지름

desired_value = list(map(float,input('Vx, Vy, Rz').split(' '))) # m/s, rad/s
convert_matrix = np.array([[1, -1, -(width + length)], [1, 1, (width + length)], [1, 1, -(width + length)], [1, -1, (width + length)]])
result = (convert_matrix*(1/radius))@np.array([[desired_value[0]], [desired_value[1]], [desired_value[2]]]) #rad/s

print(result)