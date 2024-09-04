// -------------- Header file for XCC Retargetable C Compiler --------------
#include "xcc-bigtables.h"
// Target types
typedef uint16_t tGTargetNearpointer;
typedef uint16_t tGTargetSizet;
typedef uint8_t  tGTargetSegment;
typedef int      tGTargetUintmax;
//const tGTargetSizet GTargetStackframeArgumentsstart = 4;

// xcc types
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
typedef enum eGSegment { // Acts as addressing mode as well
	//none code data udata rodata stack tls
	meGSegment_Relative,        //none
	meGSegment_Code,            //code  == cs
	meGSegment_Readonlydata,    //rodata== ds
	meGSegment_Data,            //idata == ds
	meGSegment_Udata,           //udata == ds
	meGSegment_Tls,             //tls   == es/fs
	meGSegment_Stack,           //stack == ss
	meGSegment_Stackframe,      //frame == ss:fp(/ss:sp?)
	                            // When referenced casted stackframe 
	                            // (fp-relative) -> stack (ss-relative)
	meGSegment_Far,             //program-defined bank/segment
	meGSegment_Count,
	meGSegment_Immediate,
} eGSegment, eGAddressingmode;
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
	0,
	"imm",
	0
};
typedef enum eGCallingconvention {
	eGCallingconvention_Stdcall = 1,
	eGCallingconvention_Cdecl   = 2,
} eGCallingconvention;
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
	//eGAtomictype_Neararray        = 9,
	eGAtomictype_Nearfunction     = 10,
	eGAtomictype_Farpointer       = 11,
	//eGAtomictype_Fararray         = 12,
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
	//{ eGAtomictype_Neararray                 , "neararray" },
	{ eGAtomictype_Nearfunction              , "nearfunction" },
	{ eGAtomictype_Farpointer                , "farpointer" },
	//{ eGAtomictype_Fararray                  , "fararray" },
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
	"nearptr"         ,
	"neararray"       ,
	"nearfunction"    ,
	"farptr"          ,
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
	enum eGAtomictype altsize;
	enum eGSegment segment;
} tGOpcode;
typedef struct tGInstruction {
	tGOpcode opcode; 
	char* label;
	struct tGInstruction* next;
	struct tGInstruction* jumptarget;
	tGTargetUintmax immediate;
} tGInstruction;
typedef struct { // tGTargetPointer
	bool nonconstant; // TODO: Refactor to use 'dynamic' segment instead of nonconstant specifier
	// Constant pointer - segment-offset:
	//  Allows to specify one of built-in segments, or `far` to specify bank 
	//  and offset in said bank.
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
	eGValuecategory_Rightvalue = 2,
	eGValuecategory_Farleftvalue = 3,
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
	eGValuecategory valuecategory;       // rvalue <-> lvalue
	eGSegment lvaluesegment;             // lvalues may be not just in dataseg
	eGAtomictype atomicbasetype;
	struct tGType * complexbasetype;
	char* unresolvedsymbol;
	tList /* LxNode<Declaration> */ * precompiledstructure;
	struct tGNamespace * structure;
	void /* tLxNode */ * precompiledenumeration;
	eGTypequalifiers typequalifiers;
	tListnode /* <tGType> */ * functionarguments;
	tListnode /* <tGType> */ * templatemodifiers;
	tGTargetSizet structsize;
	bool arraysizepresent;
	struct tGSymbol * dynamicarraysize;
	tGTargetSizet arraysize;
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
	bool isfar;
	eGCallingconvention callingconvention;
	//tGType* returnedtype;
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
	char* identifier;
	tSpFunctionextinfo* fextinfo;
	tList /* <tSpNode> */ * switchlabels;
	tGInstruction* boundbreak;
	tGInstruction* boundcontinue;
} tSpNode;

//
//		C Operators
//	A modified C++ chart.
//	P r e c e d e n c e   i s   n o n - s t a n d a r d . 
//	Specifically you can't use bitwise and/or as logical ones.
//	 This is not K&R C, please use logical operators.
//
//    Associativity
//  Precedence  opr. Description  
//
//                      Left-to-right →
//  0    ::      Scope resolution  
//  1    ++ --   Suffix/postfix increment and decrement  
//       ()      Function call
//       []      Array subscripting
//       .       Structure and union member access
//       ->      Structure and union member access through pointer
//       (){}    Compound literal from our beloved C99
//                       Right-to-left ←
//  2    ++ --   Prefix increment and decrement
//       + -     Unary plus and minus
//       ! ~     Logical NOT and bitwise NOT
//       ()      Type cast
//       *       Indirection (dereference)
//       &       Address-of
//       sizeof  Size-of
//                There should've been `alignof`, but there's not much
//                 reason for alignment on 8-bit machines, so...
//                      Left-to-right →
//  2.5  .* ->*  Pointer-to-member  
//  3    * / %   Multiplication, division, and remainder
//  4    + -     Addition and subtraction
//  5    << >>   Bitwise left shift and right shift
//  6    &       Bitwise AND
//  7    ^       Bitwise XOR (exclusive or)
//  8    |       Bitwise OR (inclusive or)
//  8.5  <=>     Three-way comparison operator (why though)
//  9    < <=    Comparison
//       > >=
//  10   == !=   More comparison
//  11   &&      Logical AND
//  12   ||      Logical OR
//                      Right-to-left ←
//  13   ?:      Ternary conditional
//  14   =       Simple assignment
//       += -=   Assignment by sum and difference
//       *= /=   Assignment by product, quotient, and remainder
//       %=
//       <<=     Assignment by bitwise left shift and right shift
//       >>=
//       &= ^=   Assignment by bitwise AND, XOR, and OR
//       |=  
//               Left-to-right → 
//  15   ,              Comma  
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
