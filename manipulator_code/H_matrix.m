%% Author : Hyounjun Oh
%% DRoC Lab
function array = H_matrix(theta_array)
%% DH파라미터 기입
d = [300 194 449.5 -190 350 183 228];
a = [0 0 0 0 0 0 0];
al = [90.00021 -90.0002 90.00021 -90.0002 90.00021 -90.0002 90.00021].*(pi/180);
q = [theta_array(1) theta_array(2) theta_array(3) theta_array(4) theta_array(5) theta_array(6) theta_array(7)];
%% 동차변환
T01 = DHmatrix(q(1),d(1),a(1),al(1));
T12 = DHmatrix(q(2),d(2),a(2),al(2));
T23 = DHmatrix(q(3),d(3),a(3),al(3));
T34 = DHmatrix(q(4),d(4),a(4),al(4));
T45 = DHmatrix(q(5),d(5),a(5),al(5));
T56 = DHmatrix(q(6),d(6),a(6),al(6));
T67 = DHmatrix(q(7),d(7),a(7),al(7));

T02 = T01 * T12;
T03 = T02 * T23;
T04 = T03 * T34;
T05 = T04 * T45;
T06 = T05 * T56;
T07 = T06 * T67;

P0 = [0;0;0];
P1 = T01(1:3,4);
P2 = T02(1:3,4);
P3 = T03(1:3,4);
P4 = T04(1:3,4);
P5 = T05(1:3,4);
P6 = T06(1:3,4);
P7 = T07(1:3,4);
%% plot array

Px = [0 P1(1) P2(1) P3(1) P4(1) P5(1) P6(1) P7(1)];
Py = [0 P1(2) P2(2) P3(2) P4(2) P5(2) P6(2) P7(2)];
Pz = [0 P1(3) P2(3) P3(3) P4(3) P5(3) P6(3) P7(3)];

array = [Px;Py;Pz];
end