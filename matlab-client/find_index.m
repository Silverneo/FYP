function semi_index = find_index(A, fs)

vad = harmfreq_MOLRT_VAD(A, fs);
id_ed = find((vad(1:end-1) == 0) & (vad(2:end) ~= 0));
id_st = find((vad(1:end-1) ~= 0) & (vad(2:end) == 0));
if(all(vad(id_ed(end)+1:end)))
    id_st = [0 ; id_st(1:end-1)];
else
    id_ed = [id_ed; length(vad)];
    id_st = [0; id_st];
end

semi_index = floor((id_ed - id_st) / 2 + id_st);

end