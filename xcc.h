//	Header file for XCC Retargetable C Compiler
enum tTokentype {			
	tToken_Null			= 0 ,		//	Stringless tokens
	tToken_Memberbypointer		= 1 ,		//->
	tToken_Namespace		= 2 ,		//::
	tToken_Increment		= 3 ,		//++
	tToken_Decrement		= 4 ,		//--
	tToken_Openparentheses		= 5 ,		//(
	tToken_Closeparentheses		= 6 ,		//)
	tToken_Openbrackets		= 7 ,		//[
	tToken_Closebrackets		= 8 ,		//]
	tToken_Opencurlybraces		= 9 ,		//{
	tToken_Closecurlybraces		= 10,		//}
	tToken_Dot			= 11,		//.
	tToken_Logicalnot		= 12,		//!
	tToken_Bitwisenot		= 13,		//~
	tToken_Bitwiseand		= 14,		//&
	tToken_Plus			= 15,		//+
	tToken_Minus			= 16,		//-
	tToken_Multiply			= 17,		//*
	tToken_Divide			= 18,		///
	tToken_Modulo			= 19,		//%
	tToken_Shiftleft		= 20,		//<<
	tToken_Shiftright		= 21,		//>>
	tToken_Lessthan			= 22,		//<
	tToken_Greaterthan		= 23,		//>
	tToken_Lessequal		= 24,		//<=
	tToken_Greaterequal		= 25,		//>=
	tToken_Threewaycomparison	= 26,		//<=>
	tToken_Assign			= 27,		//=
	tToken_Equals			= 28,		//==
	tToken_Notequal			= 29,		//!=
	tToken_Bitwisexor		= 30,		//^
	tToken_Bitwiseor		= 31,		//|
	tToken_Logicaland		= 32,		//&&
	tToken_Logicalor		= 33,		//||
	tToken_Expressionif		= 34,		//?
	tToken_Colon			= 35,		//:
	tToken_Addassign		= 36,		//+=
	tToken_Substractassign		= 37,		//-=
	tToken_Multiplyassign		= 38,		//*=
	tToken_Divideassign		= 39,		///=
	tToken_Moduloassign		= 40,		//%=
	tToken_Shiftleftassign		= 41,		//<<=
	tToken_Shiftrightassign		= 42,		//>>=
	tToken_Bitwiseandassign		= 43,		//&=
	tToken_Bitwisexorassign		= 44,		//^=
	tToken_Bitwiseorassign		= 45,		//|=
	tToken_Comma			= 46,		//,
	tToken_Semicolon		= 47,		//;
	tToken_Integerconstant		= 96,		//
	tToken_Whitespace		= 127,		//
	//tToken_			= 47,		//	Keywords
	tToken_Keywordalignas		= 128,		//alignas
	tToken_Keywordalignof		= 129,		//alignof
	tToken_Keywordauto		= 130,		//auto
	tToken_Keywordbool		= 131,		//bool
	tToken_Keywordbreak		= 132,		//break
	tToken_Keywordcase		= 133,		//case
	tToken_Keywordchar		= 134,		//char
	tToken_Keywordconst		= 135,		//const
	tToken_Keywordconstexpr		= 136,		//constexpr
	tToken_Keywordcontinue		= 137,		//continue
	tToken_Keyworddefault		= 138,		//default
	tToken_Keyworddo		= 139,		//do
	tToken_Keyworddouble		= 140,		//double
	tToken_Keywordelse		= 141,		//else
	tToken_Keywordenum		= 142,		//enum
	tToken_Keywordextern		= 143,		//extern
	tToken_Keywordfalse		= 144,		//false
	tToken_Keywordfloat		= 145,		//float
	tToken_Keywordfor		= 146,		//for
	tToken_Keywordgoto		= 147,		//goto
	tToken_Keywordif		= 148,		//if
	tToken_Keywordinline		= 149,		//inline
	tToken_Keywordint		= 150,		//int
	tToken_Keywordlong		= 151,		//long
	tToken_Keywordnullptr		= 152,		//nullptr
	tToken_Keywordregister		= 153,		//register
	tToken_Keywordrestrict		= 154,		//restrict
	tToken_Keywordreturn		= 155,		//return
	tToken_Keywordshort		= 156,		//short
	tToken_Keywordsigned		= 157,		//signed
	tToken_Keywordsizeof		= 158,		//sizeof
	tToken_Keywordstatic		= 159,		//static
	tToken_Keywordstatic_assert	= 160,		//static_assert
	tToken_Keywordstruct		= 161,		//struct
	tToken_Keywordswitch		= 162,		//switch
	tToken_Keywordthread_local	= 163,		//thread_local
	tToken_Keywordtrue		= 164,		//true
	tToken_Keywordtypedef		= 165,		//typedef
	tToken_Keywordtypeof		= 166,		//typeof
	tToken_Keywordtypeof_unqual	= 167,		//typeof_unqual
	tToken_Keywordunion		= 168,		//union
	tToken_Keywordunsigned		= 169,		//unsigned
	tToken_Keywordvoid		= 170,		//void
	tToken_Keywordvolatile		= 171,		//volatile
	tToken_Keywordwhile		= 172,		//while
	//tToken_			= 256,		//	String tokens
	tToken_Identifier		= 256,		//Generic identifier
	tToken_String			= 257,		//String
	tToken_Charaterconstant		= 258,		//Charater constant
	//tToken_			= 512,		//	Lexems
	tLexem_Declarationlist		= 512,		//List of declarations - root of program
	tLexem_Assign			= 513,		//Assignment statement
	tLexem_Comma			= 514,		//Comma 'operator' - rather specialpurpose
	tLexem_Expressionif		= 515,		//
	tLexem_Logicalor		= 516,		//
	tLexem_Logicaland		= 517,		//
	tLexem_Equals			= 518,		//
	tLexem_Notequal			= 519,		//
	tLexem_Greaterequal		= 520,		//
	tLexem_Lessequal		= 521,		//
	tLexem_Greaterthan		= 522,		//
	tLexem_Lessthan			= 523,		//
	tLexem_Bitwiseor		= 524,		//
	tLexem_Bitwisexor		= 525,		//
	tLexem_Bitwiseand		= 526,		//
	tLexem_Shiftleft		= 527,		//
	tLexem_Shiftright		= 528,		//
	tLexem_Add			= 529,		//
	tLexem_Substract		= 530,		//
	tLexem_Typecast			= 531,		//
	tLexem_Variabledeclaration	= 532,		//
	tLexem_Functiondeclaration	= 533,		//
	tLexem_Blockstatement		= 534,
	tLexem_Increment		= 535,
	tLexem_Decrement		= 536,
	tLexem_Unaryplus		= 537,
	tLexem_Negation			= 538,
	tLexem_Logicalnot		= 539,
	tLexem_Bitwisenot		= 540,
	tLexem_Dereference		= 541,
	tLexem_Addressof		= 542,
	tLexem_Sizeof			= 543,
	tLexem_Typeexpression		= 544,
	tLexem_Expressionstatement	= 545,
	tLexem_Integerconstant		= 546,
	tLexem_Functioncall		= 547,
	tLexem_Structuremember		= 548,		//`.`
	tLexem_Memberbypointer		= 549,		//`->`
	tLexem_Compoundliteral		= 550,
	tLexem_Arrayindex		= 551,
	tLexem_Postincrement		= 552,
	tLexem_Postdecrement		= 553,
	tLexem_Identifier		= 554,		//identifier
	tLexem_Typedefinition	        = 555,		//typedef basetype typeexpr;
	tLexem_Structdefinition	        = 556,		//struct type
	tLexem_Returnstatement	        = 557,		//return expr;
	tLexem_Ifstatement	        = 558,
	tLexem_Nullpointer	        = 559,
	tLexem_Booleantrue	        = 560,
	tLexem_Booleanfalse	        = 561,
	tLexem_Whilestatement	        = 562,
	tLexem_Forstatement	        = 563,
	tLexem_Nulldeclaration	        = 564,
	tLexem_Nullexpression	        = 565,
	tLexem_Multiply		        = 566,
	tLexem_Divide	        	= 567,
	tLexem_Modulo		        = 568,
	tLexem_Switchstatement	        = 569,
	tLexem_Switchcase	        = 570,
	tLexem_Switchdefault	        = 571,
	tLexem_Goto		        = 572,
	tLexem_Breakstatement		= 573,
	tLexem_Constmodifier	        = 574,
	tLexem_Volatilemodifier	        = 575,
	tLexem_Restrictmodifier	        = 576,
	//tLexem_	        = 577,
	//tToken_	        	= 768,		//	String lexems
	//tToken_	        	= 1024,		//	Second AST lexems
	//tToken_	        	= 1280,		//	Second AST string lexems
	//tToken_	        	= 1536,		//	IR instructions
	tIrinstruction_Nop		= 1536+1,	//No operation
	tIrinstruction_Jumptrue		= 1536+2,	//Jump if true
	tIrinstruction_Jumpfalse	= 1536+3,	//Jump if false
	tIrinstruction_Debugbreak	= 1536+4,	//Break into debugger
	tIrinstruction_Allocatestorage	= 1536+5,	//To be used like `variable uint32_t pointer_localstorage testarray: times 5 allocatestorage.uint32` (looks awesome though, might wanna make smth like that in flatassembler)
	tIrinstruction_Systemcall	= 1536+6,	//System call. Still have to figure out how to call into kernel if code is compiled with calculation stack but syscall excepts registers. Might be just RTL call.
	// v_ld_ind.T segment
	// v_st_ind.T segment
	//tToken_			= 1792,		//	To be not used
	//tToken_			= 2048,		//	Machine instructions?
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
	"tLexem_Nulldeclaration	     ",// 564
	"tLexem_Nullexpression	     ",// 565
	"tLexem_Multiply             ",// 566
	"tLexem_Divide	             ",// 567
	"tLexem_Modulo               ",// 568
	"tLexem_Switchstatement      ",// 569
	"tLexem_Switchcase           ",// 570
	"tLexem_Switchdefault        ",// 571
	"tLexem_Goto                 ",// 572
	"tLexem_Undefined            ",// 573
	"tLexem_Undefined            ",// 574
	"tLexem_Undefined            ",// 575
	"tLexem_Undefined            ",// 576
	"tLexem_Undefined            ",// 577
	"tLexem_Undefined            ",// 578
	"tLexem_Undefined            ",// 579
	"tLexem_Undefined            ",// 580
	"tLexem_Undefined            ",// 581
	"tLexem_Undefined            ",// 582
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
	//"//tToken_                      ",// 384
};
struct{char* keyword;int tokentype;} KeywordtoTokentype[]={
	{"alignas"		,tToken_Keywordalignas		},
	{"alignof"		,tToken_Keywordalignof		},
	{"auto"			,tToken_Keywordauto		},
	{"bool"			,tToken_Keywordbool		},
	{"break"		,tToken_Keywordbreak		},
	{"case"			,tToken_Keywordcase		},
	{"char"			,tToken_Keywordchar		},
	{"const"		,tToken_Keywordconst		},
	{"constexpr"		,tToken_Keywordconstexpr	},
	{"continue"		,tToken_Keywordcontinue		},
	{"default"		,tToken_Keyworddefault		},
	{"do"			,tToken_Keyworddo		},
	{"double"		,tToken_Keyworddouble		},
	{"else"			,tToken_Keywordelse		},
	{"enum"			,tToken_Keywordenum		},
	{"extern"		,tToken_Keywordextern		},
	{"false"		,tToken_Keywordfalse		},
	{"float"		,tToken_Keywordfloat		},
	{"for"			,tToken_Keywordfor		},
	{"goto"			,tToken_Keywordgoto		},
	{"if"			,tToken_Keywordif		},
	{"inline"		,tToken_Keywordinline		},
	{"int"			,tToken_Keywordint		},
	{"long"			,tToken_Keywordlong		},
	{"nullptr"		,tToken_Keywordnullptr		},
	{"register"		,tToken_Keywordregister		},
	{"restrict"		,tToken_Keywordrestrict		},
	{"return"		,tToken_Keywordreturn		},
	{"short"		,tToken_Keywordshort		},
	{"signed"		,tToken_Keywordsigned		},
	{"sizeof"		,tToken_Keywordsizeof		},
	{"static"		,tToken_Keywordstatic		},
	{"static_assert"	,tToken_Keywordstatic_assert	},
	{"struct"		,tToken_Keywordstruct		},
	{"switch"		,tToken_Keywordswitch		},
	{"thread_local"		,tToken_Keywordthread_local	},
	{"true"			,tToken_Keywordtrue		},
	{"typedef"		,tToken_Keywordtypedef		},
	{"typeof"		,tToken_Keywordtypeof		},
	{"typeof_unqual"	,tToken_Keywordtypeof_unqual	},
	{"union"		,tToken_Keywordunion		},
	{"unsigned"		,tToken_Keywordunsigned		},
	{"void"			,tToken_Keywordvoid		},
	{"volatile"		,tToken_Keywordvolatile		},
	{"while"		,tToken_Keywordwhile		},
	{""			,0				},
	{(char*)0		,0				},
};
// Target types
typedef uint16_t tGTargetNearpointer;
typedef uint8_t  tGTargetSegment;
typedef uint32_t tGTargetUintmax;
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
	meGSegment_Count
} eGSegment;
typedef enum eGAtomictype {
	eGAtomictype_Void = 0,
	eGAtomictype_Structure,
	eGAtomictype_Enumeration,
	eGAtomictype_Unresolved,
	// Internal types
	eGAtomictype_Pointer,
	eGAtomictype_Array,
	eGAtomictype_Function,
	// IR-side types (and optionally C-side)
	eGAtomictype_Int8,
	eGAtomictype_Uint8,
	eGAtomictype_Int16,
	eGAtomictype_Uint16,
	//eGAtomictype_Int24,
	//eGAtomictype_Uint24,
	eGAtomictype_Int32,
	eGAtomictype_Uint32,
	//eGAtomictype_Int48,
	//eGAtomictype_Uint48,
	eGAtomictype_Int64,
	eGAtomictype_Uint64,
	eGAtomictype_Int80,
	eGAtomictype_Uint80,
	eGAtomictype_Float32,
	eGAtomictype_Float64,
	eGAtomictype_Float80,
	// C-side types
	eGAtomictype_Char,
	eGAtomictype_Signedchar,
	eGAtomictype_Unsignedchar,
	eGAtomictype_Short,
	eGAtomictype_Unsignedshort,
	eGAtomictype_Int,
	eGAtomictype_Unsigned,
	eGAtomictype_Long,
	eGAtomictype_Unsignedlong,
	eGAtomictype_Longlong,
	eGAtomictype_Unsignedlonglong,
	eGAtomictype_Boolean,
	eGAtomictype_Float,
	eGAtomictype_Double,
	eGAtomictype_Longdouble
} eGAtomictype;
typedef struct {eGAtomictype atomictype; char* str;} GAtomictypetostring_Entry;
GAtomictypetostring_Entry GAtomictypetostring[] = {
	{ eGAtomictype_Structure                 , "struct" },
	{ eGAtomictype_Enumeration               , "enum" },
	{ eGAtomictype_Unresolved                , "unknown" },
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
	{ eGAtomictype_Float                     , "float" },
	{ eGAtomictype_Double                    , "double" },
	{ eGAtomictype_Longdouble                , "long double" },
	{ 0, 0 }
};
typedef struct tGIrOpcode { 
	short opcode;
	enum eGAtomictype instructionsize;
} tGIrOpcode;
typedef struct tGIrInstruction {
	tGIrOpcode opcode; 
	struct tGIrInstruction* next;
	struct tGIrInstruction* jumptarget;
	tGTargetUintmax immediate;
} tGIrInstruction;
typedef struct { // tGTargetPointer
	bool nonconstant;
	// Constant pointer - segment-offset:
	eGSegment segment;
	tGTargetSegment bank;
	tGTargetNearpointer offset;
	// Dynamic pointer - tGInstructon*:
	tGIrInstruction* dynamicpointer;
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
	eGValuecategory_Leftvalue = 1,
	eGValuecategory_Rightvalue = 2
} eGValuecategory;
typedef enum eGPointernessmodifier {
	eGPointernessmodifier_Null = 0,
	eGPointernessmodifier_Pointer,
	eGPointernessmodifier_PointerLocalstorage,
	eGPointernessmodifier_Function,
} eGPointernessmodifier;
typedef struct tGFunctionargumentslist {
#ifndef qvGProhibitForwardreferences // Forwardreference to tGType
	struct tGType
#else
	void
#endif
	/* struct tGType */ *argumenttype;
	struct tGFunctionargumentslist * next;
} tGFunctionargumentslist;
typedef struct tGPointernessmodifierslist {
	eGPointernessmodifier modifier;
	bool restrictmodifier;
	bool far; // Far pointer - segment:offset
	struct tGPointernessmodifierslist * next;
	tGFunctionargumentslist* functionargs;
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
	void /* struct tGNamespace */ * structure;
	tList /* LxNode<Declaration> */ * precompiledstructure;
	void /* tLxNode */ * precompiledenumeration;
	eGValuecategory valuecategory;
	eGTypequalifiers typequalifiers;
	tListnode /* <tGType> */ * templatemodifiers;
} tGType;
typedef struct tGNamespace {
	struct tGNamespace * parentnamespace;
	tList /* <tGSymbol> */ symbols;
	char* name;
} tGNamespace;
typedef struct tGSymbol {
	char* name;
	bool isconstant;
	bool isnamespace;
	bool ispointer;
	tGTargetUintmax value;
	tGNamespace* name_space;
	tGTargetPointer* allocatedstorage;
} tGSymbol;

typedef struct tLxNode {
	eTokentype type;
	struct tLxNode * initializer;
	struct tLxNode * condition;
	struct tLxNode * left;
	struct tLxNode * right;
	char* identifier;
	tGIrInstruction * codepointer; // for storage pointers, like variables
	tGTargetUintmax constant;
	struct tLxNode * parentnode;
	//tGType* basetype; //for declarations
	tGNamespace* name_space; // namespace is a keyword
	tGType* returnedtype;
} tLxNode, tSpNode;

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
	:
	| int
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
	:
	| constant
	| identifier:variablenamespace
	| openparentheses expression closeparentheses
	| prefixop expression
	| expression postfixop
	| expression operator expression
	;
*/
