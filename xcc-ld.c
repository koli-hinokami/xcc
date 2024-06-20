#include "hyperheader.h"

// -- Types --

typedef enum LdArgpOptiontags {
	// This enumeration should have all the options passable
	eLdArgpOptiontags_Architecture = 'a',
	eLdArgpOptiontags_Confdir      = 'c',
	eLdArgpOptiontags_Outputfile   = 'o',
} eLdArgpOptiontags;

// -- Forward declarations --

error_t LdArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --

tList /* <tLdInstructiondefinition> */ LdInstructionsdefined;

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

char* LdSourcefilename;
char* LdTargetfilename;
char* LdArchitecturename;
char* LdConfigdir;

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
		case eLdArgpOptiontags_Architecture:
			if(LdArchitecturename){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdArchitecturename=mtString_Clone(optionvalue);
			};
			break;
		case eLdArgpOptiontags_Outputfile:
			if(LdTargetfilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdTargetfilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_ARG:
			if(LdSourcefilename){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdSourcefilename=mtString_Clone(optionvalue);
			};
			break;
		case ARGP_KEY_END:
			if(!LdSourcefilename){
				argp_error(state,"No source file specified");
			};
			// Produce target filename if absent
			if(!LdTargetfilename){
				LdTargetfilename = mtString_Clone(LdSourcefilename);
				if(
					  (mtString_FindcharLast(LdTargetfilename,'.')!=nullptr)
					&&(mtString_FindcharLast(LdTargetfilename,'.')!=LdSourcefilename)
				){
					mtString_FindcharLast(LdTargetfilename,'.')[0]=0;
				};
				mtString_Append(&LdTargetfilename,".obj");
			};
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
	// Open archdef
	FILE* archdeffile = fopen(
		mtString_Join(
			LdConfigdir?:"/etc/xcc/",
			mtString_Join(
				LdArchitecturename,
				".irc"
			)
		),
		"r"
	);
	if(!archdeffile){
		printf(
			"LD: [F] Unable to open archdef for arch \"%s\": Error %i·%s\n",
			LdArchitecturename,
			errno,
			strerror(errno)
		);
		exit(1);
	};
	// Read archdef
	// Dump archdep
	// Open source/dest files
	FILE* srcfile = fopen(LdSourcefilename,"rb");
	if(!srcfile){
		printf(
			"LD: [F] Unable to open source file \"%s\": Error %i·%s\n",
			LdSourcefilename,
			errno,
			strerror(errno)
		);
	};
	FILE* dstfile = fopen(LdTargetfilename,"wb");
	if(!dstfile){
		printf(
			"LD: [F] Unable to open destination file \"%s\": Error %i·%s\n",
			LdTargetfilename,
			errno,
			strerror(errno)
		);
	};
	// Compile
	while(fpeekc(srcfile)!=EOF)
		fputc(fgetc(srcfile),dstfile);
	// Close it all
	fclose(srcfile);
	fclose(dstfile);
	return 0;
};
