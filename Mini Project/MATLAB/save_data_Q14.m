% Load data
load("IIR_filters_2.mat");

% Q14 scaling factor
% Q14_SCALE = 16384;

% Extract lowpass and reshape
low_b = SOS_low(:,1:3)';
low_b = low_b(:)';
low_b_q14 = float_to_q14(low_b);

low_a = SOS_low(:,4:end)';
low_a = low_a(:)';
low_a_q14 = float_to_q14(low_a);

% Extract bandpass and reshape
bp_b = SOS_bp(:,1:3)';
bp_b = bp_b(:)';
bp_b_q14 = float_to_q14(bp_b);

bp_a = SOS_bp(:,4:end)';
bp_a = bp_a(:)';
bp_a_q14 = float_to_q14(bp_a);

% Extract highpass and reshape
high_b = SOS_high(:,1:3)';
high_b = high_b(:)';
high_b_q14 = float_to_q14(high_b);

high_a = SOS_high(:,4:end)';
high_a = high_a(:)';
high_a_q14 = float_to_q14(high_a);

% Convert gains to Q14 as well
G_low_q14  = float_to_q14(G_low);
G_bp_q14   = float_to_q14(G_bp);
G_high_q14 = float_to_q14(G_high);

% Write to file
write_filter_coefficients('data_sos_q14.h', 'IIR_low', low_b_q14, low_a_q14, G_low_q14);
write_filter_coefficients('data_sos_q14.h', 'IIR_bp', bp_b_q14, bp_a_q14, G_bp_q14);
write_filter_coefficients('data_sos_q14.h', 'IIR_high', high_b_q14, high_a_q14, G_high_q14);


% Function to write filter data as int16_t
function write_filter_coefficients(filename, coef_name, b_coeffs, a_coeffs, gain)
    fid = fopen(filename, 'a');
    write_coefficients(fid, coef_name, 'B', b_coeffs);
    write_coefficients(fid, coef_name, 'A', a_coeffs);
    write_coefficients(fid, coef_name, 'G', gain);
    fclose(fid);
end

function write_coefficients(fid, coef_name, coef_type, coeffs)
    array_name = sprintf('%s_%s', coef_name, coef_type);
    define_name = sprintf('N_%s_%s', coef_name, coef_type);

    % Write define
    fprintf(fid, ['#define %s %d\r\n'], define_name, length(coeffs));
    fprintf(fid, '\r\n');

    % Write array declaration
    fprintf(fid, 'int16_t %s[] = { ', array_name);
    for ct = 1:length(coeffs)-1
        fprintf(fid, '%d, ', coeffs(ct));
    end
    fprintf(fid, '%d', coeffs(end));
    fprintf(fid, ' };\r\n\r\n');
end

% Convret to q14 
function q14 = float_to_q14(x)
    x = round(x * 16384);
    x(x > 32767) = 32767;
    x(x < -32768) = -32768;
    q14 = int16(x);


end
