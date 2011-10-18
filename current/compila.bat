rem msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c delay.o timers.o lcd6100.o ppm.o analog.o -o %1.o -lm
msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c lcd6100.o delay.o -o %1.o -lm
msp430-objdump -DS %1.o > %1.lst
msp430-objcopy -O ihex %1.o %1.hex
