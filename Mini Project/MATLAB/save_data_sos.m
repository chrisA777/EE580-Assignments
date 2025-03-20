clc
clear;
close all;

load("IIR_filters.mat");

% Extract lowpass and reshape as wanted
low_b = SOS_low(:,1:3)';
low_b = low_b(:)';

low_a = SOS_low(:,4:end)';
low_a = low_a(:)';

% Extract bandpass and reshape as wanted
bp_b = SOS_bp(:,1:3)';
bp_b = bp_b(:)';

bp_a = SOS_bp(:,4:end)';
bp_a = bp_a(:)';

% Extract highpass and reshape as wanted
high_b = SOS_high(:,1:3)';
high_b = high_b(:)';

high_a = SOS_high(:,4:end)';
high_a = high_a(:)';

% Write to file
write_filter_coefficients('data_sos.h', 'IIR_low', low_b, low_a, G_low);
write_filter_coefficients('data_sos.h', 'IIR_bp', bp_b, bp_a, G_bp);
write_filter_coefficients('data_sos.h', 'IIR_high', high_b, high_a, G_high);

function write_filter_coefficients(filename, coef_name, b_coeffs, a_coeffs, gain)
    % Open file
    fid = fopen(filename, 'a');

    % Write B coefficients
    write_coefficients(fid, coef_name, 'B', b_coeffs);

    % Write A coefficients
    write_coefficients(fid, coef_name, 'A', a_coeffs);

    % Write gains
    write_coefficients(fid, coef_name, 'G', gain);

    % Close file
    fclose(fid);
end

function write_coefficients(fid, coef_name, coef_type, coeffs)
    % Generate variable names dynamically
    array_name = sprintf('%s_%s', coef_name, coef_type);
    define_name = sprintf('N_%s_%s', coef_name, coef_type);

    % Write define statement
    fprintf(fid, ['#define %s %d' char(13) char(10)], define_name, length(coeffs));
    fwrite(fid, char([13 10]), 'uchar');

    % Write array declaration
    fprintf(fid, 'float %s[] = { ', array_name);
    for ct = 1:length(coeffs)-1
        fprintf(fid, '%.7ff, ', single(coeffs(ct)));
    end
    fprintf(fid, '%.7ff', single(coeffs(end)));
    fwrite(fid, [' };' char(13) char(10)], 'uchar');
    fwrite(fid, char([13 10]), 'uchar');
end


