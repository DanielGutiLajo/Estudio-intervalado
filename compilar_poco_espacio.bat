
gcc -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-stack-protector -o Estudio_intervalado.exe Estudio_intervalado.c
strip Estudio_intervalado.exe
