#!/bin/bash
set -euxo pipefail
mkdir -p build
gcc fileargs.c -o build/bootstrap
build/bootstrap fileargs_gcc.txt gcc fileargs.c -o build/fileargs
rm build/bootstrap
build/fileargs fileargs_gcc.txt gcc branch.c   -o build/branch
build/fileargs fileargs_gcc.txt gcc clc.c      -o build/clc
build/fileargs fileargs_gcc.txt gcc commit.c   -o build/commit
build/fileargs fileargs_gcc.txt gcc gs.c       -o build/gs
build/fileargs fileargs_gcc.txt gcc hist.c     -o build/hist
build/fileargs fileargs_gcc.txt gcc log.c      -o build/log
build/fileargs fileargs_gcc.txt gcc ref.c      -o build/ref
build/fileargs fileargs_gcc.txt gcc nav.c      -o build/nav
build/fileargs fileargs_gcc.txt gcc proclist.c -o build/proclist
build/fileargs fileargs_gcc.txt gcc linetail.c -o build/linetail
build/fileargs fileargs_gcc.txt gcc linewrap.c -o build/linewrap
build/fileargs fileargs_gcc.txt gcc lorem.c    -o build/lorem
build/fileargs fileargs_gcc.txt gcc lineno.c   -o build/lineno
cp windows/* build
cp build/exec.bat build/e.bat
cp linux/* build
cp build/exec.sh build/e
echo Done.
