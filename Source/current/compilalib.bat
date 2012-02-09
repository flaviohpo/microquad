@echo off
rem *** COMPLIB ***
rem ***************
rem
rem Arquivo batch para compilar bibliotecas para o MSP430
rem                                  (C)2010, by JCM Lima
cls

echo                    MICROCONTROLADOR SELECIONADO: %2
echo.

echo                        COMPILANDO O ARQUIVO %1.c
msp430-gcc -mmcu=msp430x2619 -c -O -O2 -Wall -g %1.c
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
echo                     SUCESSO NA COMPILACAO DO PROGRAMA!
echo                           CRIADO ARQUIVO: %1.o

rem echo.

:FIM
