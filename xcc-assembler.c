#include "hyperheader.h"

// -- Types --

typedef enum AsmArgpOptiontags {
	// This enumeration should have all the options passable
	eAsmArgpOptiontags_Architecture = 'a',
	eAsmArgpOptiontags_Outputfile   = 'o',
} eAsmArgpOptiontags;

typedef struct {
	char* /* deallocatable */ label;
	char* /* deallocatable */ instruction;
	tList /* <char* deallocatable> */ * /* deallocatable */ operands;
} tAsmSourcecodeline;
// -- Forward declarations --

error_t AsmArgpParser(
	int key, 
	char* argumentvalue, 
	struct argp_state *state
);

// -- Globals --

struct argp_option AsmArgpOptions[] = {
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
	          "\nfile.asm file.obj (internal, dont use)"
	          "\n-o out.obj file",
	.doc = 
		"Your usual crossassembler. Not a macroassembler though - "
		"use IR Compiler from the same xcc suite."
		"\n\v"
		"If --out option isn't specified, extension is replaced with "
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

// -- class tAsmSourcecodeline --


// -- Functions --

error_t AsmArgpParser(
	int optiontag, 
	char* optionvalue, 
	struct argp_state *state
){
	switch(optiontag){
		case eAsmArgpOptiontags_Outputfile:
			if(AsmTargetfilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				AsmTargetfilename=mtString_Clone(optionvalue);
			};
		case ARGP_KEY_ARG:
			if(AsmSourcefilename){
				if(AsmTargetfilename){
					return ARGP_ERR_UNKNOWN;
				}else{
					AsmTargetfilename=mtString_Clone(optionvalue);
				};
			}else{
				AsmSourcefilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_END:
			if(!AsmSourcefilename){
				argp_error(state,"No source file specified");
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
void AsmFirstpassParseline(FILE* src){
	//tAsmSourcecodeline* line = AsmGetline(src);
	//mtAsmSourcecodeline_Destroy(line);
};
void AsmSecondpassParseline(FILE* src,FILE* dst){
	//tAsmSourcecodeline* line = AsmGetline(src);
	//mtAsmSourcecodeline_Destroy(line);
};
void AsmCompilefile(FILE* src, FILE* dst){
#ifdef qvGDebug
	printf("ASM:[D] First pass \n");
#endif
	while(!feof(src))AsmFirstpassParseline(src); // First pass
	if(fseek(src,0,SEEK_SET))
		printf(
			"ASM:[F] Unable to rewind source file to start for second pass: Error %i·%s\n",
			errno,
			strerror(errno)
		);
#ifdef qvGDebug
	printf("ASM:[D] Second pass \n");
#endif
	while(!feof(src))AsmSecondpassParseline(src,dst); // Second pass 
};

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
	argp_parse(
		&AsmArgpParserstruct,
		argc,
		argv,
		0,
		0,
		0
	);
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
	FILE* srcfile = fopen(AsmSourcefilename,"r");
	if(!srcfile){
		printf(
			"ASM:[F] Unable to open source file \"%s\": Error %i·%s\n",
			AsmSourcefilename,
			errno,
			strerror(errno)
		);
	};
	FILE* dstfile = fopen(AsmTargetfilename,"w");
	if(!dstfile){
		printf(
			"ASM:[F] Unable to open destination file \"%s\": Error %i·%s\n",
			AsmTargetfilename,
			errno,
			strerror(errno)
		);
	};
	AsmCompilefile(
		srcfile,
		dstfile
	);
	fclose(srcfile);
	fclose(dstfile);
	return 0;
};
