@echo off
REM compila.bat v2.1/2008 -RCV-
cls

echo                        MCU SELECIONADO: %2
echo.

echo                             COMPILANDO ARQUIVO (C)
msp430-gcc -mmcu=%2 -c -O -O2 -Wall -g %1.c
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
echo             %1.elf      -     Arquivo ELF (carregar)

rem echo.

:FIM
