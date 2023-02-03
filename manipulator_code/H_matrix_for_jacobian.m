%% Author : Hyounjun Oh
%% DRoC Lab
function array = H_matrix_for_jacobian(theta_array)
%% DH파라미터 기입
d = [55 0 0 0];
a = [0 200 200 100];
al = [90 90 -90 0].*(pi/180);
q = [theta_array(1) theta_array(2) theta_array(3) theta_array(4)];
%% 동차변환
T01 = DHmatrix(q(1),d(1),a(1),al(1));
T12 = DHmatrix(q(2),d(2),a(2),al(2));
T23 = DHmatrix(q(3),d(3),a(3),al(3));
T34 = DHmatrix(q(4),d(4),a(4),al(4));

array = [T01, T12, T23, T34];
end