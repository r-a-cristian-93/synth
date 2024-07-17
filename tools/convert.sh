
# resample to 1024 samples 16bit, generate wav and header
./wav2hr.exe 16 ./wav_original/orchestra_flute_4_c2.wav   ./header_orchestra_unsigned_16/flute4.h         ./wav_orchestra_unsigned_16/flute4.wav       flute4       FLUTE4
./wav2hr.exe 16 ./wav_original/orchestra_flute_8_c2.wav   ./header_orchestra_unsigned_16/flute8.h         ./wav_orchestra_unsigned_16/flute8.wav       flute8       FLUTE8
./wav2hr.exe 16 ./wav_original/orchestra_trumpet_c2.wav   ./header_orchestra_unsigned_16/trumpet.h        ./wav_orchestra_unsigned_16/trumpet.wav      trumpet      TRUMPET
./wav2hr.exe 16 ./wav_original/orchestra_violin_c2.wav    ./header_orchestra_unsigned_16/violin.h         ./wav_orchestra_unsigned_16/violin.wav       violin       VIOLIN
./wav2hr.exe 16 ./wav_original/orchestra_clarinet_c2.wav  ./header_orchestra_unsigned_16/clarinet.h       ./wav_orchestra_unsigned_16/clarinet.wav     clarinet     CLARINET
./wav2hr.exe 16 ./wav_original/orchestra_bass_c2.wav      ./header_orchestra_unsigned_16/bass.h           ./wav_orchestra_unsigned_16/bass.wav         bass         BASS

./wav2hr.exe 16 ./wav_original/effect_harpsicord_c2.wav   ./header_effects_unsigned_16/harpsi.h           ./wav_effects_unsigned_16/harpsi.wav         harpsi       HARPSI
./wav2hr.exe 16 ./wav_original/effect_piano_c2.wav        ./header_effects_unsigned_16/piano.h            ./wav_effects_unsigned_16/piano.wav          piano        PIANO
./wav2hr.exe 16 ./wav_original/effect_perc4_c2.wav        ./header_effects_unsigned_16/perc4.h            ./wav_effects_unsigned_16/perc4.wav          perc4        PERC4
./wav2hr.exe 16 ./wav_original/effect_bells_c2.wav        ./header_effects_unsigned_16/bells.h            ./wav_effects_unsigned_16/bells.wav          bells        BELLS
./wav2hr.exe 16 ./wav_original/effect_synth_c2.wav        ./header_effects_unsigned_16/synth.h            ./wav_effects_unsigned_16/synth.wav          synth        SYNTH




# resample to 16bit, generate header
./wav2hc.exe 16 ./wav_original/perc_kick.wav              ./header_perc_unsigned_16_44100/perc_kick.h      none                                         perc_kick    PERC_KICK
./wav2hc.exe 16 ./wav_original/perc_shaker.wav            ./header_perc_unsigned_16_44100/perc_shaker.h    none                                         perc_shaker  PERC_SHAKER
./wav2hc.exe 16 ./wav_original/perc_snare.wav             ./header_perc_unsigned_16_44100/perc_snare.h     none                                         perc_snare   PERC_SNARE
./wav2hc.exe 16 ./wav_original/perc_stick.wav             ./header_perc_unsigned_16_44100/perc_stick.h     none                                         perc_stick   PERC_STICK
