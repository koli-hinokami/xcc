#include "hyperheader.h"
// ------------ xcc Crossdevelopment suite -- Linking assembler -------------
// ----------------- The one that assembles to object files -----------------

// -- Types --

typedef enum AsmCurrentpass {
	eAsmCurrentpass_None  =0,
	eAsmCurrentpass_First =1,
	eAsmCurrentpass_Middle=2,
	eAsmCurrentpass_Second=3,
} eAsmCurrentpass;
typedef enum AsmArgpOptiontags {
	// This enumeration should have all the options passable
	eAsmArgpOptiontags_Architecture = 'a',
	eAsmArgpOptiontags_Confdir      = 'c',
	eAsmArgpOptiontags_Outputfile   = 'o',
} eAsmArgpOptiontags;

typedef enum AsmConstraittype {
	eAsmConstraittype_Null=0,
	eAsmConstraittype_Nearref=1,
	eAsmConstraittype_Shortref=2, // currently unused
	eAsmConstraittype_Relref=3,
	eAsmConstraittype_Segment=4,
} eAsmConstraittype;

typedef struct {
	char* /* deallocatable */ label;
	char* /* deallocatable */ instruction;
	tList /* <char* deallocatable> */ * /* deallocatable */ operands;
} tAsmSourceline;

typedef enum eAsmTokentype {
	eAsmTokentype_Charater = 1,
	eAsmTokentype_Argument,
	eAsmTokentype_Newline,
	eAsmTokentype_Argumenttoken,
	eAsmTokentype_Number,
	eAsmTokentype_StringedtokensStart,
	eAsmTokentype_Identifier,
	eAsmTokentype_Label,
	eAsmTokentype_String,
} eAsmTokentype;

typedef enum eAsmBinarytokensize {
	eAsmBinarytokensize_Lobyte = 1,
	eAsmBinarytokensize_Hibyte,
	eAsmBinarytokensize_Word,
	eAsmBinarytokensize_Label,
} eAsmBinarytokensize;

typedef enum eAsmBinarytokentype {
	eAsmBinarytokentype_Newline = 1,
	eAsmBinarytokentype_Raw,
	eAsmBinarytokentype_Position,
	eAsmBinarytokentype_Expr,
	eAsmBinarytokentype_Relative,
} eAsmBinarytokentype;

typedef struct {
	eAsmTokentype type;
	union {
		char ch;
		char* /* owned */ string;
		tGTargetNearpointer number;
	};
} tAsmToken;

typedef struct {
	unsigned char size;
	eAsmBinarytokentype type;
	char* label;
	signed char arg;
	signed char disp;
} tAsmBinarytoken;

typedef struct sAsmConstrait {
	eAsmConstraittype type;
	intptr_t param;
	int operandnr;
	bool (*verifier)(struct sAsmConstrait * self, tGTargetNearpointer val);
} tAsmConstrait;
typedef bool (*tAsmConstraitverifier)(struct sAsmConstrait * self, tGTargetNearpointer val);

typedef struct { // The instructions defined
	char* /* owned */ opcode;
	tList /* <tAsmToken> */ * operands;
	tList /* <tAsmConstrait*> */ * constraits;
	tList /* <tAsmBinarytoken> */ * expansion;
} tAsmInstructiondefinition;

typedef struct { // Label
	char* /* owned */ name;
	tGTargetNearpointer offset;
	tGTargetSegment segment;
} tAsmLabel;

enum eAsmRelocationentrykind {
	eAsmRelocationentrykind_Terminator   = 0,
	eAsmRelocationentrykind_Segmentstart = 1,
	eAsmRelocationentrykind_Label        = 2,
	eAsmRelocationentrykind_Position     = 3,
	eAsmRelocationentrykind_Relative     = 4,
};
typedef struct { // Relocationentry
	enum eAsmRelocationentrykind kind;
	union {
		int segment;
		char* label;
	};
} tAsmRelocationentry;


// -- Preprocessor constants --

#define qiAsmMaxsegments 32

// -- Forward declarations --

error_t AsmArgpParser(int key,char* argumentvalue,struct argp_state *state);
tGTargetNearpointer AsmGetlabelvalue(char* name);
char* mtAsmToken_ToString(tAsmToken* self);
tAsmRelocationentry* mtAsmRelocationentry_CreatePosition();
tAsmRelocationentry* mtAsmRelocationentry_CreateRelative();
tAsmRelocationentry* mtAsmRelocationentry_CreateSegmentstart(int segment);
tGTargetNearpointer AsmGetlabelsegment(char* name);
void mtAsmRelocationentry_Emit(tAsmRelocationentry* self, FILE* dstfile);
tAsmRelocationentry* mtAsmRelocationentry_CreateLabel(char* label);


// -- Globals --

eAsmCurrentpass AsmCurrentpass;
tList /* <tAsmInstructiondefinition> */ AsmInstructionsdefined;
tList /* <tIrcToken* owned> */ * AsmBoundparameters[10];
tGTargetNearpointer AsmCurrentposition[qiAsmMaxsegments];
tGTargetNearpointer AsmCurrentdetuneposition[qiAsmMaxsegments];
tList /* <tAsmLabel* owned> */ * AsmLabels;
tList /* <tAsmLabel* owned> */ * AsmSublabels;
int iAsmRepeattimes;
unsigned AsmCurrentsegment;
char* szAsmLastlabel;
unsigned AsmCurrentsourceline;

tAsmInstructiondefinition* AsmInstructionfinderclojure_Constraitfailee;
bool AsmInhibitconstraiting;
	// ↑ In the best of Forth traditions, I'm using implicit context here.

struct argp_option AsmArgpOptions[] = {
	{	
		.name = "cfgdir",
		.key = eAsmArgpOptiontags_Confdir,
		.arg = "confdir",
		.flags = null,
		.doc = "Directory to search for config files in."
		       "Defaults to /etc/xcc.",
		.group = null,
	},
	{	
		.name = "arch",
		.key = eAsmArgpOptiontags_Architecture,
		.arg = "archname",
		.flags = null,
		.doc = "The architecture to compile assembly sources for. "
		       "Retargetablity first!",
		.group = null,
	},
	{	
		.name = "out",
		.key = eAsmArgpOptiontags_Outputfile,
		.arg = "outputfile.obj",
		.flags = null,
		.doc = "Output object file",
		.group = null,
	},
	{	// Terminator entry
		.name = nullptr,
		.key = null,
		.arg = nullptr,
		.flags = null,
		.doc = nullptr,
		.group = null,
	}
};
struct argp AsmArgpParserstruct = {
	.options = AsmArgpOptions,
	.parser = &AsmArgpParser,    //error_t (*)(
							     //    int key,   // option's tag
							     //    char *arg, // option's value
							     //    struct argp_state *state
							     //)
	.args_doc = "file.asm"      // Usage
	          "\n-o out.obj file",
	.doc = 
		"Linking assembler for xcc Crossdevelopment suite"
		"\n\v"
		"If --output option isn't specified, extension is replaced with "
		".obj (if present, otherwise created) to get output file name. "
		"\n",
	                             // If non-zero, a string containing extra text
	                             // to be printed before and after the options 
	                             // in a long help message, with the two 
	                             // sections separated by a vertical tab ('\v',
	                             // '\013') character. By convention, the 
	                             // documentation before the options is just a
	                             // short string explaining what the program 
	                             // does. Documentation printed after the 
	                             // options describe behavior in more detail.
	.children = nullptr,         // const struct argp_child *arr children
	.help_filter = nullptr,      // char*(*)(int key,char* text,void* input)
	.argp_domain = nullptr,      // wat
};

char* AsmSourcefilename;
char* AsmTargetfilename;
char* AsmArchitecturename;
char* AsmConfigdir;

FILE* AsmSourcestream;
tList /* <FILE*> */ AsmIncludelist;
tList /* <char*> */ AsmExternallabels;

// -- Auxilirally functions --

bool mtChar_AsmIsTokenterminator(char self){
	if(isalnum(self)){return false;};  //Alphanumeric - usual charater
	if(self=='_')return false;         //Underscore - allowed in tokens
	if(self=='.')return false;         //`.` - This is a macroassembler so why not
	return true;                       //The rest are 'auto-whitespace' chars that begin another token
};

bool AsmIsLabelLocal(char* /* borrows */ label){
	assert(label);
	if(label[0]=='_' || label[0]=='.')
		return true;
	return false;
};
char* /* deallocate */ AsmLocallabelfilter(char* /* borrows */ label){
	assert(label);
	if(AsmIsLabelLocal(label)){
		assert(szAsmLastlabel);
		return mtString_Join(szAsmLastlabel,label);
	}else{
		return mtString_Clone(label);
	};
};

// -- Source file get/peek wrapper --

FILE* getcurrentfile(){
	if(mtList_Isempty(&AsmIncludelist)){
		// No includes pending
		return AsmSourcestream;
	}else{
		// Pending include
		//printf("EM: [T] Test fetchcharater includes\n");
		FILE* stream = mtList_GetFirstitem(&AsmIncludelist);
		assert(stream);
		int c = fgetc(stream);
		if(c==EOF){
			// eof, mf!
			//fclose(stream);
			mtList_Removefirst(&AsmIncludelist);
			return getcurrentfile();
		}else{
			ungetc(c,stream);
			return stream;
		}
		//return fgetc(((tPrIncludebufferentry*)AsmIncludelist.first->item)->file);
	}
};

int peekcharater(){
	FILE* src = getcurrentfile();
	if(!src){
		errno = 0;
		return EOF;
	};
	return ungetc(fgetc(src),src);
};

int fetchcharater(){
	FILE* src = getcurrentfile();
	if(!src){
		errno = 0;
		return EOF;
	};
	return fgetc(src);
};

