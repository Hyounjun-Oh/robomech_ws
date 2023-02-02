%Result
% | deegre | Vx | Vy | Rz |
% [0    0.6     0       0
%  45   0.3     0.3     0
%  90   0       0.6     0
%  135  0.3     -0.3    0
%  180  -0.6    0       0
%  225  -0.3    -0.3    0
%  275  0       -0.6    0
%  315  -0.3     0.3    0]

% Max Vx, Vy = 0.3 m/s
% Max diagonal direction 0.6 m/s
% Max Rotational direction 1.7647 rad/s
%%
%rad/s
w = [5.6 6.4 6.4 5.6];

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