//	Header file for XCC Retargetable C Compiler
enum tTokentype {			
	tToken_Null                     = 0 ,   //      Stringless tokens
	tToken_Memberbypointer          = 1 ,   //->
	tToken_Namespace                = 2 ,   //::
	tToken_Increment                = 3 ,   //++
	tToken_Decrement                = 4 ,   //--
	tToken_Openparentheses          = 5 ,   //(
	tToken_Closeparentheses         = 6 ,   //)
	tToken_Openbrackets             = 7 ,   //[
	tToken_Closebrackets            = 8 ,   //]
	tToken_Opencurlybraces          = 9 ,   //{
	tToken_Closecurlybraces         = 10,   //}
	tToken_Dot                      = 11,   //.
	tToken_Logicalnot               = 12,   //!
	tToken_Bitwisenot               = 13,   //~
	tToken_Bitwiseand               = 14,   //&
	tToken_Plus                     = 15,   //+
	tToken_Minus                    = 16,   //-
	tToken_Multiply                 = 17,   //*
	tToken_Divide                   = 18,   ///
	tToken_Modulo                   = 19,   //%
	tToken_Shiftleft                = 20,   //<<
	tToken_Shiftright               = 21,   //>>
	tToken_Lessthan                 = 22,   //<
	tToken_Greaterthan              = 23,   //>
	tToken_Lessequal                = 24,   //<=
	tToken_Greaterequal             = 25,   //>=
	tToken_Threewaycomparison       = 26,   //<=>
	tToken_Assign                   = 27,   //=
	tToken_Equals                   = 28,   //==
	tToken_Notequal                 = 29,   //!=
	tToken_Bitwisexor               = 30,   //^
	tToken_Bitwiseor                = 31,   //|
	tToken_Logicaland               = 32,   //&&
	tToken_Logicalor                = 33,   //||
	tToken_Expressionif             = 34,   //?
	tToken_Colon                    = 35,   //:
	tToken_Addassign                = 36,   //+=
	tToken_Substractassign          = 37,   //-=
	tToken_Multiplyassign           = 38,   //*=
	tToken_Divideassign             = 39,   ///=
	tToken_Moduloassign             = 40,   //%=
	tToken_Shiftleftassign          = 41,   //<<=
	tToken_Shiftrightassign         = 42,   //>>=
	tToken_Bitwiseandassign         = 43,   //&=
	tToken_Bitwisexorassign         = 44,   //^=
	tToken_Bitwiseorassign          = 45,   //|=
	tToken_Comma                    = 46,   //,
	tToken_Semicolon                = 47,   //;
	tToken_Ellipsis                 = 48,   //...
	tToken_Integerconstant          = 96,   //
	tToken_Whitespace               = 127,  //
	//tToken_                       = 47,   //      Keywords
	tToken_Keywordalignas           = 128,  //alignas
	tToken_Keywordalignof           = 129,  //alignof
	tToken_Keywordauto              = 130,  //auto
	tToken_Keywordbool              = 131,  //bool
	tToken_Keywordbreak             = 132,  //break
	tToken_Keywordcase              = 133,  //case
	tToken_Keywordchar              = 134,  //char
	tToken_Keywordconst             = 135,  //const
	tToken_Keywordconstexpr         = 136,  //constexpr
	tToken_Keywordcontinue          = 137,  //continue
	tToken_Keyworddefault           = 138,  //default
	tToken_Keyworddo                = 139,  //do
	tToken_Keyworddouble            = 140,  //double
	tToken_Keywordelse              = 141,  //else
	tToken_Keywordenum              = 142,  //enum
	tToken_Keywordextern            = 143,  //extern
	tToken_Keywordfalse             = 144,  //false
	tToken_Keywordfloat             = 145,  //float
	tToken_Keywordfor               = 146,  //for
	tToken_Keywordgoto              = 147,  //goto
	tToken_Keywordif                = 148,  //if
	tToken_Keywordinline            = 149,  //inline
	tToken_Keywordint               = 150,  //int
	tToken_Keywordlong              = 151,  //long
	tToken_Keywordnullptr           = 152,  //nullptr
	tToken_Keywordregister          = 153,  //register
	tToken_Keywordrestrict          = 154,  //restrict
	tToken_Keywordreturn            = 155,  //return
	tToken_Keywordshort             = 156,  //short
	tToken_Keywordsigned            = 157,  //signed
	tToken_Keywordsizeof            = 158,  //sizeof
	tToken_Keywordstatic            = 159,  //static
	tToken_Keywordstatic_assert     = 160,  //static_assert
	tToken_Keywordstruct            = 161,  //struct
	tToken_Keywordswitch            = 162,  //switch
	tToken_Keywordthread_local      = 163,  //thread_local
	tToken_Keywordtrue              = 164,  //true
	tToken_Keywordtypedef           = 165,  //typedef
	tToken_Keywordtypeof            = 166,  //typeof
	tToken_Keywordtypeof_unqual     = 167,  //typeof_unqual
	tToken_Keywordunion             = 168,  //union
	tToken_Keywordunsigned          = 169,  //unsigned
	tToken_Keywordvoid              = 170,  //void
	tToken_Keywordvolatile          = 171,  //volatile
	tToken_Keywordwhile             = 172,  //while
	tToken_Keywordint8t             = 173,  //int8_t
	tToken_Keywordint16t            = 174,  //int16_t
	tToken_Keywordint32t            = 175,  //int32_t
	tToken_Keywordint64t            = 176,  //int64_t
	tToken_Keyworduint8t            = 177,  //uint8_t
	tToken_Keyworduint16t           = 178,  //uint16_t
	tToken_Keyworduint32t           = 179,  //uint32_t
	tToken_Keyworduint64t           = 180,  //uint64_t
	tToken_Keywordsizet             = 181,  //size_t
	//tToken_                       = 256,  //      String tokens
	tToken_Identifier               = 256,  //Generic identifier
	tToken_String                   = 257,  //String
	tToken_Charaterconstant         = 258,  //Charater constant
	//tToken_                       = 512,  //      Lexems
	tLexem_Declarationlist          = 512,  //List of declarations - root of program
	tLexem_Assign                   = 513,  //Assignment statement
	tLexem_Comma                    = 514,  //Comma 'operator' - rather specialpurpose
	tLexem_Expressionif             = 515,  //
	tLexem_Logicalor                = 516,  //
	tLexem_Logicaland               = 517,  //
	tLexem_Equals                   = 518,  //
	tLexem_Notequal                 = 519,  //
	tLexem_Greaterequal             = 520,  //
	tLexem_Lessequal                = 521,  //
	tLexem_Greaterthan              = 522,  //
	tLexem_Lessthan                 = 523,  //
	tLexem_Bitwiseor                = 524,  //
	tLexem_Bitwisexor               = 525,  //
	tLexem_Bitwiseand               = 526,  //
	tLexem_Shiftleft                = 527,  //
	tLexem_Shiftright               = 528,  //
	tLexem_Add                      = 529,  //
	tLexem_Substract                = 530,  //
	tLexem_Typecast                 = 531,  //
	tLexem_Variabledeclaration      = 532,  //
	tLexem_Functiondeclaration      = 533,  //
	tLexem_Blockstatement           = 534,  //
	tLexem_Increment                = 535,  //
	tLexem_Decrement                = 536,  //
	tLexem_Unaryplus                = 537,  //
	tLexem_Negation                 = 538,  //
	tLexem_Logicalnot               = 539,  //
	tLexem_Bitwisenot               = 540,  //
	tLexem_Dereference              = 541,  //
	tLexem_Addressof                = 542,  //
	tLexem_Sizeof                   = 543,  //
	tLexem_Typeexpression           = 544,  //
	tLexem_Expressionstatement      = 545,  //
	tLexem_Integerconstant          = 546,  //
	tLexem_Functioncall             = 547,  //
	tLexem_Structuremember          = 548,  //`.`
	tLexem_Memberbypointer          = 549,  //`->`
	tLexem_Compoundliteral          = 550,  //
	tLexem_Arrayindex               = 551,  //
	tLexem_Postincrement            = 552,  //
	tLexem_Postdecrement            = 553,  //
	tLexem_Identifier               = 554,  //identifier
	tLexem_Typedefinition           = 555,  //typedef basetype typeexpr;
	tLexem_Structdefinition         = 556,  //struct type
	tLexem_Returnstatement          = 557,  //return expr;
	tLexem_Ifstatement              = 558,  //
	tLexem_Nullpointer              = 559,  //
	tLexem_Booleantrue              = 560,  //
	tLexem_Booleanfalse             = 561,  //
	tLexem_Whilestatement           = 562,  //
	tLexem_Forstatement             = 563,  //
	tLexem_Nulldeclaration          = 564,  //
	tLexem_Nullexpression           = 565,  //
	tLexem_Multiply                 = 566,  //
	tLexem_Divide                   = 567,  //
	tLexem_Modulo                   = 568,  //
	tLexem_Switchstatement          = 569,  //
	tLexem_Switchcase               = 570,  //
	tLexem_Switchdefault            = 571,  //
	tLexem_Goto                     = 572,  //
	tLexem_Breakstatement           = 573,  //
	tLexem_Constmodifier            = 574,  //
	tLexem_Volatilemodifier         = 575,  //
	tLexem_Restrictmodifier         = 576,  //
	tLexem_Numericconstant          = 577,  //
	tLexem_Stringconstant           = 578,  //
	tLexem_Rawvariabledeclaration   = 579,  //
	tLexem_Namespace                = 580,  //
	tLexem_Using                    = 581,  //
	tLexem_Ellipsis                 = 582,  //
	//tLexem_                       = 582,  //
	//tToken_                       = 768,  //      String lexems
	//tToken_                       = 1024, //      Second AST lexems
	tSplexem_Declarationlist        = 1024, // Same as tLexem_Declaration
	tSplexem_Integerconstant        = 1025, // 
	tSplexem_Symbol                 = 1026, //
	tSplexem_Nulldeclaration        = 1027, //
	tSplexem_Blockstatement         = 1028, //
	tSplexem_Functiondeclaration    = 1029, //
	tSplexem_Returnstatement        = 1030, //
	tSplexem_Variabledeclaration    = 1031, //
	tSplexem_Nullexpression         = 1032, //
	tSplexem_Arraytopointer         = 1033, //
	tSplexem_Addition               = 1034, //
	tSplexem_Substraction           = 1035, //
	tSplexem_Dereference            = 1036, //
	tSplexem_Structuremember        = 1037, //
	tSplexem_Identifier             = 1038, //
	tSplexem_Cast                   = 1039, //Only casts types by size
	tSplexem_Expressionstatement    = 1040, //Only casts types by size
	tSplexem_Stringconstant         = 1041, //
	tSplexem_Comma                  = 1042, //
	tSplexem_Functioncall           = 1043, //
	tSplexem_Multiplication         = 1044, //
	tSplexem_Division               = 1045, //
	tSplexem_Modulo                 = 1046, //
	tSplexem_Ifstatement            = 1047, //
	tSplexem_Whilestatement         = 1048, //
	tSplexem_Forstatement           = 1049, //
	tSplexem_Switchstatement        = 1050, //
	tSplexem_Breakstatement         = 1051, //
	tSplexem_Equality               = 1052, //
	tSplexem_Nonequality            = 1053, //
	tSplexem_Assign                 = 1054, //
	//tSplexem_                     = ,     //
	//tToken_                       = 1280, //      Second AST string lexems
	//                                      //        Actually are deprecated
	//tToken_                       = 1536, //      IR instructions
	tInstruction_Cnop               = 1536, //Clash nop - used internally
	tInstruction_Nop                = 1537, //No operation
	tInstruction_Jumptrue           = 1538, //Jump if true
	tInstruction_Jumpfalse          = 1539, //Jump if false
	tInstruction_Debugbreak         = 1540, //Break into debugger
	tInstruction_Allocatestorage    = 1541, //To be used like `variable uint32_t pointer_localstorage testarray: times 5 allocatestorage.uint32` (looks awesome though, might wanna make smth like that in flatassembler)
	tInstruction_Systemcall         = 1542, //System call. Still have to figure out how to call into kernel if code is compiled with calculation stack but syscall excepts registers. Might be just RTL call.
	tInstruction_Definevalue        = 1543, //Just for strings. Use as `definevalue.char 'a'`
	tInstruction_Enterframe         = 1544, //Enter stack frame
	tInstruction_Constant           = 1545, //Push an immediate to calculation stack
	tInstruction_Return             = 1546, //Return from function. Issue after `leave`.
	tInstruction_Leaveframe         = 1547, //Leave stack frame
	tInstruction_Prereturn          = 1548, //Return value from function. Issue before `leave`.
	tInstruction_Loadindirect       = 1549, //Load value by pointer provided on calculation stack.
	tInstruction_Loadaddress        = 1550, //Load effective address of an instruction.
	tInstruction_Indexfp            = 1551, //Indexes current pointer by frame pointer.
	tInstruction_Pushleft           = 1552, // -.- Those two are used to assist codegen for platforms that can't use two stacks like IR excepts to be able to and use machine stack as both stacks.
	tInstruction_Popright           = 1553, // -'
	tInstruction_Add                = 1554, // -.- Exactly what you would except.
	tInstruction_Substract          = 1555, // -' 
	tInstruction_Drop               = 1556, //For expression statements. Ignore produced value.
	tInstruction_Pushargument       = 1557, //Throw function argument just evaluated to stack.
	tInstruction_Call               = 1558, //Call function.
	// v_ld_ind.T segment
	// v_st_ind.T segment
	//tToken_                       = 1792,         //      To be not used
	//tToken_                       = 2048,         //      Machine instructions?
};
typedef short eTokentype,tTokentype;
char *TokenidtoName[]={
	"tToken_Null                 ",// 0 ,
	"tToken_Memberbypointer      ",// 1 ,
	"tToken_Namespace            ",// 2 ,
	"tToken_Increment            ",// 3 ,
	"tToken_Decrement            ",// 4 ,
	"tToken_Openparentheses      ",// 5 ,
	"tToken_Closeparentheses     ",// 6 ,
	"tToken_Openbrackets         ",// 7 ,
	"tToken_Closebrackets        ",// 8 ,
	"tToken_Opencurlybraces      ",// 9 ,
	"tToken_Closecurlybraces     ",// 10,
	"tToken_Dot                  ",// 11,
	"tToken_Logicalnot           ",// 12,
	"tToken_Bitwisenot           ",// 13,
	"tToken_Bitwiseand           ",// 14,
	"tToken_Plus                 ",// 15,
	"tToken_Minus                ",// 16,
	"tToken_Multiply             ",// 17,
	"tToken_Divide               ",// 18,
	"tToken_Modulo               ",// 19,
	"tToken_Shiftleft            ",// 20,
	"tToken_Shiftright           ",// 21,
	"tToken_Lessthan             ",// 22,
	"tToken_Greaterthan          ",// 23,
	"tToken_Lessequal            ",// 24,
	"tToken_Greaterequal         ",// 25,
	"tToken_Threewaycomparison   ",// 26,
	"tToken_Assign               ",// 27,
	"tToken_Equals               ",// 28,
	"tToken_Notequal             ",// 29,
	"tToken_Bitwisexor           ",// 30,
	"tToken_Bitwiseor            ",// 31,
	"tToken_Logicaland           ",// 32,
	"tToken_Logicalor            ",// 33,
	"tToken_Expressionif         ",// 34,
	"tToken_Colon                ",// 35,
	"tToken_Addassign            ",// 36,
	"tToken_Substractassign      ",// 37,
	"tToken_Multiplyassign       ",// 38,
	"tToken_Divideassign         ",// 39,
	"tToken_Moduloassign         ",// 40,
	"tToken_Shiftleftassign      ",// 41,
	"tToken_Shiftrightassign     ",// 42,
	"tToken_Bitwiseandassign     ",// 43,
	"tToken_Bitwisexorassign     ",// 44,
	"tToken_Bitwiseorassign      ",// 45,
	"tToken_Comma                ",// 46,
	"tToken_Semicolon            ",// 47,
	"tToken_Undefined            ",// 48 
	"tToken_Undefined            ",// 49 
	"tToken_Undefined            ",// 50 
	"tToken_Undefined            ",// 51 
	"tToken_Undefined            ",// 52 
	"tToken_Undefined            ",// 53 
	"tToken_Undefined            ",// 54 
	"tToken_Undefined            ",// 55 
	"tToken_Undefined            ",// 56 
	"tToken_Undefined            ",// 57 
	"tToken_Undefined            ",// 58 
	"tToken_Undefined            ",// 59 
	"tToken_Undefined            ",// 60 
	"tToken_Undefined            ",// 61 
	"tToken_Undefined            ",// 62 
	"tToken_Undefined            ",// 63 
	"tToken_Undefined            ",// 64 
	"tToken_Undefined            ",// 65 
	"tToken_Undefined            ",// 66 
	"tToken_Undefined            ",// 67 
	"tToken_Undefined            ",// 68 
	"tToken_Undefined            ",// 69 
	"tToken_Undefined            ",// 70 
	"tToken_Undefined            ",// 71 
	"tToken_Undefined            ",// 72 
	"tToken_Undefined            ",// 73 
	"tToken_Undefined            ",// 74 
	"tToken_Undefined            ",// 75 
	"tToken_Undefined            ",// 76 
	"tToken_Undefined            ",// 77 
	"tToken_Undefined            ",// 78 
	"tToken_Undefined            ",// 79 
	"tToken_Undefined            ",// 80 
	"tToken_Undefined            ",// 81 
	"tToken_Undefined            ",// 82 
	"tToken_Undefined            ",// 83 
	"tToken_Undefined            ",// 84 
	"tToken_Undefined            ",// 85 
	"tToken_Undefined            ",// 86 
	"tToken_Undefined            ",// 87 
	"tToken_Undefined            ",// 88 
	"tToken_Undefined            ",// 89 
	"tToken_Undefined            ",// 90 
	"tToken_Undefined            ",// 91 
	"tToken_Undefined            ",// 92 
	"tToken_Undefined            ",// 93 
	"tToken_Undefined            ",// 94 
	"tToken_Undefined            ",// 95 
	"tToken_Integerconstant      ",// 96 
	"tToken_Undefined            ",// 97 
	"tToken_Undefined            ",// 98 
	"tToken_Undefined            ",// 99 
	"tToken_Undefined            ",// 100
	"tToken_Undefined            ",// 101
	"tToken_Undefined            ",// 102
	"tToken_Undefined            ",// 103
	"tToken_Undefined            ",// 104
	"tToken_Undefined            ",// 105
	"tToken_Undefined            ",// 106
	"tToken_Undefined            ",// 107
	"tToken_Undefined            ",// 108
	"tToken_Undefined            ",// 109
	"tToken_Undefined            ",// 110
	"tToken_Undefined            ",// 111
	"tToken_Undefined            ",// 112
	"tToken_Undefined            ",// 113
	"tToken_Undefined            ",// 114
	"tToken_Undefined            ",// 115
	"tToken_Undefined            ",// 116
	"tToken_Undefined            ",// 117
	"tToken_Undefined            ",// 118
	"tToken_Undefined            ",// 119
	"tToken_Undefined            ",// 120
	"tToken_Undefined            ",// 121
	"tToken_Undefined            ",// 122
	"tToken_Undefined            ",// 123
	"tToken_Undefined            ",// 124
	"tToken_Undefined            ",// 125
	"tToken_Undefined            ",// 126
	"tToken_Whitespace           ",// 127
	"tToken_Keywordalignas       ",// 128
	"tToken_Keywordalignof       ",// 129
	"tToken_Keywordauto          ",// 130
	"tToken_Keywordbool          ",// 131
	"tToken_Keywordbreak         ",// 132
	"tToken_Keywordcase          ",// 133
	"tToken_Keywordchar          ",// 134
	"tToken_Keywordconst         ",// 135
	"tToken_Keywordconstexpr     ",// 136
	"tToken_Keywordcontinue      ",// 137
	"tToken_Keyworddefault       ",// 138
	"tToken_Keyworddo            ",// 139
	"tToken_Keyworddouble        ",// 140
	"tToken_Keywordelse          ",// 141
	"tToken_Keywordenum          ",// 142
	"tToken_Keywordextern        ",// 143
	"tToken_Keywordfalse         ",// 144
	"tToken_Keywordfloat         ",// 145
	"tToken_Keywordfor           ",// 146
	"tToken_Keywordgoto          ",// 147
	"tToken_Keywordif            ",// 148
	"tToken_Keywordinline        ",// 149
	"tToken_Keywordint           ",// 150
	"tToken_Keywordlong          ",// 151
	"tToken_Keywordnullptr       ",// 152
	"tToken_Keywordregister      ",// 153
	"tToken_Keywordrestrict      ",// 154
	"tToken_Keywordreturn        ",// 155
	"tToken_Keywordshort         ",// 156
	"tToken_Keywordsigned        ",// 157
	"tToken_Keywordsizeof        ",// 158
	"tToken_Keywordstatic        ",// 159
	"tToken_Keywordstatic_assert ",// 160
	"tToken_Keywordstruct        ",// 161
	"tToken_Keywordswitch        ",// 162
	"tToken_Keywordthread_local  ",// 163
	"tToken_Keywordtrue          ",// 164
	"tToken_Keywordtypedef       ",// 165
	"tToken_Keywordtypeof        ",// 166
	"tToken_Keywordtypeof_unqual ",// 167
	"tToken_Keywordunion         ",// 168
	"tToken_Keywordunsigned      ",// 169
	"tToken_Keywordvoid          ",// 170
	"tToken_Keywordvolatile      ",// 171
	"tToken_Keywordwhile         ",// 172
	"tToken_Undefined            ",// 173
	"tToken_Undefined            ",// 174
	"tToken_Undefined            ",// 175
	"tToken_Undefined            ",// 176
	"tToken_Undefined            ",// 177
	"tToken_Undefined            ",// 178
	"tToken_Undefined            ",// 179
	"tToken_Undefined            ",// 180
	"tToken_Undefined            ",// 181
	"tToken_Undefined            ",// 182
	"tToken_Undefined            ",// 183
	"tToken_Undefined            ",// 184
	"tToken_Undefined            ",// 185
	"tToken_Undefined            ",// 186
	"tToken_Undefined            ",// 187
	"tToken_Undefined            ",// 188
	"tToken_Undefined            ",// 189
	"tToken_Undefined            ",// 190
	"tToken_Undefined            ",// 191
	"tToken_Undefined            ",// 192
	"tToken_Undefined            ",// 193
	"tToken_Undefined            ",// 194
	"tToken_Undefined            ",// 195
	"tToken_Undefined            ",// 196
	"tToken_Undefined            ",// 197
	"tToken_Undefined            ",// 198
	"tToken_Undefined            ",// 199
	"tToken_Undefined            ",// 200
	"tToken_Undefined            ",// 201
	"tToken_Undefined            ",// 202
	"tToken_Undefined            ",// 203
	"tToken_Undefined            ",// 204
	"tToken_Undefined            ",// 205
	"tToken_Undefined            ",// 206
	"tToken_Undefined            ",// 207
	"tToken_Undefined            ",// 208
	"tToken_Undefined            ",// 209
	"tToken_Undefined            ",// 210
	"tToken_Undefined            ",// 211
	"tToken_Undefined            ",// 212
	"tToken_Undefined            ",// 213
	"tToken_Undefined            ",// 214
	"tToken_Undefined            ",// 215
	"tToken_Undefined            ",// 216
	"tToken_Undefined            ",// 217
	"tToken_Undefined            ",// 218
	"tToken_Undefined            ",// 219
	"tToken_Undefined            ",// 220
	"tToken_Undefined            ",// 221
	"tToken_Undefined            ",// 222
	"tToken_Undefined            ",// 223
	"tToken_Undefined            ",// 224
	"tToken_Undefined            ",// 225
	"tToken_Undefined            ",// 226
	"tToken_Undefined            ",// 227
	"tToken_Undefined            ",// 228
	"tToken_Undefined            ",// 229
	"tToken_Undefined            ",// 230
	"tToken_Undefined            ",// 231
	"tToken_Undefined            ",// 232
	"tToken_Undefined            ",// 233
	"tToken_Undefined            ",// 234
	"tToken_Undefined            ",// 235
	"tToken_Undefined            ",// 236
	"tToken_Undefined            ",// 237
	"tToken_Undefined            ",// 238
	"tToken_Undefined            ",// 239
	"tToken_Undefined            ",// 240
	"tToken_Undefined            ",// 241
	"tToken_Undefined            ",// 242
	"tToken_Undefined            ",// 243
	"tToken_Undefined            ",// 244
	"tToken_Undefined            ",// 245
	"tToken_Undefined            ",// 246
	"tToken_Undefined            ",// 247
	"tToken_Undefined            ",// 248
	"tToken_Undefined            ",// 249
	"tToken_Undefined            ",// 250
	"tToken_Undefined            ",// 251
	"tToken_Undefined            ",// 252
	"tToken_Undefined            ",// 253
	"tToken_Undefined            ",// 254
	"tToken_Undefined            ",// 255
	"tToken_Identifier           ",// 256
	"tToken_String               ",// 257
	"tToken_Charaterconstant     ",// 258
	"tToken_Undefined            ",// 259
	"tToken_Undefined            ",// 260
	"tToken_Undefined            ",// 261
	"tToken_Undefined            ",// 262
	"tToken_Undefined            ",// 263
	"tToken_Undefined            ",// 264
	"tToken_Undefined            ",// 265
	"tToken_Undefined            ",// 266
	"tToken_Undefined            ",// 267
	"tToken_Undefined            ",// 268
	"tToken_Undefined            ",// 269
	"tToken_Undefined            ",// 270
	"tToken_Undefined            ",// 271
	"tToken_Undefined            ",// 272
	"tToken_Undefined            ",// 273
	"tToken_Undefined            ",// 274
	"tToken_Undefined            ",// 275
	"tToken_Undefined            ",// 276
	"tToken_Undefined            ",// 277
	"tToken_Undefined            ",// 278
	"tToken_Undefined            ",// 279
	"tToken_Undefined            ",// 280
	"tToken_Undefined            ",// 281
	"tToken_Undefined            ",// 282
	"tToken_Undefined            ",// 283
	"tToken_Undefined            ",// 284
	"tToken_Undefined            ",// 285
	"tToken_Undefined            ",// 286
	"tToken_Undefined            ",// 287
	"tToken_Undefined            ",// 288
	"tToken_Undefined            ",// 289
	"tToken_Undefined            ",// 290
	"tToken_Undefined            ",// 291
	"tToken_Undefined            ",// 292
	"tToken_Undefined            ",// 293
	"tToken_Undefined            ",// 294
	"tToken_Undefined            ",// 295
	"tToken_Undefined            ",// 296
	"tToken_Undefined            ",// 297
	"tToken_Undefined            ",// 298
	"tToken_Undefined            ",// 299
	"tToken_Undefined            ",// 300
	"tToken_Undefined            ",// 301
	"tToken_Undefined            ",// 302
	"tToken_Undefined            ",// 303
	"tToken_Undefined            ",// 304
	"tToken_Undefined            ",// 305
	"tToken_Undefined            ",// 306
	"tToken_Undefined            ",// 307
	"tToken_Undefined            ",// 308
	"tToken_Undefined            ",// 309
	"tToken_Undefined            ",// 310
	"tToken_Undefined            ",// 311
	"tToken_Undefined            ",// 312
	"tToken_Undefined            ",// 313
	"tToken_Undefined            ",// 314
	"tToken_Undefined            ",// 315
	"tToken_Undefined            ",// 316
	"tToken_Undefined            ",// 317
	"tToken_Undefined            ",// 318
	"tToken_Undefined            ",// 319
	"tToken_Undefined            ",// 320
	"tToken_Undefined            ",// 321
	"tToken_Undefined            ",// 322
	"tToken_Undefined            ",// 323
	"tToken_Undefined            ",// 324
	"tToken_Undefined            ",// 325
	"tToken_Undefined            ",// 326
	"tToken_Undefined            ",// 327
	"tToken_Undefined            ",// 328
	"tToken_Undefined            ",// 329
	"tToken_Undefined            ",// 330
	"tToken_Undefined            ",// 331
	"tToken_Undefined            ",// 332
	"tToken_Undefined            ",// 333
	"tToken_Undefined            ",// 334
	"tToken_Undefined            ",// 335
	"tToken_Undefined            ",// 336
	"tToken_Undefined            ",// 337
	"tToken_Undefined            ",// 338
	"tToken_Undefined            ",// 339
	"tToken_Undefined            ",// 340
	"tToken_Undefined            ",// 341
	"tToken_Undefined            ",// 342
	"tToken_Undefined            ",// 343
	"tToken_Undefined            ",// 344
	"tToken_Undefined            ",// 345
	"tToken_Undefined            ",// 346
	"tToken_Undefined            ",// 347
	"tToken_Undefined            ",// 348
	"tToken_Undefined            ",// 349
	"tToken_Undefined            ",// 350
	"tToken_Undefined            ",// 351
	"tToken_Undefined            ",// 352
	"tToken_Undefined            ",// 353
	"tToken_Undefined            ",// 354
	"tToken_Undefined            ",// 355
	"tToken_Undefined            ",// 356
	"tToken_Undefined            ",// 357
	"tToken_Undefined            ",// 358
	"tToken_Undefined            ",// 359
	"tToken_Undefined            ",// 360
	"tToken_Undefined            ",// 361
	"tToken_Undefined            ",// 362
	"tToken_Undefined            ",// 363
	"tToken_Undefined            ",// 364
	"tToken_Undefined            ",// 365
	"tToken_Undefined            ",// 366
	"tToken_Undefined            ",// 367
	"tToken_Undefined            ",// 368
	"tToken_Undefined            ",// 369
	"tToken_Undefined            ",// 370
	"tToken_Undefined            ",// 371
	"tToken_Undefined            ",// 372
	"tToken_Undefined            ",// 373
	"tToken_Undefined            ",// 374
	"tToken_Undefined            ",// 375
	"tToken_Undefined            ",// 376
	"tToken_Undefined            ",// 377
	"tToken_Undefined            ",// 378
	"tToken_Undefined            ",// 379
	"tToken_Undefined            ",// 380
	"tToken_Undefined            ",// 381
	"tToken_Undefined            ",// 382
	"tToken_Undefined            ",// 383
	"tToken_Undefined            ",// 384
	"tToken_Undefined            ",// 385
	"tToken_Undefined            ",// 386
	"tToken_Undefined            ",// 387
	"tToken_Undefined            ",// 388
	"tToken_Undefined            ",// 389
	"tToken_Undefined            ",// 390
	"tToken_Undefined            ",// 391
	"tToken_Undefined            ",// 392
	"tToken_Undefined            ",// 393
	"tToken_Undefined            ",// 394
	"tToken_Undefined            ",// 395
	"tToken_Undefined            ",// 396
	"tToken_Undefined            ",// 397
	"tToken_Undefined            ",// 398
	"tToken_Undefined            ",// 399
	"tToken_Undefined            ",// 400
	"tToken_Undefined            ",// 401
	"tToken_Undefined            ",// 402
	"tToken_Undefined            ",// 403
	"tToken_Undefined            ",// 404
	"tToken_Undefined            ",// 405
	"tToken_Undefined            ",// 406
	"tToken_Undefined            ",// 407
	"tToken_Undefined            ",// 408
	"tToken_Undefined            ",// 409
	"tToken_Undefined            ",// 410
	"tToken_Undefined            ",// 411
	"tToken_Undefined            ",// 412
	"tToken_Undefined            ",// 413
	"tToken_Undefined            ",// 414
	"tToken_Undefined            ",// 415
	"tToken_Undefined            ",// 416
	"tToken_Undefined            ",// 417
	"tToken_Undefined            ",// 418
	"tToken_Undefined            ",// 419
	"tToken_Undefined            ",// 420
	"tToken_Undefined            ",// 421
	"tToken_Undefined            ",// 422
	"tToken_Undefined            ",// 423
	"tToken_Undefined            ",// 424
	"tToken_Undefined            ",// 425
	"tToken_Undefined            ",// 426
	"tToken_Undefined            ",// 427
	"tToken_Undefined            ",// 428
	"tToken_Undefined            ",// 429
	"tToken_Undefined            ",// 430
	"tToken_Undefined            ",// 431
	"tToken_Undefined            ",// 432
	"tToken_Undefined            ",// 433
	"tToken_Undefined            ",// 434
	"tToken_Undefined            ",// 435
	"tToken_Undefined            ",// 436
	"tToken_Undefined            ",// 437
	"tToken_Undefined            ",// 438
	"tToken_Undefined            ",// 439
	"tToken_Undefined            ",// 440
	"tToken_Undefined            ",// 441
	"tToken_Undefined            ",// 442
	"tToken_Undefined            ",// 443
	"tToken_Undefined            ",// 444
	"tToken_Undefined            ",// 445
	"tToken_Undefined            ",// 446
	"tToken_Undefined            ",// 447
	"tToken_Undefined            ",// 448
	"tToken_Undefined            ",// 449
	"tToken_Undefined            ",// 450
	"tToken_Undefined            ",// 451
	"tToken_Undefined            ",// 452
	"tToken_Undefined            ",// 453
	"tToken_Undefined            ",// 454
	"tToken_Undefined            ",// 455
	"tToken_Undefined            ",// 456
	"tToken_Undefined            ",// 457
	"tToken_Undefined            ",// 458
	"tToken_Undefined            ",// 459
	"tToken_Undefined            ",// 460
	"tToken_Undefined            ",// 461
	"tToken_Undefined            ",// 462
	"tToken_Undefined            ",// 463
	"tToken_Undefined            ",// 464
	"tToken_Undefined            ",// 465
	"tToken_Undefined            ",// 466
	"tToken_Undefined            ",// 467
	"tToken_Undefined            ",// 468
	"tToken_Undefined            ",// 469
	"tToken_Undefined            ",// 470
	"tToken_Undefined            ",// 471
	"tToken_Undefined            ",// 472
	"tToken_Undefined            ",// 473
	"tToken_Undefined            ",// 474
	"tToken_Undefined            ",// 475
	"tToken_Undefined            ",// 476
	"tToken_Undefined            ",// 477
	"tToken_Undefined            ",// 478
	"tToken_Undefined            ",// 479
	"tToken_Undefined            ",// 480
	"tToken_Undefined            ",// 481
	"tToken_Undefined            ",// 482
	"tToken_Undefined            ",// 483
	"tToken_Undefined            ",// 484
	"tToken_Undefined            ",// 485
	"tToken_Undefined            ",// 486
	"tToken_Undefined            ",// 487
	"tToken_Undefined            ",// 488
	"tToken_Undefined            ",// 489
	"tToken_Undefined            ",// 490
	"tToken_Undefined            ",// 491
	"tToken_Undefined            ",// 492
	"tToken_Undefined            ",// 493
	"tToken_Undefined            ",// 494
	"tToken_Undefined            ",// 495
	"tToken_Undefined            ",// 496
	"tToken_Undefined            ",// 497
	"tToken_Undefined            ",// 498
	"tToken_Undefined            ",// 499
	"tToken_Undefined            ",// 500
	"tToken_Undefined            ",// 501
	"tToken_Undefined            ",// 502
	"tToken_Undefined            ",// 503
	"tToken_Undefined            ",// 504
	"tToken_Undefined            ",// 505
	"tToken_Undefined            ",// 506
	"tToken_Undefined            ",// 507
	"tToken_Undefined            ",// 508
	"tToken_Undefined            ",// 509
	"tToken_Undefined            ",// 510
	"tToken_Undefined            ",// 511
	"tLexem_Declarationlist      ",// 512
	"tLexem_Assign               ",// 513
	"tLexem_Comma                ",// 514
	"tLexem_Expressionif         ",// 515
	"tLexem_Logicalor            ",// 516
	"tLexem_Logicaland           ",// 517
	"tLexem_Equals               ",// 518
	"tLexem_Notequal             ",// 519
	"tLexem_Greaterequal         ",// 520
	"tLexem_Lessequal            ",// 521
	"tLexem_Greaterthan          ",// 522
	"tLexem_Lessthan             ",// 523
	"tLexem_Bitwiseor            ",// 524
	"tLexem_Bitwisexor           ",// 525
	"tLexem_Bitwiseand           ",// 526
	"tLexem_Shiftleft            ",// 527
	"tLexem_Shiftright           ",// 528
	"tLexem_Add                  ",// 529
	"tLexem_Substract            ",// 530
	"tLexem_Typecast             ",// 531
	"tLexem_Variabledeclaration  ",// 532
	"tLexem_Functiondeclaration  ",// 533
	"tLexem_Blockstatement       ",// 534
	"tLexem_Increment            ",// 535
	"tLexem_Decrement            ",// 536
	"tLexem_Unaryplus            ",// 537
	"tLexem_Negation             ",// 538
	"tLexem_Logicalnot           ",// 539
	"tLexem_Bitwisenot           ",// 540
	"tLexem_Dereference          ",// 541
	"tLexem_Addressof            ",// 542
	"tLexem_Sizeof               ",// 543
	"tLexem_Typeexpression       ",// 544
	"tLexem_Expressionstatement  ",// 545
	"tLexem_Integerconstant      ",// 546
	"tLexem_Functioncall         ",// 547
	"tLexem_Structuremember      ",// 548
	"tLexem_Memberbypointer      ",// 549
	"tLexem_Compoundliteral      ",// 550
	"tLexem_Arrayindex           ",// 551
	"tLexem_Postincrement        ",// 552
	"tLexem_Postdecrement        ",// 553
	"tLexem_Identifier           ",// 554
	"tLexem_Typedefinition       ",// 555
	"tLexem_Structdefinition     ",// 556
	"tLexem_Returnstatement      ",// 557
	"tLexem_Ifstatement          ",// 558
	"tLexem_Nullpointer          ",// 559
	"tLexem_Booleantrue          ",// 560
	"tLexem_Booleanfalse         ",// 561
	"tLexem_Whilestatement       ",// 562
	"tLexem_Forstatement         ",// 563
	"tLexem_Nulldeclaration      ",// 564
	"tLexem_Nullexpression       ",// 565
	"tLexem_Multiply             ",// 566
	"tLexem_Divide               ",// 567
	"tLexem_Modulo               ",// 568
	"tLexem_Switchstatement      ",// 569
	"tLexem_Switchcase           ",// 570
	"tLexem_Switchdefault        ",// 571
	"tLexem_Goto                 ",// 572
	"tLexem_Breakstatement       ",// 573
	"tLexem_Constmodifier        ",// 574
	"tLexem_Volatilemodifier     ",// 575
	"tLexem_Restrictmodifier     ",// 576
	"tLexem_Numericconstant      ",// 577
	"tLexem_Stringconstant       ",// 578
	"tLexem_Rawvariabledeclarati>",// 579
	"tLexem_Namespace            ",// 580
	"tLexem_Using                ",// 581
	"tLexem_Ellipsis             ",// 582
	"tLexem_Undefined            ",// 583
	"tLexem_Undefined            ",// 584
	"tLexem_Undefined            ",// 585
	"tLexem_Undefined            ",// 586
	"tLexem_Undefined            ",// 587
	"tLexem_Undefined            ",// 588
	"tLexem_Undefined            ",// 589
	"tLexem_Undefined            ",// 590
	"tLexem_Undefined            ",// 591
	"tLexem_Undefined            ",// 592
	"tLexem_Undefined            ",// 593
	"tLexem_Undefined            ",// 594
	"tLexem_Undefined            ",// 595
	"tLexem_Undefined            ",// 596
	"tLexem_Undefined            ",// 597
	"tLexem_Undefined            ",// 598
	"tLexem_Undefined            ",// 599
	"tLexem_Undefined            ",// 600
	"tLexem_Undefined            ",// 601
	"tLexem_Undefined            ",// 602
	"tLexem_Undefined            ",// 603
	"tLexem_Undefined            ",// 604
	"tLexem_Undefined            ",// 605
	"tLexem_Undefined            ",// 606
	"tLexem_Undefined            ",// 607
	"tLexem_Undefined            ",// 608
	"tLexem_Undefined            ",// 609
	"tLexem_Undefined            ",// 610
	"tLexem_Undefined            ",// 611
	"tLexem_Undefined            ",// 612
	"tLexem_Undefined            ",// 613
	"tLexem_Undefined            ",// 614
	"tLexem_Undefined            ",// 615
	"tLexem_Undefined            ",// 616
	"tLexem_Undefined            ",// 617
	"tLexem_Undefined            ",// 618
	"tLexem_Undefined            ",// 619
	"tLexem_Undefined            ",// 620
	"tLexem_Undefined            ",// 621
	"tLexem_Undefined            ",// 622
	"tLexem_Undefined            ",// 623
	"tLexem_Undefined            ",// 624
	"tLexem_Undefined            ",// 625
	"tLexem_Undefined            ",// 626
	"tLexem_Undefined            ",// 627
	"tLexem_Undefined            ",// 628
	"tLexem_Undefined            ",// 629
	"tLexem_Undefined            ",// 630
	"tLexem_Undefined            ",// 631
	"tLexem_Undefined            ",// 632
	"tLexem_Undefined            ",// 633
	"tLexem_Undefined            ",// 634
	"tLexem_Undefined            ",// 635
	"tLexem_Undefined            ",// 636
	"tLexem_Undefined            ",// 637
	"tLexem_Undefined            ",// 638
	"tLexem_Undefined            ",// 639
	"tLexem_Undefined            ",// 640
	"tLexem_Undefined            ",// 641
	"tLexem_Undefined            ",// 642
	"tLexem_Undefined            ",// 643
	"tLexem_Undefined            ",// 644
	"tLexem_Undefined            ",// 645
	"tLexem_Undefined            ",// 646
	"tLexem_Undefined            ",// 647
	"tLexem_Undefined            ",// 648
	"tLexem_Undefined            ",// 649
	"tLexem_Undefined            ",// 650
	"tLexem_Undefined            ",// 651
	"tLexem_Undefined            ",// 652
	"tLexem_Undefined            ",// 653
	"tLexem_Undefined            ",// 654
	"tLexem_Undefined            ",// 655
	"tLexem_Undefined            ",// 656
	"tLexem_Undefined            ",// 657
	"tLexem_Undefined            ",// 658
	"tLexem_Undefined            ",// 659
	"tLexem_Undefined            ",// 660
	"tLexem_Undefined            ",// 661
	"tLexem_Undefined            ",// 662
	"tLexem_Undefined            ",// 663
	"tLexem_Undefined            ",// 664
	"tLexem_Undefined            ",// 665
	"tLexem_Undefined            ",// 666
	"tLexem_Undefined            ",// 667
	"tLexem_Undefined            ",// 668
	"tLexem_Undefined            ",// 669
	"tLexem_Undefined            ",// 670
	"tLexem_Undefined            ",// 671
	"tLexem_Undefined            ",// 672
	"tLexem_Undefined            ",// 673
	"tLexem_Undefined            ",// 674
	"tLexem_Undefined            ",// 675
	"tLexem_Undefined            ",// 676
	"tLexem_Undefined            ",// 677
	"tLexem_Undefined            ",// 678
	"tLexem_Undefined            ",// 679
	"tLexem_Undefined            ",// 680
	"tLexem_Undefined            ",// 681
	"tLexem_Undefined            ",// 682
	"tLexem_Undefined            ",// 683
	"tLexem_Undefined            ",// 684
	"tLexem_Undefined            ",// 685
	"tLexem_Undefined            ",// 686
	"tLexem_Undefined            ",// 687
	"tLexem_Undefined            ",// 688
	"tLexem_Undefined            ",// 689
	"tLexem_Undefined            ",// 690
	"tLexem_Undefined            ",// 691
	"tLexem_Undefined            ",// 692
	"tLexem_Undefined            ",// 693
	"tLexem_Undefined            ",// 694
	"tLexem_Undefined            ",// 695
	"tLexem_Undefined            ",// 696
	"tLexem_Undefined            ",// 697
	"tLexem_Undefined            ",// 698
	"tLexem_Undefined            ",// 699
	"tLexem_Undefined            ",// 700
	"tLexem_Undefined            ",// 701
	"tLexem_Undefined            ",// 702
	"tLexem_Undefined            ",// 703
	"tLexem_Undefined            ",// 704
	"tLexem_Undefined            ",// 705
	"tLexem_Undefined            ",// 706
	"tLexem_Undefined            ",// 707
	"tLexem_Undefined            ",// 708
	"tLexem_Undefined            ",// 709
	"tLexem_Undefined            ",// 710
	"tLexem_Undefined            ",// 711
	"tLexem_Undefined            ",// 712
	"tLexem_Undefined            ",// 713
	"tLexem_Undefined            ",// 714
	"tLexem_Undefined            ",// 715
	"tLexem_Undefined            ",// 716
	"tLexem_Undefined            ",// 717
	"tLexem_Undefined            ",// 718
	"tLexem_Undefined            ",// 719
	"tLexem_Undefined            ",// 720
	"tLexem_Undefined            ",// 721
	"tLexem_Undefined            ",// 722
	"tLexem_Undefined            ",// 723
	"tLexem_Undefined            ",// 724
	"tLexem_Undefined            ",// 725
	"tLexem_Undefined            ",// 726
	"tLexem_Undefined            ",// 727
	"tLexem_Undefined            ",// 728
	"tLexem_Undefined            ",// 729
	"tLexem_Undefined            ",// 730
	"tLexem_Undefined            ",// 731
	"tLexem_Undefined            ",// 732
	"tLexem_Undefined            ",// 733
	"tLexem_Undefined            ",// 734
	"tLexem_Undefined            ",// 735
	"tLexem_Undefined            ",// 736
	"tLexem_Undefined            ",// 737
	"tLexem_Undefined            ",// 738
	"tLexem_Undefined            ",// 739
	"tLexem_Undefined            ",// 740
	"tLexem_Undefined            ",// 741
	"tLexem_Undefined            ",// 742
	"tLexem_Undefined            ",// 743
	"tLexem_Undefined            ",// 744
	"tLexem_Undefined            ",// 745
	"tLexem_Undefined            ",// 746
	"tLexem_Undefined            ",// 747
	"tLexem_Undefined            ",// 748
	"tLexem_Undefined            ",// 749
	"tLexem_Undefined            ",// 750
	"tLexem_Undefined            ",// 751
	"tLexem_Undefined            ",// 752
	"tLexem_Undefined            ",// 753
	"tLexem_Undefined            ",// 754
	"tLexem_Undefined            ",// 755
	"tLexem_Undefined            ",// 756
	"tLexem_Undefined            ",// 757
	"tLexem_Undefined            ",// 758
	"tLexem_Undefined            ",// 759
	"tLexem_Undefined            ",// 760
	"tLexem_Undefined            ",// 761
	"tLexem_Undefined            ",// 762
	"tLexem_Undefined            ",// 763
	"tLexem_Undefined            ",// 764
	"tLexem_Undefined            ",// 765
	"tLexem_Undefined            ",// 766
	"tLexem_Undefined            ",// 767
	"tCharlexem_Undefined        ",// 768
	"tCharlexem_Undefined        ",// 769
	"tCharlexem_Undefined        ",// 770
	"tCharlexem_Undefined        ",// 771
	"tCharlexem_Undefined        ",// 772
	"tCharlexem_Undefined        ",// 773
	"tCharlexem_Undefined        ",// 774
	"tCharlexem_Undefined        ",// 775
	"tCharlexem_Undefined        ",// 776
	"tCharlexem_Undefined        ",// 777
	"tCharlexem_Undefined        ",// 778
	"tCharlexem_Undefined        ",// 779
	"tCharlexem_Undefined        ",// 780
	"tCharlexem_Undefined        ",// 781
	"tCharlexem_Undefined        ",// 782
	"tCharlexem_Undefined        ",// 783
	"tCharlexem_Undefined        ",// 784
	"tCharlexem_Undefined        ",// 785
	"tCharlexem_Undefined        ",// 786
	"tCharlexem_Undefined        ",// 787
	"tCharlexem_Undefined        ",// 788
	"tCharlexem_Undefined        ",// 789
	"tCharlexem_Undefined        ",// 790
	"tCharlexem_Undefined        ",// 791
	"tCharlexem_Undefined        ",// 792
	"tCharlexem_Undefined        ",// 793
	"tCharlexem_Undefined        ",// 794
	"tCharlexem_Undefined        ",// 795
	"tCharlexem_Undefined        ",// 796
	"tCharlexem_Undefined        ",// 797
	"tCharlexem_Undefined        ",// 798
	"tCharlexem_Undefined        ",// 799
	"tCharlexem_Undefined        ",// 800
	"tCharlexem_Undefined        ",// 801
	"tCharlexem_Undefined        ",// 802
	"tCharlexem_Undefined        ",// 803
	"tCharlexem_Undefined        ",// 804
	"tCharlexem_Undefined        ",// 805
	"tCharlexem_Undefined        ",// 806
	"tCharlexem_Undefined        ",// 807
	"tCharlexem_Undefined        ",// 808
	"tCharlexem_Undefined        ",// 809
	"tCharlexem_Undefined        ",// 810
	"tCharlexem_Undefined        ",// 811
	"tCharlexem_Undefined        ",// 812
	"tCharlexem_Undefined        ",// 813
	"tCharlexem_Undefined        ",// 814
	"tCharlexem_Undefined        ",// 815
	"tCharlexem_Undefined        ",// 816
	"tCharlexem_Undefined        ",// 817
	"tCharlexem_Undefined        ",// 818
	"tCharlexem_Undefined        ",// 819
	"tCharlexem_Undefined        ",// 820
	"tCharlexem_Undefined        ",// 821
	"tCharlexem_Undefined        ",// 822
	"tCharlexem_Undefined        ",// 823
	"tCharlexem_Undefined        ",// 824
	"tCharlexem_Undefined        ",// 825
	"tCharlexem_Undefined        ",// 826
	"tCharlexem_Undefined        ",// 827
	"tCharlexem_Undefined        ",// 828
	"tCharlexem_Undefined        ",// 829
	"tCharlexem_Undefined        ",// 830
	"tCharlexem_Undefined        ",// 831
	"tCharlexem_Undefined        ",// 832
	"tCharlexem_Undefined        ",// 833
	"tCharlexem_Undefined        ",// 834
	"tCharlexem_Undefined        ",// 835
	"tCharlexem_Undefined        ",// 836
	"tCharlexem_Undefined        ",// 837
	"tCharlexem_Undefined        ",// 838
	"tCharlexem_Undefined        ",// 839
	"tCharlexem_Undefined        ",// 840
	"tCharlexem_Undefined        ",// 841
	"tCharlexem_Undefined        ",// 842
	"tCharlexem_Undefined        ",// 843
	"tCharlexem_Undefined        ",// 844
	"tCharlexem_Undefined        ",// 845
	"tCharlexem_Undefined        ",// 846
	"tCharlexem_Undefined        ",// 847
	"tCharlexem_Undefined        ",// 848
	"tCharlexem_Undefined        ",// 849
	"tCharlexem_Undefined        ",// 850
	"tCharlexem_Undefined        ",// 851
	"tCharlexem_Undefined        ",// 852
	"tCharlexem_Undefined        ",// 853
	"tCharlexem_Undefined        ",// 854
	"tCharlexem_Undefined        ",// 855
	"tCharlexem_Undefined        ",// 856
	"tCharlexem_Undefined        ",// 857
	"tCharlexem_Undefined        ",// 858
	"tCharlexem_Undefined        ",// 859
	"tCharlexem_Undefined        ",// 860
	"tCharlexem_Undefined        ",// 861
	"tCharlexem_Undefined        ",// 862
	"tCharlexem_Undefined        ",// 863
	"tCharlexem_Undefined        ",// 864
	"tCharlexem_Undefined        ",// 865
	"tCharlexem_Undefined        ",// 866
	"tCharlexem_Undefined        ",// 867
	"tCharlexem_Undefined        ",// 868
	"tCharlexem_Undefined        ",// 869
	"tCharlexem_Undefined        ",// 870
	"tCharlexem_Undefined        ",// 871
	"tCharlexem_Undefined        ",// 872
	"tCharlexem_Undefined        ",// 873
	"tCharlexem_Undefined        ",// 874
	"tCharlexem_Undefined        ",// 875
	"tCharlexem_Undefined        ",// 876
	"tCharlexem_Undefined        ",// 877
	"tCharlexem_Undefined        ",// 878
	"tCharlexem_Undefined        ",// 879
	"tCharlexem_Undefined        ",// 880
	"tCharlexem_Undefined        ",// 881
	"tCharlexem_Undefined        ",// 882
	"tCharlexem_Undefined        ",// 883
	"tCharlexem_Undefined        ",// 884
	"tCharlexem_Undefined        ",// 885
	"tCharlexem_Undefined        ",// 886
	"tCharlexem_Undefined        ",// 887
	"tCharlexem_Undefined        ",// 888
	"tCharlexem_Undefined        ",// 889
	"tCharlexem_Undefined        ",// 890
	"tCharlexem_Undefined        ",// 891
	"tCharlexem_Undefined        ",// 892
	"tCharlexem_Undefined        ",// 893
	"tCharlexem_Undefined        ",// 894
	"tCharlexem_Undefined        ",// 895
	"tCharlexem_Undefined        ",// 896
	"tCharlexem_Undefined        ",// 897
	"tCharlexem_Undefined        ",// 898
	"tCharlexem_Undefined        ",// 899
	"tCharlexem_Undefined        ",// 900
	"tCharlexem_Undefined        ",// 901
	"tCharlexem_Undefined        ",// 902
	"tCharlexem_Undefined        ",// 903
	"tCharlexem_Undefined        ",// 904
	"tCharlexem_Undefined        ",// 905
	"tCharlexem_Undefined        ",// 906
	"tCharlexem_Undefined        ",// 907
	"tCharlexem_Undefined        ",// 908
	"tCharlexem_Undefined        ",// 909
	"tCharlexem_Undefined        ",// 910
	"tCharlexem_Undefined        ",// 911
	"tCharlexem_Undefined        ",// 912
	"tCharlexem_Undefined        ",// 913
	"tCharlexem_Undefined        ",// 914
	"tCharlexem_Undefined        ",// 915
	"tCharlexem_Undefined        ",// 916
	"tCharlexem_Undefined        ",// 917
	"tCharlexem_Undefined        ",// 918
	"tCharlexem_Undefined        ",// 919
	"tCharlexem_Undefined        ",// 920
	"tCharlexem_Undefined        ",// 921
	"tCharlexem_Undefined        ",// 922
	"tCharlexem_Undefined        ",// 923
	"tCharlexem_Undefined        ",// 924
	"tCharlexem_Undefined        ",// 925
	"tCharlexem_Undefined        ",// 926
	"tCharlexem_Undefined        ",// 927
	"tCharlexem_Undefined        ",// 928
	"tCharlexem_Undefined        ",// 929
	"tCharlexem_Undefined        ",// 930
	"tCharlexem_Undefined        ",// 931
	"tCharlexem_Undefined        ",// 932
	"tCharlexem_Undefined        ",// 933
	"tCharlexem_Undefined        ",// 934
	"tCharlexem_Undefined        ",// 935
	"tCharlexem_Undefined        ",// 936
	"tCharlexem_Undefined        ",// 937
	"tCharlexem_Undefined        ",// 938
	"tCharlexem_Undefined        ",// 939
	"tCharlexem_Undefined        ",// 940
	"tCharlexem_Undefined        ",// 941
	"tCharlexem_Undefined        ",// 942
	"tCharlexem_Undefined        ",// 943
	"tCharlexem_Undefined        ",// 944
	"tCharlexem_Undefined        ",// 945
	"tCharlexem_Undefined        ",// 946
	"tCharlexem_Undefined        ",// 947
	"tCharlexem_Undefined        ",// 948
	"tCharlexem_Undefined        ",// 949
	"tCharlexem_Undefined        ",// 950
	"tCharlexem_Undefined        ",// 951
	"tCharlexem_Undefined        ",// 952
	"tCharlexem_Undefined        ",// 953
	"tCharlexem_Undefined        ",// 954
	"tCharlexem_Undefined        ",// 955
	"tCharlexem_Undefined        ",// 956
	"tCharlexem_Undefined        ",// 957
	"tCharlexem_Undefined        ",// 958
	"tCharlexem_Undefined        ",// 959
	"tCharlexem_Undefined        ",// 960
	"tCharlexem_Undefined        ",// 961
	"tCharlexem_Undefined        ",// 962
	"tCharlexem_Undefined        ",// 963
	"tCharlexem_Undefined        ",// 964
	"tCharlexem_Undefined        ",// 965
	"tCharlexem_Undefined        ",// 966
	"tCharlexem_Undefined        ",// 967
	"tCharlexem_Undefined        ",// 968
	"tCharlexem_Undefined        ",// 969
	"tCharlexem_Undefined        ",// 970
	"tCharlexem_Undefined        ",// 971
	"tCharlexem_Undefined        ",// 972
	"tCharlexem_Undefined        ",// 973
	"tCharlexem_Undefined        ",// 974
	"tCharlexem_Undefined        ",// 975
	"tCharlexem_Undefined        ",// 976
	"tCharlexem_Undefined        ",// 977
	"tCharlexem_Undefined        ",// 978
	"tCharlexem_Undefined        ",// 979
	"tCharlexem_Undefined        ",// 980
	"tCharlexem_Undefined        ",// 981
	"tCharlexem_Undefined        ",// 982
	"tCharlexem_Undefined        ",// 983
	"tCharlexem_Undefined        ",// 984
	"tCharlexem_Undefined        ",// 985
	"tCharlexem_Undefined        ",// 986
	"tCharlexem_Undefined        ",// 987
	"tCharlexem_Undefined        ",// 988
	"tCharlexem_Undefined        ",// 989
	"tCharlexem_Undefined        ",// 990
	"tCharlexem_Undefined        ",// 991
	"tCharlexem_Undefined        ",// 992
	"tCharlexem_Undefined        ",// 993
	"tCharlexem_Undefined        ",// 994
	"tCharlexem_Undefined        ",// 995
	"tCharlexem_Undefined        ",// 996
	"tCharlexem_Undefined        ",// 997
	"tCharlexem_Undefined        ",// 998
	"tCharlexem_Undefined        ",// 999
	"tCharlexem_Undefined        ",// 1000
	"tCharlexem_Undefined        ",// 1001
	"tCharlexem_Undefined        ",// 1002
	"tCharlexem_Undefined        ",// 1003
	"tCharlexem_Undefined        ",// 1004
	"tCharlexem_Undefined        ",// 1005
	"tCharlexem_Undefined        ",// 1006
	"tCharlexem_Undefined        ",// 1007
	"tCharlexem_Undefined        ",// 1008
	"tCharlexem_Undefined        ",// 1009
	"tCharlexem_Undefined        ",// 1010
	"tCharlexem_Undefined        ",// 1011
	"tCharlexem_Undefined        ",// 1012
	"tCharlexem_Undefined        ",// 1013
	"tCharlexem_Undefined        ",// 1014
	"tCharlexem_Undefined        ",// 1015
	"tCharlexem_Undefined        ",// 1016
	"tCharlexem_Undefined        ",// 1017
	"tCharlexem_Undefined        ",// 1018
	"tCharlexem_Undefined        ",// 1019
	"tCharlexem_Undefined        ",// 1020
	"tCharlexem_Undefined        ",// 1021
	"tCharlexem_Undefined        ",// 1022
	"tCharlexem_Undefined        ",// 1023
	"tSplexem_Declarationlist    ",// 1024
	"tSplexem_Integerconstant    ",// 1025
	"tSplexem_Symbol             ",// 1026
	"tSplexem_Nulldeclaration    ",// 1027
	"tSplexem_Blockstatement     ",// 1028
	"tSplexem_Functiondeclaration",// 1029
	"tSplexem_Returnstatement    ",// 1030
	"tSplexem_Variabledeclaration",// 1031
	"tSplexem_Nullexpression     ",// 1032
	"tSplexem_Typecast           ",// 1033
	"tSplexem_Addition           ",// 1034
	"tSplexem_Substraction       ",// 1035
	"tSplexem_Dereference        ",// 1036
	"tSplexem_Structuremember    ",// 1037
	"tSplexem_Identifier         ",// 1038
	"tSplexem_Cast               ",// 1039
	"tSplexem_Expressionstatement",// 1040
	"tSplexem_Stringconstant     ",// 1041
	"tSplexem_Comma              ",// 1042
	"tSplexem_Functioncall       ",// 1043
	"tSplexem_Multiplication     ",// 1044
	"tSplexem_Division           ",// 1045
	"tSplexem_Modulo             ",// 1046
	"tSplexem_Ifstatement        ",// 1047
	"tSplexem_Whilestatement     ",// 1048
	"tSplexem_Forstatement       ",// 1049
	"tSplexem_Switchstatemen     ",// 1050
	"tSplexem_Breakstatement     ",// 1051
	"tSplexem_Equality           ",// 1052
	"tSplexem_Nonequality        ",// 1053
	"tSplexem_Assign             ",// 1054
	"tSplexem_Undefined          ",// 1055
	"tSplexem_Undefined          ",// 1056
	"tSplexem_Undefined          ",// 1057
	"tSplexem_Undefined          ",// 1058
	"tSplexem_Undefined          ",// 1059
	"tSplexem_Undefined          ",// 1060
	"tSplexem_Undefined          ",// 1061
	"tSplexem_Undefined          ",// 1062
	"tSplexem_Undefined          ",// 1063
	"tSplexem_Undefined          ",// 1064
	"tSplexem_Undefined          ",// 1065
	"tSplexem_Undefined          ",// 1066
	"tSplexem_Undefined          ",// 1067
	"tSplexem_Undefined          ",// 1068
	"tSplexem_Undefined          ",// 1069
	"tSplexem_Undefined          ",// 1070
	"tSplexem_Undefined          ",// 1071
	"tSplexem_Undefined          ",// 1072
	"tSplexem_Undefined          ",// 1073
	"tSplexem_Undefined          ",// 1074
	"tSplexem_Undefined          ",// 1075
	"tSplexem_Undefined          ",// 1076
	"tSplexem_Undefined          ",// 1077
	"tSplexem_Undefined          ",// 1078
	"tSplexem_Undefined          ",// 1079
	"tSplexem_Undefined          ",// 1080
	"tSplexem_Undefined          ",// 1081
	"tSplexem_Undefined          ",// 1082
	"tSplexem_Undefined          ",// 1083
	"tSplexem_Undefined          ",// 1084
	"tSplexem_Undefined          ",// 1085
	"tSplexem_Undefined          ",// 1086
	"tSplexem_Undefined          ",// 1087
	"tSplexem_Undefined          ",// 1088
	"tSplexem_Undefined          ",// 1089
	"tSplexem_Undefined          ",// 1090
	"tSplexem_Undefined          ",// 1091
	"tSplexem_Undefined          ",// 1092
	"tSplexem_Undefined          ",// 1093
	"tSplexem_Undefined          ",// 1094
	"tSplexem_Undefined          ",// 1095
	"tSplexem_Undefined          ",// 1096
	"tSplexem_Undefined          ",// 1097
	"tSplexem_Undefined          ",// 1098
	"tSplexem_Undefined          ",// 1099
	"tSplexem_Undefined          ",// 1100
	"tSplexem_Undefined          ",// 1101
	"tSplexem_Undefined          ",// 1102
	"tSplexem_Undefined          ",// 1103
	"tSplexem_Undefined          ",// 1104
	"tSplexem_Undefined          ",// 1105
	"tSplexem_Undefined          ",// 1106
	"tSplexem_Undefined          ",// 1107
	"tSplexem_Undefined          ",// 1108
	"tSplexem_Undefined          ",// 1109
	"tSplexem_Undefined          ",// 1110
	"tSplexem_Undefined          ",// 1111
	"tSplexem_Undefined          ",// 1112
	"tSplexem_Undefined          ",// 1113
	"tSplexem_Undefined          ",// 1114
	"tSplexem_Undefined          ",// 1115
	"tSplexem_Undefined          ",// 1116
	"tSplexem_Undefined          ",// 1117
	"tSplexem_Undefined          ",// 1118
	"tSplexem_Undefined          ",// 1119
	"tSplexem_Undefined          ",// 1120
	"tSplexem_Undefined          ",// 1121
	"tSplexem_Undefined          ",// 1122
	"tSplexem_Undefined          ",// 1123
	"tSplexem_Undefined          ",// 1124
	"tSplexem_Undefined          ",// 1125
	"tSplexem_Undefined          ",// 1126
	"tSplexem_Undefined          ",// 1127
	"tSplexem_Undefined          ",// 1128
	"tSplexem_Undefined          ",// 1129
	"tSplexem_Undefined          ",// 1130
	"tSplexem_Undefined          ",// 1131
	"tSplexem_Undefined          ",// 1132
	"tSplexem_Undefined          ",// 1133
	"tSplexem_Undefined          ",// 1134
	"tSplexem_Undefined          ",// 1135
	"tSplexem_Undefined          ",// 1136
	"tSplexem_Undefined          ",// 1137
	"tSplexem_Undefined          ",// 1138
	"tSplexem_Undefined          ",// 1139
	"tSplexem_Undefined          ",// 1140
	"tSplexem_Undefined          ",// 1141
	"tSplexem_Undefined          ",// 1142
	"tSplexem_Undefined          ",// 1143
	"tSplexem_Undefined          ",// 1144
	"tSplexem_Undefined          ",// 1145
	"tSplexem_Undefined          ",// 1146
	"tSplexem_Undefined          ",// 1147
	"tSplexem_Undefined          ",// 1148
	"tSplexem_Undefined          ",// 1149
	"tSplexem_Undefined          ",// 1150
	"tSplexem_Undefined          ",// 1151
	"tSplexem_Undefined          ",// 1152
	"tSplexem_Undefined          ",// 1153
	"tSplexem_Undefined          ",// 1154
	"tSplexem_Undefined          ",// 1155
	"tSplexem_Undefined          ",// 1156
	"tSplexem_Undefined          ",// 1157
	"tSplexem_Undefined          ",// 1158
	"tSplexem_Undefined          ",// 1159
	"tSplexem_Undefined          ",// 1160
	"tSplexem_Undefined          ",// 1161
	"tSplexem_Undefined          ",// 1162
	"tSplexem_Undefined          ",// 1163
	"tSplexem_Undefined          ",// 1164
	"tSplexem_Undefined          ",// 1165
	"tSplexem_Undefined          ",// 1166
	"tSplexem_Undefined          ",// 1167
	"tSplexem_Undefined          ",// 1168
	"tSplexem_Undefined          ",// 1169
	"tSplexem_Undefined          ",// 1170
	"tSplexem_Undefined          ",// 1171
	"tSplexem_Undefined          ",// 1172
	"tSplexem_Undefined          ",// 1173
	"tSplexem_Undefined          ",// 1174
	"tSplexem_Undefined          ",// 1175
	"tSplexem_Undefined          ",// 1176
	"tSplexem_Undefined          ",// 1177
	"tSplexem_Undefined          ",// 1178
	"tSplexem_Undefined          ",// 1179
	"tSplexem_Undefined          ",// 1180
	"tSplexem_Undefined          ",// 1181
	"tSplexem_Undefined          ",// 1182
	"tSplexem_Undefined          ",// 1183
	"tSplexem_Undefined          ",// 1184
	"tSplexem_Undefined          ",// 1185
	"tSplexem_Undefined          ",// 1186
	"tSplexem_Undefined          ",// 1187
	"tSplexem_Undefined          ",// 1188
	"tSplexem_Undefined          ",// 1189
	"tSplexem_Undefined          ",// 1190
	"tSplexem_Undefined          ",// 1191
	"tSplexem_Undefined          ",// 1192
	"tSplexem_Undefined          ",// 1193
	"tSplexem_Undefined          ",// 1194
	"tSplexem_Undefined          ",// 1195
	"tSplexem_Undefined          ",// 1196
	"tSplexem_Undefined          ",// 1197
	"tSplexem_Undefined          ",// 1198
	"tSplexem_Undefined          ",// 1199
	"tSplexem_Undefined          ",// 1200
	"tSplexem_Undefined          ",// 1201
	"tSplexem_Undefined          ",// 1202
	"tSplexem_Undefined          ",// 1203
	"tSplexem_Undefined          ",// 1204
	"tSplexem_Undefined          ",// 1205
	"tSplexem_Undefined          ",// 1206
	"tSplexem_Undefined          ",// 1207
	"tSplexem_Undefined          ",// 1208
	"tSplexem_Undefined          ",// 1209
	"tSplexem_Undefined          ",// 1210
	"tSplexem_Undefined          ",// 1211
	"tSplexem_Undefined          ",// 1212
	"tSplexem_Undefined          ",// 1213
	"tSplexem_Undefined          ",// 1214
	"tSplexem_Undefined          ",// 1215
	"tSplexem_Undefined          ",// 1216
	"tSplexem_Undefined          ",// 1217
	"tSplexem_Undefined          ",// 1218
	"tSplexem_Undefined          ",// 1219
	"tSplexem_Undefined          ",// 1220
	"tSplexem_Undefined          ",// 1221
	"tSplexem_Undefined          ",// 1222
	"tSplexem_Undefined          ",// 1223
	"tSplexem_Undefined          ",// 1224
	"tSplexem_Undefined          ",// 1225
	"tSplexem_Undefined          ",// 1226
	"tSplexem_Undefined          ",// 1227
	"tSplexem_Undefined          ",// 1228
	"tSplexem_Undefined          ",// 1229
	"tSplexem_Undefined          ",// 1230
	"tSplexem_Undefined          ",// 1231
	"tSplexem_Undefined          ",// 1232
	"tSplexem_Undefined          ",// 1233
	"tSplexem_Undefined          ",// 1234
	"tSplexem_Undefined          ",// 1235
	"tSplexem_Undefined          ",// 1236
	"tSplexem_Undefined          ",// 1237
	"tSplexem_Undefined          ",// 1238
	"tSplexem_Undefined          ",// 1239
	"tSplexem_Undefined          ",// 1240
	"tSplexem_Undefined          ",// 1241
	"tSplexem_Undefined          ",// 1242
	"tSplexem_Undefined          ",// 1243
	"tSplexem_Undefined          ",// 1244
	"tSplexem_Undefined          ",// 1245
	"tSplexem_Undefined          ",// 1246
	"tSplexem_Undefined          ",// 1247
	"tSplexem_Undefined          ",// 1248
	"tSplexem_Undefined          ",// 1249
	"tSplexem_Undefined          ",// 1250
	"tSplexem_Undefined          ",// 1251
	"tSplexem_Undefined          ",// 1252
	"tSplexem_Undefined          ",// 1253
	"tSplexem_Undefined          ",// 1254
	"tSplexem_Undefined          ",// 1255
	"tSplexem_Undefined          ",// 1256
	"tSplexem_Undefined          ",// 1257
	"tSplexem_Undefined          ",// 1258
	"tSplexem_Undefined          ",// 1259
	"tSplexem_Undefined          ",// 1260
	"tSplexem_Undefined          ",// 1261
	"tSplexem_Undefined          ",// 1262
	"tSplexem_Undefined          ",// 1263
	"tSplexem_Undefined          ",// 1264
	"tSplexem_Undefined          ",// 1265
	"tSplexem_Undefined          ",// 1266
	"tSplexem_Undefined          ",// 1267
	"tSplexem_Undefined          ",// 1268
	"tSplexem_Undefined          ",// 1269
	"tSplexem_Undefined          ",// 1270
	"tSplexem_Undefined          ",// 1271
	"tSplexem_Undefined          ",// 1272
	"tSplexem_Undefined          ",// 1273
	"tSplexem_Undefined          ",// 1274
	"tSplexem_Undefined          ",// 1275
	"tSplexem_Undefined          ",// 1276
	"tSplexem_Undefined          ",// 1277
	"tSplexem_Undefined          ",// 1278
	"tSplexem_Undefined          ",// 1279
	"tSplexem_Undefined          ",// 1280
	"tSplexem_Undefined          ",// 1281
	"tSplexem_Undefined          ",// 1282
	"tSplexem_Undefined          ",// 1283
	"tSplexem_Undefined          ",// 1284
	"tSplexem_Undefined          ",// 1285
	"tSplexem_Undefined          ",// 1286
	"tSplexem_Undefined          ",// 1287
	"tSplexem_Undefined          ",// 1288
	"tSplexem_Undefined          ",// 1289
	"tSplexem_Undefined          ",// 1290
	"tSplexem_Undefined          ",// 1291
	"tSplexem_Undefined          ",// 1292
	"tSplexem_Undefined          ",// 1293
	"tSplexem_Undefined          ",// 1294
	"tSplexem_Undefined          ",// 1295
	"tSplexem_Undefined          ",// 1296
	"tSplexem_Undefined          ",// 1297
	"tSplexem_Undefined          ",// 1298
	"tSplexem_Undefined          ",// 1299
	"tSplexem_Undefined          ",// 1300
	"tSplexem_Undefined          ",// 1301
	"tSplexem_Undefined          ",// 1302
	"tSplexem_Undefined          ",// 1303
	"tSplexem_Undefined          ",// 1304
	"tSplexem_Undefined          ",// 1305
	"tSplexem_Undefined          ",// 1306
	"tSplexem_Undefined          ",// 1307
	"tSplexem_Undefined          ",// 1308
	"tSplexem_Undefined          ",// 1309
	"tSplexem_Undefined          ",// 1310
	"tSplexem_Undefined          ",// 1311
	"tSplexem_Undefined          ",// 1312
	"tSplexem_Undefined          ",// 1313
	"tSplexem_Undefined          ",// 1314
	"tSplexem_Undefined          ",// 1315
	"tSplexem_Undefined          ",// 1316
	"tSplexem_Undefined          ",// 1317
	"tSplexem_Undefined          ",// 1318
	"tSplexem_Undefined          ",// 1319
	"tSplexem_Undefined          ",// 1320
	"tSplexem_Undefined          ",// 1321
	"tSplexem_Undefined          ",// 1322
	"tSplexem_Undefined          ",// 1323
	"tSplexem_Undefined          ",// 1324
	"tSplexem_Undefined          ",// 1325
	"tSplexem_Undefined          ",// 1326
	"tSplexem_Undefined          ",// 1327
	"tSplexem_Undefined          ",// 1328
	"tSplexem_Undefined          ",// 1329
	"tSplexem_Undefined          ",// 1330
	"tSplexem_Undefined          ",// 1331
	"tSplexem_Undefined          ",// 1332
	"tSplexem_Undefined          ",// 1333
	"tSplexem_Undefined          ",// 1334
	"tSplexem_Undefined          ",// 1335
	"tSplexem_Undefined          ",// 1336
	"tSplexem_Undefined          ",// 1337
	"tSplexem_Undefined          ",// 1338
	"tSplexem_Undefined          ",// 1339
	"tSplexem_Undefined          ",// 1340
	"tSplexem_Undefined          ",// 1341
	"tSplexem_Undefined          ",// 1342
	"tSplexem_Undefined          ",// 1343
	"tSplexem_Undefined          ",// 1344
	"tSplexem_Undefined          ",// 1345
	"tSplexem_Undefined          ",// 1346
	"tSplexem_Undefined          ",// 1347
	"tSplexem_Undefined          ",// 1348
	"tSplexem_Undefined          ",// 1349
	"tSplexem_Undefined          ",// 1350
	"tSplexem_Undefined          ",// 1351
	"tSplexem_Undefined          ",// 1352
	"tSplexem_Undefined          ",// 1353
	"tSplexem_Undefined          ",// 1354
	"tSplexem_Undefined          ",// 1355
	"tSplexem_Undefined          ",// 1356
	"tSplexem_Undefined          ",// 1357
	"tSplexem_Undefined          ",// 1358
	"tSplexem_Undefined          ",// 1359
	"tSplexem_Undefined          ",// 1360
	"tSplexem_Undefined          ",// 1361
	"tSplexem_Undefined          ",// 1362
	"tSplexem_Undefined          ",// 1363
	"tSplexem_Undefined          ",// 1364
	"tSplexem_Undefined          ",// 1365
	"tSplexem_Undefined          ",// 1366
	"tSplexem_Undefined          ",// 1367
	"tSplexem_Undefined          ",// 1368
	"tSplexem_Undefined          ",// 1369
	"tSplexem_Undefined          ",// 1370
	"tSplexem_Undefined          ",// 1371
	"tSplexem_Undefined          ",// 1372
	"tSplexem_Undefined          ",// 1373
	"tSplexem_Undefined          ",// 1374
	"tSplexem_Undefined          ",// 1375
	"tSplexem_Undefined          ",// 1376
	"tSplexem_Undefined          ",// 1377
	"tSplexem_Undefined          ",// 1378
	"tSplexem_Undefined          ",// 1379
	"tSplexem_Undefined          ",// 1380
	"tSplexem_Undefined          ",// 1381
	"tSplexem_Undefined          ",// 1382
	"tSplexem_Undefined          ",// 1383
	"tSplexem_Undefined          ",// 1384
	"tSplexem_Undefined          ",// 1385
	"tSplexem_Undefined          ",// 1386
	"tSplexem_Undefined          ",// 1387
	"tSplexem_Undefined          ",// 1388
	"tSplexem_Undefined          ",// 1389
	"tSplexem_Undefined          ",// 1390
	"tSplexem_Undefined          ",// 1391
	"tSplexem_Undefined          ",// 1392
	"tSplexem_Undefined          ",// 1393
	"tSplexem_Undefined          ",// 1394
	"tSplexem_Undefined          ",// 1395
	"tSplexem_Undefined          ",// 1396
	"tSplexem_Undefined          ",// 1397
	"tSplexem_Undefined          ",// 1398
	"tSplexem_Undefined          ",// 1399
	"tSplexem_Undefined          ",// 1400
	"tSplexem_Undefined          ",// 1401
	"tSplexem_Undefined          ",// 1402
	"tSplexem_Undefined          ",// 1403
	"tSplexem_Undefined          ",// 1404
	"tSplexem_Undefined          ",// 1405
	"tSplexem_Undefined          ",// 1406
	"tSplexem_Undefined          ",// 1407
	"tSplexem_Undefined          ",// 1408
	"tSplexem_Undefined          ",// 1409
	"tSplexem_Undefined          ",// 1410
	"tSplexem_Undefined          ",// 1411
	"tSplexem_Undefined          ",// 1412
	"tSplexem_Undefined          ",// 1413
	"tSplexem_Undefined          ",// 1414
	"tSplexem_Undefined          ",// 1415
	"tSplexem_Undefined          ",// 1416
	"tSplexem_Undefined          ",// 1417
	"tSplexem_Undefined          ",// 1418
	"tSplexem_Undefined          ",// 1419
	"tSplexem_Undefined          ",// 1420
	"tSplexem_Undefined          ",// 1421
	"tSplexem_Undefined          ",// 1422
	"tSplexem_Undefined          ",// 1423
	"tSplexem_Undefined          ",// 1424
	"tSplexem_Undefined          ",// 1425
	"tSplexem_Undefined          ",// 1426
	"tSplexem_Undefined          ",// 1427
	"tSplexem_Undefined          ",// 1428
	"tSplexem_Undefined          ",// 1429
	"tSplexem_Undefined          ",// 1430
	"tSplexem_Undefined          ",// 1431
	"tSplexem_Undefined          ",// 1432
	"tSplexem_Undefined          ",// 1433
	"tSplexem_Undefined          ",// 1434
	"tSplexem_Undefined          ",// 1435
	"tSplexem_Undefined          ",// 1436
	"tSplexem_Undefined          ",// 1437
	"tSplexem_Undefined          ",// 1438
	"tSplexem_Undefined          ",// 1439
	"tSplexem_Undefined          ",// 1440
	"tSplexem_Undefined          ",// 1441
	"tSplexem_Undefined          ",// 1442
	"tSplexem_Undefined          ",// 1443
	"tSplexem_Undefined          ",// 1444
	"tSplexem_Undefined          ",// 1445
	"tSplexem_Undefined          ",// 1446
	"tSplexem_Undefined          ",// 1447
	"tSplexem_Undefined          ",// 1448
	"tSplexem_Undefined          ",// 1449
	"tSplexem_Undefined          ",// 1450
	"tSplexem_Undefined          ",// 1451
	"tSplexem_Undefined          ",// 1452
	"tSplexem_Undefined          ",// 1453
	"tSplexem_Undefined          ",// 1454
	"tSplexem_Undefined          ",// 1455
	"tSplexem_Undefined          ",// 1456
	"tSplexem_Undefined          ",// 1457
	"tSplexem_Undefined          ",// 1458
	"tSplexem_Undefined          ",// 1459
	"tSplexem_Undefined          ",// 1460
	"tSplexem_Undefined          ",// 1461
	"tSplexem_Undefined          ",// 1462
	"tSplexem_Undefined          ",// 1463
	"tSplexem_Undefined          ",// 1464
	"tSplexem_Undefined          ",// 1465
	"tSplexem_Undefined          ",// 1466
	"tSplexem_Undefined          ",// 1467
	"tSplexem_Undefined          ",// 1468
	"tSplexem_Undefined          ",// 1469
	"tSplexem_Undefined          ",// 1470
	"tSplexem_Undefined          ",// 1471
	"tSplexem_Undefined          ",// 1472
	"tSplexem_Undefined          ",// 1473
	"tSplexem_Undefined          ",// 1474
	"tSplexem_Undefined          ",// 1475
	"tSplexem_Undefined          ",// 1476
	"tSplexem_Undefined          ",// 1477
	"tSplexem_Undefined          ",// 1478
	"tSplexem_Undefined          ",// 1479
	"tSplexem_Undefined          ",// 1480
	"tSplexem_Undefined          ",// 1481
	"tSplexem_Undefined          ",// 1482
	"tSplexem_Undefined          ",// 1483
	"tSplexem_Undefined          ",// 1484
	"tSplexem_Undefined          ",// 1485
	"tSplexem_Undefined          ",// 1486
	"tSplexem_Undefined          ",// 1487
	"tSplexem_Undefined          ",// 1488
	"tSplexem_Undefined          ",// 1489
	"tSplexem_Undefined          ",// 1490
	"tSplexem_Undefined          ",// 1491
	"tSplexem_Undefined          ",// 1492
	"tSplexem_Undefined          ",// 1493
	"tSplexem_Undefined          ",// 1494
	"tSplexem_Undefined          ",// 1495
	"tSplexem_Undefined          ",// 1496
	"tSplexem_Undefined          ",// 1497
	"tSplexem_Undefined          ",// 1498
	"tSplexem_Undefined          ",// 1499
	"tSplexem_Undefined          ",// 1500
	"tSplexem_Undefined          ",// 1501
	"tSplexem_Undefined          ",// 1502
	"tSplexem_Undefined          ",// 1503
	"tSplexem_Undefined          ",// 1504
	"tSplexem_Undefined          ",// 1505
	"tSplexem_Undefined          ",// 1506
	"tSplexem_Undefined          ",// 1507
	"tSplexem_Undefined          ",// 1508
	"tSplexem_Undefined          ",// 1509
	"tSplexem_Undefined          ",// 1510
	"tSplexem_Undefined          ",// 1511
	"tSplexem_Undefined          ",// 1512
	"tSplexem_Undefined          ",// 1513
	"tSplexem_Undefined          ",// 1514
	"tSplexem_Undefined          ",// 1515
	"tSplexem_Undefined          ",// 1516
	"tSplexem_Undefined          ",// 1517
	"tSplexem_Undefined          ",// 1518
	"tSplexem_Undefined          ",// 1519
	"tSplexem_Undefined          ",// 1520
	"tSplexem_Undefined          ",// 1521
	"tSplexem_Undefined          ",// 1522
	"tSplexem_Undefined          ",// 1523
	"tSplexem_Undefined          ",// 1524
	"tSplexem_Undefined          ",// 1525
	"tSplexem_Undefined          ",// 1526
	"tSplexem_Undefined          ",// 1527
	"tSplexem_Undefined          ",// 1528
	"tSplexem_Undefined          ",// 1529
	"tSplexem_Undefined          ",// 1530
	"tSplexem_Undefined          ",// 1531
	"tSplexem_Undefined          ",// 1532
	"tSplexem_Undefined          ",// 1533
	"tSplexem_Undefined          ",// 1534
	"tSplexem_Undefined          ",// 1535
	"v.compilernop"               ,// 1536
	"v.nop"                       ,// 1537
	"v.jcond"                     ,// 1538
	"v.jncond"                    ,// 1539
	"v.int3"                      ,// 1540
	"v.r"                         ,// 1541
	"v.syscall"                   ,// 1542
	"v.d"                         ,// 1543
	"v.enter"                     ,// 1544
	"v.const"                     ,// 1545
	"v.return"                    ,// 1546
	"v.leave"                     ,// 1547
	"v.prereturn"                 ,// 1548
	"v.ld"                        ,// 1549
	"v.lea"                       ,// 1550
	"v.indexfp"                   ,// 1551
	"v.pha"                       ,// 1552
	"v.plb"                       ,// 1553
	"v.add"                       ,// 1554
	"v.substract"                 ,// 1555
	"?"                           ,// 1556
	"?"                           ,// 1557
	"?"                           ,// 1558
	"?"                           ,// 1559
	"?"                           ,// 1560
	"?"                           ,// 1561
	"?"                           ,// 1562
	"?"                           ,// 1563
	"?"                           ,// 1564
	"?"                           ,// 1565
	"?"                           ,// 1566
	"?"                           ,// 1567
	"?"                           ,// 1568
	"?"                           ,// 1569
	"?"                           ,// 1570
	"?"                           ,// 1571
	"?"                           ,// 1572
	"?"                           ,// 1573
	"?"                           ,// 1574
	"?"                           ,// 1575
	"?"                           ,// 1576
	"?"                           ,// 1577
	"?"                           ,// 1578
	"?"                           ,// 1579
	"?"                           ,// 1580
	"?"                           ,// 1581
	"?"                           ,// 1582
	"?"                           ,// 1583
	"?"                           ,// 1584
	"?"                           ,// 1585
	"?"                           ,// 1586
	"?"                           ,// 1587
};
char *TokenidtoName_Compact[]={
	"(nil)",// 0 ,
	"->",// 1 ,
	"::" ,// 2 ,
	"++" ,// 3 ,
	"--" ,// 4 ,
	"("  ,// 5 ,
	")"  ,// 6 ,
	"["  ,// 7 ,
	"]"  ,// 8 ,
	"{"  ,// 9 ,
	"}"  ,// 10,
	"."  ,// 11,
	"!"  ,// 12,
	"~"  ,// 13,
	"&"  ,// 14,
	"+"  ,// 15,
	"-"  ,// 16,
	"*"  ,// 17,
	"/"  ,// 18,
	"%"  ,// 19,
	"<<" ,// 20,
	">>" ,// 21,
	"<"  ,// 22,
	">"  ,// 23,
	"<=" ,// 24,
	">=",// 25,
	"<=>",// 26,
	"="  ,// 27,
	"==" ,// 28,
	"!=" ,// 29,
	"^"  ,// 30,
	"|"  ,// 31,
	"&&" ,// 32,
	"||" ,// 33,
	"?"  ,// 34,
	":"  ,// 35,
	"+=" ,// 36,
	"-=" ,// 37,
	"*=" ,// 38,
	"/=" ,// 39,
	"%=" ,// 40,
	"<<=",// 41,
	">>=",// 42,
	"&=" ,// 43,
	"^=" ,// 44,
	"|=" ,// 45,
	","  ,// 46,
	";"  ,// 47,
	"...",// 48 
	"?",// 49 
	"?",// 50 
	"?",// 51 
	"?",// 52 
	"?",// 53 
	"?",// 54 
	"?",// 55 
	"?",// 56 
	"?",// 57 
	"?",// 58 
	"?",// 59 
	"?",// 60 
	"?",// 61 
	"?",// 62 
	"?",// 63 
	"?",// 64 
	"?",// 65 
	"?",// 66 
	"?",// 67 
	"?",// 68 
	"?",// 69 
	"?",// 70 
	"?",// 71 
	"?",// 72 
	"?",// 73 
	"?",// 74 
	"?",// 75 
	"?",// 76 
	"?",// 77 
	"?",// 78 
	"?",// 79 
	"?",// 80 
	"?",// 81 
	"?",// 82 
	"?",// 83 
	"?",// 84 
	"?",// 85 
	"?",// 86 
	"?",// 87 
	"?",// 88 
	"?",// 89 
	"?",// 90 
	"?",// 91 
	"?",// 92 
	"?",// 93 
	"?",// 94 
	"?",// 95 
	"?",// 96 
	"?",// 97 
	"?",// 98 
	"?",// 99 
	"?",// 100
	"?",// 101
	"?",// 102
	"?",// 103
	"?",// 104
	"?",// 105
	"?",// 106
	"?",// 107
	"?",// 108
	"?",// 109
	"?",// 110
	"?",// 111
	"?",// 112
	"?",// 113
	"?",// 114
	"?",// 115
	"?",// 116
	"?",// 117
	"?",// 118
	"?",// 119
	"?",// 120
	"?",// 121
	"?",// 122
	"?",// 123
	"?",// 124
	"?",// 125
	"?",// 126
	"?",// 127
	"alignas"      ,// 128
	"alignof"      ,// 129
	"auto"         ,// 130
	"bool"         ,// 131
	"break"        ,// 132
	"case"         ,// 133
	"char"         ,// 134
	"const"        ,// 135
	"constexpr"    ,// 136
	"continue"     ,// 137
	"default"      ,// 138
	"do"           ,// 139
	"double"       ,// 140
	"else"         ,// 141
	"enum"         ,// 142
	"extern"       ,// 143
	"false"        ,// 144
	"float"        ,// 145
	"for"          ,// 146
	"goto"         ,// 147
	"if"           ,// 148
	"inline"       ,// 149
	"int"          ,// 150
	"long"         ,// 151
	"nullptr"      ,// 152
	"register"     ,// 153
	"restrict"     ,// 154
	"return"       ,// 155
	"short"        ,// 156
	"signed"       ,// 157
	"sizeof"       ,// 158
	"static"       ,// 159
	"static_assert",// 160
	"struct"       ,// 161
	"switch"       ,// 162
	"thread_local" ,// 163
	"true"         ,// 164
	"typedef"      ,// 165
	"typeof"       ,// 166
	"typeof_unqual",// 167
	"union"        ,// 168
	"unsigned"     ,// 169
	"void"         ,// 170
	"volatile"     ,// 171
	"while"        ,// 172
	"int8_t"       ,// 173
	"int16_t"      ,// 174
	"int32_t"      ,// 175
	"int64_t"      ,// 176
	"uint8_t"      ,// 177
	"uint16_t"     ,// 178
	"uint32_t"     ,// 179
	"uint64_t"     ,// 180
	"?",// 181
	"?",// 182
	"?",// 183
	"?",// 184
	"?",// 185
	"?",// 186
	"?",// 187
	"?",// 188
	"?",// 189
	"?",// 190
	"?",// 191
	"?",// 192
	"?",// 193
	"?",// 194
	"?",// 195
	"?",// 196
	"?",// 197
	"?",// 198
	"?",// 199
	"?",// 200
	"?",// 201
	"?",// 202
	"?",// 203
	"?",// 204
	"?",// 205
	"?",// 206
	"?",// 207
	"?",// 208
	"?",// 209
	"?",// 210
	"?",// 211
	"?",// 212
	"?",// 213
	"?",// 214
	"?",// 215
	"?",// 216
	"?",// 217
	"?",// 218
	"?",// 219
	"?",// 220
	"?",// 221
	"?",// 222
	"?",// 223
	"?",// 224
	"?",// 225
	"?",// 226
	"?",// 227
	"?",// 228
	"?",// 229
	"?",// 230
	"?",// 231
	"?",// 232
	"?",// 233
	"?",// 234
	"?",// 235
	"?",// 236
	"?",// 237
	"?",// 238
	"?",// 239
	"?",// 240
	"?",// 241
	"?",// 242
	"?",// 243
	"?",// 244
	"?",// 245
	"?",// 246
	"?",// 247
	"?",// 248
	"?",// 249
	"?",// 250
	"?",// 251
	"?",// 252
	"?",// 253
	"?",// 254
	"?",// 255
	"?",// 256
	"?",// 257
	"?",// 258
	"?",// 259
	"?",// 260
	"?",// 261
	"?",// 262
	"?",// 263
	"?",// 264
	"?",// 265
	"?",// 266
	"?",// 267
	"?",// 268
	"?",// 269
	"?",// 270
	"?",// 271
	"?",// 272
	"?",// 273
	"?",// 274
	"?",// 275
	"?",// 276
	"?",// 277
	"?",// 278
	"?",// 279
	"?",// 280
	"?",// 281
	"?",// 282
	"?",// 283
	"?",// 284
	"?",// 285
	"?",// 286
	"?",// 287
	"?",// 288
	"?",// 289
	"?",// 290
	"?",// 291
	"?",// 292
	"?",// 293
	"?",// 294
	"?",// 295
	"?",// 296
	"?",// 297
	"?",// 298
	"?",// 299
	"?",// 300
	"?",// 301
	"?",// 302
	"?",// 303
	"?",// 304
	"?",// 305
	"?",// 306
	"?",// 307
	"?",// 308
	"?",// 309
	"?",// 310
	"?",// 311
	"?",// 312
	"?",// 313
	"?",// 314
	"?",// 315
	"?",// 316
	"?",// 317
	"?",// 318
	"?",// 319
	"?",// 320
	"?",// 321
	"?",// 322
	"?",// 323
	"?",// 324
	"?",// 325
	"?",// 326
	"?",// 327
	"?",// 328
	"?",// 329
	"?",// 330
	"?",// 331
	"?",// 332
	"?",// 333
	"?",// 334
	"?",// 335
	"?",// 336
	"?",// 337
	"?",// 338
	"?",// 339
	"?",// 340
	"?",// 341
	"?",// 342
	"?",// 343
	"?",// 344
	"?",// 345
	"?",// 346
	"?",// 347
	"?",// 348
	"?",// 349
	"?",// 350
	"?",// 351
	"?",// 352
	"?",// 353
	"?",// 354
	"?",// 355
	"?",// 356
	"?",// 357
	"?",// 358
	"?",// 359
	"?",// 360
	"?",// 361
	"?",// 362
	"?",// 363
	"?",// 364
	"?",// 365
	"?",// 366
	"?",// 367
	"?",// 368
	"?",// 369
	"?",// 370
	"?",// 371
	"?",// 372
	"?",// 373
	"?",// 374
	"?",// 375
	"?",// 376
	"?",// 377
	"?",// 378
	"?",// 379
	"?",// 380
	"?",// 381
	"?",// 382
	"?",// 383
	"?",// 384
	"?",// 385
	"?",// 386
	"?",// 387
	"?",// 388
	"?",// 389
	"?",// 390
	"?",// 391
	"?",// 392
	"?",// 393
	"?",// 394
	"?",// 395
	"?",// 396
	"?",// 397
	"?",// 398
	"?",// 399
	"?",// 400
	"?",// 401
	"?",// 402
	"?",// 403
	"?",// 404
	"?",// 405
	"?",// 406
	"?",// 407
	"?",// 408
	"?",// 409
	"?",// 410
	"?",// 411
	"?",// 412
	"?",// 413
	"?",// 414
	"?",// 415
	"?",// 416
	"?",// 417
	"?",// 418
	"?",// 419
	"?",// 420
	"?",// 421
	"?",// 422
	"?",// 423
	"?",// 424
	"?",// 425
	"?",// 426
	"?",// 427
	"?",// 428
	"?",// 429
	"?",// 430
	"?",// 431
	"?",// 432
	"?",// 433
	"?",// 434
	"?",// 435
	"?",// 436
	"?",// 437
	"?",// 438
	"?",// 439
	"?",// 440
	"?",// 441
	"?",// 442
	"?",// 443
	"?",// 444
	"?",// 445
	"?",// 446
	"?",// 447
	"?",// 448
	"?",// 449
	"?",// 450
	"?",// 451
	"?",// 452
	"?",// 453
	"?",// 454
	"?",// 455
	"?",// 456
	"?",// 457
	"?",// 458
	"?",// 459
	"?",// 460
	"?",// 461
	"?",// 462
	"?",// 463
	"?",// 464
	"?",// 465
	"?",// 466
	"?",// 467
	"?",// 468
	"?",// 469
	"?",// 470
	"?",// 471
	"?",// 472
	"?",// 473
	"?",// 474
	"?",// 475
	"?",// 476
	"?",// 477
	"?",// 478
	"?",// 479
	"?",// 480
	"?",// 481
	"?",// 482
	"?",// 483
	"?",// 484
	"?",// 485
	"?",// 486
	"?",// 487
	"?",// 488
	"?",// 489
	"?",// 490
	"?",// 491
	"?",// 492
	"?",// 493
	"?",// 494
	"?",// 495
	"?",// 496
	"?",// 497
	"?",// 498
	"?",// 499
	"?",// 500
	"?",// 501
	"?",// 502
	"?",// 503
	"?",// 504
	"?",// 505
	"?",// 506
	"?",// 507
	"?",// 508
	"?",// 509
	"?",// 510
	"?",// 511
	"?",// 512
	"?",// 513
	"?",// 514
	"?",// 515
	"?",// 516
	"?",// 517
	"?",// 518
	"?",// 519
	"?",// 520
	"?",// 521
	"?",// 522
	"?",// 523
	"?",// 524
	"?",// 525
	"?",// 526
	"?",// 527
	"?",// 528
	"?",// 529
	"?",// 530
	"?",// 531
	"?",// 532
	"?",// 533
	"?",// 534
	"?",// 535
	"?",// 536
	"?",// 537
	"?",// 538
	"?",// 539
	"?",// 540
	"?",// 541
	"?",// 542
	"?",// 543
	"?",// 544
	"?",// 545
	"?",// 546
	"?",// 547
	"?",// 548
	"?",// 549
	"?",// 550
	"?",// 551
	"?",// 552
	"?",// 553
	"?",// 554
	"?",// 555
	"?",// 556
	"?",// 557
	"?",// 558
	"?",// 559
	"?",// 560
	"?",// 561
	"?",// 562
	"?",// 563
	"?",// 564
	"?",// 565
	"?",// 566
	"?",// 567
	"?",// 568
	"?",// 569
	"?",// 570
	"?",// 571
	"?",// 572
	"?",// 573
	"?",// 574
	"?",// 575
	"?",// 576
	"?",// 577
	"?",// 578
	"?",// 579
	"?",// 580
	"?",// 581
	"?",// 582
	"?",// 583
	"?",// 584
	"?",// 585
	"?",// 586
	"?",// 587
	"?",// 588
	"?",// 589
	"?",// 590
	"?",// 591
	"?",// 592
	"?",// 593
	"?",// 594
	"?",// 595
	"?",// 596
	"?",// 597
	"?",// 598
	"?",// 599
	"?",// 600
	"?",// 601
	"?",// 602
	"?",// 603
	"?",// 604
	"?",// 605
	"?",// 606
	"?",// 607
	"?",// 608
	"?",// 609
	"?",// 610
	"?",// 611
	"?",// 612
	"?",// 613
	"?",// 614
	"?",// 615
	"?",// 616
	"?",// 617
	"?",// 618
	"?",// 619
	"?",// 620
	"?",// 621
	"?",// 622
	"?",// 623
	"?",// 624
	"?",// 625
	"?",// 626
	"?",// 627
	"?",// 628
	"?",// 629
	"?",// 630
	"?",// 631
	"?",// 632
	"?",// 633
	"?",// 634
	"?",// 635
	"?",// 636
	"?",// 637
	"?",// 638
	"?",// 639
	"?",// 640
	"?",// 641
	"?",// 642
	"?",// 643
	"?",// 644
	"?",// 645
	"?",// 646
	"?",// 647
	"?",// 648
	"?",// 649
	"?",// 650
	"?",// 651
	"?",// 652
	"?",// 653
	"?",// 654
	"?",// 655
	"?",// 656
	"?",// 657
	"?",// 658
	"?",// 659
	"?",// 660
	"?",// 661
	"?",// 662
	"?",// 663
	"?",// 664
	"?",// 665
	"?",// 666
	"?",// 667
	"?",// 668
	"?",// 669
	"?",// 670
	"?",// 671
	"?",// 672
	"?",// 673
	"?",// 674
	"?",// 675
	"?",// 676
	"?",// 677
	"?",// 678
	"?",// 679
	"?",// 680
	"?",// 681
	"?",// 682
	"?",// 683
	"?",// 684
	"?",// 685
	"?",// 686
	"?",// 687
	"?",// 688
	"?",// 689
	"?",// 690
	"?",// 691
	"?",// 692
	"?",// 693
	"?",// 694
	"?",// 695
	"?",// 696
	"?",// 697
	"?",// 698
	"?",// 699
	"?",// 700
	"?",// 701
	"?",// 702
	"?",// 703
	"?",// 704
	"?",// 705
	"?",// 706
	"?",// 707
	"?",// 708
	"?",// 709
	"?",// 710
	"?",// 711
	"?",// 712
	"?",// 713
	"?",// 714
	"?",// 715
	"?",// 716
	"?",// 717
	"?",// 718
	"?",// 719
	"?",// 720
	"?",// 721
	"?",// 722
	"?",// 723
	"?",// 724
	"?",// 725
	"?",// 726
	"?",// 727
	"?",// 728
	"?",// 729
	"?",// 730
	"?",// 731
	"?",// 732
	"?",// 733
	"?",// 734
	"?",// 735
	"?",// 736
	"?",// 737
	"?",// 738
	"?",// 739
	"?",// 740
	"?",// 741
	"?",// 742
	"?",// 743
	"?",// 744
	"?",// 745
	"?",// 746
	"?",// 747
	"?",// 748
	"?",// 749
	"?",// 750
	"?",// 751
	"?",// 752
	"?",// 753
	"?",// 754
	"?",// 755
	"?",// 756
	"?",// 757
	"?",// 758
	"?",// 759
	"?",// 760
	"?",// 761
	"?",// 762
	"?",// 763
	"?",// 764
	"?",// 765
	"?",// 766
	"?",// 767
	"?",// 768
	"?",// 769
	"?",// 770
	"?",// 771
	"?",// 772
	"?",// 773
	"?",// 774
	"?",// 775
	"?",// 776
	"?",// 777
	"?",// 778
	"?",// 779
	"?",// 780
	"?",// 781
	"?",// 782
	"?",// 783
	"?",// 784
	"?",// 785
	"?",// 786
	"?",// 787
	"?",// 788
	"?",// 789
	"?",// 790
	"?",// 791
	"?",// 792
	"?",// 793
	"?",// 794
	"?",// 795
	"?",// 796
	"?",// 797
	"?",// 798
	"?",// 799
	"?",// 800
	"?",// 801
	"?",// 802
	"?",// 803
	"?",// 804
	"?",// 805
	"?",// 806
	"?",// 807
	"?",// 808
	"?",// 809
	"?",// 810
	"?",// 811
	"?",// 812
	"?",// 813
	"?",// 814
	"?",// 815
	"?",// 816
	"?",// 817
	"?",// 818
	"?",// 819
	"?",// 820
	"?",// 821
	"?",// 822
	"?",// 823
	"?",// 824
	"?",// 825
	"?",// 826
	"?",// 827
	"?",// 828
	"?",// 829
	"?",// 830
	"?",// 831
	"?",// 832
	"?",// 833
	"?",// 834
	"?",// 835
	"?",// 836
	"?",// 837
	"?",// 838
	"?",// 839
	"?",// 840
	"?",// 841
	"?",// 842
	"?",// 843
	"?",// 844
	"?",// 845
	"?",// 846
	"?",// 847
	"?",// 848
	"?",// 849
	"?",// 850
	"?",// 851
	"?",// 852
	"?",// 853
	"?",// 854
	"?",// 855
	"?",// 856
	"?",// 857
	"?",// 858
	"?",// 859
	"?",// 860
	"?",// 861
	"?",// 862
	"?",// 863
	"?",// 864
	"?",// 865
	"?",// 866
	"?",// 867
	"?",// 868
	"?",// 869
	"?",// 870
	"?",// 871
	"?",// 872
	"?",// 873
	"?",// 874
	"?",// 875
	"?",// 876
	"?",// 877
	"?",// 878
	"?",// 879
	"?",// 880
	"?",// 881
	"?",// 882
	"?",// 883
	"?",// 884
	"?",// 885
	"?",// 886
	"?",// 887
	"?",// 888
	"?",// 889
	"?",// 890
	"?",// 891
	"?",// 892
	"?",// 893
	"?",// 894
	"?",// 895
	"?",// 896
	"?",// 897
	"?",// 898
	"?",// 899
	"?",// 900
	"?",// 901
	"?",// 902
	"?",// 903
	"?",// 904
	"?",// 905
	"?",// 906
	"?",// 907
	"?",// 908
	"?",// 909
	"?",// 910
	"?",// 911
	"?",// 912
	"?",// 913
	"?",// 914
	"?",// 915
	"?",// 916
	"?",// 917
	"?",// 918
	"?",// 919
	"?",// 920
	"?",// 921
	"?",// 922
	"?",// 923
	"?",// 924
	"?",// 925
	"?",// 926
	"?",// 927
	"?",// 928
	"?",// 929
	"?",// 930
	"?",// 931
	"?",// 932
	"?",// 933
	"?",// 934
	"?",// 935
	"?",// 936
	"?",// 937
	"?",// 938
	"?",// 939
	"?",// 940
	"?",// 941
	"?",// 942
	"?",// 943
	"?",// 944
	"?",// 945
	"?",// 946
	"?",// 947
	"?",// 948
	"?",// 949
	"?",// 950
	"?",// 951
	"?",// 952
	"?",// 953
	"?",// 954
	"?",// 955
	"?",// 956
	"?",// 957
	"?",// 958
	"?",// 959
	"?",// 960
	"?",// 961
	"?",// 962
	"?",// 963
	"?",// 964
	"?",// 965
	"?",// 966
	"?",// 967
	"?",// 968
	"?",// 969
	"?",// 970
	"?",// 971
	"?",// 972
	"?",// 973
	"?",// 974
	"?",// 975
	"?",// 976
	"?",// 977
	"?",// 978
	"?",// 979
	"?",// 980
	"?",// 981
	"?",// 982
	"?",// 983
	"?",// 984
	"?",// 985
	"?",// 986
	"?",// 987
	"?",// 988
	"?",// 989
	"?",// 990
	"?",// 991
	"?",// 992
	"?",// 993
	"?",// 994
	"?",// 995
	"?",// 996
	"?",// 997
	"?",// 998
	"?",// 999
	"?",// 1000
	"?",// 1001
	"?",// 1002
	"?",// 1003
	"?",// 1004
	"?",// 1005
	"?",// 1006
	"?",// 1007
	"?",// 1008
	"?",// 1009
	"?",// 1010
	"?",// 1011
	"?",// 1012
	"?",// 1013
	"?",// 1014
	"?",// 1015
	"?",// 1016
	"?",// 1017
	"?",// 1018
	"?",// 1019
	"?",// 1020
	"?",// 1021
	"?",// 1022
	"?",// 1023
	"?",// 1024
	"?",// 1025
	"?",// 1026
	"?",// 1027
	"?",// 1028
	"?",// 1029
	"?",// 1030
	"?",// 1031
	"?",// 1032
	"?",// 1033
	"?",// 1034
	"?",// 1035
	"?",// 1036
	"?",// 1037
	"?",// 1038
	"?",// 1039
	"?",// 1040
	"?",// 1041
	"?",// 1042
	"?",// 1043
	"?",// 1044
	"?",// 1045
	"?",// 1046
	"?",// 1047
	"?",// 1048
	"?",// 1049
	"?",// 1050
	"?",// 1051
	"?",// 1052
	"?",// 1053
	"?",// 1054
	"?",// 1055
	"?",// 1056
	"?",// 1057
	"?",// 1058
	"?",// 1059
	"?",// 1060
	"?",// 1061
	"?",// 1062
	"?",// 1063
	"?",// 1064
	"?",// 1065
	"?",// 1066
	"?",// 1067
	"?",// 1068
	"?",// 1069
	"?",// 1070
	"?",// 1071
	"?",// 1072
	"?",// 1073
	"?",// 1074
	"?",// 1075
	"?",// 1076
	"?",// 1077
	"?",// 1078
	"?",// 1079
	"?",// 1080
	"?",// 1081
	"?",// 1082
	"?",// 1083
	"?",// 1084
	"?",// 1085
	"?",// 1086
	"?",// 1087
	"?",// 1088
	"?",// 1089
	"?",// 1090
	"?",// 1091
	"?",// 1092
	"?",// 1093
	"?",// 1094
	"?",// 1095
	"?",// 1096
	"?",// 1097
	"?",// 1098
	"?",// 1099
	"?",// 1100
	"?",// 1101
	"?",// 1102
	"?",// 1103
	"?",// 1104
	"?",// 1105
	"?",// 1106
	"?",// 1107
	"?",// 1108
	"?",// 1109
	"?",// 1110
	"?",// 1111
	"?",// 1112
	"?",// 1113
	"?",// 1114
	"?",// 1115
	"?",// 1116
	"?",// 1117
	"?",// 1118
	"?",// 1119
	"?",// 1120
	"?",// 1121
	"?",// 1122
	"?",// 1123
	"?",// 1124
	"?",// 1125
	"?",// 1126
	"?",// 1127
	"?",// 1128
	"?",// 1129
	"?",// 1130
	"?",// 1131
	"?",// 1132
	"?",// 1133
	"?",// 1134
	"?",// 1135
	"?",// 1136
	"?",// 1137
	"?",// 1138
	"?",// 1139
	"?",// 1140
	"?",// 1141
	"?",// 1142
	"?",// 1143
	"?",// 1144
	"?",// 1145
	"?",// 1146
	"?",// 1147
	"?",// 1148
	"?",// 1149
	"?",// 1150
	"?",// 1151
	"?",// 1152
	"?",// 1153
	"?",// 1154
	"?",// 1155
	"?",// 1156
	"?",// 1157
	"?",// 1158
	"?",// 1159
	"?",// 1160
	"?",// 1161
	"?",// 1162
	"?",// 1163
	"?",// 1164
	"?",// 1165
	"?",// 1166
	"?",// 1167
	"?",// 1168
	"?",// 1169
	"?",// 1170
	"?",// 1171
	"?",// 1172
	"?",// 1173
	"?",// 1174
	"?",// 1175
	"?",// 1176
	"?",// 1177
	"?",// 1178
	"?",// 1179
	"?",// 1180
	"?",// 1181
	"?",// 1182
	"?",// 1183
	"?",// 1184
	"?",// 1185
	"?",// 1186
	"?",// 1187
	"?",// 1188
	"?",// 1189
	"?",// 1190
	"?",// 1191
	"?",// 1192
	"?",// 1193
	"?",// 1194
	"?",// 1195
	"?",// 1196
	"?",// 1197
	"?",// 1198
	"?",// 1199
	"?",// 1200
	"?",// 1201
	"?",// 1202
	"?",// 1203
	"?",// 1204
	"?",// 1205
	"?",// 1206
	"?",// 1207
	"?",// 1208
	"?",// 1209
	"?",// 1210
	"?",// 1211
	"?",// 1212
	"?",// 1213
	"?",// 1214
	"?",// 1215
	"?",// 1216
	"?",// 1217
	"?",// 1218
	"?",// 1219
	"?",// 1220
	"?",// 1221
	"?",// 1222
	"?",// 1223
	"?",// 1224
	"?",// 1225
	"?",// 1226
	"?",// 1227
	"?",// 1228
	"?",// 1229
	"?",// 1230
	"?",// 1231
	"?",// 1232
	"?",// 1233
	"?",// 1234
	"?",// 1235
	"?",// 1236
	"?",// 1237
	"?",// 1238
	"?",// 1239
	"?",// 1240
	"?",// 1241
	"?",// 1242
	"?",// 1243
	"?",// 1244
	"?",// 1245
	"?",// 1246
	"?",// 1247
	"?",// 1248
	"?",// 1249
	"?",// 1250
	"?",// 1251
	"?",// 1252
	"?",// 1253
	"?",// 1254
	"?",// 1255
	"?",// 1256
	"?",// 1257
	"?",// 1258
	"?",// 1259
	"?",// 1260
	"?",// 1261
	"?",// 1262
	"?",// 1263
	"?",// 1264
	"?",// 1265
	"?",// 1266
	"?",// 1267
	"?",// 1268
	"?",// 1269
	"?",// 1270
	"?",// 1271
	"?",// 1272
	"?",// 1273
	"?",// 1274
	"?",// 1275
	"?",// 1276
	"?",// 1277
	"?",// 1278
	"?",// 1279
	"?",// 1280
	"?",// 1281
	"?",// 1282
	"?",// 1283
	"?",// 1284
	"?",// 1285
	"?",// 1286
	"?",// 1287
	"?",// 1288
	"?",// 1289
	"?",// 1290
	"?",// 1291
	"?",// 1292
	"?",// 1293
	"?",// 1294
	"?",// 1295
	"?",// 1296
	"?",// 1297
	"?",// 1298
	"?",// 1299
	"?",// 1300
	"?",// 1301
	"?",// 1302
	"?",// 1303
	"?",// 1304
	"?",// 1305
	"?",// 1306
	"?",// 1307
	"?",// 1308
	"?",// 1309
	"?",// 1310
	"?",// 1311
	"?",// 1312
	"?",// 1313
	"?",// 1314
	"?",// 1315
	"?",// 1316
	"?",// 1317
	"?",// 1318
	"?",// 1319
	"?",// 1320
	"?",// 1321
	"?",// 1322
	"?",// 1323
	"?",// 1324
	"?",// 1325
	"?",// 1326
	"?",// 1327
	"?",// 1328
	"?",// 1329
	"?",// 1330
	"?",// 1331
	"?",// 1332
	"?",// 1333
	"?",// 1334
	"?",// 1335
	"?",// 1336
	"?",// 1337
	"?",// 1338
	"?",// 1339
	"?",// 1340
	"?",// 1341
	"?",// 1342
	"?",// 1343
	"?",// 1344
	"?",// 1345
	"?",// 1346
	"?",// 1347
	"?",// 1348
	"?",// 1349
	"?",// 1350
	"?",// 1351
	"?",// 1352
	"?",// 1353
	"?",// 1354
	"?",// 1355
	"?",// 1356
	"?",// 1357
	"?",// 1358
	"?",// 1359
	"?",// 1360
	"?",// 1361
	"?",// 1362
	"?",// 1363
	"?",// 1364
	"?",// 1365
	"?",// 1366
	"?",// 1367
	"?",// 1368
	"?",// 1369
	"?",// 1370
	"?",// 1371
	"?",// 1372
	"?",// 1373
	"?",// 1374
	"?",// 1375
	"?",// 1376
	"?",// 1377
	"?",// 1378
	"?",// 1379
	"?",// 1380
	"?",// 1381
	"?",// 1382
	"?",// 1383
	"?",// 1384
	"?",// 1385
	"?",// 1386
	"?",// 1387
	"?",// 1388
	"?",// 1389
	"?",// 1390
	"?",// 1391
	"?",// 1392
	"?",// 1393
	"?",// 1394
	"?",// 1395
	"?",// 1396
	"?",// 1397
	"?",// 1398
	"?",// 1399
	"?",// 1400
	"?",// 1401
	"?",// 1402
	"?",// 1403
	"?",// 1404
	"?",// 1405
	"?",// 1406
	"?",// 1407
	"?",// 1408
	"?",// 1409
	"?",// 1410
	"?",// 1411
	"?",// 1412
	"?",// 1413
	"?",// 1414
	"?",// 1415
	"?",// 1416
	"?",// 1417
	"?",// 1418
	"?",// 1419
	"?",// 1420
	"?",// 1421
	"?",// 1422
	"?",// 1423
	"?",// 1424
	"?",// 1425
	"?",// 1426
	"?",// 1427
	"?",// 1428
	"?",// 1429
	"?",// 1430
	"?",// 1431
	"?",// 1432
	"?",// 1433
	"?",// 1434
	"?",// 1435
	"?",// 1436
	"?",// 1437
	"?",// 1438
	"?",// 1439
	"?",// 1440
	"?",// 1441
	"?",// 1442
	"?",// 1443
	"?",// 1444
	"?",// 1445
	"?",// 1446
	"?",// 1447
	"?",// 1448
	"?",// 1449
	"?",// 1450
	"?",// 1451
	"?",// 1452
	"?",// 1453
	"?",// 1454
	"?",// 1455
	"?",// 1456
	"?",// 1457
	"?",// 1458
	"?",// 1459
	"?",// 1460
	"?",// 1461
	"?",// 1462
	"?",// 1463
	"?",// 1464
	"?",// 1465
	"?",// 1466
	"?",// 1467
	"?",// 1468
	"?",// 1469
	"?",// 1470
	"?",// 1471
	"?",// 1472
	"?",// 1473
	"?",// 1474
	"?",// 1475
	"?",// 1476
	"?",// 1477
	"?",// 1478
	"?",// 1479
	"?",// 1480
	"?",// 1481
	"?",// 1482
	"?",// 1483
	"?",// 1484
	"?",// 1485
	"?",// 1486
	"?",// 1487
	"?",// 1488
	"?",// 1489
	"?",// 1490
	"?",// 1491
	"?",// 1492
	"?",// 1493
	"?",// 1494
	"?",// 1495
	"?",// 1496
	"?",// 1497
	"?",// 1498
	"?",// 1499
	"?",// 1500
	"?",// 1501
	"?",// 1502
	"?",// 1503
	"?",// 1504
	"?",// 1505
	"?",// 1506
	"?",// 1507
	"?",// 1508
	"?",// 1509
	"?",// 1510
	"?",// 1511
	"?",// 1512
	"?",// 1513
	"?",// 1514
	"?",// 1515
	"?",// 1516
	"?",// 1517
	"?",// 1518
	"?",// 1519
	"?",// 1520
	"?",// 1521
	"?",// 1522
	"?",// 1523
	"?",// 1524
	"?",// 1525
	"?",// 1526
	"?",// 1527
	"?",// 1528
	"?",// 1529
	"?",// 1530
	"?",// 1531
	"?",// 1532
	"?",// 1533
	"?",// 1534
	"?",// 1535
	"compilernop"                 ,// 1536
	"nop"                         ,// 1537
	"jcond"                       ,// 1538
	"jncond"                      ,// 1539
	"int3"                        ,// 1540
	"r"                           ,// 1541
	"syscall"                     ,// 1542
	"d"                           ,// 1543
	"enter"                       ,// 1544
	"const"                       ,// 1545
	"return"                      ,// 1546
	"leave"                       ,// 1547
	"prereturn"                   ,// 1548
	"ld"                          ,// 1549
	"lea"                         ,// 1550
	"indexfp"                     ,// 1551
	"pha"                         ,// 1552
	"plb"                         ,// 1553
	"add"                         ,// 1554
	"sub"                         ,// 1555
	"drop"                        ,// 1556
	"push"                        ,// 1557
	"call"                        ,// 1558
	"?"                           ,// 1559
	"?"                           ,// 1560
	"?"                           ,// 1561
	"?"                           ,// 1562
	"?"                           ,// 1563
	"?"                           ,// 1564
	"?"                           ,// 1565
	"?"                           ,// 1566
	"?"                           ,// 1567
	"?"                           ,// 1568
	"?"                           ,// 1569
	"?"                           ,// 1570
	"?"                           ,// 1571
	"?"                           ,// 1572
	"?"                           ,// 1573
	"?"                           ,// 1574
	"?"                           ,// 1575
	"?"                           ,// 1576
	"?"                           ,// 1577
	"?"                           ,// 1578
	"?"                           ,// 1579
	"?"                           ,// 1580
	"?"                           ,// 1581
	"?"                           ,// 1582
	"?"                           ,// 1583
	"?"                           ,// 1584
	"?"                           ,// 1585
	"?"                           ,// 1586
	"?"                           ,// 1587
};
struct{char* keyword;int tokentype;} KeywordtoTokentype[]={
	{"alignas"      ,tToken_Keywordalignas          },
	{"alignof"      ,tToken_Keywordalignof          },
	{"auto"         ,tToken_Keywordauto             },
	{"bool"         ,tToken_Keywordbool             },
	{"break"        ,tToken_Keywordbreak            },
	{"case"         ,tToken_Keywordcase             },
	{"char"         ,tToken_Keywordchar             },
	{"const"        ,tToken_Keywordconst            },
	{"constexpr"    ,tToken_Keywordconstexpr        },
	{"continue"     ,tToken_Keywordcontinue         },
	{"default"      ,tToken_Keyworddefault          },
	{"do"           ,tToken_Keyworddo               },
	{"double"       ,tToken_Keyworddouble           },
	{"else"         ,tToken_Keywordelse             },
	{"enum"         ,tToken_Keywordenum             },
	{"extern"       ,tToken_Keywordextern           },
	{"false"        ,tToken_Keywordfalse            },
	{"float"        ,tToken_Keywordfloat            },
	{"for"          ,tToken_Keywordfor              },
	{"goto"         ,tToken_Keywordgoto             },
	{"if"           ,tToken_Keywordif               },
	{"int8_t"       ,tToken_Keywordint8t            },
	{"int16_t"      ,tToken_Keywordint16t           },
	{"int32_t"      ,tToken_Keywordint32t           },
	{"int64_t"      ,tToken_Keywordint64t           },
	{"inline"       ,tToken_Keywordinline           },
	{"int"          ,tToken_Keywordint              },
	{"long"         ,tToken_Keywordlong             },
	{"nullptr"      ,tToken_Keywordnullptr          },
//	{"namespace"    ,tToken_Keywordnamespace        },
	{"register"     ,tToken_Keywordregister         },
	{"restrict"     ,tToken_Keywordrestrict         },
	{"return"       ,tToken_Keywordreturn           },
	{"short"        ,tToken_Keywordshort            },
	{"signed"       ,tToken_Keywordsigned           },
	{"sizeof"       ,tToken_Keywordsizeof           },
	{"size_t"       ,tToken_Keywordsizet            },
	{"static"       ,tToken_Keywordstatic           },
	{"static_assert",tToken_Keywordstatic_assert    },
	{"struct"       ,tToken_Keywordstruct           },
	{"switch"       ,tToken_Keywordswitch           },
	{"thread_local" ,tToken_Keywordthread_local     },
	{"true"         ,tToken_Keywordtrue             },
	{"typedef"      ,tToken_Keywordtypedef          },
	{"typeof"       ,tToken_Keywordtypeof           },
	{"typeof_unqual",tToken_Keywordtypeof_unqual    },
	{"uint8_t"      ,tToken_Keyworduint8t           },
	{"uint16_t"     ,tToken_Keyworduint16t          },
	{"uint32_t"     ,tToken_Keyworduint32t          },
	{"uint64_t"     ,tToken_Keyworduint64t          },
	{"union"        ,tToken_Keywordunion            },
	{"unsigned"     ,tToken_Keywordunsigned         },
	{"void"         ,tToken_Keywordvoid             },
	{"volatile"     ,tToken_Keywordvolatile         },
	{"while"        ,tToken_Keywordwhile            },
	{""             ,0                              },
	{(char*)0       ,0                              },
};
// Target types
typedef uint16_t tGTargetNearpointer;
typedef uint16_t tGTargetSizet;
typedef uint8_t  tGTargetSegment;
typedef int      tGTargetUintmax;
//const tGTargetSizet GTargetStackframeArgumentsstart = 4;

