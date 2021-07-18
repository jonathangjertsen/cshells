IF NOT EXIST build mkdir build
call gcc fileargs.c -o build\bootstrap.exe                                    &&^
call build\bootstrap fileargs_gcc.txt gcc -v branch.c   -o build\branch.exe   &&^
call build\bootstrap fileargs_gcc.txt gcc -v commit.c   -o build\commit.exe   &&^
call build\bootstrap fileargs_gcc.txt gcc -v gs.c       -o build\gs.exe       &&^
call build\bootstrap fileargs_gcc.txt gcc -v fileargs.c -o build\fileargs.exe &&^
del build\bootstrap.exe &&^
echo Done.
