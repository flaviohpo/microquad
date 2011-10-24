@echo off
REM compila.bat v2.1/2008 -RCV-
cls

echo                        MCU SELECIONADO: %3
echo.

IF NOT EXIST %1.%2 GOTO ERRO1

echo                             COMPILANDO ARQUIVO (C)
msp430-gcc -mmcu=%3 -O -O2 -Wall -g %1.%2 -o %1.o
rem msp430-gcc -mmcu=%3 -O -O2 -Wall -g %1.%2 -o %1.o
IF ERRORLEVEL 1 GOTO ERRO2

echo.
echo                          CRIANDO DESASSEMBLED TEXT
IF EXIST %1.lst del %1.lst > NUL

msp430-objdump -DS %1.o > %1.lst
IF ERRORLEVEL 1 GOTO ERRO2

echo.
echo                         CONVERTENDO ARQUIVO PARA .elf
rem IF EXIST %1.elf del %1.elf > NUL
msp430-objcopy -O ihex %1.o %1.hex 
msp430-objcopy -O elf32-msp430 %1.o %1.elf

IF ERRORLEVEL 1 GOTO ERRO2

GOTO SUCESSO


:ERRO0
echo.
echo                    ERRO! EXTENSAO OU NOME DO ARQUIVO
echo                                INVALIDOS
GOTO FIM


:ERRO1
echo.
echo                     ERRO! O ARQUIVO NAO EXISTE
GOTO FIM


:ERRO2
echo.
echo              ERRO! VERIFIQUE CODIGO DE ORIGEM OU MCU ESCOLHIDO 
GOTO FIM

:SUCESSO
IF EXIST %1.i del %1.i > NUL
echo.
echo          SUCESSO NA COMPILACAO DO PROGRAMA. ARQUIVOS CRIADOS:
echo             %1.lst      -     Disassembled text
echo             %1.hex      -     Arquivo HEX (carregar)
echo             %1.elf      -     Arquivo ELF (carregar)

rem echo.

:FIM
