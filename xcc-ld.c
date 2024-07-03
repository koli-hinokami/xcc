#include "hyperheader.h"
// ----------------- xcc Crossdevelopment suite -- Linker -------------------

// -- Types --

typedef enum LdArgpOptiontags {
	// This enumeration should have all the options passable
	eLdArgpOptiontags_Architecture = 'a',
	eLdArgpOptiontags_Confdir      = 'c',
	eLdArgpOptiontags_Outputfile   = 'o',
} eLdArgpOptiontags;

typedef enum eAsmBinarytokensize {
	eAsmBinarytokensize_Lobyte = 1,
	eAsmBinarytokensize_Hibyte,
	eAsmBinarytokensize_Word,
	eAsmBinarytokensize_Label,
} eAsmBinarytokensize;

typedef struct {
	tGTargetSegment segment;
	tGTargetNearpointer offset;
} tLdDataentry;

enum eAsmRelocationentrykind {
	eAsmRelocationentrykind_Terminator = 0,
	eAsmRelocationentrykind_Segmentstart = 1,
};

// -- Preprocessor constants --

#define qiLdMaxsegments 16

// -- Forward declarations --

error_t LdArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --

struct argp_option LdArgpOptions[] = {
	{	
		.name = "cfgdir",
		.key = eLdArgpOptiontags_Confdir,
		.arg = "confdir",
		.flags = null,
		.doc = "Directory to search for config files in."
		       "Defaults to /etc/xcc.",
		.group = null,
	},
	{	
		.name = "arch",
		.key = eLdArgpOptiontags_Architecture,
		.arg = "archname",
		.flags = null,
		.doc = "The architecture to compile assembly sources for. "
		       "Retargetablity first!",
		.group = null,
	},
	{	
		.name = "out",
		.key = eLdArgpOptiontags_Outputfile,
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
struct argp LdArgpParserstruct = {
	.options = LdArgpOptions,
	.parser = &LdArgpParser,    //error_t (*)(
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

FILE* LdTargetfile;
char* LdArchitecturename;
char* LdConfigdir;

tList /* <tLdDataentry> */ LdSegments[qiLdMaxsegments];

// -- Auxilirally functions --

// -- class FILE --

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};

// -- Reading an object file --

void LdReadfile(FILE* srcfile){
	int i = 0;
	while(fpeekc(srcfile)!=EOF){
		printf("LD: [T] Entry %i\n",i++);
		// Get segment
		int segment = fgetc(srcfile);
		assert(segment<qiLdMaxsegments);
		// Fetch size
		int size = fgetc(srcfile);
		// Fetch displacement
		int16_t disp  = fgetc(srcfile);
		        disp |= fgetc(srcfile)<<8;
		// Fetch relocations
		while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
			switch(fgetc(srcfile)){
				case eAsmRelocationentrykind_Terminator:
					assert(false); 
					break;
				case eAsmRelocationentrykind_Segmentstart:
					assert(false); 
					break;
				default:
					assert(false);
					break;
			};
		// Verify that we get terminator
		if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
		// Register in internal base
		assert(false);
	};
};
void LdCompileoutputbinary(void){
	fprintf(stdout,"LD: [D] Generating symbols \n");
	assert(false);
	fprintf(stdout,"LD: [D] Emitting binary\n");
	assert(false);
};

// -- Functions --

error_t LdArgpParser(int optiontag,char* optionvalue,struct argp_state *state){
	switch(optiontag){
		case eLdArgpOptiontags_Confdir:
			if(LdConfigdir){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdConfigdir=mtString_Clone(optionvalue);
			};
			break;
		case eLdArgpOptiontags_Architecture: {
			// Open archdef
			FILE* archdeffile = fopen(
				mtString_Join(
					LdConfigdir?:"/etc/xcc/",
					mtString_Join(
						optionvalue,
						".ld"
					)
				),
				"r"
			);
			if(!archdeffile){
				printf(
					"LD: [F] Unable to open archdef for arch \"%s\": Error %i·%s\n",
					optionvalue,
					errno,
					strerror(errno)
				);
				//exit(1);
			};
			// Read archdef
			// TODO: Actually read archdef
		};	break;
		case eLdArgpOptiontags_Outputfile:
			if(LdTargetfile){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdTargetfile = fopen(optionvalue,"wb");
				if(!LdTargetfile){
					printf("LD: [E] Unable to open output file \"%s\": %i•%s\n",
						optionvalue,errno,strerror(errno));
					ErfError();
					return 0;
				};
			};
			break;
		case ARGP_KEY_ARG: {
			if(!LdTargetfile){
				printf("LD: [E] Output file needs to be specified before source files\n");
				exit(2);
			};
			// Read file into memory
			FILE* srcfile = fopen(optionvalue,"rb");
			if(!srcfile){
				printf("LD: [E] Unable to open source file \"%s\": %i•%s\n",
					optionvalue,errno,strerror(errno));
				ErfError();
				return 0;
			};
			fprintf(stdout,"LD: [D] Reading object file \"%s\"\n",optionvalue);
			LdReadfile(srcfile);
		};	break;
		case ARGP_KEY_END:
			// Output the files
			LdCompileoutputbinary();
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
};
FILE* LdFileerrorfilter(FILE* self){
	if(self)return self;
	printf("LD: [E] LdFileerrorfilter: Error %i·%s\n",errno,strerror(errno));
	return nullptr;
};

// -- Main loop --

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
		&LdArgpParserstruct,
		argc,
		argv,
		0,
		0,
		0
	);
	// Today main loop is in LdArgpParser
	return 0;
};
