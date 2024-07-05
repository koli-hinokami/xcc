#include "hyperheader.h"
// ----------------- xcc Crossdevelopment suite -- Linker -------------------
// --------- The one that links object files to binary executables ----------

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
	enum eAsmBinarytokensize size;
	int16_t displacement;
	tList /* <tLdRelocation> */ * relocations;
} tLdDataentry;

enum eAsmRelocationentrykind {
	eAsmRelocationentrykind_Terminator = 0,
	eAsmRelocationentrykind_Segmentstart = 1,
};

typedef struct {
	enum eAsmRelocationentrykind kind;
	int segment;
} tLdRelocation;

typedef enum eLdLinkerscriptentrykind {
	eLdLinkerscriptentrykind_Segment = 1,
	eLdLinkerscriptentrykind_Align,
	eLdLinkerscriptentrykind_Pad,
} eLdLinkerscriptentrykind;
typedef struct {
	// taggedunion : enum eLdLinkerscriptentrykind type {
	// |   .segment : 
	// };
	enum eLdLinkerscriptentrykind type;
	union {
		int segnumber;
	};
} tLdLinkerscriptentry;

enum eLdTokenkind {
	eLdTokenkind_Identifier = 1,
	eLdTokenkind_Charater,
	eLdTokenkind_Number,
};

typedef struct {
	enum eLdTokenkind type;
	char* str;
	int   number;
	char  ch;
} tLdToken;
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
		"Linker"
		"\n\v"
		""
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

tList /* <tLdExternlabel> */ LdExportedsymbols;
tGTargetNearpointer LdCurrentposition;
tList /* <tLdLinkerscriptentry> */ LdLinkerscript;
tList /* <FILE> */ LdSourcefiles;

// -- Auxilirally functions --

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};

bool mtChar_LdIsTokenseparator(char ch){
	return isspace(ch);
};
tLdToken* mtLdToken_Fetch(FILE* stream){
	tLdToken* self = calloc(1,sizeof(tLdToken));
	while(isspace(fpeekc(stream)))fgetc(stream); // Skip whitespace
	if(mtChar_LdIsTokenseparator(fpeekc(stream))){
		self->type = eLdTokenkind_Charater;
		self->ch   = fgetc(stream);
	}else{
		self->type = eLdTokenkind_Identifier;
		char* str = mtString_Create();
		while(!mtChar_LdIsTokenseparator(fpeekc(stream)))
			mtString_Append(&str,(char[2]){fgetc(stream),0});
		self->str = str;
		if(isdigit(*str)){
			self->type=eLdTokenkind_Number;
			char* strtol_tail = nullptr;
			self->number = strtol(str,&strtol_tail,0);
		};
	};
	return self;
};

// -- class tLdRelocation --

tLdRelocation* mtLdRelocation_CreateSegmentstart(int segment){
	tLdRelocation* self = malloc(sizeof(tLdRelocation));
	self->kind = eAsmRelocationentrykind_Segmentstart;
	self->segment = segment;
	return self;
};

tGTargetNearpointer LdGetrelocationvalue(tLdRelocation* self){
	assert(false);
	return 0;
};

// -- class tLdLinkerscriptentry --

tLdLinkerscriptentry* mtLdLinkerscriptentry_Fetchfromfile(FILE* stream){
	assert(false);
};

// -- Reading Linkerscript --

void LdReadlinkerscript(FILE* archdeffile){
	for(;;){
		// Check for whitespace and end of file
		if(fpeekc(archdeffile)==EOF)return;
		while(isspace(fpeekc(archdeffile))) fgetc(archdeffile);
		if(fpeekc(archdeffile)==EOF)return;
		// Allocate Linkerscript entry
		tLdLinkerscriptentry* self = calloc(1,sizeof(tLdLinkerscriptentry));
		// Pull token that tells the class of Linkerscript entry
		tLdToken* tok;
		tok = mtLdToken_Fetch(archdeffile);
		// Check it's type
		assert(tok->type == eLdTokenkind_Identifier);
		if(strcmp(tok->str,"segment")==0){
			// segment <id>
			self->type = eLdLinkerscriptentrykind_Segment;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Number);
			self->segnumber = tok->number;
		}else{
			assert(false); // write a proper error message here
		};
		mtList_Append(&LdLinkerscript,self);
	};
};

