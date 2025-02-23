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

%% Plot Time Domain Signals
stem(x_1(1:27))
title('3 Cycles of x_1')
xlabel('n')
ylabel('Amplitude')

figure;
stem(x_2(1:27));
title('3 Cycles of x_2')
xlabel('n')
ylabel('Amplitude')

%% Plot one sided spectrum

N = L/2;                        % Half spectrum length
f = fs * (0:N-1) / L;           % Frequency axis for 1 sided spectrum

figure;
plot(f, abs(X_1(1:N)) / N);     
title("One-Sided FFT of X_1");
xlabel("Frequency (Hz)");
ylabel("Magnitude");

figure;
plot(f, abs(X_2(1:N)) / N);
title("One-Sided FFT of X_2");
xlabel("Frequency (Hz)");
ylabel("Magnitude");

%% Filter Signals and plot results

y_1 = conv(w_1,x_1);

figure;
hold on;
stem(x_1(54:81))
stem(y_1(54:81))

legend("x_1", "y_1")
title('3 Cycles of filtered signal, y_1')
xlabel('n')

% Plot FFTs
Y_1=fft(y_1,1024);
X_1=fft(x_1,1024);

figure
L=1024;
hold on;
plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_1)),"LineStyle","--")
plot(fs/L*(-L/2:L/2-1),abs(fftshift(Y_1)))

legend("FFT of X_1", "FFT of Y_1")
title("FFT of Y_1")

y_2 = conv(w_2,x_2);

figure;
hold on;
stem(x_2(20:47))
stem(y_2(20:47))

legend("x_1", "y_1")
title('3 Cycles of filtered signal, y_2')
xlabel('n')

legend("FFT of X_2", "FFT of Y_2")

% Plot FFTs
Y_2=fft(y_2,1024);
X_2=fft(x_2,1024);

figure
L=1024;
hold on;

plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_2)),"LineStyle","--")
plot(fs/L*(-L/2:L/2-1),abs(fftshift(Y_2)))

legend("FFT of X_2", "FFT of Y_2")
title("FFT of Y_2")


%% Load in C Filtered Signals

y_1_dsp = table2array(readtable("filtered_signal_1.txt"));
y_2_dsp = table2array(readtable("filtered_signal_2.txt"));

figure;
stem(y_1(20:47));

figure;
stem(y_1_dsp(20:47));



% figure;
% hold on;
% stem(y_1_(20:47))
% stem(y_1_dsp(20:47))
% 
% legend("x_1", "y_1")
% title('3 Cycles of filtered signal, y_2')
% xlabel('n')

