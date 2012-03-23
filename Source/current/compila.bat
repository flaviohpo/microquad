rem quad current
msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c lcd6100.o menu.o delay.o eeprom.o i2c.o analog.o usefullibs.o -o %1.o -lm

rem vibration test
rem msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c mma7455.o delay.o lcd6100.o i2c.o -o %1.o -lm

rem motor tester 
rem msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c delay.o eeprom.o i2c.o analog.o uart.o -o %1.o -lm

rem msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c  -o %1.o -lm

rem msp430-gcc -mmcu=%2 -O  -O2 -Wall -g %1.c delay.o i2c.o lcd6100.o analog.o miniquad.o misc.o hardware_config.o -o %1.o -lm

msp430-objdump -DS %1.o > %1.lst
msp430-objcopy -O ihex %1.o %1.hex
