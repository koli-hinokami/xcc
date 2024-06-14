 -------------------------------- xcc Readme --------------------------------
- vim:tw=78:ts=8:sw=0:noet
- if you can't read it in github just read from vim, made for 78 columns 
  monowidth utf-8 syntax=markdown

xcc Retargetable C Compiler is what you may desire to call a toy C Compiler,
and sure it is one being written as one, but like `chibicc` it's also intended
to be used as a real cc for compiling real programs, albeit mostly as a
crosscompiler for development for retrocomputers and retroconsoles.

xcc is also made to target primairly 16-bit banked/segmented architectures 
with plenty of decisions like farpointer support influenced by supporting such
targets.

Goals:
 • Deviating from C standard where necessary
   • For one forward-references to functions declared in the same file are 
     resolved and forward-declarations local to current object file are 
     ignored, instead actual function prototypes are used.
 • First-class retargetablity
   • With final goal of using one instance of xcc installed system-wide to 
     compile programs for every supported architecture
   • Specifically achieved with configuration files for all the tools from
     ircompiler to linker and packer and choosing which one is used with
     targetarchitecture parameter passed *somewhere*.
 • Far pointer support
   • Obvious that it's needed if I'm intending to properly support 16-bit
     targets
 • Forceful boundchecking
   • This time the best bound-checker of all C Compilers!! lol
 • Lambdas and **clojures**
   • Proper clojures in particular won't be easy especially with all the
     tiny/small/medium/compact/big/huge memory models I'll have to deal with,
     but let's hope we'll figure out how to get all the mess of { hidden 
     structs, packing them, depacking them, passing them, allocating them and
     dealing with cases like near function but far clojureargs and far
     function but near clojureargs } going.
 • Proper preprocessor
   • Kinda almost finished by now, even if as two stages of Embedder and
     Preprocessor itself

Late-development goals:
 • Fat pointers
   • Like clojures, we'll have to deal with things like various combinations 
     of pointer size and item size
 • Refactor to be able to run on something like PDP-11/70 with BSD2.9
   • Was intended to be this way from the start, gone Singlestage for
     development simplicity and unlikely to get back without major refactoring
 • cfront and deoxid
 • Optimizer
   • Though not even interstatement one, let alone interprocedural.
     • But who knows what will I achieve
 • *just maybe* Analyzer like gcc's `-fanalyzer`

