IF NOT EXIST build mkdir build
set FLAGS=-Werror -Wall -Os -s
gcc gs.c %FLAGS% -o build/gs.exe              &&^
gcc commit.c %FLAGS% -o build/commit.exe
