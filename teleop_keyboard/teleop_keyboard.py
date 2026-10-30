#!/usr/bin/env python
#
# Copyright (c) 2011, Willow Garage, Inc.
# All rights reserved.
#
# Software License Agreement (BSD License 2.0)
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of {copyright_holder} nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Author: Darby Lim

import os
import select
import sys
import rclpy

from geometry_msgs.msg import Twist
from rclpy.qos import QoSProfile

if os.name == 'nt':
    import msvcrt
else:
    import termios
    import tty

MAX_LIN = 0.3
MAX_ROT = 4.0
LIN_VEL_STEP_SIZE = 0.3
ANG_VEL_STEP_SIZE = 0.3

# 속도 벡터 인덱스: [Vx, Vy, Rz]
VX, VY, RZ = range(3)
STEP_SIZES = (LIN_VEL_STEP_SIZE, LIN_VEL_STEP_SIZE, ANG_VEL_STEP_SIZE)
# NOTE: 기존 동작 유지를 위해 Rz도 MAX_LIN으로 제한한다 (MAX_ROT는 사용되지 않음).
LIMITS = (MAX_LIN, MAX_LIN, MAX_LIN)

# 키 -> (축, 증감 방향)
KEY_BINDINGS = {
    'w': (VX, +1),
    'x': (VX, -1),
    'a': (VY, +1),
    'd': (VY, -1),
    'q': (RZ, +1),
    'e': (RZ, -1),
}
STOP_KEYS = (' ', 's')
CTRL_C = '\x03'
HELP_INTERVAL = 20  # 이 횟수만큼 속도를 바꾸면 도움말을 다시 출력

msg = """
혼자서도 잘해요
[Mecanum wheel Mobile Manipulator]
---------------------------
키배치:
    Q   W   E
    A   S   D
        X

w/x : 증가/감소 Vx [최고속도 : 0.3 m/s]
a/d : 증가/감소 Vy [최고속도 : 0.3 m/s]
q/e : 증가/감소 Rz [최고속도 : 1.7647 rad/s]
s   : 강제 정지
CTRL-C : 컨트롤러 종료
"""


def get_key(settings):
    if os.name == 'nt':
        return msvcrt.getch().decode('utf-8')
    tty.setraw(sys.stdin.fileno())
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    key = sys.stdin.read(1) if rlist else ''

    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key


def print_vels(target):
    print('currently:\tlinear X velocity {0} \tlinear Y velocity {1}\t angular velocity {2} '.format(*target))


def make_simple_profile(output, input, slop):
    if input > output:
        return min(input, output + slop)
    if input < output:
        return max(input, output - slop)
    return input


def constrain(value, low_bound, high_bound):
    return max(low_bound, min(value, high_bound))


def make_twist(vx=0.0, vy=0.0, rz=0.0):
    twist = Twist()
    twist.linear.x = vx
    twist.linear.y = vy
    twist.linear.z = 0.0
    twist.angular.x = 0.0
    twist.angular.y = 0.0
    twist.angular.z = rz
    return twist


def main():
    settings = None
    if os.name != 'nt':
        settings = termios.tcgetattr(sys.stdin)

    rclpy.init()

    qos = QoSProfile(depth=10)
    node = rclpy.create_node('teleop_keyboard')
    pub = node.create_publisher(Twist, 'cmd_vel', qos)

    status = 0
    target = [0.0, 0.0, 0.0]
    control = [0.0, 0.0, 0.0]

    try:
        print(msg)
        while True:
            key = get_key(settings)
            if key in KEY_BINDINGS:
                axis, sign = KEY_BINDINGS[key]
                target[axis] = constrain(
                    target[axis] + sign * STEP_SIZES[axis], -LIMITS[axis], LIMITS[axis])
                status += 1
                print_vels(target)
            elif key in STOP_KEYS:
                target = [0.0, 0.0, 0.0]
                control = [0.0, 0.0, 0.0]
                print_vels(target)
            elif key == CTRL_C:
                break

            if status == HELP_INTERVAL:
                print(msg)
                status = 0

            control = [
                make_simple_profile(c, t, step / 2.0)
                for c, t, step in zip(control, target, STEP_SIZES)
            ]
            pub.publish(make_twist(*control))

    except Exception as e:
        print(e)

    finally:
        pub.publish(make_twist())

        if os.name != 'nt':
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)


if __name__ == '__main__':
    main()
