@echo off
IF NOT EXIST build mkdir build
call gcc fileargs.c -o build\bootstrap.exe                                          &&^
call build\bootstrap fileargs_gcc.txt gcc fileargs.c -o build\fileargs.exe          &&^
del build\bootstrap.exe                                                             &&^
call build\fileargs fileargs_gcc.txt gcc branch.c   -o build\branch.exe             &&^
call build\fileargs fileargs_gcc.txt gcc commit.c   -o build\commit.exe             &&^
call build\fileargs fileargs_gcc.txt gcc gs.c       -o build\gs.exe                 &&^
call build\fileargs fileargs_gcc.txt gcc hist.c     -o build\hist.exe               &&^
call build\fileargs fileargs_gcc.txt gcc log.c      -o build\log.exe                &&^
call build\fileargs fileargs_gcc.txt gcc ref.c      -o build\ref.exe                &&^
call build\fileargs fileargs_gcc.txt gcc nav.c      -o build\nav.exe                &&^
call build\fileargs fileargs_gcc.txt gcc proclist.c -o build\proclist.exe           &&^
call cp windows/* build                                                             &&^
call cp build/exec.bat build/e.bat                                                  &&^
echo Done.
