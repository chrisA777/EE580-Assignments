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

% Plot time domain of x_1 (3 cycles)
stem(x_1(1:27))
title('3 Cycles of x_1')
xlabel('n')
ylabel('Amplitude')

% Plot time domain of x_2 (3 cycles)
figure;
stem(x_2(1:27));
title('3 Cycles of x_2')
xlabel('n')
ylabel('Amplitude')

% Calculate FFTs
X_1=fft(x_1,1024);
X_2=fft(x_2,1024);

% Plot FFT of x_1
figure;
L=1024;
plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_1)))
title("FFT of X_1")
xlabel("Frequency (Hz)")
ylabel("Magnitude")

% Plot FFT of x_2
figure;
plot(fs/L*(-L/2:L/2-1),abs(fftshift(X_2)))
title("FFT of X_2")
xlabel("Frequency (Hz)")
ylabel("Magnitude")
%% Filter Signals and plot results

% Convolve signal with filter weights
y_1 = conv(w_1,x_1);

% Plot time domain of filtered signals (3 cycles) after transient response
figure;
hold on;
stem(x_1(55:54+27))
stem(y_1(55:54+27))
legend("x_1", "y_1")
title('3 Cycles of filtered signal, y_1')
xlabel('n')
ylabel('Amplitude')

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
xlabel("Frequency (Hz)")
ylabel("Magnitude")

% Convolve signal with filter weights
y_2 = conv(w_2,x_2);

figure;
hold on;
stem(x_2(55:54+27))
stem(y_2(55:54+27))

legend("x_1", "y_1")
title('3 Cycles of filtered signal, y_2')
xlabel('n')
ylabel('Amplitude')

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
xlabel("Frequency (Hz)")
ylabel("Magnitude")

