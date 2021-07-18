IF NOT EXIST build mkdir build
call gcc fileargs.c -o build\bootstrap.exe                                    &&^
call build\bootstrap fileargs_gcc.txt gcc -v fileargs.c -o build\fileargs.exe &&^
del build\bootstrap.exe                                                       &&^
call build\fileargs fileargs_gcc.txt gcc -v branch.c   -o build\branch.exe    &&^
call build\fileargs fileargs_gcc.txt gcc -v commit.c   -o build\commit.exe    &&^
call build\fileargs fileargs_gcc.txt gcc -v gs.c       -o build\gs.exe        &&^
call build\fileargs fileargs_gcc.txt gcc -v hist.c     -o build\hist.exe      &&^
call build\fileargs fileargs_gcc.txt gcc -v log.c      -o build\log.exe       &&^
call build\fileargs fileargs_gcc.txt gcc -v ref.c      -o build\ref.exe       &&^
call cp windows/* build                                                       &&^
echo Done.
