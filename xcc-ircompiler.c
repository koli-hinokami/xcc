#include "hyperheader.h"

// -- Types --

typedef enum IrcArgpOptiontags {
	// This enumeration should have all the options passable
	eIrcArgpOptiontags_Architecture = 'a',
	eIrcArgpOptiontags_Confdir      = 'c',
	eIrcArgpOptiontags_Outputfile   = 'o',
} eIrcArgpOptiontags;

typedef struct {
	char* /* deallocatable */ label;
	char* /* deallocatable */ instruction;
	tList /* <char* deallocatable> */ * /* deallocatable */ operands;
} tIrcSourceline;

typedef enum eIrcTokentype {
	eIrcTokentype_Charater,
	eIrcTokentype_Newline,
	eIrcTokentype_Argumenttoken,
	eIrcTokentype_StringedtokensStart,
	eIrcTokentype_Identifier,
	eIrcTokentype_Label,
} eIrcTokentype;

typedef struct {
	eIrcTokentype type;
	union {
		char ch;
		char* /* owned */ string;
	};
} tIrcToken;

typedef struct { // The instructions defined
	char* /* owned */ opcode;
	tList /* <tToken> */ * operands;
	tList /* <tToken> */ * expansion;
} tIrcInstructiondefinition;

// -- Forward declarations --

error_t IrcArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --

tList /* <tIrcInstructiondefinition> */ IrcInstructionsdefined;
tList /* <tIrcToken* owned> */ *IrcBoundparameters[10];