// -- First pass --

void LdFirstpassfile(int currentsegment, FILE* srcfile){
	int i = 0;
	while(fpeekc(srcfile)!=EOF){
		printf("LD: [T] LdFirstpass: Entry %i\n",i++);
		// Get segment
		int segment = fgetc(srcfile);
		assert(segment<qiLdMaxsegments);
		// Fetch size
		int size = fgetc(srcfile);
		// Check for label
		if(size!=eAsmBinarytokensize_Label){
			// Fetch displacement
			int16_t disp  = fgetc(srcfile);
					disp |= fgetc(srcfile)<<8;
			// Fetch relocations
			tList /* <tLdRelocation> */ relocs;
			memset(&relocs,0,sizeof(tList));
			while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
				switch(fgetc(srcfile)){
					case eAsmRelocationentrykind_Terminator:
						// wat
						assert(false);
						break;
					case eAsmRelocationentrykind_Segmentstart:
						// Relative to start of segment
						mtList_Append(
							&relocs,
							mtLdRelocation_CreateSegmentstart(
								fgetc(srcfile)
							)
						);
						break;
					default:
						// Something unrecognized
						assert(false);
						break;
				};
			// Verify that we get terminator
			if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
			// Handle Linkerscript entry
			// TODO: Actually do it
			assert(false);
		}else{
			// Handle an exported label
			assert(false);
		};
	};
};

void LdFirstpass(tLdLinkerscriptentry* self){
	switch(self->type){
		case eLdLinkerscriptentrykind_Segment:
			// Parse segment and generate exported symbols' position 
			// in that segment
			for(tListnode* j = LdSourcefiles.first;j;j=j->next)
				LdFirstpassfile(self->segnumber,j->item);
		default:
			assert(false);
			break;
	};
};

// -- Second pass --

void LdSecondpassfile(int currentsegment, FILE* srcfile, FILE* dstfile){
	int i = 0;
	while(fpeekc(srcfile)!=EOF){
		printf("LD: [T] LdSecondpass: Entry %i\n",i++);
		// Get segment
		int segment = fgetc(srcfile);
		assert(segment<qiLdMaxsegments);
		// Fetch size
		int size = fgetc(srcfile);
		// Check for label
		if(size!=eAsmBinarytokensize_Label){
			// Fetch displacement
			int16_t disp  = fgetc(srcfile);
					disp |= fgetc(srcfile)<<8;
			// Fetch relocations
			tList /* <tLdRelocation> */ relocs;
			memset(&relocs,0,sizeof(tList));
			while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
				switch(fgetc(srcfile)){
					case eAsmRelocationentrykind_Terminator:
						// wat
						assert(false);
						break;
					case eAsmRelocationentrykind_Segmentstart:
						// Relative to start of segment
						mtList_Append(
							&relocs,
							mtLdRelocation_CreateSegmentstart(
								fgetc(srcfile)
							)
						);
						break;
					default:
						// Something unrecognized
						assert(false);
						break;
				};
			// Verify that we get terminator
			if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
			// Apply relocations
			for(tListnode* i=relocs.first; i; i=i->next)
				disp += LdGetrelocationvalue(i->item);
			// . Check if segment Linkerscript tells us to emit is what we
			// ' got on hands
			if(segment==currentsegment){
				// Emit
				switch(size){
					case eAsmBinarytokensize_Lobyte:
						fputc((uint8_t)disp,dstfile);
						LdCurrentposition+=1;
						break;
					case eAsmBinarytokensize_Hibyte:
						fputc((uint8_t)(disp>>8),dstfile);
						LdCurrentposition+=1;
						break;
					case eAsmBinarytokensize_Word:
						fputc((uint8_t)(disp),dstfile);
						fputc((uint8_t)(disp>>8),dstfile);
						LdCurrentposition+=2;
						break;
					default:
						printf("LD: [E] LdSecondpassfile: "
						       "Unrecognized token size %i\n",
							   size
						);
						assert(false);
						break;
				};
			};
		}else{
			// Handle an exported label
			// TODO: Verify ignorability in second pass
			//       Should be fine, but just in case
			assert(false);
		};
	};
};

