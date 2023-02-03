%% Author : Hyounjun Oh
%% DRoC Lab
function [J,P4] = jacobian7(theta_array)
%% HOMOGENEOUS TRANSFORMATION
array = H_matrix_for_jacobian(theta_array);
T01 = array(:,1:4);
T02 = T01 * array(:,5:8);
T03 = T02 * array(:,9:12);
T04 = T03 * array(:,13:16);

% Z vector
Z0 = [0;0;1];
Z1 = T01(1:3,3);
Z2 = T02(1:3,3);
Z3 = T03(1:3,3);

% P vector
P0 = [0;0;0];
P1 = T01(1:3,4);
P2 = T02(1:3,4);
P3 = T03(1:3,4);
P4 = T04(1:3,4);

%Geometric Jacobian Matrix

J = [cross(Z0,(P4 - P0)) cross(Z1,(P4 - P1)) cross(Z2,(P4 - P2)) cross(Z3,(P4 - P3));
    Z0, Z1, Z2, Z3];
end