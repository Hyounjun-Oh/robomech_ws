% Robomeca 4-DOF Manipulator Simulator
% Hyounjun Oh
% E-main : ohj_980918@naver.com
%% 요구사항
% 1. ROS 메세지로 Position과 Orientation값을 전달 받는다.
% 2. 자코비안 역기구학을 이용하여 샘플링 타임동안의 4개의 조인트 값을 저장한다.
% 3. 1 ~ 2번 까지의 과정이 시뮬레이션으로 잘 가동될 시 파이썬코드로 재구성하여 사용한다.
%% 문제점
% 초반 8샘플 타임에서 조인트 값 튐 발생. 원인 찾기.
%%
%close all
clf
clear
clc
format default
%%
%close all
clf
clear
clc
format long
%% CONFIGURATION
% DH Table
theta = [0 0 0 0].*(pi/180);
d = [55 0 0 0];
a_dh = [0 200 200 100];
alpha = [90 90 -90 0].*(pi/180);
%% DATA INITIALIZATION

% Value of each Actuator
vi = [0 0 0 0 0 0];
vf = [0 0 0 0 0 0];
ai = [0 0 0 0 0 0];
af = [0 0 0 0 0 0];

% Sampling Time
ti = 0; tf = 2; t = linspace(ti,tf,100*(tf-ti));

% Desired Pose
desired_pose_1 = [200 200 -200 0 0 0];

desired_pose_array = [300 200 200 0 0 0;
                      100 100 200 0 0 0;
                      200 -200 200 0 0 0];
% Current Pose
current_pose = zeros(3,length(t));  %현재는 position만 고려한다.

% Current Joints
current_joints = zeros(4,length(t));
%% FIND CURRENT INITIAL POSE

initial_joints = [90.0001 0 0 0].*(pi/180);
initial_pose = H_matrix(initial_joints);
%% FIND TRAJECTORIES

% Quintic Polynomial
M = [1 ti ti^2 ti^3 ti^4 ti^5;
     0 1 2*ti 3*(ti^2) 4*(ti^3) 5*(ti^4);
     0 0 2 6*ti 12*(ti^2) 20*(ti^3);
     1 tf tf^2 tf^3 tf^4 tf^5;
     0 1 2*tf 3*(tf^2) 4*(tf^3) 5*(tf^4);
     0 0 2 6*tf 12*(tf^2) 20*(tf^3)];
