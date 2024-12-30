segment 1  ; .header    - ex~ file headers
segment 2  ; .text      - loader flat
segment 3  ; .bss       - loader bss
segment 4  ; .zp        - Zeropage
segment 5  ; .lowcode   - Nonpageable code
segment 6  ; .lowdata   - Same deal
pad 0x800
segment 7  ; .code      - Transientkernel code
segment 8  ; .data      - Transientkernel data
segment 9  ; .rodata    - Transientkernel readonly data
segment 10 ; .udata     - Transientkernel uninitialized data
align 256
segment 16 ; .data_al   - Page-aligned data. Mostly process table.
symbol GConstructorsStart
segment 20 ; .constr    - Constructors
symbol GConstructorsEnd
symbol GDestructorsStart
segment 21 ; .destr     - Destructors
symbol GDestructorsEnd