// -- class FILE --

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};

// -- class tAsmLabel --

tAsmLabel* mtAsmLabel_Clone(tAsmLabel* self){
	return memcpy(malloc(sizeof(tAsmLabel)),self,sizeof(tAsmLabel));
};
void mtAsmLabel_Destroy(tAsmLabel* self){
	free(self->name);
	free(self);
};
tAsmLabel* mtAsmLabel_CreateNear(char* /* borrows */ name, tGTargetNearpointer offset){
	return mtAsmLabel_Clone(
		&(tAsmLabel){
			.name=mtString_Clone(name),
			.offset=offset
		}
	);
};

// -- class tAsmBinarytoken --

tAsmBinarytoken* mtAsmBinarytoken_Allocate(void){return calloc(1,sizeof(tAsmBinarytoken));};
tAsmBinarytoken* mtAsmBinarytoken_Create(void){return calloc(1,sizeof(tAsmBinarytoken));};
tAsmBinarytoken* mtAsmBinarytoken_Clone(tAsmBinarytoken* self){
	//
	void* i = memcpy(malloc(sizeof(tAsmBinarytoken)),self,sizeof(tAsmBinarytoken));;
#ifdef qvGTrace
	printf("ASM:[T] mtAsmBinarytoken_Clone(self* %p)->%p: Entered\n",self,i);
#endif
	return i;//memcpy(malloc(sizeof(tAsmBinarytoken)),self,sizeof(tAsmBinarytoken));;
};
bool mtAsmBinarytoken_HasString(tAsmBinarytoken* self){
	return false; // self->type>=eAsmBinarytokentype_StringedtokensStart;
};
void mtAsmBinarytoken_Destroy(tAsmBinarytoken* self){
	//
#ifdef qvGTrace
	printf("ASM:[T] mtAsmBinarytoken_Destroy(self* %p): Entered\n",self);
#endif
	free(self);
};
tAsmBinarytoken* mtAsmBinarytoken_Get(FILE* src){ // Constructor
	//tAsmBinarytoken* self = mtAsmBinarytoken_Allocate();
#ifdef qvGTrace
	printf("ASM:[T] mtAsmBinarytoken_Get(FILE* (%p)): Entered\n",src);
#endif
	errno=0;
	switch(fpeekc(src)){
		case EOF: // If errno==0 quietly return nullptr elseways yell at the user that we gone bad
			//if(errno==0)return nullptr;
			// But now that I think of the special cases like reading arguments while encountering EOF...
			return mtAsmBinarytoken_Clone(
				&(tAsmBinarytoken){
					.type = eAsmBinarytokentype_Newline,
				}
			);
			printf("ASM:[E] mtAsmBinarytoken_Get(FILE* %p): Error %i•\"%s\" while fetching charater\n",src,errno,strerror(errno));
			ErfError();
			return nullptr;
		case ';':
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmBinarytoken_Get: Comment - recursing tailcall \n");
#endif
			while(fpeekc(src)!='\n')fgetc(src);
			return mtAsmBinarytoken_Get(src);
		case '\n':
			fgetc(src);
			return mtAsmBinarytoken_Clone(
				&(tAsmBinarytoken){
					.type = eAsmBinarytokentype_Newline,
				}
			);
			break;
		case ' ':
		case '\t':
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmBinarytoken_Get: whitespace \n");
#endif
			fgetc(src);
			return mtAsmBinarytoken_Get(src);
			break;
		default: {
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmBinarytoken_Get: Default at char %i•%c\n",fpeekc(src),fpeekc(src));
#endif
			// Used ASCII charaters:
			// !
			// "
			// #
			// $	position // relative->absolute
			// %	relative // absolute->relative
			// &	raw      // literal
			// '
			// (
			// )
			// *
			// +
			// ,
			// -
			// .
			// /
			// :
			// ;	comment
			// <	lobyte
			// =	hibyte
			// >	word
			// ?
			tAsmBinarytoken* self = mtAsmBinarytoken_Create();
			switch(fpeekc(src)){ // Size/mode - lobyte, hibyte, word
				case '>': fgetc(src);          self->size = eAsmBinarytokensize_Hibyte;     break;
				case '<': fgetc(src); default: self->size = eAsmBinarytokensize_Lobyte;     break;
				case '=': fgetc(src);          self->size = eAsmBinarytokensize_Word;       break;
			};
			{	// Displacement
				bool negate = false;
				unsigned base = 10;
				if(fpeekc(src)=='-'){fgetc(src);negate = true;};
				if(fpeekc(src)=='0'){
					fgetc(src);
					switch(fpeekc(src)){
						case 'b': fgetc(src);          base = 2;  break;
						case 'o': fgetc(src); default: base = 8;  break;
						case 'd': fgetc(src);          base = 10; break;
						case 'x': fgetc(src);          base = 16; break;
					};
					// Don't get confused that default base is octal.
					// The default here is if leading 0 is present, proper 
					// default base is in `base`'s initializer.
				};
				while((base==16?isxdigit:isdigit)(fpeekc(src))){
					self->disp*=base;
					unsigned char ch = fgetc(src);
					self->disp+=
							ch>='a'&&ch<='f'
						?	ch-'a'+10
						:		ch>='A'&&ch<='F'
							?	ch-'A'+10
							:		ch>='0'&&ch<='9'
								?	ch-'0'
								:	(assert(false),0)
						;
				};
				if(negate)self->disp=-self->disp;
			};
			switch(fpeekc(src)){ // Type
				case '$': fgetc(src);          self->type = eAsmBinarytokentype_Position;   break;
				case '%': fgetc(src);          self->type = eAsmBinarytokentype_Relative;
				                               self->arg  = fgetc(src)-'0';
				                               break;
				case '#': fgetc(src);          self->type = eAsmBinarytokentype_Expr;
				                               self->arg  = fgetc(src)-'0';
				                               break;
				case '&': fgetc(src); default: self->type = eAsmBinarytokentype_Raw;        break;
			};
			return self;
		};
	};
	return nullptr;
};
char* mtAsmBinarytoken_ToString(tAsmBinarytoken* self){
	return mtString_Join(
		 self->size==eAsmBinarytokensize_Lobyte?"<"
		:self->size==eAsmBinarytokensize_Hibyte?">"
		:self->size==eAsmBinarytokensize_Word  ?"="
		:"(unknein size)",
		mtString_Join(
			mtString_FromInteger(self->disp),
			 self->type==eAsmBinarytokentype_Raw     ?"&"
			:self->type==eAsmBinarytokentype_Expr    ?mtString_Join(
			                                              "#",
			                                              mtString_FromInteger(
			                                                self->arg)
			                                          )
			:self->type==eAsmBinarytokentype_Position?"$"
			:self->type==eAsmBinarytokentype_Relative?"%"
			:self->type==eAsmBinarytokentype_Newline ?"\\n"
			:mtString_Join(
				mtString_Join(
					"(unknein type ",
					mtString_FromInteger(self->type)
				),
				")"
			)
		)
	);
}
void mtAsmBinarytoken_Subdryemit(tAsmBinarytoken* self){
	switch(self->size){
		case eAsmBinarytokensize_Lobyte:
		case eAsmBinarytokensize_Hibyte:
			AsmCurrentdetuneposition[AsmCurrentsegment]+=1;
			break;
		case eAsmBinarytokensize_Word:
			AsmCurrentdetuneposition[AsmCurrentsegment]+=2;
			break;
		default:
			printf("ASM:[E] mtAsmBinarytoken_Dryemit: Unrecognized token <%s> size %i\n",
			mtAsmBinarytoken_ToString(self),
			self->size);
			break;
	};
};
void mtAsmBinarytoken_Dryemit(tAsmBinarytoken* self){
	switch(self->size){
		case eAsmBinarytokensize_Lobyte:
		case eAsmBinarytokensize_Hibyte:
			AsmCurrentposition[AsmCurrentsegment]+=1;
			break;
		case eAsmBinarytokensize_Word:
			AsmCurrentposition[AsmCurrentsegment]+=2;
			break;
		default:
			printf("ASM:[E] mtAsmBinarytoken_Dryemit: Unrecognized token <%s> size %i\n",
			mtAsmBinarytoken_ToString(self),
			self->size);
			break;
	};
};
intptr_t mtAsmRelocationentry_Evaulate2(
	tAsmRelocationentry* self
){
	switch(self->kind){
		default:
			ErfError_String2(mtString_Format("ASM:[E] mtAsmRelocationentry_Evaulate2: Unsupported relocation type %i\n",self->kind));
			return 0;
	}
};
tGTargetNearpointer AsmEvaulateexpression(
	tList /* <tAsmRelocationentry*> */ * relocs,
	tList /* <tAsmToken*> */ * expr
){
	tGTargetNearpointer exprval = 0;
	for(tListnode* i = expr->first;i;i=i->next){
		tAsmToken* j = i->item;
#ifdef qvGTrace
		printf("ASM:[T] Evalexpr for token <%s>\n", mtAsmToken_ToString(j));
#endif
		int mode = 1; // 1 - addictive, 2 - substractive
		if(j->type==eAsmTokentype_Charater && j->ch=='+')
			{ i=i->next; mode = 1; };
		if(j->type==eAsmTokentype_Charater && j->ch=='-')
			{ i=i->next; mode = 2; };
		assert(i);
		j=i->item;
		tGTargetNearpointer val = 0;
		switch(j->type){
			case eAsmTokentype_Charater:
				switch(j->ch){
					case '$':
						val=0;
						assert(mode==1);
						if(relocs)
							mtList_Append(relocs,
								mtAsmRelocationentry_CreatePosition()
							);
						break;
					default:
						printf("ASM:[E] mtAsmBinarytoken_Emit: "
							   "Evalexpr for token <%s>: "
							   "unrecognized char \'%c\'\n", 
							mtAsmToken_ToString(j),j->ch
						);
						ErfError();
						val = 0;
						break;
				};
				break;
			case eAsmTokentype_Identifier:
				if(
					mtList_Find_Clojure(
						&AsmExternallabels,
						(bool(*)(void*,void*))mtString_Equals,
						AsmLocallabelfilter(j->string)
					)
				){
					// External label
					assert(mode==1);
					if(relocs)
						mtList_Append(relocs,
							mtAsmRelocationentry_CreateLabel(
								AsmLocallabelfilter(j->string)
							)
						);
				}else{
					// Label handled internally
					val = AsmGetlabelvalue( // Label offset
						AsmLocallabelfilter(j->string));
					assert(mode==1);
					if(
						AsmGetlabelsegment(
							AsmLocallabelfilter(j->string)
						)
					)
						if(relocs)
							mtList_Append(relocs, // Relocate by 
							                      // label's segment
								mtAsmRelocationentry_CreateSegmentstart(
									AsmGetlabelsegment(
										AsmLocallabelfilter(j->string)
									)
								)
							);
				};
				break;
			case eAsmTokentype_Number:
				val = j->number;
				break;
			default:
				printf("ASM:[E] mtAsmBinarytoken_Emit: "
					   "Evalexpr for token <%s>: "
					   "unrecognized type %i\n", 
					mtAsmToken_ToString(j),j->type
				);
				ErfError();
				val = 0;
		};
		switch(mode){
			case 1:
				exprval+=val;
				break;
			case 2:
				exprval-=val;
				break;
			default:
				assert(false);
				break;
		};
	};
	return exprval;
}
void mtAsmBinarytoken_Emit(tAsmBinarytoken* self,FILE* dst){
	mtAsmBinarytoken_Dryemit(self);
	tGTargetNearpointer exprval = self->disp;
	tList /* <tAsmRelocationentry> */ * relocations = mtList_Create();
	switch(self->type){ // Evaulate value of the binarytoken 
		case eAsmBinarytokentype_Raw:
			break;
		case eAsmBinarytokentype_Position:
			mtList_Append(relocations, mtAsmRelocationentry_CreatePosition());
			break;
		case eAsmBinarytokentype_Relative:
			mtList_Append(relocations, mtAsmRelocationentry_CreateRelative());
		case eAsmBinarytokentype_Expr: { // Evaulate the expression
			assert(self->arg>=0);
			assert(self->arg<10);
			tList /* tAsmToken */ * expr = AsmBoundparameters[self->arg];
			exprval += AsmEvaulateexpression(relocations,expr);
		};	break;
		default:
			printf("ASM:[E] mtAsmBinarytoken_Emit: Unrecognized token <%s> type %i\n",mtAsmBinarytoken_ToString(self),self->type);
			ErfError();
			break;
	};
	fputc(AsmCurrentsegment,         dst);            // Segment
	fputc(self->size                ,dst);            // Type
	fputc(exprval                   ,dst);            // Displacement
	fputc(exprval>>8                ,dst);            // Highdisplacement
	for(tListnode* i = relocations->first;i;i=i->next) // Relocations
		mtAsmRelocationentry_Emit(i->item,dst);
	fputc(eAsmRelocationentrykind_Terminator,dst);    // Terminator
	
};
// -- class tAsmToken --

