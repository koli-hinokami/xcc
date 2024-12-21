#include "hyperheader.h"
//----- xcc Crossdevelopment suite -- Intermediate Representation fuser -----

// -- Macroses --

#define MfClone(self) (memcpy(malloc(sizeof(typeof(self))),self,sizeof(typeof(self))))

// -- Types --

typedef struct {
	char* instr1;
	char* instr2;
	char* targetinstr;
} tIrfFusedictentry;

typedef enum IrcArgpOptiontags {
	// This enumeration should have all the options passable
	eIrcArgpOptiontags_Architecture = 'a',
	eIrcArgpOptiontags_Confdir      = 'c',
	eIrcArgpOptiontags_Outputfile   = 'o',
} eIrcArgpOptiontags;

// -- Forward declarations --

error_t IrcArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --

tList /* <tIrfFusedictentry*> */ IrfFusedictionary;

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
		.arg = "outputfile.irf",
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
	.args_doc = "file.ir"      // Usage
	          "\n-o out.irf file",
	.doc = 
		"Intermediate Representation Fuser for xcc."
		"\n\v"
		"If --output option isn't specified, extension is replaced with "
		".irf (if present, otherwise created) to get output file name. "
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

bool mtChar_IsTokenterminator(char self){
	if(isalnum(self)){return false;};  //Alphanumeric - usual charater
	if(self=='_')return false;         //Underscore - allowed in tokens
	if(self=='.')return false;         //`.` - This is a macroassembler so why not
	if(self==':')return false;         //`:` - A hack for label detection
	return true;                       //The rest are 'auto-whitespace' chars that begin another token
};
bool mtChar_IsWhitespace(char self){
	if(self=='\n') return false;        //This is an assembler (of sorts), so 
	                                    // newlines get special treatment
	return isspace(self); // Aside from newlines, this is libc isspace
};

// -- class FILE --

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return ungetc(ch,self);
};

// -- Tokenizer --

char* IrfGettoken(FILE* src){
	// Skip initial whitespace
	while(mtChar_IsWhitespace(fpeekc(src))) if(fgetc(src)==EOF) assert(false);
	// Read token
	char* tok = mtString_Create();
	//mtString_Append(&tok,(char[2]){fgetc(src),0});
	if(fpeekc(src)==';'){ // comment
		while(fgetc(src)!='\n');
		return IrfGettoken(src);
	}
	mtString_Append(&tok,(char[2]){fgetc(src),0}); // a bypass
	while(!mtChar_IsTokenterminator(fpeekc(src)))
		mtString_Append(&tok,(char[2]){fgetc(src),0});
	// Skip trailing whitespace
	while(mtChar_IsWhitespace(fpeekc(src))) if(fgetc(src)==EOF) assert(false);
	return tok;
}

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
				mtString_Append(&IrcTargetfilename,".irf");
			};
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
};

// -- Instruction defining --

// -- Main loop --

