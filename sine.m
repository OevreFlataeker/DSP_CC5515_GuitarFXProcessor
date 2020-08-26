function [values] = sine() 
%t = [0:1/24000:119/24000]; 
%A = 1*sin(2*pi*1*t);
t = 0:1/2400:1;
A = 1000*sin(2*pi*t);
values = int16(A);
%dlmwrite('1000HzSine',values,'delimiter',',','precision','%i')
end