tAsmToken* mtAsmToken_Allocate(void){return calloc(1,sizeof(tAsmToken));};
tAsmToken* mtAsmToken_Clone(tAsmToken* self){
	//
	void* i = memcpy(malloc(sizeof(tAsmToken)),self,sizeof(tAsmToken));;
#ifdef qvGTrace
	//printf("ASM:[T] mtAsmToken_Clone(self* %p)->%p: Entered\n",self,i);
#endif
	return i;//memcpy(malloc(sizeof(tAsmToken)),self,sizeof(tAsmToken));;
};
tAsmToken* mtAsmToken_CreateCharaterd(eAsmTokentype type,char ch){
	tAsmToken* i = calloc(1,sizeof(tAsmToken));
	i->type=type;
	i->ch=ch;
	return i;
};
bool mtAsmToken_HasString(tAsmToken* self){
	assert(self);
	return self->type>=eAsmTokentype_StringedtokensStart;
};
void mtAsmToken_Destroy(tAsmToken* self){
#ifdef qvGTrace
	//printf("ASM:[T] mtAsmToken_Destroy(self* %p): Entered\n",self);
#endif
	if(mtAsmToken_HasString(self))mtString_Destroy(self->string);
	memset(self,0,sizeof(tAsmToken));
	free(self);
};
tAsmToken* mtAsmToken_Get(FILE* src){ // Constructor
	//tAsmToken* self = mtAsmToken_Allocate();
#ifdef qvGTrace
	//printf("ASM:[T] mtAsmToken_Get(FILE* (%p)): Entered\n",src);
	//printf("ASM:[T] mtAsmToken_Get: Charater \'%c\'\n",fpeekc(src));
#endif
	errno=0;
	switch(fpeekc(src)){
		case EOF: // If errno==0 quietly return nullptr elseways yell at the user that we gone bad
			//if(errno==0)return nullptr;
			// But now that I think of the special cases like reading arguments while encountering EOF...
			if(errno==0)
				return mtAsmToken_Clone(
					&(tAsmToken){
						.type = eAsmTokentype_Newline,
					}
				);
			printf("ASM:[E] mtAsmToken_Get(FILE* %p): Error %i•\"%s\" while fetching charater\n",src,errno,strerror(errno));
			ErfError();
			return nullptr;
		case ';':
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmToken_Get: Comment - recursing tailcall \n");
#endif
			while(fpeekc(src)!='\n')fgetc(src);
			return mtAsmToken_Get(src);
		case '\n':
			fgetc(src);
			AsmCurrentsourceline++;
			return mtAsmToken_Clone(
				&(tAsmToken){
					.type = eAsmTokentype_Newline,
				}
			);
			break;
		case '#':
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmToken_Get: Argument \n");
#endif
			fgetc(src);
			return mtAsmToken_Clone(
				&(tAsmToken){
					.type = eAsmTokentype_Argument,
					.ch = fgetc(src)
				}
			);
			break;
		case ' ':
		case '\t':
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmToken_Get: whitespace \n");
#endif
			fgetc(src);
			return mtAsmToken_Get(src);
			break;
		case '\'': { // A charater
			fgetc(src);
			unsigned value=0;
			for(bool cont=true;cont;){
				uint8_t i=0;
				switch(fpeekc(src)){
					case EOF:
						fprintf(
							stderr,
							"ASM:[F] mtAsmToken_Get: "
							"Charater constant: "
							"Unexcepted end of file\n"
						);
						ErfFatal();
						assert(false);
						break;
					case '\'':
						fgetc(src);
						cont = false;
						continue;
					case '\\':
						fgetc(src);
						int j;
						i = j = fgetc(src);
						switch(j){
							case EOF:
								fprintf(
									stderr,
									"ASM:[F] mtAsmToken_Get: "
									"Charater constant: "
									"Unexcepted end of file\n"
								);
								ErfFatal();
								assert(false);
								break;
							case 'a':  i = '\a';       break;
							case 'b':  i = '\b';       break;
							case 'e':  i = '\e';       break;
							case 'f':  i = '\f';       break;
							case 'n':  i = '\n';       break;
							case 'r':  i = '\r';       break;
							case 't':  i = '\t';       break;
							case 'u':  assert(false);  break;
							case 'v':  i = '\v';       break;
							case 'x':  assert(false);  break;
							case '\'': i = '\'';       break;
							case '\"': i = '\"';       break;
							default:
								//i = fgetc(src);
						};
						break;
					default:
						i = fgetc(src);
						break;
				};
				value<<=8;
				value|=i;
			};
			return mtAsmToken_Clone(
				&(tAsmToken){
					.type = eAsmTokentype_Number,
					.number = value,
				}
			);
			break;
		};	break;
		case '\"': { // A string
			fgetc(src);
			char* str = mtString_Create();
			while(fpeekc(src)!='\"'){ // To end of string
				if(fpeekc(src)=='\\'){ // If escape seen
					fgetc(src); // Skip the backslash
					char buf;
					switch(buf = fgetc(src)){
						case 'a':  buf = '\a';      break;
						case 'b':  buf = '\b';      break;
						case 'e':  buf = '\e';      break;
						case 'f':  buf = '\f';      break;
						case 'n':  buf = '\n';      break;
						case 'r':  buf = '\r';      break;
						case 't':  buf = '\t';      break;
						case 'u':  assert(false);   break;
						case 'v':  buf = '\v';      break;
						case 'x':  assert(false);   break;
						case '\'': buf = '\'';      break;
						case '\"': buf = '\"';      break;
						default:
					};
					mtString_Append(&str,(char[2]){buf,0}); 
				}else{ // Else if no escape
					mtString_Append(&str,(char[2]){fgetc(src),0}); 
					// Plain add charater to string.
				};
			};
			fgetc(src); // Skip string terminator
			return mtAsmToken_Clone(
				&(tAsmToken){
					.type  = eAsmTokentype_String,
					.string= str,
				}
			);
		};	break;
		default: {
#ifdef qvGTrace
			//printf("ASM:[T] mtAsmToken_Get: Unrecognized char %i•%c\n",fpeekc(src),fpeekc(src));
#endif
			if(mtChar_AsmIsTokenterminator(fpeekc(src))){
#ifdef qvGTrace
				//printf("ASM:[T] mtAsmToken_Get: Token-separating charater\n");
#endif
				return mtAsmToken_Clone(
					&(tAsmToken){
						.type = eAsmTokentype_Charater,
						.ch = fgetc(src),
					}
				);
			}else{
#ifdef qvGTrace
				//printf("ASM:[T] mtAsmToken_Get: Identifier\n");
#endif
				char* str = mtString_Create();
				while(!mtChar_AsmIsTokenterminator(fpeekc(src))){
#ifdef qvGTrace
					//printf("ASM:[T] mtAsmToken_Get: adding ch \'%c\'\n",fpeekc(src));
#endif
					mtString_Append(&str,(char[2]){fgetc(src),0});
				}
				if(isdigit(str[0])){
					assert(str[0]);
					char* strtol_tail;
					tGTargetNearpointer val;
					if(str[0]=='0' && str[1]=='x'){
						val=strtol(str+2,&strtol_tail,16);
						assert(strtol_tail[0]==0);
					}else{
						val=strtol(str,&strtol_tail,10);
						if(strtol_tail[0]!=0){
							printf("ASM:[T] mtAsmToken_Get: Unrecognized number token <%s>\n",str);
						};
						assert(strtol_tail[0]==0);
					};
					assert(strtol_tail[0]==0);
					return mtAsmToken_Clone(
						&(tAsmToken){
							.type = eAsmTokentype_Number,
							.number = val,
						}
					);
				}else if(fpeekc(src)==':'){
					fgetc(src);
					return mtAsmToken_Clone(
						&(tAsmToken){
							.type = eAsmTokentype_Label,
							.string = str,
						}
					);
				}else{
					return mtAsmToken_Clone(
						&(tAsmToken){
							.type = eAsmTokentype_Identifier,
							.string = str,
						}
					);
				}
			};
		};
	};
	return nullptr;
};
char* mtAsmToken_ToString(tAsmToken* self){
	return mtString_Clone(
		 self->type==eAsmTokentype_Charater   ?(char[2]){self->ch,0}
		:self->type==eAsmTokentype_Argument   ?(char[3]){'#',self->ch,0}
		:self->type==eAsmTokentype_Newline    ?"\\n"
		//:self->type==eAsmTokentype_Argumenttoken ?TODO
		:self->type==eAsmTokentype_Number     ?mtString_Join("&",mtString_FromInteger(self->number))
		:self->type==eAsmTokentype_Identifier ?self->string
		:self->type==eAsmTokentype_Label      ?mtString_Join(self->string,":")
		:self->type==eAsmTokentype_String     ?mtString_Format("\"%s\"", self->string)
		:mtString_Format("(token %p unknein type %i)",self,self->type)
	);
}

