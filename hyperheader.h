#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
//#include <process.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#define nullptr ((void*)0)
#define null 0

#include "cstrings.h"
#include "lists.h"
#include "dictionary.h"
#include "softpipe.h"
#include "basictypes.h"
#include "xcc.h"
#include "xcc-targetconfiguration.h"

/*
	Used namespaces:
		G	compiler Global
		Ln	Launcher
		Tk	Tokenizer
		Lx	Lexer
		Sg	Symbolgen
		Sp	Semantic Parser
		Ig	IR Generator
		Cg	Code Generator
		Lf	Logging Facilities
*/

// Here come macroses - but not now, xcc-preprocess doesnt support them yet.
//   But defining them *is* already present, sòó~...
//   i should get a different main loop though
