shared = build/fileargs shell_utils.h
compile = build/fileargs fileargs_gcc.txt gcc

.PHONY = all
all: \
	build/fileargs \
	build/branch \
	build/commit \
	build/hist \
	build/log \
	build/ref \
	build/nav \
	build/proclist \
	build/linetail \
	build/linewrap \
	build/lineno \
	build/lorem \
	build/s

build/bootstrap: fileargs.c
	mkdir -p build
	gcc fileargs.c -o $@

build/fileargs: build/bootstrap fileargs.c shell_utils.h
	build/bootstrap fileargs_gcc.txt gcc fileargs.c -o $@

build/branch: $(shared) branch.c
	$(compile) branch.c -o $@

build/commit: $(shared) commit.c
	$(compile) commit.c -o $@

build/gs: $(shared) gs.c
	$(compile) gs.c -o $@

build/hist: $(shared) hist.c
	$(compile) hist.c -o $@

build/log: $(shared) log.c
	$(compile) log.c -o $@

build/ref: $(shared) ref.c
	$(compile) ref.c -o $@

build/nav: $(shared) nav.c
	$(compile) nav.c -o $@

build/proclist: $(shared) proclist.c
	$(compile) proclist.c -o $@

build/linetail: $(shared) linetail.c
	$(compile) linetail.c -o $@

build/linewrap: $(shared) linewrap.c
	$(compile) linewrap.c -o $@

build/lorem: $(shared) lorem.c
	$(compile) lorem.c -o $@

build/lineno: $(shared) lineno.c
	$(compile) lineno.c -o $@

build/s: $(shared) l.c
	$(compile) l.c -o $@

