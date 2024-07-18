#include "hyperheader.h"
// ------------ xcc Crossdevelopment suite -- Linking assembler -------------
// ----------------- The one that assembles to object files -----------------

// -- Types --

typedef enum AsmArgpOptiontags {
	// This enumeration should have all the options passable
	eAsmArgpOptiontags_Architecture = 'a',
	eAsmArgpOptiontags_Confdir      = 'c',
	eAsmArgpOptiontags_Outputfile   = 'o',
} eAsmArgpOptiontags;

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

typedef struct { // The instructions defined
	char* /* owned */ opcode;
	tList /* <tToken> */ * operands;
	tList /* <tBinarytoken> */ * expansion;
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
};
typedef struct { // Relocationentry
	enum eAsmRelocationentrykind kind;
	union {
		int segment;
		char* label;
	};
} tAsmRelocationentry;


// -- Preprocessor constants --

#define qiAsmMaxsegments 16

// -- Forward declarations --

error_t AsmArgpParser(int key,char* argumentvalue,struct argp_state *state);
tGTargetNearpointer AsmGetlabelvalue(char* name);
char* mtAsmToken_ToString(tAsmToken* self);
tAsmRelocationentry* mtAsmRelocationentry_CreatePosition();
tAsmRelocationentry* mtAsmRelocationentry_CreateSegmentstart(int segment);
tGTargetNearpointer AsmGetlabelsegment(char* name);
void mtAsmRelocationentry_Emit(tAsmRelocationentry* self, FILE* dstfile);
tAsmRelocationentry* mtAsmRelocationentry_CreateLabel(char* label);

// -- Globals --

