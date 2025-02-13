fid = fopen('data.h','w');


%% FIR FILTER
%% B
fprintf(fid,['#define N_FIR_B %d' char([13 10])], length(b_fir));
fwrite(fid,char([13 10]),'uchar');

fwrite(fid,'float b_fir[] = { ','uchar');
for ct = 1:length(b_fir)-1
    fprintf(fid,'%.7ff, ', single(b_fir(ct)));
end
fprintf(fid,'%.7ff', single(b_fir(end)));
fwrite(fid,[' };' char([13 10])],'uchar');
fwrite(fid,char([13 10]),'uchar');
%% A
fprintf(fid,['#define N_FIR_A %d' char([13 10])], length(a_fir));
fwrite(fid,char([13 10]),'uchar');
fwrite(fid,'float a_fir[] = { ','uchar');
for ct = 1:length(a_fir)-1
    fprintf(fid,'%.7ff, ', single(a_fir(ct)));
end
fprintf(fid,'%.7ff', single(a_fir(end)));
fwrite(fid,[' };' char([13 10])],'uchar');
fwrite(fid,char([13 10]),'uchar');


fclose(fid);
