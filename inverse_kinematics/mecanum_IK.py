# 메카넘휠 역기구학 솔버
# Hyounjun Oh
# mail : ohj_980918@naver.com
# ####[reference]####
# 2015 International Seminar on Intelligent Technology and Its Applications
# Inverse Kinematic Implementation of Four-Wheels Mecanum Drive Mobile Robot Using Stepper Motors
import numpy as np

WIDTH = 170  # 중심점으로부터 모터의 가로 위치
LENGTH = 170  # 중심점으로부터 모터의 세로 위치
RADIUS = 0.05  # 메카넘휠 반지름 [m]


def mecanum_ik(vx, vy, wz, width=WIDTH, length=LENGTH, radius=RADIUS):
    """로봇 속도 (Vx [m/s], Vy [m/s], Rz [rad/s]) -> 4개 바퀴 각속도 [rad/s] (4x1)"""
    k = width + length
    convert_matrix = np.array([
        [1, -1, -k],
        [1, 1, k],
        [1, 1, -k],
        [1, -1, k],
    ])
    return (convert_matrix / radius) @ np.array([[vx], [vy], [wz]])


if __name__ == '__main__':
    vx, vy, wz = map(float, input('Vx, Vy, Rz').split())
    print(mecanum_ik(vx, vy, wz))
