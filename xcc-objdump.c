#include "hyperheader.h"
// ----------- xcc Crossdevelopment suite -- Object file dumper -------------

// -- Types --

typedef enum ObjdArgpOptiontags {
	// This enumeration should have all the options passable
	eObjdArgpOptiontags_Outputfile   = 'o',
} eObjdArgpOptiontags;

typedef enum eAsmBinarytokensize {
	eAsmBinarytokensize_Lobyte = 1,
	eAsmBinarytokensize_Hibyte,
	eAsmBinarytokensize_Word,
	eAsmBinarytokensize_Label,
} eAsmBinarytokensize;

enum eAsmRelocationentrykind {
	eAsmRelocationentrykind_Terminator = 0,
	eAsmRelocationentrykind_Segmentstart = 1,
};

// -- Preprocessor constants --

#define qiObjdMaxsegments 16

// -- Forward declarations --

error_t ObjdArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --

tGTargetNearpointer ObjdCurrentposition[qiObjdMaxsegments];
unsigned ObjdCurrentsegment;

struct argp_option ObjdArgpOptions[] = {
	{	
		.name = "out",
		.key = eObjdArgpOptiontags_Outputfile,
		.arg = "outputfile.lst",
		.flags = null,
		.doc = "Output listing file. \n Written in plain text.",
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
struct argp ObjdArgpParserstruct = {
	.options = ObjdArgpOptions,
	.parser = &ObjdArgpParser,    //error_t (*)(
							     //    int key,   // option's tag
							     //    char *arg, // option's value
							     //    struct argp_state *state
							     //)
	.args_doc = "file"       // Usage
	          "\n-o out.lst file",
	.doc = 
		"Linking assembler for xcc Crossdevelopment suite"
		"\n\v"
		"If --outoption isn't specified, object file is dumped to standard "
		"output, elseways it's dumped to the file specified."
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

FILE* ObjdTargetstream;
tList /* <FILE*> */ ObjdIncludelist;

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

// -- Dumper --

void ObjdDumpfile(FILE* dstfile, char* srcfilename, FILE* srcfile){
	fprintf(dstfile," -- Object file dump: \"%s\" -- \n",srcfilename);
	while(fpeekc(srcfile)!=EOF){
		fprintf(dstfile," ");
		// Segment
		ObjdCurrentsegment = fgetc(srcfile);
		fprintf(dstfile,"%02x:",ObjdCurrentsegment);
		// Address - tracked internally
		fprintf(dstfile,"%04x "  ,ObjdCurrentposition[ObjdCurrentsegment]);
		fprintf(dstfile,"(%05u) ",ObjdCurrentposition[ObjdCurrentsegment]);
		// Size
		int size = fgetc(srcfile);
		switch(size){
			case eAsmBinarytokensize_Lobyte:
			case eAsmBinarytokensize_Hibyte:
				ObjdCurrentposition[ObjdCurrentsegment]+=1;
				break;
			case eAsmBinarytokensize_Word:
				ObjdCurrentposition[ObjdCurrentsegment]+=2;
			default:
				break;
		};
		fprintf(dstfile,"%s",
			 size==eAsmBinarytokensize_Lobyte?"lobyte "
			:size==eAsmBinarytokensize_Hibyte?"hibyte "
			:size==eAsmBinarytokensize_Word  ?"word   "
			:"?"
		);
		// Displacement
		int16_t disp  = fgetc(srcfile);
		        disp |= fgetc(srcfile)<<8;
		fprintf(dstfile,"%i",disp);
		// Relocations
		while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
			switch(fgetc(srcfile)){
				case eAsmRelocationentrykind_Terminator:     assert(false); break;
				case eAsmRelocationentrykind_Segmentstart:
					fprintf(dstfile," seg_%i",fgetc(srcfile));
					break;
					
			};
		// Terminator
		if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
		fprintf(dstfile,"\n");
	};
};

// -- Functions --

error_t ObjdArgpParser(int optiontag,char* optionvalue,struct argp_state *state){
	switch(optiontag){
		case eObjdArgpOptiontags_Outputfile:
			if(ObjdTargetstream){
				return ARGP_ERR_UNKNOWN;
			}else{
				ObjdTargetstream=fopen(optionvalue,"w");
			};
			break;
		case ARGP_KEY_ARG: {
			FILE* f = fopen(optionvalue,"rb");
			if(f==nullptr){
				printf(
					"OBJD[E] Unable to open source file %s - error %i•%s \n",
					optionvalue,errno,strerror(errno)
				);
				ErfError();
				return 0;
			};
			ObjdDumpfile(ObjdTargetstream?:stdout,optionvalue,f);
		};	break;
		case ARGP_KEY_END:
			if(ObjdTargetstream)fclose(ObjdTargetstream);
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
};
// -- Launcher aka init code --
void LnFailedassertionhandler(int signum){
	fprintf(stderr,"OBJD[F] Failed assertion catched! \n");
	ErfFatal();
};
void LnNullpointerhandler(int signum){
	fprintf(stderr,"OBJD[F] Segfault catched! \n");
	ErfFatal();
};
int main(int argc, char** argv){
	signal(SIGSEGV,LnNullpointerhandler);
	signal(SIGABRT,LnFailedassertionhandler);
	// Parse arguments
	argp_parse(
		&ObjdArgpParserstruct,
		argc,
		argv,
		0,
		0,
		0
	);
};