void LdSecondpass(tLdLinkerscriptentry* self){
	switch(self->type){
		case eLdLinkerscriptentrykind_Segment:
			// Emit a segment while applying relocations
			for(tListnode* j = LdSourcefiles.first;j;j=j->next)
				LdSecondpassfile(self->segnumber,j->item, LdTargetfile);
		default:
			assert(false);
			break;
	};
};

// -- Reading an object file --

//void LdReadfile(FILE* srcfile){ // Remnant of old architecture
//	// Read the file itself
//	int i = 0;
//	while(fpeekc(srcfile)!=EOF){
//		printf("LD: [T] Entry %i\n",i++);
//		// Get segment
//		int segment = fgetc(srcfile);
//		assert(segment<qiLdMaxsegments);
//		// Fetch size
//		int size = fgetc(srcfile);
//		// Check for label
//		if(size!=eAsmBinarytokensize_Label){
//			// Fetch displacement
//			int16_t disp  = fgetc(srcfile);
//					disp |= fgetc(srcfile)<<8;
//			// Fetch relocations
//			tList /* <tLdRelocation> */ relocs;
//			memset(&relocs,0,sizeof(tList));
//			while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
//				switch(fgetc(srcfile)){
//					case eAsmRelocationentrykind_Terminator:
//						// wat
//						assert(false);
//						break;
//					case eAsmRelocationentrykind_Segmentstart:
//						// Relative to start of segment
//						mtList_Append(
//							&relocs,
//							mtLdRelocation_CreateSegmentstart(
//								fgetc(srcfile)
//							)
//						);
//						break;
//					default:
//						// Something unrecognized
//						assert(false);
//						break;
//				};
//			// Verify that we get terminator
//			if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
//			// Create entry
//			tLdDataentry entry;
//			// Fill up the entry
//			memset(&entry,0,sizeof(tLdDataentry));
//			// | Set position
//			entry.offset = LdPositions[segment];
//			// | Advance position
//			LdPositions[segment]+=
//				 size==eAsmBinarytokensize_Lobyte?1
//				:size==eAsmBinarytokensize_Hibyte?1
//				:size==eAsmBinarytokensize_Word  ?2
//				:(assert(false),0);
//			// | Set size
//			entry.size = size;
//			// | Set displacement
//			entry.displacement = disp;
//			// | Set relocations
//			entry.relocations = mtList_Shallowclone(&relocs);
//			// Register in internal base
//			mtList_Append(
//				&(LdSegments[segment]),
//				memcpy(malloc(sizeof(tLdDataentry)),&entry,sizeof(tLdDataentry))
//			);
//		}else{
//			// Handle an exported label
//			assert(false);
//		};
//	};
//};

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
			// Open Linkerscript
			FILE* archdeffile = fopen(
				mtString_Join(
					LdConfigdir?:"/etc/xcc/",
					mtString_Join(
						optionvalue,
						".lds"
					)
				),
				"r"
			);
			if(!archdeffile){
				printf(
					"LD: [F] Unable to open linkerscript for arch \"%s\": Error %i·%s\n",
					optionvalue,
					errno,
					strerror(errno)
				);
				//exit(1);
			};
			// Read Linkerscript
			LdReadlinkerscript(archdeffile);
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
			FILE* srcfile = fopen(optionvalue,"rb");
			if(!srcfile){
				printf("LD: [E] Unable to open source file \"%s\": %i•%s\n",
					optionvalue,errno,strerror(errno));
				ErfError();
				return 0;
			};
			mtList_Append(&LdSourcefiles,srcfile);
		};	break;
		case ARGP_KEY_END: // Create and emit output binary
			// First pass on linker script entries - get addresses
			for(tListnode* i = LdLinkerscript.first;i;i=i->next)
				LdFirstpass(i->item);
			// Interpass
			LdCurrentposition = 0;
			// Second pass on linker script entries - emit while relocating
			for(tListnode* i = LdLinkerscript.first;i;i=i->next)
				LdSecondpass(i->item);
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