b = [initial_pose(1:3,5)', 0, 0, 0;vi;ai;desired_pose_array(1,:);vf;af];
a = M\b;
c = ones(size(t));

% Desired trajectory
pxd = a(1).*c + a(2).*t +a(3).*t.^2 + a(4).*t.^3 +a(5).*t.^4 + a(6).*t.^5;
vxd = a(2).*c +2*a(3).*t +3*a(4).*t.^2 +4*a(5).*t.^3 +5*a(6).*t.^4;
axd = 2*a(3).*c + 6*a(4).*t +12*a(5).*t.^2 +20*a(6).*t.^3;

pyd = a(1,2).*c + a(2,2).*t +a(3,2).*t.^2 + a(4,2).*t.^3 +a(5,2).*t.^4 + a(6,2).*t.^5;
vyd = a(2,2).*c +2*a(3,2).*t +3*a(4,2).*t.^2 +4*a(5,2).*t.^3 +5*a(6,2).*t.^4;
ayd = 2*a(3,2).*c + 6*a(4,2).*t +12*a(5,2).*t.^2 +20*a(6,2).*t.^3;

pzd = a(1,3).*c + a(2,3).*t +a(3,3).*t.^2 + a(4,3).*t.^3 +a(5,3).*t.^4 + a(6,3).*t.^5;
vzd = a(2,3).*c +2*a(3,3).*t +3*a(4,3).*t.^2 +4*a(5,3).*t.^3 +5*a(6,3).*t.^4;
azd = 2*a(3,3).*c + 6*a(4,3).*t +12*a(5,3).*t.^2 +20*a(6,3).*t.^3;

%% INVERSE KINEMATICS
given_pose = length(desired_pose_array(:,1));
iter = 0;
while 1
    if iter ~= given_pose
        iter = iter + 1;
        if iter == 1
            initial_pose = H_matrix(initial_joints);
            for k = 1:length(t)-1
                if k == 1
                    jacobian = jacobian7(initial_joints);
                    q_dot_f = pinv(jacobian) * ([vxd(k) vyd(k) vzd(k) 0 0 0])';
                    current_joints(1:4,k) = initial_joints;
                    qf = initial_joints' + q_dot_f.*(t(k+1)-t(k));
                    Plot_Points = H_matrix(qf);
                    current_pose(1:3,k) = Plot_Points(1:3,5);
                    current_joints(1:4,k+1) = qf;
                    plot3(Plot_Points(1,:),Plot_Points(2,:),Plot_Points(3,:),'-or','LineWidth',3);
                    axis([-500,500,-500,500,-100,500]);
                    grid on
                    pause(0.001)
                else
                    Plot_Points = H_matrix(qf);
                    vf = [vxd(k);vyd(k);vzd(k);0;0;0];
                    p_dot_ref = vf + (1.5*([pxd(k);pyd(k);pzd(k);0;0;0] - [Plot_Points(1:3,5);0;0;0]))./(t(k+1)-t(k));
                    jacobian = jacobian7(qf);
                    q_dot_f = pinv(jacobian) * p_dot_ref;
                    
                    qf = qf + q_dot_f*(t(k+1)-t(k));
                    Plot_Points = H_matrix(qf);
                    current_pose(1:3,k) = Plot_Points(1:3,5);
                    current_joints(1:4,k+1) = qf;
                    plot3(Plot_Points(1,:),Plot_Points(2,:),Plot_Points(3,:),'-or','LineWidth',3);
                    axis([-500,500,-500,500,-100,500]);
                    grid on
                    pause(0.001)
                end
            end
        else
            initial_pose = H_matrix(qf);
            %% FIND TRAJECTORIES
            vi = [0 0 0 0 0 0];
            vf = [0 0 0 0 0 0];
            ai = [0 0 0 0 0 0];
            af = [0 0 0 0 0 0];
            % Quintic Polynomial
            M = [1 ti ti^2 ti^3 ti^4 ti^5;
                 0 1 2*ti 3*(ti^2) 4*(ti^3) 5*(ti^4);
                 0 0 2 6*ti 12*(ti^2) 20*(ti^3);
                 1 tf tf^2 tf^3 tf^4 tf^5;
                 0 1 2*tf 3*(tf^2) 4*(tf^3) 5*(tf^4);
                 0 0 2 6*tf 12*(tf^2) 20*(tf^3)];
            b = [initial_pose(1:3,5)', 0, 0, 0;vi;ai;desired_pose_array(iter,:);vf;af];
            a = M\b;
            c = ones(size(t));
            
            % Desired trajectory
            pxd = a(1).*c + a(2).*t +a(3).*t.^2 + a(4).*t.^3 +a(5).*t.^4 + a(6).*t.^5;
            vxd = a(2).*c +2*a(3).*t +3*a(4).*t.^2 +4*a(5).*t.^3 +5*a(6).*t.^4;
            axd = 2*a(3).*c + 6*a(4).*t +12*a(5).*t.^2 +20*a(6).*t.^3;
            
            pyd = a(1,2).*c + a(2,2).*t +a(3,2).*t.^2 + a(4,2).*t.^3 +a(5,2).*t.^4 + a(6,2).*t.^5;
            vyd = a(2,2).*c +2*a(3,2).*t +3*a(4,2).*t.^2 +4*a(5,2).*t.^3 +5*a(6,2).*t.^4;
            ayd = 2*a(3,2).*c + 6*a(4,2).*t +12*a(5,2).*t.^2 +20*a(6,2).*t.^3;
            
            pzd = a(1,3).*c + a(2,3).*t +a(3,3).*t.^2 + a(4,3).*t.^3 +a(5,3).*t.^4 + a(6,3).*t.^5;
            vzd = a(2,3).*c +2*a(3,3).*t +3*a(4,3).*t.^2 +4*a(5,3).*t.^3 +5*a(6,3).*t.^4;
            azd = 2*a(3,3).*c + 6*a(4,3).*t +12*a(5,3).*t.^2 +20*a(6,3).*t.^3;
    
            for k = 1:length(t)-1
                if k == 1
                    jacobian = jacobian7(qf);
                    q_dot_f = pinv(jacobian) * ([vxd(k) vyd(k) vzd(k) 0 0 0])';
                    current_joints(1:4,k) = qf;
                    qf = qf + q_dot_f.*(t(k+1)-t(k));
                    Plot_Points = H_matrix(qf);
                    current_pose(1:3,k) = Plot_Points(1:3,5);
                    current_joints(1:4,k+1) = qf;
                    plot3(Plot_Points(1,:),Plot_Points(2,:),Plot_Points(3,:),'-or','LineWidth',3);
                    axis([-500,500,-500,500,-100,500]);
                    grid on
                    pause(0.001)
                else
                    Plot_Points = H_matrix(qf);
                    vf = [vxd(k);vyd(k);vzd(k);0;0;0];
                    p_dot_ref = vf + (1*([pxd(k);pyd(k);pzd(k);0;0;0] - [Plot_Points(1:3,5);0;0;0]))./(t(k+1)-t(k));
                    q_dot_f = pinv(jacobian7(qf)) * p_dot_ref;
                    qf = qf + q_dot_f*(t(k+1)-t(k));
                    Plot_Points = H_matrix(qf);
                    current_pose(1:3,k) = Plot_Points(1:3,5);
                    current_joints(1:4,k+1) = qf;
                    plot3(Plot_Points(1,:),Plot_Points(2,:),Plot_Points(3,:),'-or','LineWidth',3);
                    axis([-500,500,-500,500,-100,500]);
                    grid on
                    pause(0.001)
                end
            end
        end
    else
        disp('종료')
        break
    end
end