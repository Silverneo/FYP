% script for speech recognition on the cloud
% note the server must be turned on first
% Author @ Zhang Chunmeng
% Last Modified @ 07/02/15


clear all;
close all;
clc

[Y, fs, nbits] = wavread('test_origin.wav');

sil_index = int32(find_index(Y, fs) * 0.01 * 16000 * 2);

DATALEN = 1024;
PORT = 4950; % port number for the server process is listening
SERVERIP = '128.199.123.155'; % my server ip address
Y = typecast(int16(Y * 2^15), 'uint8');
%N = size(Y, 1);

t = tcpip(SERVERIP, PORT, 'NetworkRole', 'client', 'OutputBufferSize', 1024);

utt_start = sil_index(1);

for utt_end = sil_index(2:end)'
    
    N = (utt_end - utt_start);
    
    fopen(t);
    
    fwrite(t, typecast(int32(N), 'uint8'));
    
    ii = 1;
    tic;
    % send wav file using packets
    while(ii <= N)
        if (N - ii + 1 < DATALEN)
            slen = N - ii + 1;
        else
            slen = DATALEN;
        end
        fwrite(t, Y(utt_start + ii : utt_start + ii + slen - 1));
        ii = ii + slen;
        nbytes = 0;
        while nbytes == 0
            nbytes = t.BytesAvailable;
        end
        hyp = '==>';
        while nbytes % read hypothesis text returned from server
            [temp, n] = fread(t, nbytes, 'uint8');
            hyp = [hyp temp'];
            nbytes = nbytes - n;
        end
        %disp(hyp); % display the result
    end
    disp(hyp);
    toc;
    fclose(t);
    
    utt_start = utt_end;
end