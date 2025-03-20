clc;
clear;
close all;

load("IIR_filters.mat");

% Parameters
fs = 8000;          % Sampling frequency (Hz)
t = 0:1/fs:1-1/fs;  % Time vector (1 second)
noise = randn(size(t));  % Generate random noise

% Frequency domain representation
N = length(noise);
f = fs * (0:(N/2)) / N;
Y = fft(noise);
P2 = abs(Y / N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2 * P1(2:end-1);

% Plotting the frequency domain output
figure;
plot(f, P1);
title('Frequency Domain of Unfiltered Noise');
xlabel('Frequency (Hz)');
ylabel('Magnitude');
grid on;


% Filter activation flags
useLowPass = true;
useBandPass = false;
useHighPass = false;

% Low-pass filter design (Butterworth)
if useLowPass
    noise = filter(b_low, a_low, noise);
end

% Band-pass filter design (Butterworth)
if useBandPass
    noise = filter(b_bp, a_bp, noise);
end

% High-pass filter design (Butterworth)
if useHighPass
    noise = filter(b_high, a_high, noise);
end

% Frequency domain representation
N = length(noise);
f = fs * (0:(N/2)) / N;
Y = fft(noise);
P2 = abs(Y / N);
P1 = P2(1:N/2+1);
P1(2:end-1) = 2 * P1(2:end-1);

% Plotting the frequency domain output
figure;
plot(f, P1);
title('Frequency Domain of Filtered Noise');
xlabel('Frequency (Hz)');
ylabel('Magnitude');
grid on;
