#include "hyperheader.h"

// -- Types --

typedef enum IrcArgpOptiontags {
	// This enumeration should have all the options passable
	eIrcArgpOptiontags_Architecture = 'a',
	eIrcArgpOptiontags_Outputfile   = 'o',
} eIrcArgpOptiontags;

typedef struct {
	char* /* deallocatable */ label;
	char* /* deallocatable */ instruction;
	tList /* <char* deallocatable> */ * /* deallocatable */ operands;
} tIrcSourceline;

typedef enum eIrcTokentype {
	eIrcTokentype_Identifier        = 1,
	eIrcTokentype_Wildcharater      = 2,
} eIrcTokentype;

typedef struct {
	eIrcTokentype type;
	union {
		char ch;
		char* /* owned */ string;
	};
} tIrcToken;

// -- Forward declarations --

error_t IrcArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --
struct argp_option IrcArgpOptions[] = {
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

// -- Auxilirally functions --

bool mtChar_IrcIsTokenterminator(char self){
	if(isalnum(self)){return false;};  //Alphanumeric - usual charater
	if(self=='_')return false;         //Underscore - allowed in tokens
	return true;                       //The rest are 'auto-whitespace' chars that begin another token
};

// -- class FILE --

int fpeekc(FILE* self){return ungetc(fgetc(self),self);};

// -- class tIrcToken

tIrcToken* mtIrcToken_Allocate(void){return calloc(1,sizeof(tIrcToken));};
tIrcToken* mtIrcToken_Clone(tIrcToken* self){
	return memcpy(malloc(sizeof(tIrcToken)),self,sizeof(tIrcToken));
};
void mtIrcToken_Destroy(tIrcToken* self){
	mtString_Destroy(self->string);
	free(self);
};
tIrcToken* mtIrcToken_Get(FILE* src){ // Constructor
	//tIrcToken* self = mtIrcToken_Allocate();
	switch(fpeekc(src)){
		case '\t':
			fgetc(src);
			return mtIrcToken_Get(src);
			break;
		default: {
			if(mtChar_IrcIsTokenterminator(fpeekc(src))){
				return mtIrcToken_Clone(
					&(tIrcToken){
						.type = eIrcTokentype_Wildcharater,
						.ch=fgetc(src),
					}
				);
			}else{
				char* str = mtString_Create();
				while(mtChar_IrcIsTokenterminator(fpeekc(src)))
					mtString_Append(&str,(char[2]){fgetc(src),0});
				return mtIrcToken_Clone(
					&(tIrcToken){
						.type = eIrcTokentype_Identifier,
						.string = str,
					}
				);
			};
		};
	};
	return nullptr;
};

// -- Functions --

error_t IrcArgpParser(int optiontag,char* optionvalue,struct argp_state *state){
	switch(optiontag){
		case eIrcArgpOptiontags_Outputfile:
			if(IrcTargetfilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				IrcTargetfilename=mtString_Clone(optionvalue);
			};
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
	printf("ASM:[E] IrcFileerrorfilter: Error %i·%s\n",errno,strerror(errno));
	return nullptr;
};

// -- Main loop --

void IrcFirstpassParseline(FILE* src){
	tIrcToken* token = mtIrcToken_Get(src);
	mtIrcToken_Destroy(token);
};
void IrcSecondpassParseline(FILE* src,FILE* dst){
};
void IrcCompilefile(FILE* src, FILE* dst){
	//
#ifdef qvGDebug
	printf("ASM:[D] First pass \n");
#endif
	while(!feof(src))IrcFirstpassParseline(src); // First pass
	if(fseek(src,0,SEEK_SET))
		printf(
			"ASM:[F] Unable to rewind source file to start for second pass: Error %i·%s\n",
			errno,
			strerror(errno)
		);
#ifdef qvGDebug
	printf("ASM:[D] Second pass \n");
#endif
	while(!feof(src))IrcSecondpassParseline(src,dst); // Second pass
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
	// Produce target filename if absent
	FILE* srcfile = fopen(IrcSourcefilename,"r");
	if(!srcfile){
		printf(
			"ASM:[F] Unable to open source file \"%s\": Error %i·%s\n",
			IrcSourcefilename,
			errno,
			strerror(errno)
		);
	};
	FILE* dstfile = fopen(IrcTargetfilename,"w");
	if(!dstfile){
		printf(
			"ASM:[F] Unable to open destination file \"%s\": Error %i·%s\n",
			IrcTargetfilename,
			errno,
			strerror(errno)
		);
	};
	IrcCompilefile(
		srcfile,
		dstfile
	);
	fclose(srcfile);
	fclose(dstfile);
	return 0;
};

