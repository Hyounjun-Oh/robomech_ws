%rad/s
w = [0 -16 -16 0];

% The data of real robot
% a = 0.395;
% b = 0.325;
% R = 0.125;

% The data of research paper.
a = 0.170;
b = 0.170;
R = 0.050;

radius_sum = a+b;




% result = [Vx, Vy, w(z-axis)]
result = pinv([1 -1 -radius_sum; ...
               1  1  radius_sum; ...
               1  1 -radius_sum; ...
               1 -1  radius_sum])*(w'.*R);