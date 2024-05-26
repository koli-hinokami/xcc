# Makefile for XCC Retargetable C Compiler

CC = gcc
CFLAGS = -g -O0 -Wall -Wno-char-subscripts \
 -D__timestamp__=\"$(TIMESTAMP)\" -D__buildlab__=\"kolishome\" \
 -DqszGVersion=\"1.0.1.0\" \
 -DqvGDebug -DqvGTrace -D_qvGTraceexpressions -D_qvGIgnorefatals
#-D__buildlab__=\"$(USER)\"
TIMESTAMP = `date +%y%m%d-%H%M`
DEPS = \
	hyperheader.h \
	xcc.h \
	softpipe.h \
	cstrings.h \
	basictypes.h \
	lists.h \
	Makefile


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

xcc-all: xcc xcc-singlestage xcc-embedder xcc-preprocess xcc-tokenizer 
xcc-singlestage: \
	xcc-singlestage.c \
	xcc-singlestage-tokenizer.c \
	xcc-singlestage-lexicalparser.c \
	xcc-singlestage-semanticpreparser.c \
	xcc-singlestage-symbolgen.c \
	xcc-singlestage-semanticparser.c \
	xcc-singlestage-irmaker.c \
	xcc-singlestage-fetcher.c \
	xcc-singlestage-launcher.c

%: %.c $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

