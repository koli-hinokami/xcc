# Makefile for XCC Retargetable C Compiler

CC = gcc
CFLAGS = -g -O0 -D__timestamp__=\"$(TIMESTAMP)\" -D__buildlab__=\"kolishome\" \
-DqvGDebug -D_qvGTrace
#-D__buildlab__=\"$(USER)\"
TIMESTAMP = `date +%y%m%d-%H%M`
DEPS = hyperheader.h xcc.h softpipe.h cstrings.h basictypes.h lists.h

#build: xcc xcc-preprocess xcc-tokenizer xcc-lexer
#run: build
#	./xcc ./xcc.c ./xcc-preprocess.c ./xcc-tokenizer.c ./xcc-lexer.c 
#	
#debug: build
#	gdb ./xcc ./xcc.c
.PHONY: clean rebuild
build: xcc-all
rebuild: clean build
clean: 
	rm -f xcc
	rm -f xcc-singlestage
	rm -f xcc-embedder
	rm -f xcc-preprocess
	rm -f xcc-tokenizer
	rm -f xcc-lexer
run: build
#	cpp ./xcc-singlestage.c -o ./xcc-singlestage.cpr
	./xcc -m ./helloworld.c
#	./xcc -m ./xcc-singlestage.c
#	./xcc-singlestage ./xcc-singlestage.cpr
debug: build
	gdb ./xcc-singlestage ./xcc-singlestage.c

xcc-test: xcc-all
	./xcc -m xcc-singlestage.c

xcc-all: xcc xcc-singlestage xcc-embedder xcc-preprocess xcc-tokenizer xcc-lexer
xcc-singlestage: \
	xcc-singlestage.c \
	xcc-singlestage-tokenizer.c \
	xcc-singlestage-lexicalparser.c

%: %.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<
