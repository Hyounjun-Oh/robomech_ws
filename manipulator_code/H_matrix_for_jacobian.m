%% Author : Hyounjun Oh
%% DRoC Lab
function array = H_matrix_for_jacobian(theta_array)
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

array = [T01, T12, T23, T34, T45, T56, T67];
end