fid = fopen('data.h','w');


%% FIR FILTER
%% B_1
fprintf(fid,['#define N_FIR_B_1 %d' char([13 10])], length(b_fir));
fwrite(fid,char([13 10]),'uchar');

fwrite(fid,'float b_fir_1[] = { ','uchar');
for ct = 1:length(b_fir_1)-1
    fprintf(fid,'%.7ff, ', single(b_fir_1(ct)));
end
fprintf(fid,'%.7ff', single(b_fir_1(end)));
fwrite(fid,[' };' char([13 10])],'uchar');
fwrite(fid,char([13 10]),'uchar');

%% B_2
fprintf(fid,['#define N_FIR_B_2 %d' char([13 10])], length(b_fir_2));
fwrite(fid,char([13 10]),'uchar');

fwrite(fid,'float b_fir_2[] = { ','uchar');
for ct = 1:length(b_fir_2)-1
    fprintf(fid,'%.7ff, ', single(b_fir_2(ct)));
end
fprintf(fid,'%.7ff', single(b_fir_2(end)));
fwrite(fid,[' };' char([13 10])],'uchar');
fwrite(fid,char([13 10]),'uchar');

%% A_1
fprintf(fid,['#define N_FIR_A_1 %d' char([13 10])], length(a_fir_1));
fwrite(fid,char([13 10]),'uchar');
fwrite(fid,'float a_fir_1[] = { ','uchar');
for ct = 1:length(a_fir_1)-1
    fprintf(fid,'%.7ff, ', single(a_fir_1(ct)));
end
fprintf(fid,'%.7ff', single(a_fir_1(end)));
fwrite(fid,[' };' char([13 10])],'uchar');
fwrite(fid,char([13 10]),'uchar');



%% A_2
fprintf(fid,['#define N_FIR_A_2 %d' char([13 10])], length(a_fir_2));
fwrite(fid,char([13 10]),'uchar');
fwrite(fid,'float a_fir_2[] = { ','uchar');
for ct = 1:length(a_fir_2)-1
    fprintf(fid,'%.7ff, ', single(a_fir_2(ct)));
end
fprintf(fid,'%.7ff', single(a_fir_2(end)));
fwrite(fid,[' };' char([13 10])],'uchar');
fwrite(fid,char([13 10]),'uchar');

fclose(fid);

%% test


% Quantising
w_1_quantised = single(w_1);

% Get zeros (no poles except 0)
z_pre = roots(w_1);
z_post = roots(w_1_quantised);

% Sort
z_pre_sorted = sort(z_pre);
z_post_sorted = sort(z_post);

% Compute percentage difference 
percent_diff = abs(z_pre_sorted - z_post_sorted) ./ abs(z_pre_sorted) * 100;

% Compute overall mean percentage difference
mean_percent_diff = mean(percent_diff);

% Display results
fprintf('Mean Percentage Difference: %.4f%%\n', mean_percent_diff);