// XCC types
typedef struct tToken {
	short type;
	unsigned linenumber;
	tGTargetUintmax constant;
	// Multistage tokenizer compatibility...
	// Going to rewrite it later
	short reserved1;
	short reserved2;
	short reserved3;
#ifndef qvmtToken_Usestringvla
	char *string;
#else
	char string[];
#endif
} tToken, *ptToken;
typedef enum eGSegment {
	//none code data udata rodata stack tls
	meGSegment_Relative,	//none
	meGSegment_Code,	//code  == cs
	meGSegment_Readonlydata,//rodata== ds
	meGSegment_Data,	//idata == ds
	meGSegment_Udata,	//udata == ds
	meGSegment_Tls,		//tls   == es/fs
	meGSegment_Stack,	//stack == ss
	meGSegment_Stackframe,	//frame == ss:fp(/ss:sp?)
			       // When referenced casted stackframe 
	                       // (fp-relative) -> stack (ss-relative)
	meGSegment_Far,		// program-defined bank/segment
	meGSegment_Count
} eGSegment;
char* meGSegment_ToStringTable[]= {
	"relative",
	"code",
	"rodata",
	"data",
	"udata",
	"tls",
	"stack",
	"frame",
	"far",
	0
};
typedef enum eGAtomictype {
	eGAtomictype_Void             = 0,
	eGAtomictype_Structure        = 1,
	eGAtomictype_Enumeration      = 2,
	eGAtomictype_Unresolved       = 3,
	// Internal types
	eGAtomictype_Union            = 4, // Temporary type - gets converted to struct later on
	eGAtomictype_Pointer          = 5,
	eGAtomictype_Array            = 6,
	eGAtomictype_Function         = 7,
	eGAtomictype_Nearpointer      = 8,
	eGAtomictype_Neararray        = 9,
	eGAtomictype_Nearfunction     = 10,
	eGAtomictype_Farpointer       = 11,
	eGAtomictype_Fararray         = 12,
	eGAtomictype_Farfunction      = 13,
	// IR-side types (and optionally C-side)
	eGAtomictype_Int8             = 14,   
	eGAtomictype_Uint8            = 15,
	eGAtomictype_Int16            = 16,
	eGAtomictype_Uint16           = 17,
	//eGAtomictype_Int24          = 18,
	//eGAtomictype_Uint24         = 19,
	eGAtomictype_Int32            = 20,
	eGAtomictype_Uint32           = 21,
	//eGAtomictype_Int48          = 22,
	//eGAtomictype_Uint48         = 23,
	eGAtomictype_Int64            = 24,
	eGAtomictype_Uint64           = 25,
	eGAtomictype_Int80            = 26,
	eGAtomictype_Uint80           = 27,
	eGAtomictype_Float32          = 28,
	eGAtomictype_Float64          = 29,
	eGAtomictype_Float80          = 30,
	// C-side types
	eGAtomictype_Char             = 31,  
	eGAtomictype_Signedchar       = 32,
	eGAtomictype_Unsignedchar     = 33,
	eGAtomictype_Short            = 34,
	eGAtomictype_Unsignedshort    = 35,
	eGAtomictype_Int              = 36,
	eGAtomictype_Unsigned         = 37,
	eGAtomictype_Long             = 38,
	eGAtomictype_Unsignedlong     = 39,
	eGAtomictype_Longlong         = 40,
	eGAtomictype_Unsignedlonglong = 41,
	eGAtomictype_Boolean          = 42,
	eGAtomictype_Float            = 43,
	eGAtomictype_Double           = 44,
	eGAtomictype_Longdouble       = 45,
	eGAtomictype_Intptr           = 46,
	eGAtomictype_Intnearptr       = 47,
	eGAtomictype_Intfarptr        = 48,
	// Some specific types
	eGAtomictype_Sizet            = 49,
} eGAtomictype;
typedef struct {eGAtomictype atomictype; char* str;} GAtomictypetostring_Entry;
GAtomictypetostring_Entry GAtomictypetostring[] = {
	{ eGAtomictype_Structure                 , "struct" },
	{ eGAtomictype_Union                     , "union" },
	{ eGAtomictype_Enumeration               , "enum" },
	{ eGAtomictype_Unresolved                , "unresolved" },
	{ eGAtomictype_Nearpointer               , "nearpointer" },
	{ eGAtomictype_Neararray                 , "neararray" },
	{ eGAtomictype_Nearfunction              , "nearfunction" },
	{ eGAtomictype_Farpointer                , "farpointer" },
	{ eGAtomictype_Fararray                  , "fararray" },
	{ eGAtomictype_Farfunction               , "farfunction" },
	{ eGAtomictype_Pointer                   , "pointer" },
	{ eGAtomictype_Array                     , "array" },
	{ eGAtomictype_Function                  , "function" },
	{ eGAtomictype_Int8                      ,  "int8_t" },
	{ eGAtomictype_Uint8                     , "uint8_t" },
	{ eGAtomictype_Int16                     ,  "int16_t" },
	{ eGAtomictype_Uint16                    , "uint16_t" },
	{ eGAtomictype_Int32                     ,  "int32_t" },
	{ eGAtomictype_Uint32                    , "uint32_t" },
	{ eGAtomictype_Int64                     ,  "int64_t" },
	{ eGAtomictype_Uint64                    , "uint64_t" },
	{ eGAtomictype_Int80                     ,  "int80_t" },
	{ eGAtomictype_Uint80                    , "uint80_t" },
	{ eGAtomictype_Float32                   , "float32_t" },
	{ eGAtomictype_Float64                   , "float64_t" },
	{ eGAtomictype_Float80                   , "float80_t" },
	// C-side types
	{ eGAtomictype_Char                      , "char" },
	{ eGAtomictype_Signedchar                , "signed char" },
	{ eGAtomictype_Unsignedchar              , "unsigned char" },
	{ eGAtomictype_Short                     , "short" },
	{ eGAtomictype_Unsignedshort             , "unsigned short" },
	{ eGAtomictype_Int                       , "int" },
	{ eGAtomictype_Unsigned                  , "unsigned" },
	{ eGAtomictype_Long                      , "long" },
	{ eGAtomictype_Unsignedlong              , "unsigned long" },
	{ eGAtomictype_Longlong                  , "long long" },
	{ eGAtomictype_Unsignedlonglong          , "unsigned long long" },
	{ eGAtomictype_Boolean                   , "bool" },
	{ eGAtomictype_Float                     , "float" },
	{ eGAtomictype_Double                    , "double" },
	{ eGAtomictype_Longdouble                , "long double" },
	{ eGAtomictype_Void                      , "void" },
	{ 0, 0 }
};
char* meGAtomictype_ToStringTable[] = {
	"void"            ,
	"structure"       ,
	"enumeration"     ,
	"unresolved"      ,
	"union"           ,
	"pointer"         ,
	"array"           ,
	"function"        ,
	"nearpointer"     ,
	"neararray"       ,
	"nearfunction"    ,
	"farpointer"      ,
	"fararray"        ,
	"farfunction"     ,
	"int8"            ,
	"uint8"           ,
	"int16"           ,
	"uint16"          ,
	"int24"           ,
	"uint24"          ,
	"int32"           ,
	"uint32"          ,
	"int48"           ,
	"uint48"          ,
	"int64"           ,
	"uint64"          ,
	"int80"           ,
	"uint80"          ,
	"float32"         ,
	"float64"         ,
	"float80"         ,
	"char"            ,
	"signedchar"      ,
	"unsignedchar"    ,
	"short"           ,
	"unsignedshort"   ,
	"int"             ,
	"unsigned"        ,
	"long"            ,
	"unsignedlong"    ,
	"longlong"        ,
	"unsignedlonglong",
	"boolean"         ,
	"float"           ,
	"double"          ,
	"longdouble"      ,
	"intptr"          ,
	"intnearptr"      ,
	"intfarptr"       ,
	"sizet"           ,
};
typedef struct tGOpcode { 
	short opr;
	enum eGAtomictype isize;
	enum eGSegment segment;
} tGOpcode;
typedef struct tGInstruction {
	tGOpcode opcode; 
	struct tGInstruction* next;
	struct tGInstruction* jumptarget;
	tGTargetUintmax immediate;
} tGInstruction;
typedef struct { // tGTargetPointer
	bool nonconstant; // TODO: Refactor to use 'dynamic' segment instead of nonconstant specifier
	// Constant pointer - segment-offset:
	eGSegment segment; 
	tGTargetSegment bank;
	tGTargetNearpointer offset;
	// Dynamic pointer - tGInstruction*:
	tGInstruction* dynamicpointer;
} tGTargetPointer;
/* // Some details about types written forever ago
 *	struct tGType {
 *		enum eGAtomictype atomicbasetype;
 *		struct tGType * complexbasetype;
 *		enum eGValuecategory valuecategory; //lvalue/rvalue
 *		tGPointernessmodifierslist pointerness;
 *		union {
 *			tGTargetPointer allocatedstorage; // for lvalues
 *			// local storage
 *		}
 *	};
 *	
 *	struct tGPointernessmodifierslist {
 *		enum {
 *			.null = 0,
 *			.pointer,
 *			.pointer_localstorage,
 *			.function,
 *		} item;
 *		self* next;
 *		tSpFunctionargumentslist* functionargs;
 *	};
 *	
 *	struct tSpFunctionargumentslist {
 *		tSpType* item;
 *		self* next;
 *	};
 */
