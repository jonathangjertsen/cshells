shared = build/fileargs shell_utils.h
compile = build/fileargs fileargs_gcc.txt gcc

.PHONY = all
all: \
	build/fileargs \
	build/branch \
	build/clc \
	build/commit \
	build/hist \
	build/log \
	build/ref \
	build/nav \
	build/proclist \
	build/linetail \
	build/linewrap \
	build/lineno \
	build lorem \
	build runifnewer \
	build listfiles

build/bootstrap: fileargs.c
	gcc fileargs.c -o $@

build/fileargs: build/bootstrap fileargs.c shell_utils.h
	$(compile) fileargs.c -o $@

build/branch: $(shared) branch.c
	$(compile) branch.c -o $@

build/clc: $(shared) clc.c
	$(compile) clc.c -o $@ -lm

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

build/runifnewer: $(shared) runifnewer.c
	$(compile) runifnewer.c -o $@

build/listfiles: $(shared) listfiles.c
	$(compile) listfiles.c -o $@

