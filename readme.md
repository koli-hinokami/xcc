 -------------------------------- xcc Readme --------------------------------
- if you can't read it in github just read from vim, made for 78 columns 
  monowidth utf-8 syntax=markdown

# Current state of hiatus

xcc currently is in hiatus due to main and before the hiatus the only 
contributor of me, Koli Hinokami, getting tired with developing xcc.

One thing for sure is everything is mine and written by me singlehandedly
without assistance from other people.

I got to admit the compiler was pushed *a lot*. It's working very well just 
like I wished, and crosscompilation for a variety of platforms is as easy as
I wanted. Some things like sideeffect infrastructure are lacking though.

As for when xcc will go out of hiatus, I have *no* idea!
For one, and for other I want to keep developing xcc singlehandedly with
keeping things done by others away in xcc-contrib side of things.

If you are thinking of taking xcc in you hands (and I'm sure you do as xcc is
very promising), *pertrain to me* and keep your things as xcc-contrib like I
want to. Some changes will get integrated into master, others will not.

Taking the opportunity, archaic indev versions of xcc are deep into `stable`
branch. `main` still starts from 0.9.8b.

# Information

xcc Retargetable C Compiler is what you may desire to call a toy C Compiler,
and sure it is one being written as one, but like `chibicc` it's also intended
to be used as a real cc for compiling real programs, albeit mostly as a
crosscompiler for development for retrocomputers and retroconsoles.

xcc is also made to target primairly 16-bit banked/segmented architectures 
with plenty of decisions like farpointer support influenced by supporting such
targets.

# Building

`<platform>` is the platform for which you're building.
`cygwin` if building on Cygwin, `debian` if building on Debian.
On other platforms, you're on your own.

Full build: (xcc Crosscompiler, it's libc and installing in path)
```bash
./configure <platform>
make build
sudo make install
make stdlib
sudo make install
```
`sudo` is not needed if you have write permissions to `/etc` and
`/usr/local/bin`. (I *do* in my Cygwin environment)

# Goals
 * Deviating from C standard where necessary
   * For one forward-references to functions declared in the same file are 
     resolved and forward-declarations local to current object file are 
     ignored, instead actual function prototypes are used.
 * First-class retargetablity
   * With final goal of using one instance of xcc installed system-wide to 
     compile programs for every supported architecture
   * Specifically achieved with configuration files for all the tools from
     ircompiler to linker and packer and choosing which one is used with
     targetarchitecture parameter passed *somewhere*.
 * Far pointer support
   * Obvious that it's needed if I'm intending to properly support 16-bit
     targets
 * Forceful boundchecking
   * This time the best bound-checker of all C Compilers!! lol
 * Lambdas and **clojures**
   * Proper clojures in particular won't be easy especially with all the
     tiny/small/medium/compact/big/huge memory models I'll have to deal with,
     but let's hope we'll figure out how to get all the mess of { hidden 
     structs, packing them, depacking them, passing them, allocating them and
     dealing with cases like near function but far clojureargs and far
     function but near clojureargs } going.
 * Proper preprocessor
   * Kinda almost finished by now, even if as two stages of Embedder and
     Preprocessor itself

## Late-development goals
 * Fat pointers
   * Like clojures, we'll have to deal with things like various combinations 
     of pointer size and item size
 * Refactor to be able to run on something like PDP-11/70 with BSD2.9
   * Was intended to be this way from the start, gone Singlestage for
     development simplicity and unlikely to get back without major refactoring
 * cfront and deoxid
 * Optimizer
   * Though not even interstatement one, let alone interprocedural.
     * But who knows what will I achieve
 * *just maybe* Analyzer like gcc's `-fanalyzer`

# vim:tw=78:ts=8:sw=0:noet
