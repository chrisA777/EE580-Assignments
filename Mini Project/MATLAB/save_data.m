
write_filter_coefficients('data.h', 'IIR_low', b_low, a_low);
write_filter_coefficients('data.h', 'IIR_bp', b_bp, a_bp);
write_filter_coefficients('data.h', 'IIR_high', b_high, a_high);

function write_filter_coefficients(filename, coef_name, b_coeffs, a_coeffs)
    % Open file
    fid = fopen(filename, 'a');

    % Write B coefficients
    write_coefficients(fid, coef_name, 'B', b_coeffs);

    % Write A coefficients
    write_coefficients(fid, coef_name, 'A', a_coeffs);

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