typedef enum eGValuecategory {
	eGValuecategory_Null = 0,
	eGValuecategory_Novalue = 0,
	eGValuecategory_Leftvalue = 1,
	eGValuecategory_Rightvalue = 2
} eGValuecategory;
typedef enum eGPointernessmodifier {
	eGPointernessmodifier_Null = 0,
	eGPointernessmodifier_Pointer,
	eGPointernessmodifier_PointerLocalstorage,
	eGPointernessmodifier_Function,
} eGPointernessmodifier;
typedef struct tGPointernessmodifierslist {
	eGPointernessmodifier modifier;
	bool restrictmodifier;
	bool far; // Far pointer - segment:offset
	struct tGPointernessmodifierslist * next;
	tListnode /* <tGType> */ * functionargs;
} tGPointernessmodifierslist;
typedef enum eGTypequalifiers {
	eGTypequalifiers_Null = 0,
	eGTypequalifiers_Const = 1,
	eGTypequalifiers_Volatile = 2,
	eGTypequalifiers_Restrict = 4,
} eGTypequalifiers;
typedef struct tGType {
	char* unresolvedsymbol;
	eGAtomictype atomicbasetype;
	struct tGType * complexbasetype;
	struct tGNamespace * structure;
	tList /* LxNode<Declaration> */ * precompiledstructure;
	void /* tLxNode */ * precompiledenumeration;
	eGValuecategory valuecategory;
	eGTypequalifiers typequalifiers;
	tListnode /* <tGType> */ * functionarguments;
	tListnode /* <tGType> */ * templatemodifiers;
	tGTargetSizet structsize;
} tGType;
typedef struct tGNamespace {
	struct tGNamespace * parentnamespace;
	tList /* <tGSymbol> */ symbols;
	char* name;
} tGNamespace;
enum mtGSymbol_eType {
	mtGSymbol_eType_Null = 0,
	mtGSymbol_eType_Constant, // consts
	mtGSymbol_eType_Namespace,
	mtGSymbol_eType_Pointer, // Functions and uninit. globals
	mtGSymbol_eType_Deferredevaulation, // Initialized globals
	mtGSymbol_eType_Typedef // Typedefs
};
typedef struct tGSymbol {
	char* name;
	enum mtGSymbol_eType symbolkind;
	tGType* type;
	//union {
		tGTargetUintmax value;
		tGNamespace* name_space;
		tGTargetPointer* allocatedstorage;
		struct tLxNode* deferredexpression;
	//};
} tGSymbol;
typedef enum eGStoragespecifier {
	eGStoragespecifier_None = 0,
	eGStoragespecifier_Auto,
	eGStoragespecifier_Static,
	eGStoragespecifier_Extern,
	eGStoragespecifier_Register,
	eGStoragespecifier_Threadlocal, // That's a compiler for 8 and 16 bit CPUs
	                                // on which multithreading makes next to
									// no sense, but interrups didn't vanish
									// all of a sudden
} eGStoragespecifier;
typedef struct tLxNode {
	eTokentype type;
	struct tLxNode * initializer;
	struct tLxNode * condition;
	struct tLxNode * left;
	struct tLxNode * right;
	char* identifier;
	tGInstruction * codepointer; // for storage pointers, like variables
	tGTargetUintmax constant;
	struct tLxNode * parentnode;
	//tGType* basetype; //for declarations
	tGNamespace* name_space; // namespace is a keyword
	tGType* returnedtype;
	enum eGStoragespecifier storage;
} tLxNode;