struct argp_option IrcArgpOptions[] = {
	{	
		.name = "cfgdir",
		.key = eIrcArgpOptiontags_Confdir,
		.arg = "confdir",
		.flags = null,
		.doc = "Directory to search for config files in."
		       "Defaults to /etc/xcc.",
		.group = null,
	},
	{	
		.name = "arch",
		.key = eIrcArgpOptiontags_Architecture,
		.arg = "archname",
		.flags = null,
		.doc = "The architecture to compile assembly sources for. "
		       "Retargetablity first!",
		.group = null,
	},
	{	
		.name = "out",
		.key = eIrcArgpOptiontags_Outputfile,
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
struct argp IrcArgpParserstruct = {
	.options = IrcArgpOptions,
	.parser = &IrcArgpParser,    //error_t (*)(
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

char* IrcSourcefilename;
char* IrcTargetfilename;
char* IrcArchitecturename;
char* IrcConfigdir;

// -- Auxilirally functions --

bool mtChar_IrcIsTokenterminator(char self){
	if(isalnum(self)){return false;};  //Alphanumeric - usual charater
	if(self=='_')return false;         //Underscore - allowed in tokens
	if(self=='.')return false;         //`.` - This is a macroassembler so why not
	return true;                       //The rest are 'auto-whitespace' chars that begin another token
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

// -- class tIrcToken --

tIrcToken* mtIrcToken_Allocate(void){return calloc(1,sizeof(tIrcToken));};
tIrcToken* mtIrcToken_Clone(tIrcToken* self){
	//
	void* i = memcpy(malloc(sizeof(tIrcToken)),self,sizeof(tIrcToken));;
#ifdef qvGTrace
	printf("IRC:[T] mtIrcToken_Clone(self* %p)->%p: Entered\n",self,i);
#endif
	return i;//memcpy(malloc(sizeof(tIrcToken)),self,sizeof(tIrcToken));;
};
tIrcToken* mtIrcToken_CreateCharaterd(eIrcTokentype type,char ch){
	return mtIrcToken_Clone(
		&(tIrcToken){
			.type=type,
			.ch=ch
		}
	);
};
bool mtIrcToken_HasString(tIrcToken* self){
	return self->type>=eIrcTokentype_StringedtokensStart;
};
void mtIrcToken_Destroy(tIrcToken* self){
#ifdef qvGTrace
	printf("IRC:[T] mtIrcToken_Destroy(self* %p): Entered\n",self);
#endif
	if(mtIrcToken_HasString(self))mtString_Destroy(self->string);
	free(self);
};
tIrcToken* mtIrcToken_Get(FILE* src){ // Constructor
	//tIrcToken* self = mtIrcToken_Allocate();
#ifdef qvGTrace
	printf("IRC:[T] mtIrcToken_Get(FILE* (%p)): Entered\n",src);
	printf("IRC:[T] mtIrcToken_Get: Charater \'%c\'\n",fpeekc(src));
#endif
	errno=0;
	switch(fpeekc(src)){
		case EOF:
			printf("IRC:[E] mtIrcToken_Get(FILE* %p): Error %i•\"%s\" while fetching charater\n",src,errno,strerror(errno));
			ErfError();
			return nullptr;
		case ';':
#ifdef qvGTrace
			printf("IRC:[T] mtIrcToken_Get: Comment - recursing tailcall \n");
#endif
			while(fgetc(src)!='\n');
			return mtIrcToken_Get(src);
		case '\n':
			fgetc(src);
			return mtIrcToken_Clone(
				&(tIrcToken){
					.type = eIrcTokentype_Newline,
				}
			);
			break;
		case ' ':
		case '\t':
#ifdef qvGTrace
			printf("IRC:[T] mtIrcToken_Get: whitespace \n");
#endif
			fgetc(src);
			return mtIrcToken_Get(src);
			break;
		default: {
#ifdef qvGTrace
			printf("IRC:[T] mtIrcToken_Get: Unrecognized char %i•%c\n",fpeekc(src),fpeekc(src));
#endif
			if(mtChar_IrcIsTokenterminator(fpeekc(src))){
#ifdef qvGTrace
				printf("IRC:[T] mtIrcToken_Get: Token-separating charater\n");
#endif
				return mtIrcToken_Clone(
					&(tIrcToken){
						.type = eIrcTokentype_Charater,
						.ch = fgetc(src),
					}
				);
			}else{
#ifdef qvGTrace
				printf("IRC:[T] mtIrcToken_Get: Identifier\n");
#endif
				char* str = mtString_Create();
				while(!mtChar_IrcIsTokenterminator(fpeekc(src))){
#ifdef qvGTrace
					//printf("IRC:[T] mtIrcToken_Get: adding ch \'%c\'\n",fpeekc(src));
#endif
					mtString_Append(&str,(char[2]){fgetc(src),0});
				}
				if(fpeekc(src)==':'){
					fgetc(src);
					return mtIrcToken_Clone(
						&(tIrcToken){
							.type = eIrcTokentype_Label,
							.string = str,
						}
					);
				}else{
					return mtIrcToken_Clone(
						&(tIrcToken){
							.type = eIrcTokentype_Identifier,
							.string = str,
						}
					);
				}
			};
		};
	};
	return nullptr;
};
void mtIrcToken_Emit(tIrcToken* self,FILE* dst){
	switch(self->type){
		case eIrcTokentype_Identifier:
			fprintf(dst,"%s ",self->string);
			break;
		case eIrcTokentype_Label:
			fprintf(dst,"%s:\t",self->string);
			break;
		case eIrcTokentype_Newline:
			fprintf(dst,"\n");
			break;
		case eIrcTokentype_Charater:
			fprintf(dst,"%c ",self->ch);
			break;
		default:
			printf("IRC:[E] mtIrcToken_Emit: Unrecognized token type %i\n",self->type);
			ErfError();
	};
};
bool mtIrcToken_Equals(tIrcToken* self,tIrcToken* tok){
	if(self->type!=tok->type)return false;
	if(self->type==eIrcTokentype_Identifier)
		if(strcmp(self->string,tok->string)!=0)return false;
	if(self->type==eIrcTokentype_Charater)
		if(self->ch!=tok->ch)return false;
	return true;
};


// -- Functions --

error_t IrcArgpParser(int optiontag,char* optionvalue,struct argp_state *state){
	switch(optiontag){
		case eIrcArgpOptiontags_Confdir:
			if(IrcConfigdir){
				return ARGP_ERR_UNKNOWN;
			}else{
				IrcConfigdir=mtString_Clone(optionvalue);
			};
			break;
		case eIrcArgpOptiontags_Architecture:
			if(IrcArchitecturename){
				return ARGP_ERR_UNKNOWN;
			}else{
				IrcArchitecturename=mtString_Clone(optionvalue);
			};
			break;
		case eIrcArgpOptiontags_Outputfile:
			if(IrcTargetfilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				IrcTargetfilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_ARG:
			if(IrcSourcefilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				IrcSourcefilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_END:
			if(!IrcSourcefilename){
				argp_error(state,"No source file specified");
			};
			// Produce target filename if absent
			if(!IrcTargetfilename){
				IrcTargetfilename = mtString_Clone(IrcSourcefilename);
				if(
					  (mtString_FindcharLast(IrcTargetfilename,'.')!=nullptr)
					&&(mtString_FindcharLast(IrcTargetfilename,'.')!=IrcSourcefilename)
				){
					mtString_FindcharLast(IrcTargetfilename,'.')[0]=0;
				};
				mtString_Append(&IrcTargetfilename,".obj");
			};
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
};
FILE* IrcFileerrorfilter(FILE* self){
	if(self)return self;
	printf("IRC:[E] IrcFileerrorfilter: Error %i·%s\n",errno,strerror(errno));
	return nullptr;
};

// -- Instruction defining --

void IrcReadinstructiondefinition(FILE* src){
	//
#ifdef qvGTrace
	printf("IRC:[T] IrcReadinstructiondefinition: Entered \n");
#endif
	tIrcToken* token = mtIrcToken_Get(src);
	if(token){
		if(token->type==eIrcTokentype_Identifier){
			if(strcmp(token->string,"opcode")==0){
#ifdef qvGTrace
				printf("IRC:[T] IrcReadinstructiondefinition: Opcode definition \n");
#endif
				mtIrcToken_Destroy(token);
				// Get opcode
				token = mtIrcToken_Get(src);
				if(token->type!=eIrcTokentype_Identifier){
					printf("IRC:[F] IrcReadinstructiondefinition: Unrecoginzed token "
						   "when fetching instruction definition\n");
					ErfFatal();
				};
				// Allocate instruction
				tIrcInstructiondefinition* instruction =
					calloc(1,sizeof(tIrcInstructiondefinition));
				instruction->opcode = mtString_Clone(token->string);
				instruction->operands = mtList_Create();
				instruction->expansion = mtList_Create();
				//mtList_Append(instruction->operands,mtIrcToken_Clone(token));
				mtIrcToken_Destroy(token);
				// Read arguments
				while(
					  ((token=mtIrcToken_Get(src))->type!=eIrcTokentype_Charater)
					||(token->ch!=':')
				){
					mtList_Append(instruction->operands,token);
				};
				if(0)mtList_Append(instruction->operands,
					mtIrcToken_CreateCharaterd(
						eIrcTokentype_Newline,
						'\n'
					)
				);
				// Read expansion
				mtIrcToken_Destroy(token);
				while(
					(token=mtIrcToken_Get(src))->type!=eIrcTokentype_Newline
				){
					if(
						  (token->type==eIrcTokentype_Charater)
						&&(token->ch=='|')
					){
						token->type=eIrcTokentype_Newline;
						token->ch='\n';
					};
					mtList_Append(instruction->expansion,token);
				};
				mtList_Append(instruction->operands,mtIrcToken_Clone(token));
				// Register instruction
				mtList_Append(&IrcInstructionsdefined,instruction);
			}else{
				printf("IRC:[F] IrcReadinstructiondefinition: Unrecoginzed token"
					   "when fetching instruction definition\n");
				ErfFatal();
			};
		}else{
			printf("IRC:[F] IrcReadinstructiondefinition: Unrecoginzed token"
				   "when fetching instruction definition\n");
			ErfFatal();
		};
		mtIrcToken_Destroy(token);
	}else{
		return;
	};
};
void IrcReadinstructiondefinitions(FILE* src){
	//
#ifdef qvGTrace
	printf("IRC:[T] IrcReadinstructiondefinitions: Entered \n");
#endif
	while(fpeekc(src)!=EOF)IrcReadinstructiondefinition(src);
};

// -- Main loop --

bool IrcInstructionfinderclojure(
	struct {
		char* opcode;
		tList /* <tIrcToken> */ * args;
	} * clojureargs,
	tIrcInstructiondefinition * item
){
	// Check opcode
	if(strcmp(item->opcode,clojureargs->opcode)!=0)return false;
	// Try to match arguments
	for(
		tListnode /* <tIrcToken> */ 
			*i=item->operands->first,
			*j=clojureargs->args->first;
		;
		
	){
		tIrcToken* sourcetok = i->item;
		tIrcToken* desttok = j->item;
		if(
			  (sourcetok->type==eIrcTokentype_Charater)
			&&(sourcetok->ch=='#')
		){
			// Argument that would need to be matched into the array
			// of bound args
			i=i->next;
			if(((tIrcToken*)i->item)->type!=eIrcTokentype_Charater){
				printf("IRC:[F] Invalid instruction definition: `#` not followed by charater \n");
				ErfFatal();
			};
			int argindex = ((tIrcToken*)i->item)->ch-'0';
			assert(argindex>=0);
			assert(argindex<10);
			i=i->next;
			tIrcToken* matchtoken = i->item;
			tList /* <tIrcToken* owned> */ * list = mtList_Create();
			IrcBoundparameters[argindex]=list;
			while(!mtIrcToken_Equals(matchtoken,j->item)){
				mtList_Append(list,mtIrcToken_Clone(j->item));
				j=j->next;
			};
		}else{
			// Ordinary token
			if(!mtIrcToken_Equals(sourcetok,desttok))return false;
		};
	};
	// Found it.
	return true;
};
void IrcParseline(FILE* src,FILE* dst){
#ifdef qvGTrace
	printf("IRC:[T] IrcParseline: Entered \n");
#endif
	// Read instruction
	tList /* <tIrcToken> */ * arguments = mtList_Create();
	tIrcToken* tok = mtIrcToken_Get(src);
	if(!tok)return;
	if(tok->type==eIrcTokentype_Label){
		fprintf(dst,"%s:\n",tok->string);
		mtIrcToken_Destroy(tok);
		return;
	};
	if(tok->type!=eIrcTokentype_Identifier){
		printf("IRC:[F] IrcParseline: Unrecognized token while parsing instruction opcode \n");
	};
	char* opcode = tok->string;
	for(
		tok = mtIrcToken_Get(src);
		tok->type!=eIrcTokentype_Newline;
		tok=mtIrcToken_Get(src)
	)
		mtList_Append(arguments,tok);
	// Find expansion
	tIrcInstructiondefinition * instrdef = mtList_Find_Clojure(
		&IrcInstructionsdefined,
		(bool(*)(void*,void*))IrcInstructionfinderclojure,
		&(
			struct {
				char* opcode;
				tList /* <tIrcToken> */ * args;
			}
		){
			.opcode=opcode,
			.args=arguments,
		}
	);
	// Emit expansion
	for(tListnode /* <tIrcToken> */ * i = instrdef->expansion->first;i;i=i->next){
		tIrcToken* tok = i->item;
		if(
			(tok->type==eIrcTokentype_Charater)
			&&(tok->ch=='#')
		){
			for(
				tListnode /* <tIrcToken> */ * j = IrcBoundparameters[
					((tIrcToken*)i->next->item)->ch-'0']->first;
				j;
				j=j->next
			){
				mtIrcToken_Emit(j->item,dst);
			}
		}
	};
	// Destroy bound arguments
	for(int i=0;i<10;i++){
		mtList_Foreach(IrcBoundparameters[i],(void(*)(void*))mtIrcToken_Destroy);
		mtList_Destroy(IrcBoundparameters[i]);
	};
};
void IrcCompilefile(FILE* src, FILE* dst){
	//
#ifdef qvGDebug
	printf("IRC:[D] Parsing \n");
#endif
	while(fpeekc(src)!=EOF)IrcParseline(src,dst);
};
// -- Launcher aka init code --
void LnFailedassertionhandler(int signum){
	fprintf(stderr,"Ln: [F] Failed assertion catched! \n");
	ErfFatal();
};
void LnNullpointerhandler(int signum){
	fprintf(stderr,"Ln: [F] Segfault catched! \n");
	ErfFatal();
};
int main(int argc, char** argv){
	signal(SIGSEGV,LnNullpointerhandler);
	signal(SIGABRT,LnFailedassertionhandler);
	// Parse arguments
	argp_parse(
		&IrcArgpParserstruct,
		argc,
		argv,
		0,
		0,
		0
	);
	// Open archdef
	FILE* archdeffile = fopen(
		mtString_Join(
			IrcConfigdir?:"/etc/xcc/",
			mtString_Join(
				IrcArchitecturename,
				".irc"
			)
		),
		"r"
	);
	if(!archdeffile){
		printf(
			"IRC:[F] Unable to open archdef for arch \"%s\": Error %i·%s\n",
			IrcArchitecturename,
			errno,
			strerror(errno)
		);
		exit(1);
	};
	// Read archdef
	IrcReadinstructiondefinitions(archdeffile);
	// Dump archdep
	if(0)for(tListnode * i = IrcInstructionsdefined.first;i;i=i->next){
		tIrcInstructiondefinition * j = i->item;
		printf("IRC:[T] Opcode %s : ",j->opcode);
		for(tListnode * k = j->operands->first;k;k=k->next){
			tIrcToken * l = k->item;
			if(mtIrcToken_HasString(l))
				printf("%s ",l->string);
			else
				printf("%c ",l->ch?l->ch:'%');
		};
		printf("|");
		for(tListnode * k = j->expansion->first;k;k=k->next){
			tIrcToken * l = k->item;
			if(mtIrcToken_HasString(l))
				printf("%s ",l->string);
			else
				printf("%c ",l->ch?:'%');
		};
		printf("\n");
	};
	// Open source/dest files
	FILE* srcfile = fopen(IrcSourcefilename,"r");
	if(!srcfile){
		printf(
			"IRC:[F] Unable to open source file \"%s\": Error %i·%s\n",
			IrcSourcefilename,
			errno,
			strerror(errno)
		);
	};
	FILE* dstfile = fopen(IrcTargetfilename,"w");
	if(!dstfile){
		printf(
			"IRC:[F] Unable to open destination file \"%s\": Error %i·%s\n",
			IrcTargetfilename,
			errno,
			strerror(errno)
		);
	};
	// Compile
	IrcCompilefile(
		srcfile,
		dstfile
	);
	// Close it all
	fclose(srcfile);
	fclose(dstfile);
	return 0;
};

