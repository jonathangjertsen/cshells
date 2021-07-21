@echo off
IF NOT EXIST build mkdir build
call gcc fileargs.c -o build\bootstrap.exe
call build\bootstrap fileargs_gcc.txt gcc fileargs.c -o build\fileargs.exe
del build\bootstrap.exe
call build\fileargs fileargs_gcc.txt gcc runifnewer.c -o build\runifnewer.exe
call build\runifnewer branch.c      build\branch.exe    build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer commit.c      build\commit.exe    build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer gs.c          build\gs.exe        build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer hist.c        build\hist.exe      build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer log.c         build\log.exe       build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer ref.c         build\ref.exe       build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer nav.c         build\nav.exe       build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer proclist.c    build\proclist.exe  build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer linetail.c    build\linetail.exe  build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer linewrap.c    build\linewrap.exe  build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer lorem.c       build\lorem.exe     build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer lineno.c      build\lineno.exe    build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call build\runifnewer listfiles.c   build\listfiles.exe build\fileargs fileargs_gcc.txt gcc [SRC] -o [DEST]
call cp windows/* build
call cp build/exec.bat build/e.bat
call cp linux/* build
call cp linux/exec.sh linux/e.sh
echo Done.
