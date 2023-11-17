@echo off
:: appname 程序名
:: inputname 输入文件名
:: outputname 输出文件名
:: resultname 程序控制台输出重定向文件名

set appname="C:\Users\20963\Desktop\Grade3\计算机网络\lab\lab2\TCP\Debug\TCP.exe"
set inputname="input.txt"
set outputname="output.txt"
set resultname="result_tcp.txt"

for /l %%i in (1,1,10) do (
    echo Test %appname% %%i:
    %appname% > %resultname% 2>&1
    fc /N %inputname% %outputname%
)
pause