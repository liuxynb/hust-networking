@echo off
:: appname ������
:: inputname �����ļ���
:: outputname ����ļ���
:: resultname �������̨����ض����ļ���

set appname="C:\Users\20963\Desktop\Grade3\���������\lab\lab2\GBN\Debug\GBN.exe"
set inputname="input.txt"
set outputname="output.txt"
set resultname="result_gbn.txt"

for /l %%i in (1,1,10) do (
    echo Test %appname% %%i:
    %appname% > %resultname% 2>&1
    fc /N %inputname% %outputname%
)
pause