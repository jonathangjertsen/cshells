IF NOT EXIST build mkdir build
set FLAGS=-Werror -Wall -Os -s -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-math-errno -fno-ident -Wl,--build-id=none
call gcc gs.c %FLAGS% -o build/gs.exe              &&^
call gcc commit.c %FLAGS% -o build/commit.exe
