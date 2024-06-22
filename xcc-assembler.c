#include "hyperheader.h"

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
	eAsmTokentype_Charater,
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
	eAsmBinarytokensize_Lobyte,
	eAsmBinarytokensize_Hibyte,
	eAsmBinarytokensize_Word,
} eAsmBinarytokensize;

typedef enum eAsmBinarytokentype {
	eAsmBinarytokentype_Newline,
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

// -- Forward declarations --

error_t AsmArgpParser(int key,char* argumentvalue,struct argp_state *state);
tGTargetNearpointer AsmGetlabelvalue(char* name);
char* mtAsmToken_ToString(tAsmToken* self);

// -- Globals --

tList /* <tAsmInstructiondefinition> */ AsmInstructionsdefined;
tList /* <tIrcToken* owned> */ * AsmBoundparameters[10];
tGTargetNearpointer AsmCurrentposition;          // Actual position in file
tGTargetNearpointer AsmCurrentexternalposition;  // The position to use for $
                                                 // inside expansion - updated
                                                 // only at instruction
                                                 // boundaries
tList /* <tAsmLabel* owned> */ * AsmLabels;
int iAsmRepeattimes;

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
		"IR Compiler for xcc."
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

// -- Auxilirally functions --

bool mtChar_AsmIsTokenterminator(char self){
	if(isalnum(self)){return false;};  //Alphanumeric - usual charater
	if(self=='_')return false;         //Underscore - allowed in tokens
	if(self=='.')return false;         //`.` - This is a macroassembler so why not
	return true;                       //The rest are 'auto-whitespace' chars that begin another token
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
	if(0)printf("ASM:[T] mtAsmBinarytoken_Clone(self* %p)->%p: Entered\n",self,i);
#endif
	return i;//memcpy(malloc(sizeof(tAsmBinarytoken)),self,sizeof(tAsmBinarytoken));;
};
bool mtAsmBinarytoken_HasString(tAsmBinarytoken* self){
	return false; // self->type>=eAsmBinarytokentype_StringedtokensStart;
};
void mtAsmBinarytoken_Destroy(tAsmBinarytoken* self){
	//
#ifdef qvGTrace
	if(0)printf("ASM:[T] mtAsmBinarytoken_Destroy(self* %p): Entered\n",self);
#endif
	free(self);
};
tAsmBinarytoken* mtAsmBinarytoken_Get(FILE* src){ // Constructor
	//tAsmBinarytoken* self = mtAsmBinarytoken_Allocate();
#ifdef qvGTrace
	if(0)printf("ASM:[T] mtAsmBinarytoken_Get(FILE* (%p)): Entered\n",src);
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
			switch(fpeekc(src)){
				case '>': fgetc(src);          self->size = eAsmBinarytokensize_Hibyte;     break;
				case '<': fgetc(src); default: self->size = eAsmBinarytokensize_Lobyte;     break;
				case '=': fgetc(src);          self->size = eAsmBinarytokensize_Word;       break;
			};
			switch(fpeekc(src)){
				case '$': fgetc(src);          self->type = eAsmBinarytokentype_Position;   break;
				case '#': fgetc(src);          self->type = eAsmBinarytokentype_Expr;       break;
				case '&': fgetc(src); default: self->type = eAsmBinarytokentype_Raw;        break;
			};
			char* str = mtString_Create();
			while(!mtChar_AsmIsTokenterminator(fpeekc(src))){
				mtString_Append(&str,(char[2]){fgetc(src),0});
			}
			char* strtol_tail;
			if(str[0]==0){
				self->disp = 0;
			}else if(str[0]=='0' && str[1]=='x'){
				self->disp=strtol(str+2,&strtol_tail,16);
				assert(strtol_tail[0]==0);
			}else{
				self->disp=strtol(str,&strtol_tail,10);
				assert(strtol_tail[0]==0);
			};
			free(str);
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
			 self->type==eAsmBinarytokentype_Raw     ?"&"
			:self->type==eAsmBinarytokentype_Expr    ?"#"
			:self->type==eAsmBinarytokentype_Position?"$"
			:self->type==eAsmBinarytokentype_Newline ?"\\n"
			:mtString_Join(
				mtString_Join(
					"(unknein type ",
					mtString_FromInteger(self->type)
				),
				")"
			),
			mtString_FromInteger(self->disp)
		)
	);
}
void mtAsmBinarytoken_Dryemit(tAsmBinarytoken* self){
	switch(self->size){
		case eAsmBinarytokensize_Lobyte:
		case eAsmBinarytokensize_Hibyte:
			AsmCurrentposition+=1;
			break;
		case eAsmBinarytokensize_Word:
			AsmCurrentposition+=2;
			break;
		default:
			printf("ASM:[E] mtAsmBinarytoken_Dryemit: Unrecognized token size %i\n",self->size);
			break;
	};
};
void mtAsmBinarytoken_Emit(tAsmBinarytoken* self,FILE* dst){
	tGTargetNearpointer exprval = self->disp;
	switch(self->type){
		case eAsmBinarytokentype_Raw:
			break;
		case eAsmBinarytokentype_Position:
			exprval+=AsmCurrentexternalposition;
		case eAsmBinarytokentype_Expr: {
			exprval = 0;
			assert(self->disp>=0);
			assert(self->disp<10);
			tList /* tAsmToken */ * expr = AsmBoundparameters[self->disp];
			for(tListnode* i = expr->first;i;i=i->next){
				tAsmToken* j = i->item;
#ifdef qvGTrace
				if(0)printf("ASM:[T] Evalexpr for token <%s>\n", mtAsmToken_ToString(j));
#endif
				int mode = 1;
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
								val = AsmCurrentexternalposition;
								break;
							default:
								printf("ASM:[E] mtAsmBinarytoken_Emit: Evalexpr for token <%s>: unrecognized char \'%c\'\n", mtAsmToken_ToString(j),j->ch);
								ErfError();
								val = 0;
								break;
						};
						break;
					case eAsmTokentype_Identifier:
						val = AsmGetlabelvalue(j->string);
						break;
					case eAsmTokentype_Number:
						val = j->number;
						break;
					case eAsmTokentype_String:
						val = AsmGetlabelvalue(j->string);
						break;
					default:
						printf("ASM:[E] mtAsmBinarytoken_Emit: Evalexpr for token <%s>: unrecognized type %i\n", mtAsmToken_ToString(j),j->type);
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
			printf("ASM:[E] mtAsmBinarytoken_Emit: Unrecognized token %s of type %i\n",mtAsmBinarytoken_ToString(self),self->type);
			ErfError();
			break;
	};
	switch(self->size){
		case eAsmBinarytokensize_Lobyte:
			fputc((uint8_t)exprval,dst);
			AsmCurrentposition+=1;
			break;
		case eAsmBinarytokensize_Hibyte:
			fputc((uint8_t)(exprval>>8),dst);
			AsmCurrentposition+=1;
			break;
		case eAsmBinarytokensize_Word:
			fputc((uint8_t)(exprval>>8),dst);
			fputc((uint8_t)(exprval),dst);
			AsmCurrentposition+=2;
			break;
		default:
			printf("ASM:[E] mtAsmBinarytoken_Emit: Unrecognized token size %i\n",self->size);
			break;
	};
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
	return mtAsmToken_Clone(
		&(tAsmToken){
			.type=type,
			.ch=ch
		}
	);
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
		:"(unknein)"
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
	if(0)printf("ASM:[T] AsmReadinstructiondefinition: Entered \n");
#endif
	tAsmToken* token = mtAsmToken_Get(src);
	if(token){
		if(token->type==eAsmTokentype_Identifier){
			if(strcmp(token->string,"opcode")==0){
#ifdef qvGTrace
				if(0)printf("ASM:[T] AsmReadinstructiondefinition: Opcode definition \n");
#endif
				mtAsmToken_Destroy(token);
				// Get opcode
				token = mtAsmToken_Get(src);
				if(token->type!=eAsmTokentype_Identifier){
					printf("ASM:[F] AsmReadinstructiondefinition: Unrecoginzed token ");
					printf("%s ",
						 token->type==eAsmTokentype_Identifier ?token->string
						:token->type==eAsmTokentype_Newline    ?"\\n"
						:token->type==eAsmTokentype_Charater   ?(char[2]){token->ch,0}
						:"(unknein)"
					);
					printf(" when fetching instruction definition's opcode\n");
					ErfFatal();
				};
				if(0)printf("ASM:[T] AsmReadinstructiondefinition: Opcode %s\n",
					mtAsmToken_ToString(token)
				);
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
					if(0)printf("ASM:[T] AsmReadinstructiondefinition: Argument %s\n",
						mtAsmToken_ToString(token)
					);
					mtList_Append(instruction->operands,token);
				};
				mtList_Append(instruction->operands,
					mtAsmToken_CreateCharaterd(
						eAsmTokentype_Newline,
						'\n'
					)
				);
				mtAsmToken_Destroy(token);
				// Read expansion
				tAsmBinarytoken* bintoken = nullptr;
				while(
					(bintoken=mtAsmBinarytoken_Get(src))->type!=eAsmBinarytokentype_Newline
				){
#ifdef qvGTrace
					if(0)printf("ASM:[T] AsmReadinstructiondefinition: "
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
				printf("<%s>",
					 token->type==eAsmTokentype_Identifier ?token->string
					:token->type==eAsmTokentype_Newline    ?"\\n"
					:token->type==eAsmTokentype_Charater   ?(char[2]){token->ch,0}
					:"(unknein)"
				);
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
			printf("<%s>",
				 token->type==eAsmTokentype_Identifier ?token->string
				:token->type==eAsmTokentype_Newline    ?"\\n"
				:token->type==eAsmTokentype_Charater   ?(char[2]){token->ch,0}
				:"(unknein)"
			);
			printf(" while excepted an identifier \n");
			ErfFatal();
		};
		mtAsmToken_Destroy(token);
	}else{
		printf("ASM:[F] AsmReadinstructiondefinition: Unable to fetch token from archdef \n");
		ErfError();
		return;
	};
};
void AsmReadinstructiondefinitions(FILE* src){
	//
#ifdef qvGTrace
	//printf("ASM:[T] AsmReadinstructiondefinitions: Entered \n");
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
	if(0)printf(
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
		//printf("ASM:[T] AsmInstructionfinderclojure: Iteration on tokens <%s> and <%s> \n",mtAsmToken_ToString(i->item),mtAsmToken_ToString(j->item));
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
				printf("<%s>",
					 ((tAsmToken*)i->item)->type==eAsmTokentype_Identifier ?((tAsmToken*)i->item)->string
					:((tAsmToken*)i->item)->type==eAsmTokentype_Newline    ?"\\n"
					:((tAsmToken*)i->item)->type==eAsmTokentype_Charater   ?(char[2]){((tAsmToken*)i->item)->ch,0}
					:((tAsmToken*)i->item)->type==eAsmTokentype_Argument   ?(char[3]){'#',((tAsmToken*)i->item)->ch,0}
					:"(unknein)"
				);
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
			if(0)printf("ASM:[T] AsmInstructionfinderclojure: . Fetching argument %i until token <%s> \n",
				argindex,
				 ((tAsmToken*)i->item)->type==eAsmTokentype_Identifier ?((tAsmToken*)i->item)->string
				:((tAsmToken*)i->item)->type==eAsmTokentype_Newline    ?"\\n"
				:((tAsmToken*)i->item)->type==eAsmTokentype_Charater   ?(char[2]){((tAsmToken*)i->item)->ch,0}
				:((tAsmToken*)i->item)->type==eAsmTokentype_Argument   ?(char[3]){'#',((tAsmToken*)i->item)->ch,0}
				:"(unknein)"
			);
#endif
			while(!mtAsmToken_Equals(matchtoken,j->item)){
				if(0)printf("ASM:[T] AsmInstructionfinderclojure: | Token <%s> against <%s> \n",
					mtAsmToken_ToString(matchtoken),
					mtAsmToken_ToString(j->item)
				);
				mtList_Append(list,mtAsmToken_Clone(j->item));
				j=j->next;
				assert(j);
			};
#ifdef qvGTrace
			if(0)printf("ASM:[T] AsmInstructionfinderclojure: ' Done \n");
#endif
		}else{
			// Ordinary token
			if(!mtAsmToken_Equals(sourcetok,desttok))return false;
		};
	};
	// Found it.
#ifdef qvGTrace
	if(0)printf("ASM:[T] AsmInstructionfinderclojure: Found instrdef, returning \n");
#endif
	return true;
};
void AsmCreatelabel(char* /* borrows */ name, tGTargetNearpointer position){
	assert(false);
};

tAsmInstructiondefinition * AsmCommonparseline(){ // Returns 0 when skip emitting
	for(int i=0;i<10;i++)AsmBoundparameters[i]=0;
	// Read instruction
	tList /* <tAsmToken> */ * arguments = mtList_Create();
	tAsmToken* tok = mtAsmToken_Get(getcurrentfile());
	if(!tok)return 0;
	if(tok->type==eAsmTokentype_Newline)return 0;
	if(tok->type==eAsmTokentype_Label){
		printf("ASM:[D] | Label %-15s at %u (%x) \n",
			tok->string,(unsigned)AsmCurrentposition,(unsigned)AsmCurrentposition
		);
		mtList_Append(AsmLabels,mtAsmLabel_CreateNear(tok->string,AsmCurrentposition));
		mtAsmToken_Destroy(tok);
		return 0;
	};
	if(tok->type!=eAsmTokentype_Identifier){
		printf("ASM:[F] AsmCommonparseline: Unrecognized token while parsing instruction opcode \n");
		printf("ASM:[F]                     got <%s>, excepted identifier \n",
			 tok->type==eAsmTokentype_Identifier ?tok->string
			:tok->type==eAsmTokentype_Newline    ?"\\n"
			:tok->type==eAsmTokentype_Charater   ?(char[2]){tok->ch,0}
			:tok->type==eAsmTokentype_Argument   ?(char[3]){'#',tok->ch,0}
			:"(unknein)"
		);
		ErfFatal();
	};
	//  by now tok is known identifier but we also need to check for directives
	if(strcmp(tok->string,".times")==0){
		mtAsmToken_Destroy(tok);
		tok = mtAsmToken_Get(getcurrentfile());
		assert(tok->type==eAsmTokentype_Number);
		//printf("ASM:[T] .times %i\n",tok->number);
		iAsmRepeattimes = tok->number;
		mtAsmToken_Destroy(tok);
		return AsmCommonparseline();
	}else{
		if(strcmp(tok->string,".org")==0){
			mtAsmToken_Destroy(tok);
			tok = mtAsmToken_Get(getcurrentfile());
			assert(tok->type==eAsmTokentype_Identifier);
			char* strtol_tail;
			AsmCurrentposition = strtol(tok->string,&strtol_tail,10);
			assert(strtol_tail[0]==0);
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
					"AsmCommonparseline: Directive .include: Unable to open file \"%s\": Error %i•%s\n",
					fname,
					errno,
					strerror(errno)
				);
			};
			mtList_Prepend(&AsmIncludelist,stream);
			mtString_Destroy(fname);
			return 0;
		};
	};
	char* opcode = tok->string;
	for(
		tok = mtAsmToken_Get(getcurrentfile());
		tok->type!=eAsmTokentype_Newline;
		tok=mtAsmToken_Get(getcurrentfile())
	)
		mtList_Append(arguments,tok);
	mtList_Append(arguments,tok); // The newline as well
	// Find expansion
	tAsmInstructiondefinition * instrdef = mtList_Find_Clojure(
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
	if(!instrdef){
		printf("ASM:[E] AsmCommonparseline: Undefined instruction < ");
		printf("%s | ", opcode);
		for(tListnode* i = arguments->first;i;i=i->next){tAsmToken* j = i->item;
			printf("%s ",
				 j->type==eAsmTokentype_Identifier?j->string
				:j->type==eAsmTokentype_Newline?"\\n"
				:j->type==eAsmTokentype_Charater?(char[2]){j->ch,0}
				:"(unknein)"
			);
		};
		printf("> \n");
		ErfError();
		return 0;
	};
#ifdef qvGTrace
	if(0)printf("ASM:[T] AsmCommonparseline: Returning pointer %p\n",instrdef);
#endif
	return instrdef;
}
void AsmSecondpassline(FILE* dst){
	//
	assert(dst);
#ifdef qvGTrace
	//printf("ASM:[T] AsmSecondpassline: Entered \n");
#endif
	iAsmRepeattimes = 1;
	tAsmInstructiondefinition * instrdef = AsmCommonparseline();
	if(!instrdef)return;
	// Emit expansion
	for(int i=1;i<=iAsmRepeattimes;i++){
		AsmCurrentexternalposition=AsmCurrentposition;
		if(instrdef->expansion->first!=0){
			for(tListnode /* <tAsmBinarytoken> */ * i = instrdef->expansion->first;i;i=i->next){
				assert(i);
				assert(i->item);
				tAsmBinarytoken* tok = i->item;
#ifdef qvGTrace
				//printf("ASM:[T] AsmSecondpassline:  Emitting expansion for token %s \n",mtAsmBinarytoken_ToString(tok));
#endif
				mtAsmBinarytoken_Emit(tok,dst);
			};
		};
	};
	// Destroy bound arguments
	if(0)for(int i=0;i<10;i++)if(AsmBoundparameters[i]){
		mtList_Foreach(AsmBoundparameters[i],(void(*)(void*))mtAsmToken_Destroy);
		mtList_Destroy(AsmBoundparameters[i]);
	};
};
void AsmFirstpassline(){
	//
#ifdef qvGTrace
	//printf("ASM:[T] AsmFirstpassline: Entered \n");
#endif
	iAsmRepeattimes = 1;
	tAsmInstructiondefinition * instrdef = AsmCommonparseline();
	if(!instrdef)return;
	// Emit expansion
	for(int i=1;i<=iAsmRepeattimes;i++){
		AsmCurrentexternalposition=AsmCurrentposition;
		for(tListnode /* <tAsmBinarytoken> */ * i = instrdef->expansion->first;i;i=i->next){
			tAsmBinarytoken* tok = i->item;
#ifdef qvGTrace
			if(0)printf("ASM: AsmFirstpassline:  Dry-emitting expansion for token %s \n",mtAsmBinarytoken_ToString(tok));
#endif
			mtAsmBinarytoken_Dryemit(tok);
		};
	}
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
	FILE* archdeffile = fopen(
		mtString_Join(
			AsmConfigdir?:"/etc/xcc/",
			mtString_Join(
				AsmArchitecturename,
				".asm"
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
	if(0){
		printf("ASM:[D] . Dumping instructions defined \n");
		for(tListnode * i = AsmInstructionsdefined.first;i;i=i->next){
			tAsmInstructiondefinition * j = i->item;
			printf("ASM:[T] | Opcode %s : ",j->opcode);
			for(tListnode * k = j->operands->first;k;k=k->next){
				printf("%s ",mtAsmToken_ToString(k->item));
			};
			printf("|");
			for(tListnode * k = j->expansion->first;k;k=k->next){
				printf("%s ",mtAsmBinarytoken_ToString(k->item));
			};
			printf("\n");
		};
		printf("ASM:[D] ' \n");
	}
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
#ifdef qvGDebug
	printf("ASM:[D] First pass \n");
#endif
	while(fpeekc(AsmSourcestream)!=EOF)AsmFirstpassline();
	// Interpass seek
	AsmCurrentposition=0;
	if(fseek(AsmSourcestream,0,SEEK_SET)){
		printf(
			"ASM:[F] Unable to rewind source file to start: Error %i·%s\n",
			errno,
			strerror(errno)
		);
	};
	// Second pass
#ifdef qvGDebug
	printf("ASM:[D] Second pass \n");
#endif
	while(fpeekc(AsmSourcestream)!=EOF)AsmSecondpassline(dstfile);
	// Close it all
	fclose(AsmSourcestream);
	fclose(dstfile);
	return 0;
};

