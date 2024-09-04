 ------------------------ xcc Internals documentation ------------------------

# Overall architecture

To be done

# Entities

Others to be done

## Primary AST

What it produced by Lexicalparser *and processed by Lexicalpostparser*.

## Secondary AST

Typed and annotated AST produced by Semantic parser.

## Intermediate representation

Current format is `v.<opcode>.<segment>.<size>`

### Basics

Stack-based.
Forth but with multiple integer types if you may like.

MISC if you would like - what normally would be one 
`v.add.idxcst.tls.i32 iUSomerandomthreadlocalvariable` here would be
`v.lea.dynamic.nearptr iUSomerandomthreadlocalvariable`/
`v.add.relative.nearptr`/
`v.ld.tls.i32`/
`v.add.relative.i32`.

### Opcode
Base operation. 

There also are some special-purpose opcodes like `v.pha`/`v.plb`. Those are to
assist Codegen.

### Segment
Segment to do operation on.
Kinda doubles as addressing mode in some cases.

## Secondary IR (?)

Current draft is `v.<opcode>.<segment>.<size>`

### Basics
Mostly identical to Main IR.

### Opcode
Base operation. 

### Opcode modifiers
- `p`: Pop from stack as in `st` stores and leaves on stack but `st_p` stores
  and removes from stack

### Addressing mode
I know it's stack based but it doesn't mean there won't be fancy composite
instructions like `v.add_p.abs.dataseg.u16 uUSomerandomvariable`

# Cfront

A C++ -> C translator I envision having

# Embedder 

## Someone write this please

# Preprocessor

## General notes

Doesn't handle comments, that's now on Embedder.

## Overall architecture

- Launcher - parses command line arguments, opens input/output files
and goes into Mainloop.
- Mainloop - parses files line-by-line, for each line deciding if it's a preprocessor directive or a normal line.
- Primitives for Macroexpansion.
- Primitives for parsing Directives.
- Primitives for evaulating conditions.
- Primitives for handling interfacing to input and output files
- Various support primitives

## Main loop
`PrHandlefile` preprocesses an opened file. It's what is responsible for
initiating rest of preprocessing

`PrHandleline` preprocesses a single line in a file and returns.

As `#if` allows to suppress lines in output, there's also `PrGetIgnorelines`
to get whether current line should be ignored. Internally obtained from a list
of current `#if`s.

`PrPreprocessdirective` processes a directive.
`PrPreprocesscrucialdirective` does the same, except under suppressed part
of `#if`/`#else`/`#endif`.
Both call to PriPreprocesscrucialdirective and PriPreprocessdirective to
handle directives. Directives that are supposed to be handled even inside 
suppressed part of an `#if`/`#else`/`#endif` are in
PriPreprocesscrucialdirective, rest (like `#define`) is in 
PriPreprocessdirective.

## Defines
There's a tList<tPrMacrodefinition> that stores all macroses currently in use.

## Rest to be done!

# Tokenizer

TkTokenize(), just a switch(fgetc(srcfile)).

# Lexicalpreparser

Needed because typedefs. If not typedefs it wouldn't be necessary, and
actually for quite some time xcc was operating without Lexicalpreparser and
there was a plan to continue like that until end of time but I broke up after
being unable to selfcompile without Lexicalpreparser, so here we are.

There's a list of known typedefs `tList<tString> LxTypesdeclared` as well as
a flag `bool LxUsedeclaredtypeslist` that tells `LxParseBasetype()` to use said
list of typedefs - if it's set, when a possible typedef'd type is absent in
LxTypesdeclared a nullptr will be returned instead of an unresolved type.

# Lexicalparser

There are functions that consume Tokens and return what they are supposed to,
say, LxParseDeclaration parses a declaration and returns it as a parsed node,
recursing to other functions as needed - for typedef it's LxParseBasetype and
LxParseTypeexpression.

