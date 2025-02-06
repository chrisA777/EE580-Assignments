% Define two inputs
student_num_1 = [2 0 2 0 3 7 6 9 6];
student_num_2 = [2 0 2 0 3 2 1 7 5];

% Repeat student num 100 times
x_1 = repmat(student_num_1, 1, 100);
x_2 = repmat(student_num_2, 1, 100);

% Mean of two signals
mu_1 = mean(x_1);
mu_2 = mean(x_2);

% Zero mean signals
x_1 = x_1 - mu_1;
x_2 = x_2 - mu_2;

% Last four digits
N = 4;
last_four_1 = student_num_1(end - N+1: end);
last_four_2 = student_num_2(end - N+1: end);
last_four_1 = str2double(sprintf('%d', last_four_1));
last_four_2 = str2double(sprintf('%d', last_four_2));

% Sampling frequency
fs = round((last_four_1 + last_four_2)/2);

%% Plot Signals
stem(x_1(1:27))
title('3 Cycles of x_1')
xlabel('n')


figure;
stem(x_2(1:27));
title('3 Cycles of x_2')
xlabel('n')

% Plot FFTs
X_1=fft(x_1,1024);
X_2=fft(x_2,1024);

figure;
L=1024;
plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_1)))
title("FFT of X_1")

figure;
L=1024;
plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_2)))
title("FFT of X_2")

%% Filter Signals and plot results

y_1 = filter(w_1,1,x_1);

figure;
hold on;
stem(y_1(20:47))
stem(x_1(20:47))

title('3 Cycles of x_1')
xlabel('n')



% Plot FFTs
Y_1=fft(y_1,1024);
X_2=fft(x_2,1024);

figure
L=1024;
hold on;
plot(fs/L*(-L/2:L/2-1),abs(fftshift(Y_1)),"LineWidth",3)
plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_1)),"LineStyle","--")

title("FFT of Y_1")


