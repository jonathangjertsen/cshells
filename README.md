# Some shell utilities written in C

Just for fun and to see how easy it is to use C as a scripting language.

## How to build

On Windows, get the gcc toolchain from [winlibs.com][winlibs.com] and make sure `gcc` is in the PATH.
Check that `gcc --version` is 11 or later, since the source uses some recent features.
Then run `build.bat`.
Add the `build` directory to PATH to use the utilities that were just built.

## List of utilities

### `branch`

Shows the current git branch.

Usage: `branch`

### `clc`

Evaluates and prints the expression.

Supported operators: + - * / % & | ^ ( )
Supported number formats: decimal, hex, binary, octal, float

Usage: `clc <expression>`

### `commit`

Interactive git add and commit helper

Goes through each file with changes, for each file prompts whether to:

* `[A]dd` (git add)
* `[P]atch` (git add --patch)
* `[D]iff` (git diff)
* `[S]kip` (do nothing)
* `abor[T]` exit the process
* `[M]essage` skip remaining files and go to commit message

Usage: `commit`

### `fileargs`

Applies command line arguments from a file to the command.
Newline is interpreted as a space.
If `--verbose` is set, prints the generated commands.
If `--after` is set, arguments from the file will be placed after the extra args

Usage: `fileargs <file.txt> <command> --any --extra --args`

### `e`

Runs a command and executes the output of it as a separate command
Needed for some programs (`hist`, `nav`) to break out of the child process

Usage: `e <program>`

### `gs`

Prints `git status`

Usage: `gs`

### `hist`

Prints the command needed to produce the line history.

Usage: `e hist`

### `lineno`

Adds line numbers to stdin.

Usage: `command | lineno`

### `linetail`

Prints the `n` last characters in each line from stdin

Usage: `command | linetail <n>`

### `linewrap`

Wraps the input from standard in to `n` characters

Usage: `command | linewrap <n>`

### `log`

Prints up to `n` lines from the git log

Usage: `log <n>`

### `lorem`

Prints `n` lines of lorem ipsum.

Usage: `lorem <n>`

### `nav`

Prints the command needed to move to a `$NAVDIR/dir`,
where `$NAVDIR` is an environment variable.

Usage: `e nav dir`

### `proclist`

Prints the list of running processes, optionally with a filter
to only include processes with a memory footprint exceeding `n` megabytes

Usage: `proclist -t <n>`

### `ref`

Prints up to `n` lines from the git ref log

Usage: `ref <n>`


[winlibs.com]: https://winlibs.com/