There's also one quirk - LxParseExpression and LxParseTypeexpression use the
Fetcher given to them from start to end in one shot and except you to dispose
it unlike other LxParse<something> functions that parse whatever they are 
meant to and except same fetcher to be reused for parsing more declarations,
so you need to do `mtLxFetcher_Fetchuntil_Parenthesized(...,tToken_Semicolon)`
to get the throwaway Fetcher to pass to LxParseExpression.

# Lexicalpostparser

Creates namespaces and binds them to nodes, as well as places parent pointers.
Also resolves Rawvariabledeclaration lexem produced by Lexicalparser.

# Semanticpreparser

Initially was meant as a stage to preprocess Primary AST a bit more but in the
end became a namespace to put various functions into like `SppGeneratetype` or
`SppParsefunctionarguments`.

# Symbolgen

Consists of four subpasses:
• SgParse: Main pass, generates all symbols.
• SgFindunresolvedtypes: Finds unresolved types and registers them into
  `tList<tGType> SgUnresolvedtypes`. For some reason also registers compilable
  structures and structure tag usages and definitions (currently as typedefs).
• SgResolveunresolvedtypes: Resolves said types as well as structure tag
  usages. Currently from GRootnamespace, later possibly from the namespace 
  said type was encountered in. Will have to invesigate.
• SgCompilestructures: Compiles all structures. Does more work than needed
  though.

# Semanticparser

**The thing.** Does everything that makes xcc as C compiler. 
* Annotates Primary AST with expressions
* Resolves symbols
* Allocates local variables
* As well as global variables
* Deals with initializers
* Inserts all sort of implied casts

# Semanticoptimizer

Currently absent.
Intended to do some optimizations that won't be as easily doable on IR.

# IR Generator

Compiles Secondary AST into IR. 

# IR Compiler

Compiles IR into assembly.

# Assembler

Thy usual double-pass assembler except instructions are defined not internally
but in the configuration file like in Telemark Crossassembler.

*If you want to use it as a macroassembler - don't. There's IR Compiler for a
reason*.

Also **there's a dirty hack** - iAsmRepeattimes to get `.times` directive
going.


# Linker

Thy usual configurable linker, just like in cc65 suite.

## Main loop

Does two passes - first generates symbol locations and export symbol
locations, second pass emits into output binary file while applying
relocations.

## Configuration file aka Linkerscript

For now, list of actions linker should do.

Actions that are intented to be present are:
 • `segment` - Emit a segment into the output file.
               Iterates over all files, emitting the segment from modules
               one by one.
 • `align`   - Pad output file to closest address that's multiple of alignment.
 • `pad`     - Pad output file to the given address. Not align.

# Common between Linker and Assembler

## Segments allocated

  id  owner   name        rwx       description
*  0:         .null                  Reserved, do not use.
*  1: crt0    .header     r   init  executable file header, provided by crt0
*  2: crt0    .text       rx  init  crt0 code and read-only data
*  3: crt0    .bss        rw  bss   crt0 writable data segment
*  4: user    .zp         rx  init  Zeropage
*  5: user    .lowcode    rx  init  Nonpageable code
*  6: user    .lowdata    rw  init  Nonpageable data
*  7: user    .code       rx  init  Code segment
*  8: user    .data       rw  init  Data segment
*  9: user    .rodata     r   init  Readonly data segment
* 10: user    .udata      rw  bss   Uninitialized data segment
* 11: user    .farcode    rx  init  Far code segment
* 12: user    .fardata    rw  init  Far uninitialized data segment
* 13: user    .farrodata  r   init  Far readonly data segment
* 14: user    .farbss     rw  bss   Far uninitialized data segment
* 15: user    .lostack    rw  bss   Resident stack
* 16: user    .data_al    rw  init  Page-aligned data (256 bytes commonly)
* 20: user    .constr     r   init  `int (*constructors[])(void)`
                                     Pointers to constructor functions { of
                                     various modules } that are called by libc.
* 21: user    .destr      r   init  `int (*destructors[])(void)`
                                     Same as constructors but destructors.


Note that .code/.data/.rodata/.udata segments may be swapped in and out on
some systems so interrupt handlers and such have to reside in .lowcode and
.lowdata segments.

# vim:tw=78:ts=4:sw=4:et:
