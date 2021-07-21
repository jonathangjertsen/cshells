@echo off
FOR /F "tokens=* USEBACKQ" %%F IN (`%*`) DO (SET command=%%F)
%command%
