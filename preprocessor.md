 ---------- xcc Preprocessor - Manuals and internals documentation ---------- 
- vim:tw=78:ts=4:sw=0:et

# User manual
Someone write this please!

# Internals documentations
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

