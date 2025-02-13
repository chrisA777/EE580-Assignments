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