typedef struct tSppNode {
	eTokentype type;
	struct tSppNode* initializer;
	struct tSppNode* condition;
	struct tSppNode* left;
	struct tSppNode* right;

	struct tSppNode* parent;
	tGTargetUintmax constant;
	tGNamespace* name_space;
	tGNamespace* symbol;
	tGType* returnedtype;
	char* identifier;
} tSppNode;

typedef struct tSpFunctionextinfo {
	tGTargetSizet localssize;
	tGTargetSizet argumentssize;
} tSpFunctionextinfo;

typedef struct tSpNode {
	eTokentype type;
	tGType* returnedtype;
	struct tSpNode * initializer;
	struct tSpNode * condition;
	struct tSpNode * left;
	struct tSpNode * right;
	tGTargetUintmax constant;
	tGSymbol* symbol;
	tSpFunctionextinfo* fextinfo;
} tSpNode;

//
//		C Operators
//	A modified C++ chart.
//	P r e c e d e n c e   i s   n o n - s t a n d a r d . 
//	Specifically you can't use bitwise and/or as logical ones.
//	 This is not K&R C, please use logical operators.
//
//				Associativity
//  Precedence 	opr.	Description 	
//
// 				Left-to-right →
// 	0 	:: 	Scope resolution 	
// 	1 	++ -- 	Suffix/postfix increment and decrement 	
// 		() 	Function call
// 		[] 	Array subscripting
// 		. 	Structure and union member access
// 		-> 	Structure and union member access through pointer
// 		(){} 	Compound literal from our beloved C99
// 				Right-to-left ←
// 	2 	++ --	Prefix increment and decrement
// 		+ - 	Unary plus and minus
// 		! ~	Logical NOT and bitwise NOT
// 		() 	Type cast
// 		* 	Indirection (dereference)
// 		& 	Address-of
// 		sizeof 	Size-of[note 1]
// 			There should've been `alignof`, but there's not much
// 			 reason for alignment on 8-bit machines, so...
// 				Left-to-right →
// 	2.5 	.* ->*	Pointer-to-member 	
// 	3 	* / % 	Multiplication, division, and remainder
// 	4 	+ - 	Addition and subtraction
// 	5 	<< >> 	Bitwise left shift and right shift
// 	6  	&	Bitwise AND
// 	7  	^ 	Bitwise XOR (exclusive or)
// 	8  	| 	Bitwise OR (inclusive or)
// 	8.5 	<=>	Three-way comparison operator (why though)
// 	9	< <=    Comparison
// 		> >=
// 	10 	== !=	More comparison
// 	11 	&& 	Logical AND
// 	12 	|| 	Logical OR
// 				Right-to-left ←
// 	13 	?:	Ternary conditional
// 	14	= 	Simple assignment
// 		+= -= 	Assignment by sum and difference
// 		*= /=  	Assignment by product, quotient, and remainder
// 		%=
// 		<<=  	Assignment by bitwise left shift and right shift
// 		>>=
// 		&= ^=  	Assignment by bitwise AND, XOR, and OR
// 		|=		
//				Left-to-right → 
// 	15 	, 	Comma 	
//

/* // Grammar declarations

atomic {
	eof	// as end of file
	nullptr
}
program
	: declaration program
	| eof
	;
declaration
	: basetype expression:typeexpr semicolon
	| basetype expression:typeexpr assign expression:initializer semicolon
	| basetype expression(lastnode==operator::function) blockstatement
	| semicolon // empty declaration
	;
basetype
	: int
	| unsigned
	| short
	| unsigned short
	| char
	| signed char
	| unsigned char
	| long
	| unsigned long
	
	| struct structdefinition
	| enum enumdefinition
	| union uniondefinition
	| identifier:typedefnamespace
	;
expression
	: constant
	| identifier:variablenamespace
	| openparentheses expression closeparentheses
	| prefixop expression
	| expression postfixop
	| expression operator expression
	;
*/