void mtAsmToken_Emit(tAsmToken* self,FILE* dst){
	assert(false);
};
bool mtAsmToken_Equals(tAsmToken* self,tAsmToken* tok){
	assert(self);
	assert(tok);
	if(self->type!=tok->type)return false;
	if(self->type==eAsmTokentype_Identifier)
		if(strcmp(self->string,tok->string)!=0)return false;
	if(self->type==eAsmTokentype_Charater)
		if(self->ch!=tok->ch)return false;
	return true;
};


// -- class tAsmRelocationentry --

tAsmRelocationentry* mtAsmRelocationentry_CreatePosition(){
	tAsmRelocationentry* self = calloc(1,sizeof(tAsmRelocationentry));
	self->kind = eAsmRelocationentrykind_Position;
	return self;
};
tAsmRelocationentry* mtAsmRelocationentry_CreateRelative(){
	tAsmRelocationentry* self = calloc(1,sizeof(tAsmRelocationentry));
	self->kind = eAsmRelocationentrykind_Relative;
	return self;
};
tAsmRelocationentry* mtAsmRelocationentry_CreateSegmentstart(int segment){
	tAsmRelocationentry* self = calloc(1,sizeof(tAsmRelocationentry));
	self->kind = eAsmRelocationentrykind_Segmentstart;
	self->segment = segment;
	return self;
};
tAsmRelocationentry* mtAsmRelocationentry_CreateLabel(char* label){
	tAsmRelocationentry* self = calloc(1,sizeof(tAsmRelocationentry));
	self->kind = eAsmRelocationentrykind_Label;
	self->label = label;
	return self;
};

void mtAsmRelocationentry_Emit(tAsmRelocationentry* self, FILE* dstfile){
	switch(self->kind){
		case eAsmRelocationentrykind_Position:
			fputc(self->kind,dstfile);
			break;
		case eAsmRelocationentrykind_Relative:
			fputc(self->kind,dstfile);
			break;
		case eAsmRelocationentrykind_Segmentstart:
			fputc(self->kind,dstfile);
			fputc(self->segment,dstfile);
			break;
		case eAsmRelocationentrykind_Label:
			fputc(self->kind,dstfile);
			for(char* i=self->label;*i;i++) fputc(*i,dstfile);
			fputc(0,dstfile);
			break;
		default:
			assert(false);
			break;
	};
};
// -- class tAsmConstrait --
tAsmConstrait* mtAsmConstrait_Clone(tAsmConstrait* self){
	return memcpy(malloc(sizeof(tAsmConstrait)),self,sizeof(tAsmConstrait));
}
tAsmConstrait* mtAsmConstrait_CreateExpr_Generic(
	eAsmConstraittype type, int operandnr
){
	return mtAsmConstrait_Clone(&(tAsmConstrait){
		.type=type,
		.operandnr=operandnr,
		.verifier=nullptr,
	});
}
tAsmConstrait* mtAsmConstrait_CreateExpr_Parametered(
	eAsmConstraittype type, intptr_t param, int operandnr
){
	return mtAsmConstrait_Clone(&(tAsmConstrait){
		.type=type,
		.param=param,
		.operandnr=operandnr,
		.verifier=nullptr,
	});
}
bool mtAsmConstrait_Verify(tAsmConstrait* self){
	if(AsmCurrentpass==eAsmCurrentpass_First) return false; // When preparsing,
		                                                    // all constraits
		                                                    // are unsatisfied
		                                                    // by default.
		                                                    // flatassembler
		                                                    // fans (including
		                                                    // me) are going to
		                                                    // kill me for this.
	switch(self->type){
		case eAsmConstraittype_Nearref:
		case eAsmConstraittype_Shortref:
		case eAsmConstraittype_Relref:
		{
			tList relocations[1] = {};
			int flags = 0;
			tGTargetPtrdifft val = AsmEvaulateexpression(
				relocations,
				AsmBoundparameters[self->operandnr]
			);
			for(tListnode* i=relocations->first;i;i=i->next){tAsmRelocationentry*j=i->item;
				switch(j->kind){
					case eAsmRelocationentrykind_Segmentstart:
						assert(j->segment==AsmCurrentsegment); // no crosssegment shortreljumps
						flags++;
						break;
					default:
						assert(false); // Unsupported relocation
				}
			}
			val-=AsmCurrentposition[AsmCurrentsegment];
			assert(flags);
			switch(self->type){
				default: assert(false); exit(2);
				case eAsmConstraittype_Nearref:  return flags && (val<120) && (val>-120);
				case eAsmConstraittype_Shortref: return flags && (val<32760) && (val>-32760);
				case eAsmConstraittype_Relref:   return flags && (val<self->param) && (val>-self->param);
			}
		}	break;
		case eAsmConstraittype_Segment: {
			tList relocations[1] = {};
			tGTargetPtrdifft val = AsmEvaulateexpression(
				relocations,
				AsmBoundparameters[self->operandnr]
			);
			for(tListnode* i=relocations->first;i;i=i->next){tAsmRelocationentry*j=i->item;
				switch(j->kind){
					case eAsmRelocationentrykind_Segmentstart:
						if(j->segment!=self->param)
							return false; // no fit here
						break;
					default:
						assert(false); // Unsupported relocation
				}
			}
			return true;
		}	break;
		default: assert(false); break;
	}
	ErfError_String2("ASM:[E] mtAsmConstrait_Verify: No value returned!\n");
	return false;
}
bool mtAsmConstrait_VerifyInverted(tAsmConstrait* self){
	return !mtAsmConstrait_Verify(self);
}
// -- Functions --