void IrcCompilefile(FILE* src, FILE* dst){
	//
#ifdef qvGDebug
	printf("IRF:[D] Parsing \n");
#endif
	// Skip initial whitespace
	while(fpeekc(src) == ' ' || fpeekc(src) == '\t') fgetc(src);
	// Main loop
	char* fusebuffer = nullptr;
	tList /* <char*> */ fuseoperands[1] = {};
	char* opcode = nullptr;
	tList /* <char*> */ operands[1] = {};
	while(fpeekc(src)!=EOF){
		//tIrfInstruction newinstr = mtIrfInstruction_Fetch(src);
		//if(fuseable(fusebuffer,newinstr)){
		//	fusebuffer=IrfFuseinstructions(fusebuffer,newinstr);
		//}else{
		//	emit(fusebuffer);
		//	fusebuffer=newinstr;
		//}
		// Read token
		char* tok = IrfGettoken(src);
		assert(tok);
		if(mtString_Getlast(tok)==':'){
			// Label read
			mtString_Trimlast(tok);
#ifdef qvGTrace
			printf("IRF:[T] IrfGettoken: Read label \"%s\"\n",tok);
#endif
			// Flush fusebuffer
			if(fusebuffer){
				fprintf(dst,"\t%s\t",fusebuffer);
				for(tListnode /* <char**> */ * i = fuseoperands->first;i;i=i->next)
					fprintf(dst,"%s%s",(char*)i->item,i->next?",\t":"");
				*fuseoperands=*operands;
				*operands=(tList){};
				fusebuffer=nullptr;
				fprintf(dst,"\n");
			}
			// Mirror label
			fprintf(dst,"%s:\n",tok);
		}else{
			// Opcode
			opcode = tok;
#ifdef qvGTrace
			printf("IRF:[T] IrfGettoken: Read opcode \"%s\"\n",opcode);
#endif
			// Read operands
			while(fpeekc(src)!='\n') {
				char* tok = IrfGettoken(src);
				if(strcmp(tok,",")==0) continue;
#ifdef qvGTrace
				printf("IRF:[T] IrfGettoken: Read operand \"%s\"\n",tok);
#endif
				mtList_Append(operands,tok);
			}
			assert(fgetc(src)=='\n');
			// Fuse
			if(!fusebuffer){
				// No instruction to fuse
				fusebuffer=opcode;
				*fuseoperands=*operands;
				*operands=(tList){};
			}else{
				tIrfFusedictentry* result = nullptr;
				for(tListnode /* <tIrfFusedictentry*> */ * i = IrfFusedictionary.first;i;i=i->next){
					tIrfFusedictentry* j = i->item;
					if(
						   (strcmp(j->instr1,fusebuffer)==0)
						&& (strcmp(j->instr2,opcode)==0)
					){
						// Found!
						assert(!result);
						result = j;
					}
				}
				if(result){
					// Fuse!
#ifdef qvGTrace
					printf("IRF:[T] Fusing %s•%s→%s\n",result->instr1,result->instr2,result->targetinstr);
#endif
					fusebuffer=result->targetinstr;
					for(tListnode /* <char**> */ * i = operands->first;i;i=i->next)
						mtList_Append(fuseoperands,i->item);
					*operands=(tList){};
				}else{
					// No fusion - emit and recharge
					fprintf(dst,"\t%s\t",fusebuffer);
					for(tListnode /* <char**> */ * i = fuseoperands->first;i;i=i->next)
						fprintf(dst,"%s%s",(char*)i->item,i->next?",\t":"");
					*fuseoperands=*operands;
					*operands=(tList){};
					fprintf(dst,"\n");
					fusebuffer=opcode;
				}
			}
		}
		while(fpeekc(src)=='\n') {
			printf("IRF:[T] IrfGettoken: Wrapping\n");
			fgetc(src);
			while(fpeekc(src) == ' ' || fpeekc(src) == '\t') if(fgetc(src)==EOF) return;
		}
	}
	// Flush fusebuffer
	if(fusebuffer){
		fprintf(dst,"\t%s\t",fusebuffer);
		for(tListnode /* <char**> */ * i = fuseoperands->first;i;i=i->next)
			fprintf(dst,"%s%s",(char*)i->item,i->next?",\t":"");
		*fuseoperands=*operands;
		*operands=(tList){};
		fusebuffer=nullptr;
		fprintf(dst,"\n");
	}
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
				"/archdef.irc"
			)
		),
		"r"
	);
	if(!archdeffile){
		printf(
			"IRF:[F] Unable to open archdef for arch \"%s\": Error %i·%s\n",
			IrcArchitecturename,
			errno,
			strerror(errno)
		);
		exit(1);
	};
	// Read archdef
	//mtList_Prepend(
	//	&IrfFusedictionary,
	//	memcpy(
	//		malloc(sizeof(tIrfFusedictentry)),
	//		&(tIrfFusedictentry){
	//			.instr1="lit",
	//			.instr2="+",
	//			.targetinstr="lit+"
	//		},
	//		sizeof(tIrfFusedictentry)
	//	)
	//);
#ifdef qvGTrace
	printf("IRF:[T] Reading archdef\n");
#endif
	while(fpeekc(archdeffile)!=EOF){
		char* tok = IrfGettoken(archdeffile);
#ifdef qvGTrace
		printf("IRF:[T] Archdef line %s\n",tok);
#endif
		if(strcmp(tok,"fusion")==0){
			// Our directive!
			char* s1 = IrfGettoken(archdeffile);
			char* s2 = IrfGettoken(archdeffile);
			char* s3 = IrfGettoken(archdeffile);
			while(fpeekc(archdeffile)!='\n') if(fgetc(archdeffile)==EOF) break;
			fgetc(archdeffile);
			mtList_Append(
				&IrfFusedictionary,
				memcpy(
					malloc(sizeof(tIrfFusedictentry)),
					&(tIrfFusedictentry){
						.instr1=s1,
						.instr2=s2,
						.targetinstr=s3,
					},
					sizeof(tIrfFusedictentry)
				)
			);
		}else{
			// Not our directive! Drop it, drop it!
			while(fpeekc(archdeffile)!='\n') if(fgetc(archdeffile)==EOF) break;
			fgetc(archdeffile);
		}
	};
	// Dump archdef
#ifdef qvGTrace
	if(1)for(tListnode * i = IrfFusedictionary.first;i;i=i->next){
		tIrfFusedictentry * j = i->item;
		printf("IRF:[T] Fuse %s•%s→%s\n",j->instr1,j->instr2,j->targetinstr);
	};
#endif
	// Open source/dest files
	FILE* srcfile = fopen(IrcSourcefilename,"r");
	if(!srcfile){
		printf(
			"IRF:[F] Unable to open source file \"%s\": Error %i·%s\n",
			IrcSourcefilename,
			errno,
			strerror(errno)
		);
	};
	FILE* dstfile = fopen(IrcTargetfilename,"w");
	if(!dstfile){
		printf(
			"IRF:[F] Unable to open destination file \"%s\": Error %i·%s\n",
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

