% Load saved filter weights
load('filter_weights.mat')                                                       

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

L=1024;
% Half spectrum length
N = L/2;           
% Frequency axis for 1 sided spectrum
f = fs * (0:N-1) / L;                                                        

X_1=fft(x_1,L);
X_2=fft(x_2,L);

% Plotting one sided spectrum of X_1
figure;                                                                      
plot(f, abs(X_1(1:N)) / N);     
title("One-Sided FFT of X_1");
xlabel("Frequency (Hz)");
ylabel("Magnitude");

% Plotting one sided spectrum of X_2
figure;                                                                      
plot(f, abs(X_2(1:N)) / N);
title("One-Sided FFT of X_2");
xlabel("Frequency (Hz)");
ylabel("Magnitude");

%% Filter Signals 

% Convolution to obtain filter outputs
y_1 = conv(b_fir_1,x_1);                                                        
y_2 = conv(b_fir_2,x_2);

%% Plot results

% Account for gd to avoid misalignment - same for both filters 
group_delay = (length(b_fir_1) - 1)/2;                             

% Plot time domain comparison of x_1 and y_1
figure;
hold on;
stem(x_1(55:81))
stem(y_1(55 + group_delay:81+ group_delay))


legend("x_1", "y_1")
title('3 Cycles of filtered signal, y_1')
xlabel('n')
ylabel("Amplitude")

% Plot FFTs
Y_1=fft(y_1,L);
X_1=fft(x_1,L);

figure
hold on;
plot(f, abs(X_1(1:N)) / N, "LineStyle", "--"); 
plot(f, abs(Y_1(1:N)) / N); 

legend("FFT of X_1", "FFT of Y_1")
title("One-Sided FFT of Y_1");
xlabel("Frequency (Hz)");
ylabel("Magnitude");


% Plot time domain comparison of x_2 and y_2
figure;
hold on;
stem(x_2(55:81))
stem(y_2(55 + group_delay:81+ group_delay))

legend("x_1", "y_1")
title('3 Cycles of filtered signal, y_2')
xlabel('n')
ylabel("Amplitude")
legend("x_2", "y_2")

% Plot FFTs
Y_2=fft(y_2,1024);
X_2=fft(x_2,1024);

figure
hold on;
plot(f, abs(X_2(1:N)) / N, "LineStyle", "--"); 
plot(f, abs(Y_2(1:N)) / N); 

legend("FFT of X_2", "FFT of Y_2")
title("One-Sided FFT of Y_2");
xlabel("Frequency (Hz)");
ylabel("Magnitude");

%% Comparison with C implementation

% Load in C Filtered Signals
y_1_dsp = table2array(readtable("filtered_signal_1.txt"));
y_2_dsp = table2array(readtable("filtered_signal_2.txt"));

% Calculate average differecnce
diff_y1 = mean(abs(y_1(:) - y_1_dsp(:)));
diff_y2 = mean(abs(y_2(:) - y_2_dsp(:)));

% Time domain y_1
figure;
hold on;
stem(y_1_dsp(55 + group_delay:81 + group_delay), 'LineStyle', ':', 'Marker', 'd');
stem(y_1(55 + group_delay:81+ group_delay));
legend('C Filtered y_1', 'MATLAB Filtered y_1')
title('Time Domain Comparison (3 Cycles) for y_1')
xlabel('n')
ylabel('Amplitude')

% Time domain y_2
figure;
hold on
stem(y_2_dsp(55 + group_delay:81 + group_delay), 'LineStyle', ':', 'Marker', 'd');
stem(y_2(55 + group_delay:81+ group_delay));
legend('C Filtered y_2', 'MATLAB Filtered y_2')
title('Time Domain Comparison (3 Cycles) for y_2')
xlabel('n')
ylabel('Amplitude')

% FFT y_1
Y_1=fft(y_1,1024);
Y_1_dsp=fft(y_1_dsp,1024);

figure
hold on;
plot(f, abs(Y_1_dsp(1:N)) / N);
plot(f, abs(Y_1(1:N)) / N, "LineStyle", "--");  
legend('C Filtered Y_1', 'MATLAB Filtered Y_1')
title('Frequency Domain Comparison for Y_1')
xlabel("Frequency (Hz)");
ylabel("Magnitude");

% FFT y_2
Y_2=fft(y_2,1024);
Y_2_dsp=fft(y_2_dsp,1024);

figure
hold on;
plot(f, abs(Y_2_dsp(1:N)) / N);
plot(f, abs(Y_2(1:N)) / N, "LineStyle", "--");  
legend('C Filtered Y_2', 'MATLAB Filtered Y_2')
title('Frequency Domain Comparison for Y_2')
xlabel("Frequency (Hz)");
ylabel("Magnitude");