error_t AsmArgpParser(int optiontag,char* optionvalue,struct argp_state *state){
	switch(optiontag){
		case eAsmArgpOptiontags_Confdir:
			if(AsmConfigdir){
				return ARGP_ERR_UNKNOWN;
			}else{
				AsmConfigdir=mtString_Clone(optionvalue);
			};
			break;
		case eAsmArgpOptiontags_Architecture:
			if(AsmArchitecturename){
				return ARGP_ERR_UNKNOWN;
			}else{
				AsmArchitecturename=mtString_Clone(optionvalue);
			};
			break;
		case eAsmArgpOptiontags_Outputfile:
			if(AsmTargetfilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				AsmTargetfilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_ARG:
			if(AsmSourcefilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				AsmSourcefilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_END:
			if(!AsmSourcefilename){
				argp_error(state,"No source file specified");
			};
			// Produce target filename if absent
			if(!AsmTargetfilename){
				AsmTargetfilename = mtString_Clone(AsmSourcefilename);
				if(
					  (mtString_FindcharLast(AsmTargetfilename,'.')!=nullptr)
					&&(mtString_FindcharLast(AsmTargetfilename,'.')!=AsmSourcefilename)
				){
					mtString_FindcharLast(AsmTargetfilename,'.')[0]=0;
				};
				mtString_Append(&AsmTargetfilename,".obj");
			};
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
};
FILE* AsmFileerrorfilter(FILE* self){
	if(self)return self;
	printf("ASM:[E] AsmFileerrorfilter: Error %i·%s\n",errno,strerror(errno));
	return nullptr;
};
bool AsmLabelfinderclojure(char* name, tAsmLabel* label){
	if(strcmp(name,label->name)==0)return true;
	return false;
};
void dumplabel(void* l){
	tAsmLabel* label=l;
	printf("ASM:[T] Defined label %20s at %i:%i\n",label->name,label->segment,label->offset);
}
tGTargetNearpointer AsmGetlabelsegment(char* name){
	if(!
		mtList_Find_Clojure(
			AsmLabels,
			(bool(*)(void*,void*))&AsmLabelfinderclojure,
			name
		)
	){
		printf("ASM:[E] AsmGetlabelsegment: Undefined label \"%s\"\n",name);
		ErfError();
		return 0;
	};
	return (
		(tAsmLabel*)
		(
			mtList_Find_Clojure(
				AsmLabels,
				(bool(*)(void*,void*))&AsmLabelfinderclojure,
				name
			)
		)
	)->segment;
};
tGTargetNearpointer AsmGetlabelvalue(char* name){
	if(!
		mtList_Find_Clojure(
			AsmLabels,
			(bool(*)(void*,void*))&AsmLabelfinderclojure,
			name
		)
	){
		mtList_Foreach(AsmLabels,dumplabel);
		printf("ASM:[E] AsmGetlabelvalue: Undefined label \"%s\"\n",name);
		ErfError();
		return 0;
	};
	return (
		(tAsmLabel*)
		(
			mtList_Find_Clojure(
				AsmLabels,
				(bool(*)(void*,void*))&AsmLabelfinderclojure,
				name
			)
		)
	)->offset;
};
void AsmEmitByte(FILE* dst, uint8_t byte){
	tAsmBinarytoken tok;
	memset(&tok, 0, sizeof(tok));
	tok.size = eAsmBinarytokensize_Lobyte;
	tok.type = eAsmBinarytokentype_Raw;
	tok.label = nullptr;
	tok.arg = 0;
	tok.disp = byte;
	mtAsmBinarytoken_Emit(&tok, dst);
	mtAsmBinarytoken_Subdryemit(&tok);
};
void AsmDryemitByte(uint8_t byte){
	tAsmBinarytoken tok;
	memset(&tok, 0, sizeof(tok));
	tok.size = eAsmBinarytokensize_Lobyte;
	tok.type = eAsmBinarytokentype_Raw;
	tok.label = nullptr;
	tok.arg = 0;
	tok.disp = byte;
	mtAsmBinarytoken_Dryemit(&tok);
	mtAsmBinarytoken_Subdryemit(&tok);
};
// -- Instruction defining --

void AsmReadinstructiondefinition(FILE* src){
	//
#ifdef qvGTrace
	printf("ASM:[T] AsmReadinstructiondefinition: Entered \n");
#endif
	tAsmToken* token = mtAsmToken_Get(src);
	if(token){
		if(token->type==eAsmTokentype_Identifier){
			if(strcmp(token->string,"opcode")==0){
#ifdef qvGTrace
				printf("ASM:[T] AsmReadinstructiondefinition: Opcode definition \n");
#endif
				mtAsmToken_Destroy(token);
				// Get opcode
				token = mtAsmToken_Get(src);
				if(token->type!=eAsmTokentype_Identifier){
					printf("ASM:[F] AsmReadinstructiondefinition: Unrecoginzed token ");
					printf("%s ",mtAsmToken_ToString(token));
					printf(" when fetching instruction definition's opcode\n");
					ErfFatal();
				};
#ifdef qvGTrace
				printf("ASM:[T] AsmReadinstructiondefinition: Opcode %s\n",
					mtAsmToken_ToString(token)
				);
#endif
				// Allocate instruction
				tAsmInstructiondefinition* instruction =
					calloc(1,sizeof(tAsmInstructiondefinition));
				instruction->opcode = mtString_Clone(token->string);
				instruction->operands = mtList_Create();
				instruction->expansion = mtList_Create();
				//mtList_Append(instruction->operands,mtAsmToken_Clone(token));
				mtAsmToken_Destroy(token);
				// Read arguments
				while(
					  ((token=mtAsmToken_Get(src))->type!=eAsmTokentype_Charater)
					||(token->ch!=':')
				){
#ifdef qvGTrace
					printf("ASM:[T] AsmReadinstructiondefinition: Argument %s\n",
						mtAsmToken_ToString(token)
					);
#endif
					mtList_Append(instruction->operands,mtAsmToken_Clone(token));
				};
				mtAsmToken_Destroy(token);
				mtList_Append(instruction->operands,
					mtAsmToken_CreateCharaterd(
						eAsmTokentype_Newline,
						'\n'
					)
				);
				// Read expansion
				tAsmBinarytoken* bintoken = nullptr;
				while(
					(bintoken=mtAsmBinarytoken_Get(src))->type!=eAsmBinarytokentype_Newline
				){
#ifdef qvGTrace
					printf("ASM:[T] AsmReadinstructiondefinition: "
					       "Expansion token %s \n",
						mtAsmBinarytoken_ToString(bintoken)
					);
#endif
					mtList_Append(instruction->expansion,mtAsmBinarytoken_Clone(bintoken));
				};
				// Register instruction
				mtList_Append(&AsmInstructionsdefined,instruction);
			}else if(strcmp(token->string,"constraitedopcode")==0){
#ifdef qvGTrace
				printf("ASM:[T] AsmReadinstructiondefinition: Constraited opcode definition \n");
#endif
				mtAsmToken_Destroy(token);
				tList /* <tAsmConstrait*> */ * constraits = mtList_Create();
				// Read constraits
				while(
					  ((token=mtAsmToken_Get(src))->type!=eAsmTokentype_Charater)
					||(token->ch!=':')
				){
#ifdef qvGTrace
					printf("ASM:[T] AsmReadinstructiondefinition: Constrait %s\n",
						mtAsmToken_ToString(token)
					);
#endif
					// Handle constrait
					switch(token->type){
						case eAsmTokentype_Identifier:
							if(strcmp(token->string,"none")==0){
								// ignore
							}else if(strcmp(token->string,"shortref")==0){
								mtAsmToken_Destroy(token);
								token=mtAsmToken_Get(src);
								assert(token->type==eAsmTokentype_Number);
								mtList_Append(constraits, 
									mtAsmConstrait_CreateExpr_Generic(
										eAsmConstraittype_Shortref,
										token->number
									)
								);
								//assert(false); // todo
							}else if(strcmp(token->string,"nearref")==0){
								mtAsmToken_Destroy(token);
								token=mtAsmToken_Get(src);
								assert(token->type==eAsmTokentype_Number);
								mtList_Append(constraits, 
									mtAsmConstrait_CreateExpr_Parametered(
										eAsmConstraittype_Relref,
										32000, // retargeting hook: nearref range
										token->number
									)
								);
								//assert(false); // todo
							}else if(strcmp(token->string,"segment")==0){
								mtAsmToken_Destroy(token);
								// Read segment nr
								token=mtAsmToken_Get(src);
								int segnum;
								if(token->type==eAsmTokentype_Identifier){
									if(strcmp(token->string,".null"     )==0) {segnum= 0; goto doneseg; }
									if(strcmp(token->string,".header"   )==0) {segnum= 1; goto doneseg; }
									if(strcmp(token->string,".text"     )==0) {segnum= 2; goto doneseg; }
									if(strcmp(token->string,".bss"      )==0) {segnum= 3; goto doneseg; }
									if(strcmp(token->string,".zp"       )==0) {segnum= 4; goto doneseg; }
									if(strcmp(token->string,".lowcode"  )==0) {segnum= 5; goto doneseg; }
									if(strcmp(token->string,".lowdata"  )==0) {segnum= 6; goto doneseg; }
									if(strcmp(token->string,".code"     )==0) {segnum= 7; goto doneseg; }
									if(strcmp(token->string,".data"     )==0) {segnum= 8; goto doneseg; }
									if(strcmp(token->string,".rodata"   )==0) {segnum= 9; goto doneseg; }
									if(strcmp(token->string,".udata"    )==0) {segnum=10; goto doneseg; }
									if(strcmp(token->string,".farcode"  )==0) {segnum=11; goto doneseg; }
									if(strcmp(token->string,".fardata"  )==0) {segnum=12; goto doneseg; }
									if(strcmp(token->string,".farrodata")==0) {segnum=13; goto doneseg; }
									if(strcmp(token->string,".farbss"   )==0) {segnum=14; goto doneseg; }
									if(strcmp(token->string,".lostack"  )==0) {segnum=15; goto doneseg; }
									if(strcmp(token->string,".data_al"  )==0) {segnum=16; goto doneseg; }
									if(strcmp(token->string,".constr"   )==0) {segnum=20; goto doneseg; }
									if(strcmp(token->string,".destr"    )==0) {segnum=21; goto doneseg; }
									doneseg:
								}else{
									if(token->type!=eAsmTokentype_Number)
										ErfError_String2(mtString_Format(
											"ASM:[E] AsmReadinstructiondefinition: constraited opcode: constraits: "              "\n"
											"ASM:[E]                               constrait `segment`: "     "\n"
											"ASM:[E]                               unrecognized token <%s>"   "\n"
											,
											mtAsmToken_ToString(token)
										));
									segnum = token->number;
								}
								mtAsmToken_Destroy(token);
								// Read expr nr
								token=mtAsmToken_Get(src);
								assert(token->type==eAsmTokentype_Number);
								mtList_Append(constraits, 
									mtAsmConstrait_CreateExpr_Parametered(
										eAsmConstraittype_Segment,
										segnum,
										token->number
									)
								);
								//assert(false); // todo
							}else{
								ErfError_String2(mtString_Format(
									"ASM:[E] AsmReadinstructiondefinition: "
									"Unrecognized constrait \"%s\"\n",
									token->string
								));
								assert(false);
								// We probably are now desynchronized with 
								// archdef by now so continuing might not
								// make sense
							}
							break;
						default:
							ErfError_String2(mtString_Format(
								"ASM:[E] AsmReadinstructiondefinition: "
								"Unrecognized constrait <%s>\n",
								mtAsmToken_ToString(token)
							));
							assert(false);
							break;
					}
				};
				mtAsmToken_Destroy(token); // the colon
				// Get opcode
				token = mtAsmToken_Get(src);
				if(token->type!=eAsmTokentype_Identifier){
					printf("ASM:[F] AsmReadinstructiondefinition: "
					       "Unrecoginzed token <%s> "
					       " when fetching instruction definition's opcode\n",
					       mtAsmToken_ToString(token)
					);
					ErfFatal();
				};
#ifdef qvGTrace
				printf("ASM:[T] AsmReadinstructiondefinition: Opcode %s\n",
					mtAsmToken_ToString(token)
				);
#endif
				// Allocate instruction
				tAsmInstructiondefinition* instruction =
					calloc(1,sizeof(tAsmInstructiondefinition));
				instruction->opcode = mtString_Clone(token->string);
				instruction->operands = mtList_Create();
				instruction->expansion = mtList_Create();
				instruction->constraits = constraits;
				//mtList_Append(instruction->operands,mtAsmToken_Clone(token));
				mtAsmToken_Destroy(token);
				// Read arguments
				while(
					  ((token=mtAsmToken_Get(src))->type!=eAsmTokentype_Charater)
					||(token->ch!=':')
				){
#ifdef qvGTrace
					printf("ASM:[T] AsmReadinstructiondefinition: Argument %s\n",
						mtAsmToken_ToString(token)
					);
#endif
					mtList_Append(instruction->operands,mtAsmToken_Clone(token));
				};
				mtAsmToken_Destroy(token); // the colon
				mtList_Append(instruction->operands,
					mtAsmToken_CreateCharaterd(
						eAsmTokentype_Newline,
						'\n'
					)
				);
				// Read expansion
				tAsmBinarytoken* bintoken = nullptr;
				while(
					(bintoken=mtAsmBinarytoken_Get(src))->type!=eAsmBinarytokentype_Newline
				){
#ifdef qvGTrace
					printf("ASM:[T] AsmReadinstructiondefinition: "
					       "Expansion token %s \n",
						mtAsmBinarytoken_ToString(bintoken)
					);
#endif
					mtList_Append(instruction->expansion,mtAsmBinarytoken_Clone(bintoken));
				};
				// Register instruction
				mtList_Append(&AsmInstructionsdefined,instruction);
			}else{
				printf("ASM:[F] AsmReadinstructiondefinition: Unrecoginzed identifier ");
				printf("<%s>",mtAsmToken_ToString(token));
				printf(" while fetching instruction definition, excepted <opcode>\n");
				ErfFatal();
			};
		}else if(token->type==eAsmTokentype_Newline){
			return;
		}else{
			printf("ASM:[F] AsmReadinstructiondefinition: Unrecoginzed token"
			       " when fetching \n");
			printf("ASM:[F]                               instruction definition class - \n");
			printf("ASM:[F]                               got ");
			printf("<%s>",mtAsmToken_ToString(token));
			printf(" while excepted an identifier \n");
			ErfFatal();
		};
	}else{
		printf("ASM:[F] AsmReadinstructiondefinition: Unable to fetch token from archdef \n");
		ErfError();
		return;
	};
};
void AsmReadinstructiondefinitions(FILE* src){
	//
#ifdef qvGTrace
	printf("ASM:[T] AsmReadinstructiondefinitions: Entered \n");
#endif
	while(fpeekc(src)!=EOF)AsmReadinstructiondefinition(src);
};

// -- Main loop --

bool AsmInstructionfinderclojure(
	struct {
		char* opcode;
		tList /* <tAsmToken> */ * args;
	} * clojureargs,
	tAsmInstructiondefinition * item
){
#ifdef qvGDeeptrace
	assert((void*)clojureargs->opcode>=(void*)0x100);
	assert((void*)item->opcode>=(void*)0x100);
	printf(
		"ASM:[T] AsmInstructionfinderclojure[%s %p](%s %p %p %p): Entered \n",
		clojureargs?clojureargs->opcode?:"":"",
		clojureargs?clojureargs->args:0,
		item?item->opcode?:"":"",
		item?item->operands:0,
		item?item->expansion:0,
		item
	);
#endif
	// Check opcode
#ifdef qvGDeeptrace
	printf(
			strcmp(item->opcode,clojureargs->opcode)==0
		?	"ASM:[T] AsmInstructionfinderclojure: Opcode matches\n"
		:	"ASM:[T] AsmInstructionfinderclojure: Wrong opcode\n"
	);
#endif
	if(strcmp(item->opcode,clojureargs->opcode)!=0)return false;
	// Try to match arguments
	for(
		tListnode /* <tAsmToken> */ 
			*i=item->operands->first,
			*j=clojureargs->args->first;
		i&&j;
		i=i->next,j=j->next
	){
#ifdef qvGDeeptrace
		printf(
			"ASM:[T] AsmInstructionfinderclojure: Iteration on tokens <%s> and <%s> \n",
			mtAsmToken_ToString(i->item),mtAsmToken_ToString(j->item)
		);
#endif
		tAsmToken* sourcetok = i->item;
		tAsmToken* desttok = j->item;
		if(sourcetok->type==eAsmTokentype_Argument){
			// Argument that would need to be matched into the array
			// of bound args
			int argindex = ((tAsmToken*)i->item)->ch-'0';
			if(argindex<0 || argindex >= 10){
				printf("ASM:[F] AsmInstructionfinderclojure: Invalid instruction definition: \n");
				printf("ASM:[F]                              argindex out of range \n");
				printf("ASM:[F]                              got token ");
				printf("<%s>",mtAsmToken_ToString(i->item));
				printf(", excepted <#1> \n");
				ErfFatal();
			};
			assert(argindex>=0);
			assert(argindex<10);
			i=i->next;
			assert(i);
			tAsmToken* matchtoken = i->item;
			assert(matchtoken);
			tList /* <tAsmToken* owned> */ * list = mtList_Create();
			assert(list);
			AsmBoundparameters[argindex]=list;
#ifdef qvGDeeptrace
			printf("ASM:[T] AsmInstructionfinderclojure: . Fetching argument %i until token <%s> \n",
				argindex,
				mtAsmToken_ToString(i->item)
			);
#endif
			while(!mtAsmToken_Equals(matchtoken,j->item)){
#ifdef qvGDeeptrace
				printf("ASM:[T] AsmInstructionfinderclojure: | Token <%s> against <%s> \n",
					mtAsmToken_ToString(matchtoken),
					mtAsmToken_ToString(j->item)
				);
#endif
				mtList_Append(list,mtAsmToken_Clone(j->item));
				j=j->next;
				assert(j);
			};
#ifdef qvGDeeptrace
			printf("ASM:[T] AsmInstructionfinderclojure: ' Done \n");
#endif
		}else{
			// Ordinary token
			if(!mtAsmToken_Equals(sourcetok,desttok))return false;
		};
	};
	// Validate constraits
	if(
		!AsmInhibitconstraiting &&
		item->constraits &&
		mtList_Find_Lambda(item->constraits,
			(bool(*)(void*))&mtAsmConstrait_VerifyInverted
		)
	){
		AsmInstructionfinderclojure_Constraitfailee = item;
		return false; // constrait failed!
	}
	// Found it.
#ifdef qvGDeeptrace
	printf("ASM:[T] AsmInstructionfinderclojure: Found instrdef, returning \n");
#endif
	return true;
};
void AsmCreatelabel_ValueSegment(char* /* borrows */ name, int value, unsigned char segment){
	mtList_Append(
		AsmLabels,
		mtAsmLabel_Clone(
			&(tAsmLabel){
				.name = mtString_Clone(name),
				.offset = value,
				.segment= segment,
			}
		)
	);
};
void AsmCreatesublabel_ValueSegment(char* /* borrows */ name, int value, unsigned char segment){
	for(tListnode *i = AsmSublabels->first;i;i=i->next){tAsmLabel* j = i->item;
		if(strcmp(j->name,name)==0){
			fprintf(stderr,"ASM:[E] Adding duplicate label \"%s\"\n",name);
			fprintf(stdout,"ASM:[E] Adding duplicate label \"%s\"\n",name);
			ErfError();
		};
	};
	mtList_Append(
		AsmSublabels,
		mtAsmLabel_Clone(
			&(tAsmLabel){
				.name = mtString_Clone(name),
				.offset = value,
				.segment= segment,
			}
		)
	);
};
void AsmCreatelabel(char* /* borrows */ name){
	for(tListnode *i = AsmLabels->first;i;i=i->next){tAsmLabel* j = i->item;
		if(strcmp(j->name,name)==0){
			fprintf(stderr,"ASM:[E] Adding duplicate label \"%s\"\n",name);
			fprintf(stdout,"ASM:[E] Adding duplicate label \"%s\"\n",name);
			ErfError();
		};
	};
	mtList_Append(
		AsmLabels,
		mtAsmLabel_Clone(
			&(tAsmLabel){
				.name = mtString_Clone(name),
				.offset = AsmCurrentposition[AsmCurrentsegment],
				.segment= AsmCurrentsegment,
			}
		)
	);
};
void AsmCreatesublabel(char* /* borrows */ name){
	for(tListnode *i = AsmSublabels->first;i;i=i->next){tAsmLabel* j = i->item;
		if(strcmp(j->name,name)==0){
			fprintf(stderr,"ASM:[E] Adding duplicate label \"%s\"\n",name);
			fprintf(stdout,"ASM:[E] Adding duplicate label \"%s\"\n",name);
			ErfError();
		};
	};
	mtList_Append(
		AsmSublabels,
		mtAsmLabel_Clone(
			&(tAsmLabel){
				.name = mtString_Clone(name),
				.offset = AsmCurrentdetuneposition[AsmCurrentsegment],
				.segment= AsmCurrentsegment,
			}
		)
	);
};

tAsmInstructiondefinition * AsmFindinstructiondefinition(char* opcode,tList /* tAsmToken */ * arguments){
	AsmInstructionfinderclojure_Constraitfailee = nullptr;
	return mtList_Find_Clojure(
		&AsmInstructionsdefined,
		(bool(*)(void*,void*))AsmInstructionfinderclojure,
		&(
			struct {
				char* opcode;
				tList /* <tAsmToken> */ * args;
			}
		){
			.opcode=opcode,
			.args=arguments,
		}
	);
};
void AsmCommonpassline(FILE* dst){
	//assert(dst);
#ifdef qvGTrace
	printf("ASM:[T] %s: Entered at position plain:%i detune:%i\n",
		AsmCurrentpass==eAsmCurrentpass_First ?"AsmFirstpass":
		AsmCurrentpass==eAsmCurrentpass_Middle?"AsmMiddlepass":
		AsmCurrentpass==eAsmCurrentpass_Second?"AsmSecondpass":
		"AsmCommonpassline",
		AsmCurrentposition[AsmCurrentsegment],AsmCurrentdetuneposition[AsmCurrentsegment]
	);
#endif
	ErfUpdate_String(mtString_Format("Commonpass at line %u",AsmCurrentsourceline));
	// Clear bound parameters
	for(int i=0;i<10;i++)AsmBoundparameters[i]=0;
	// Read instruction
	tList /* <tAsmToken> */ * arguments = mtList_Create();
	tAsmToken* tok = mtAsmToken_Get(getcurrentfile());
	if(!tok)return;
	switch(tok->type){
		case eAsmTokentype_Newline:
			return;
			break;
		case eAsmTokentype_Label:
			if(!AsmIsLabelLocal(tok->string))
				szAsmLastlabel = mtString_Clone(tok->string);
			switch(AsmCurrentpass){ default: assert(false); break;
				case eAsmCurrentpass_First:
					AsmCreatelabel(AsmLocallabelfilter(tok->string));
					break;
				case eAsmCurrentpass_Middle:
					AsmCreatesublabel(AsmLocallabelfilter(tok->string));
					break;
				case eAsmCurrentpass_Second:
					for(tListnode *i = AsmLabels->first;i;i=i->next){tAsmLabel* j = i->item;
						if(strcmp(j->name,tok->string)==0){
							if(j->offset!=AsmCurrentposition[AsmCurrentsegment]){
								fprintf(stderr,"ASM:[E] Label \"%s\" desynchronized %i->%i\n",
									tok->string,j->offset,AsmCurrentposition[AsmCurrentsegment]);
								ErfError();
							}
						};
					};
					break;
			}
			mtAsmToken_Destroy(tok);
			break;
		case eAsmTokentype_Identifier:
			// by now tok is known identifier but we also need to check for directives
			if(strcmp(tok->string,".times")==0){
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Number);
				iAsmRepeattimes = tok->number;
				mtAsmToken_Destroy(tok);
				AsmCommonpassline(dst);
				iAsmRepeattimes = 1;
				break;
				return;
			}else if(strcmp(tok->string,".include")==0){
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_String);
				char* fname = mtString_Clone(tok->string);
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Newline);
				mtAsmToken_Destroy(tok);
				FILE* stream = fopen(fname,"r");
				if(!stream){
					printf(
						"AsmSecondpassline: Directive .include: Unable to open file \"%s\": Error %i•%s\n",
						fname, errno, strerror(errno)
					);
				};
				mtList_Prepend(&AsmIncludelist,stream);
				mtString_Destroy(fname);
				return;
			}else if(strcmp(tok->string,".gl_include")==0){
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_String);
				char* fname = mtString_Clone(tok->string);
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Newline);
				mtAsmToken_Destroy(tok);
				FILE* stream = fopen(mtString_Format("/etc/xcc/%s/%s",AsmArchitecturename,fname),"r");
				if(!stream){
					printf(
						"AsmSecondpassline: Directive .gl_include: Unable to open file \"%s\": Error %i•%s\n",
						fname, errno, strerror(errno)
					);
				};
				mtList_Prepend(&AsmIncludelist,stream);
				mtString_Destroy(fname);
				return;
			}else if(strcmp(tok->string,".segment")==0){
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Number);
				AsmCurrentsegment = tok->number;
				return;
			}else if(strcmp(tok->string,".global")==0){
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				if(AsmCurrentpass==eAsmCurrentpass_Second){
					if(tok->type==eAsmTokentype_Newline){
						fputc((uint8_t)AsmCurrentsegment,dst);
						fputc((uint8_t)eAsmBinarytokensize_Label,dst);
						for(char* i=szAsmLastlabel;*i;i++) fputc(*i,dst);
						fputc((uint8_t)0,dst);
						mtAsmToken_Destroy(tok);
					}else if(tok->type==eAsmTokentype_Identifier){
						fputc((uint8_t)AsmCurrentsegment,dst);
						fputc((uint8_t)eAsmBinarytokensize_Label,dst);
						for(char* i=tok->string;*i;i++) fputc(*i,dst);
						fputc((uint8_t)0,dst);
						mtAsmToken_Destroy(tok);
					}else{
						printf("ASM:[E] Second pass: `.global`: Unexcepted token \"%s\"\n",mtAsmToken_ToString(tok));
						ErfError();
					};
				}else{
					mtAsmToken_Destroy(tok);
				};
				return;
			}else if(strcmp(tok->string,".define")==0){
				mtAsmToken_Destroy(tok);
				// Define an identifier as a constant
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Identifier);
				char* identifier = mtString_Clone(tok->string);
				mtAsmToken_Destroy(tok);
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Number);
				if(AsmCurrentpass==eAsmCurrentpass_First){
					AsmCreatelabel_ValueSegment(identifier,tok->number,0);
				}else if(AsmCurrentpass==eAsmCurrentpass_Middle){
					AsmCreatesublabel_ValueSegment(identifier,tok->number,0);
				}else{
					// nothing
				}
				mtAsmToken_Destroy(tok);
				return;
			}else if(strcmp(tok->string,".sz")==0){
				mtAsmToken_Destroy(tok);
				// Emit a byte zeroterminated string
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_String);
				char* str = mtString_Clone(tok->string);
				mtAsmToken_Destroy(tok);
				if(AsmCurrentpass==eAsmCurrentpass_Second){
					for(char* i = str; *i; i++)
						AsmEmitByte(dst, *i);
					AsmEmitByte(dst, 0);
				}else{
					for(char* i = str; *i; i++)
						AsmDryemitByte(*i);
					AsmDryemitByte(0);
				}
				return;
			}else if(strcmp(tok->string,".extern")==0){
				mtAsmToken_Destroy(tok);
				// This time ignore - externed labels are already registered,
				// we just need to emit relocations for them
				tok = mtAsmToken_Get(getcurrentfile());
				if(AsmCurrentpass==eAsmCurrentpass_First){
					assert(tok->type==eAsmTokentype_Identifier);
					mtList_Append(&AsmExternallabels,mtString_Clone(tok->string));
				}
				mtAsmToken_Destroy(tok);
				return;
			}else{
				char* opcode = tok->string;
				// Fetch arguments
				for(
					tok = mtAsmToken_Get(getcurrentfile());
					tok->type!=eAsmTokentype_Newline;
					tok=mtAsmToken_Get(getcurrentfile())
				)
					mtList_Append(arguments,tok);
				mtList_Append(arguments,tok); // The newline as well
				// Find expansion
				if(AsmCurrentpass==eAsmCurrentpass_Middle) AsmInhibitconstraiting=true;
				tAsmInstructiondefinition * instrdef = 
					AsmFindinstructiondefinition(opcode,arguments);
				/* if(AsmCurrentpass==eAsmCurrentpass_Middle) */ AsmInhibitconstraiting=false;
				//if(AsmInstructionfinderclojure_Constraitfailee) {
				//	printf("ASM:[D] trace %i: Instruction failed constraits! - <",AsmCurrentpass);
				//	printf("%s | ", AsmInstructionfinderclojure_Constraitfailee->opcode);
				//	for(tListnode* i = AsmInstructionfinderclojure_Constraitfailee->operands->first;i;i=i->next){tAsmToken* j = i->item;
				//		printf("%s ",mtAsmToken_ToString(i->item));
				//	};
				//	printf("> \n");
				//}
				// Report undefined instructions
				if(!instrdef){
					printf("ASM:[E] AsmCommonparseline: Undefined instruction < ");
					printf("%s | ", opcode);
					for(tListnode* i = arguments->first;i;i=i->next){tAsmToken* j = i->item;
						printf("%s ",mtAsmToken_ToString(i->item));
					};
					printf("> \n");
					if(AsmInstructionfinderclojure_Constraitfailee){
						printf("ASM:[E]                     There was an otherwise fitting instruction that failed its constraits -\n"
						       "ASM:[E]                     < ");
						printf("%s | ", AsmInstructionfinderclojure_Constraitfailee->opcode);
						for(tListnode* i = AsmInstructionfinderclojure_Constraitfailee->operands->first;i;i=i->next){tAsmToken* j = i->item;
							printf("%s ",mtAsmToken_ToString(i->item));
						};
						printf("> \n");
					}
					ErfError();
					return;
				};
				if(AsmCurrentpass==eAsmCurrentpass_Middle){
					// Find subexpansion
					tAsmInstructiondefinition * subinstrdef = 
						AsmFindinstructiondefinition(opcode,arguments);
					// Report undefined subinstruction
					if(!subinstrdef){
						printf("ASM:[E] AsmCommonparseline: Undefined subinstruction < ");
						printf("%s | ", opcode);
						for(tListnode* i = arguments->first;i;i=i->next){tAsmToken* j = i->item;
							printf("%s ",mtAsmToken_ToString(i->item));
						};
						printf("> \n");
						ErfError();
					};
					// Emit subexpansion
					for(int i=1;i<=iAsmRepeattimes;i++){ // .times directive
						if(subinstrdef->expansion->first!=nullptr){
							for(tListnode /* <tAsmBinarytoken> */ * i = subinstrdef->expansion->first;i;i=i->next){
								assert(i);
								assert(i->item);
								tAsmBinarytoken* tok = i->item;
#ifdef qvGTrace
								printf("ASM:[T] AsmSecondpassline:  Emitting expansion for token %s \n",mtAsmBinarytoken_ToString(tok));
#endif
								mtAsmBinarytoken_Subdryemit(tok);
							};
						};
					}
				}
				// Emit expansion
				for(int i=1;i<=iAsmRepeattimes;i++){ // .times directive
					//AsmCurrentexternalposition=AsmCurrentposition;
					if(instrdef->expansion->first!=nullptr){
						for(tListnode /* <tAsmBinarytoken> */ * i = instrdef->expansion->first;i;i=i->next){
							assert(i);
							assert(i->item);
							tAsmBinarytoken* tok = i->item;
#ifdef qvGTrace
							printf("ASM:[T] AsmSecondpassline:  Emitting expansion for token %s \n",mtAsmBinarytoken_ToString(tok));
#endif
							switch(AsmCurrentpass){ default: assert(false); break;
								case eAsmCurrentpass_First:
									mtAsmBinarytoken_Dryemit(tok);
									break;
								case eAsmCurrentpass_Middle:
									mtAsmBinarytoken_Dryemit(tok);
									break;
								case eAsmCurrentpass_Second:
									mtAsmBinarytoken_Emit(tok,dst);
									break;
							}
						};
					};
				};
				// Destroy bound arguments
				for(int i=0;i<10;i++)if(AsmBoundparameters[i]){
					mtList_Foreach(AsmBoundparameters[i],(void(*)(void*))mtAsmToken_Destroy);
					mtList_Destroy(AsmBoundparameters[i]);
				};
				// Finalize
				iAsmRepeattimes = 1;
			};
			break;
		default:
			printf("ASM:[F] AsmCommonparseline: Unrecognized token while parsing instruction opcode \n");
			printf("ASM:[F]                     got <%s>, excepted identifier \n",
				mtAsmToken_ToString(tok)
			);
			ErfFatal();
			break;
	};
};
// -- Launcher aka init code --
void LnFailedassertionhandler(int signum){
	fprintf(stderr,"ASM:[F] Failed assertion catched! \n");
	ErfFatal();
};
void LnNullpointerhandler(int signum){
	fprintf(stderr,"ASM:[F] Segfault catched! \n");
	ErfFatal();
};
int main(int argc, char** argv){
	signal(SIGSEGV,LnNullpointerhandler);
	signal(SIGABRT,LnFailedassertionhandler);
	// Parse arguments
	argp_parse(
		&AsmArgpParserstruct,
		argc,
		argv,
		0,
		0,
		0
	);
	// Open archdef
	if(!AsmArchitecturename){
		printf("ASM:[F] No architecture provided!"); 
		ErfError();
	}else{
		char* archdeffname = 
			mtString_Join(
				AsmConfigdir?:"/etc/xcc",
				mtString_Join(
					"/",
					mtString_Join(
						AsmArchitecturename,
						"/archdef.asm"
					)
				)
			);
		FILE* archdeffile = fopen(archdeffname,"r");
		if(!archdeffile){
			printf(
				"ASM:[F] Unable to open archdef for arch \"%s\" (path \"%s\"): Error %i·%s\n",
				AsmArchitecturename,
				archdeffname,
				errno,
				strerror(errno)
			);
			exit(1);
		};
		// Read archdef
		AsmReadinstructiondefinitions(archdeffile);
		// Dump archdep
#ifdef qvGTrace
		if(1){
			printf("ASM:[D] . Dumping instructions defined \n");
			fflush(stdout);
			for(tListnode * i = AsmInstructionsdefined.first;i;i=i->next){
				tAsmInstructiondefinition * j = i->item;
				printf("ASM:[T] | Opcode %s : ",j->opcode);
				fflush(stdout);
				for(tListnode * k = j->operands->first;k;k=k->next){
					printf("%s ",mtAsmToken_ToString(k->item));
				};
				fflush(stdout);
				printf("|");
				for(tListnode * k = j->expansion->first;k;k=k->next){
					printf("%s ",mtAsmBinarytoken_ToString(k->item));
				};
				printf("\n");
			};
			printf("ASM:[D] ' \n");
		}
#endif
	};
	// Open source/dest files
	AsmSourcestream = fopen(AsmSourcefilename,"r");
	if(!AsmSourcestream){
		printf(
			"ASM:[F] Unable to open source file \"%s\": Error %i·%s\n",
			AsmSourcefilename,
			errno,
			strerror(errno)
		);
		ErfError();
		exit(0);
	};
	FILE* dstfile = fopen(AsmTargetfilename,"wb");
	if(!dstfile){
		printf(
			"ASM:[F] Unable to open destination file \"%s\": Error %i·%s\n",
			AsmTargetfilename,
			errno,
			strerror(errno)
		);
	};
	AsmLabels = mtList_Create();
	AsmSublabels = mtList_Create();
	// First pass
	fprintf(stderr,"ASM:[M] Assembling \"%s\": First pass\n",AsmSourcefilename);
	AsmCurrentpass = eAsmCurrentpass_First;
	iAsmRepeattimes = 1;
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
	memset(AsmCurrentdetuneposition,0,sizeof(AsmCurrentdetuneposition));
	AsmCurrentsourceline = 1;
#ifdef qvGDebug
	printf("ASM:[D] First pass \n");
#endif
	ErfEnter_String("Firstpass");
	while(fpeekc(AsmSourcestream)!=EOF)AsmCommonpassline(nullptr);//AsmFirstpassline();
	ErfLeave();
	// Interpass seek
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
	memset(AsmCurrentdetuneposition,0,sizeof(AsmCurrentdetuneposition));
	if(fseek(AsmSourcestream,0,SEEK_SET)){
		printf(
			"ASM:[F] Unable to rewind source file to start: Error %i·%s\n",
			errno,
			strerror(errno)
		);
	};
	fprintf(stderr,"ASM:[M] Assembling \"%s\": Intermediate pass\n",AsmSourcefilename);
	// Intermediate pass
	AsmCurrentpass = eAsmCurrentpass_Middle;
	iAsmRepeattimes = 1;
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
	memset(AsmCurrentdetuneposition,0,sizeof(AsmCurrentdetuneposition));
	AsmCurrentsourceline = 1;
#ifdef qvGDebug
	printf("ASM:[D] Intermediate pass \n");
#endif
	ErfEnter_String("Intermediatepass");
	while(fpeekc(AsmSourcestream)!=EOF)AsmCommonpassline(nullptr);//AsmIntermediatepassline();
	ErfLeave();
	// Interpass seek
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
	memset(AsmCurrentposition,0,sizeof(AsmCurrentdetuneposition));
	if(fseek(AsmSourcestream,0,SEEK_SET)){
		printf(
			"ASM:[F] Unable to rewind source file to start: Error %i·%s\n",
			errno,
			strerror(errno)
		);
	};
	// Label-sublabel debug
#ifdef qvGDebug
	for(tListnode *i = AsmLabels->first;i;i=i->next){tAsmLabel* j=i->item;
		printf("ASM:[D] Label    %02i:%6i • %s\n",j->segment,j->offset,j->name);
	}
	for(tListnode *i = AsmSublabels->first;i;i=i->next){tAsmLabel* j=i->item;
		printf("ASM:[D] Sublabel %02i:%6i • %s\n",j->segment,j->offset,j->name);
	}
#endif
	// Sublabel merging
	mtList_Destroy(AsmLabels);
	AsmLabels=AsmSublabels;
	//for(tListnode *i = AsmLabels->first;i;i=i->next){tAsmLabel* j=i->item;
	//	printf("ASM:[D] Label    %6i • %s\n",j->offset,j->name);
	//}
	AsmSublabels = mtList_Create();
	// Emit header
	// Second pass
	fprintf(stderr,"ASM:[M] Assembling \"%s\": Second pass\n",AsmSourcefilename);
	AsmCurrentpass = eAsmCurrentpass_Second;
	iAsmRepeattimes = 1;
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
	memset(AsmCurrentposition,0,sizeof(AsmCurrentdetuneposition));
	AsmCurrentsourceline = 1;
#ifdef qvGDebug
	printf("ASM:[D] Second pass \n");
#endif
	ErfEnter_String("Secondpass");
	while(fpeekc(AsmSourcestream)!=EOF)AsmCommonpassline(dstfile);//AsmSecondpassline(dstfile);
	ErfLeave();
	// Emit relocations
	// Close it all
	fclose(AsmSourcestream);
	fclose(dstfile);
	return 0;
};