tList /* <tAsmInstructiondefinition> */ AsmInstructionsdefined;
tList /* <tIrcToken* owned> */ * AsmBoundparameters[10];
tGTargetNearpointer AsmCurrentposition[qiAsmMaxsegments];
tList /* <tAsmLabel* owned> */ * AsmLabels;
int iAsmRepeattimes;
unsigned AsmCurrentsegment;
char* szAsmLastlabel;

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
	if(label[0]=='_' || label[0]=='.')
		return true;
	return false;
};
char* /* deallocate */ AsmLocallabelfilter(char* /* borrows */ label){
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
void mtAsmBinarytoken_Emit(tAsmBinarytoken* self,FILE* dst){
	tGTargetNearpointer exprval = self->disp;
	tList /* <tAsmRelocationentry> */ * relocations = mtList_Create();
	switch(self->type){ // Evaulate value of the binarytoken 
		case eAsmBinarytokentype_Raw:
			break;
		case eAsmBinarytokentype_Position:
			mtList_Append(relocations, mtAsmRelocationentry_CreatePosition());
			break;
		case eAsmBinarytokentype_Expr: { // Evaulate the expression
			assert(self->arg>=0);
			assert(self->arg<10);
			tList /* tAsmToken */ * expr = AsmBoundparameters[self->arg];
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
								mtList_Append(relocations,
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
							mtList_Append(relocations,
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
								mtList_Append(relocations, // Relocate by 
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
							case 'n': i='\n'; break;
							case 'r': i='\r'; break;
							default:
								i = fgetc(src);
						};
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
					mtString_Append(&str,(char[2]){fgetc(src),0}); 
					// And handle the escape. Currently just put
					// whatever charater is after backslash even
					// if it is string terminator.
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
		 self->type==eAsmTokentype_Identifier ?self->string
		:self->type==eAsmTokentype_Newline    ?"\\n"
		:self->type==eAsmTokentype_Charater   ?(char[2]){self->ch,0}
		:self->type==eAsmTokentype_Argument   ?(char[3]){'#',self->ch,0}
		:self->type==eAsmTokentype_Number     ?mtString_Join("&",mtString_FromInteger(self->number))
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

tAsmRelocationentry* mtAsmRelocationentry_CreatePosition(){assert(false); };
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
#ifdef qvGTrace
	assert((void*)clojureargs->opcode>=(void*)0x100);
	assert((void*)item->opcode>=(void*)0x100);
	if(1)printf(
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
#ifdef qvGTrace
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
#ifdef qvGTrace
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
#ifdef qvGTrace
			printf("ASM:[T] AsmInstructionfinderclojure: . Fetching argument %i until token <%s> \n",
				argindex,
				mtAsmToken_ToString(i->item)
			);
#endif
			while(!mtAsmToken_Equals(matchtoken,j->item)){
#ifdef qvGTrace
				printf("ASM:[T] AsmInstructionfinderclojure: | Token <%s> against <%s> \n",
					mtAsmToken_ToString(matchtoken),
					mtAsmToken_ToString(j->item)
				);
#endif
				mtList_Append(list,mtAsmToken_Clone(j->item));
				j=j->next;
				assert(j);
			};
#ifdef qvGTrace
			printf("ASM:[T] AsmInstructionfinderclojure: ' Done \n");
#endif
		}else{
			// Ordinary token
			if(!mtAsmToken_Equals(sourcetok,desttok))return false;
		};
	};
	// Found it.
#ifdef qvGTrace
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
void AsmCreatelabel(char* /* borrows */ name){
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

tAsmInstructiondefinition * AsmFindinstructiondefinition(char* opcode,tList /* tAsmToken */ * arguments){
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
void AsmSecondpassline(FILE* dst){
	assert(dst);
#ifdef qvGTrace
	printf("ASM:[T] AsmSecondpassline: Entered \n");
#endif
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
				AsmSecondpassline(dst);
				iAsmRepeattimes = 1;
				break;
				return;
			}else
			if(strcmp(tok->string,".include")==0){
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
						"AsmCommonparseline: Directive .include: Unable to open file \"%s\": Error %i•%s\n",
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
				//AsmCreatelabel_ValueSegment(identifier,tok->number,0);
				mtAsmToken_Destroy(tok);
				return;
			}else if(strcmp(tok->string,".extern")==0){
				mtAsmToken_Destroy(tok);
				// This time ignore - externed labels are already registered,
				// we just need to emit relocations for them
				tok = mtAsmToken_Get(getcurrentfile());
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
				tAsmInstructiondefinition * instrdef = 
					AsmFindinstructiondefinition(opcode,arguments);
				// Report undefined instructions
				if(!instrdef){
					printf("ASM:[E] AsmCommonparseline: Undefined instruction < ");
					printf("%s | ", opcode);
					for(tListnode* i = arguments->first;i;i=i->next){tAsmToken* j = i->item;
						printf("%s ",mtAsmToken_ToString(i->item));
					};
					printf("> \n");
					ErfError();
					return;
				};
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
							mtAsmBinarytoken_Emit(tok,dst);
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
void AsmFirstpassline(){
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
			AsmCreatelabel(AsmLocallabelfilter(tok->string));
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
				AsmFirstpassline();
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
						"ASM:[E] AsmFirstpassline: Directive .include: Unable to open file \"%s\": Error %i•%s\n",
						fname, errno, strerror(errno)
					);
				}else{
					mtList_Prepend(&AsmIncludelist,stream);
				};
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
				mtAsmToken_Destroy(tok);
				// Ignore - first pass doesn't bother with globals
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
				AsmCreatelabel_ValueSegment(identifier,tok->number,0);
				mtAsmToken_Destroy(tok);
				return;
			}else if(strcmp(tok->string,".extern")==0){
				mtAsmToken_Destroy(tok);
				// Get identifier and register as external label
				tok = mtAsmToken_Get(getcurrentfile());
				assert(tok->type==eAsmTokentype_Identifier);
				mtList_Append(&AsmExternallabels,mtString_Clone(tok->string));
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
				tAsmInstructiondefinition * instrdef = 
					AsmFindinstructiondefinition(opcode,arguments);
				// Report undefined instructions
				if(!instrdef){
					printf("ASM:[E] AsmFirstpassline: Undefined instruction < ");
					printf("%s | ", opcode);
					for(tListnode* i = arguments->first;i;i=i->next){tAsmToken* j = i->item;
						printf("%s ",mtAsmToken_ToString(i->item));
					};
					printf("> \n");
					ErfError();
					return;
				};
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
							mtAsmBinarytoken_Dryemit(tok);
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
			printf("ASM:[F] AsmFirstpassline: Unrecognized token while parsing instruction opcode \n");
			printf("ASM:[F]                   got <%s>, excepted identifier \n",
				mtAsmToken_ToString(tok)
			);
			ErfFatal();
			break;
	};
};
//void AsmFirstpassline_old(){
//	//
//#ifdef qvGTrace
//	//printf("ASM:[T] AsmFirstpassline: Entered \n");
//#endif
//	iAsmRepeattimes = 1;
//	// Initialize
//	for(int i=0;i<10;i++)AsmBoundparameters[i]=0;
//	// Read instruction
//	tList /* <tAsmToken> */ * arguments = mtList_Create();
//	tAsmToken* tok = mtAsmToken_Get(getcurrentfile());
//	if(!tok)return 0;
//	if(tok->type==eAsmTokentype_Newline)return 0;
//	if(tok->type==eAsmTokentype_Label){
//		//printf("ASM:[D] | Label %-15s at %u (%x) \n",
//		//	tok->string,(unsigned)AsmCurrentposition,(unsigned)AsmCurrentposition
//		//);
//		AsmCreatelabel(tok->string);
//		mtAsmToken_Destroy(tok);
//		return 0;
//	};
//	if(tok->type!=eAsmTokentype_Identifier){
//		printf("ASM:[F] AsmCommonparseline: Unrecognized token while parsing instruction opcode \n");
//		printf("ASM:[F]                     got <%s>, excepted identifier \n",
//			 tok->type==eAsmTokentype_Identifier ?tok->string
//			:tok->type==eAsmTokentype_Newline    ?"\\n"
//			:tok->type==eAsmTokentype_Charater   ?(char[2]){tok->ch,0}
//			:tok->type==eAsmTokentype_Argument   ?(char[3]){'#',tok->ch,0}
//			:"(unknein)"
//		);
//		ErfFatal();
//	};
//	//  by now tok is known identifier but we also need to check for directives
//	if(strcmp(tok->string,".times")==0){
//		mtAsmToken_Destroy(tok);
//		tok = mtAsmToken_Get(getcurrentfile());
//		assert(tok->type==eAsmTokentype_Number);
//		//printf("ASM:[T] .times %i\n",tok->number);
//		iAsmRepeattimes = tok->number;
//		mtAsmToken_Destroy(tok);
//		return AsmCommonparseline();
//	}else{
//		if(strcmp(tok->string,".include")==0){
//			mtAsmToken_Destroy(tok);
//			tok = mtAsmToken_Get(getcurrentfile());
//			assert(tok->type==eAsmTokentype_String);
//			char* fname = mtString_Clone(tok->string);
//			mtAsmToken_Destroy(tok);
//			tok = mtAsmToken_Get(getcurrentfile());
//			assert(tok->type==eAsmTokentype_Newline);
//			mtAsmToken_Destroy(tok);
//			FILE* stream = fopen(fname,"r");
//			if(!stream){
//				printf(
//					"AsmCommonparseline: Directive .include: Unable to open file \"%s\": Error %i•%s\n",
//					fname,
//					errno,
//					strerror(errno)
//				);
//			};
//			mtList_Prepend(&AsmIncludelist,stream);
//			mtString_Destroy(fname);
//			return 0;
//		}else if(strcmp(tok->string,".segment")==0){
//			mtAsmToken_Destroy(tok);
//			tok = mtAsmToken_Get(getcurrentfile());
//			assert(tok->type==eAsmTokentype_Number);
//			AsmCurrentsegment = tok->number;
//			return 0;
//		}else if(strcmp(tok->string,".global")==0){
//			mtAsmToken_Destroy(tok);
//			tok = mtAsmToken_Get(getcurrentfile());
//			assert(tok->type==eAsmTokentype_Identifier);
//			assert(false);
//			return 0;
//		};
//	};
//	char* opcode = tok->string;
//	for(
//		tok = mtAsmToken_Get(getcurrentfile());
//		tok->type!=eAsmTokentype_Newline;
//		tok=mtAsmToken_Get(getcurrentfile())
//	)
//		mtList_Append(arguments,tok);
//	mtList_Append(arguments,tok); // The newline as well
//	// Find expansion
//	tAsmInstructiondefinition * instrdef = mtList_Find_Clojure(
//		&AsmInstructionsdefined,
//		(bool(*)(void*,void*))AsmInstructionfinderclojure,
//		&(
//			struct {
//				char* opcode;
//				tList /* <tAsmToken> */ * args;
//			}
//		){
//			.opcode=opcode,
//			.args=arguments,
//		}
//	);
//	if(!instrdef){
//		printf("ASM:[E] AsmCommonparseline: Undefined instruction < ");
//		printf("%s | ", opcode);
//		for(tListnode* i = arguments->first;i;i=i->next){tAsmToken* j = i->item;
//			printf("%s ",
//				 j->type==eAsmTokentype_Identifier?j->string
//				:j->type==eAsmTokentype_Newline?"\\n"
//				:j->type==eAsmTokentype_Charater?(char[2]){j->ch,0}
//				:"(unknein)"
//			);
//		};
//		printf("> \n");
//		ErfError();
//		return 0;
//	};
//#ifdef qvGTrace
//	if(0)printf("ASM:[T] AsmCommonparseline: Returning pointer %p\n",instrdef);
//#endif
//	if(!instrdef)return;
//	// Emit expansion
//	for(int i=1;i<=iAsmRepeattimes;i++){
//		//AsmCurrentexternalposition=AsmCurrentposition;
//		for(tListnode /* <tAsmBinarytoken> */ * i = instrdef->expansion->first;i;i=i->next){
//			tAsmBinarytoken* tok = i->item;
//#ifdef qvGTrace
//			if(0)printf("ASM: AsmFirstpassline:  Dry-emitting expansion for token %s \n",mtAsmBinarytoken_ToString(tok));
//#endif
//			mtAsmBinarytoken_Dryemit(tok);
//		};
//	}
//};
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
		FILE* archdeffile = fopen(
			mtString_Join(
				AsmConfigdir?:"/etc/xcc/",
				mtString_Join(
					AsmArchitecturename,
					"/archdef.asm"
				)
			),
			"r"
		);
		if(!archdeffile){
			printf(
				"ASM:[F] Unable to open archdef for arch \"%s\": Error %i·%s\n",
				AsmArchitecturename,
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
	// First pass
	iAsmRepeattimes = 1;
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
#ifdef qvGDebug
	printf("ASM:[D] First pass \n");
#endif
	while(fpeekc(AsmSourcestream)!=EOF)AsmFirstpassline();
	// Interpass seek
	memset(AsmCurrentposition,0,sizeof(AsmCurrentposition));
	if(fseek(AsmSourcestream,0,SEEK_SET)){
		printf(
			"ASM:[F] Unable to rewind source file to start: Error %i·%s\n",
			errno,
			strerror(errno)
		);
	};
	// Emit header

	// Second pass
	iAsmRepeattimes = 1;
#ifdef qvGDebug
	printf("ASM:[D] Second pass \n");
#endif
	while(fpeekc(AsmSourcestream)!=EOF)AsmSecondpassline(dstfile);
	// Emit relocations
	// Close it all
	fclose(AsmSourcestream);
	fclose(dstfile);
	return 0;
};

