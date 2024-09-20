// ------------------ Launcher ------------------
//void LnRunprogram(char* program, char** argv){
//	printf("L:  Running: ");	//a test mid-line comment
//	for(int i=0;argv[i]!=0;i++){
//		printf("%s ",argv[i]);
//	};
//	printf("\n");
//	printf("L:   Exitcode: %i\n",spawnv(_P_WAIT,program,(const char * const *)argv));
//};
char* LnTrimextension(char* file){
	//Gives you ownership of dynamic memory, borrows argument
	int len=(int)(mtString_FindcharLast(file,'.')-file);
	char* str=malloc(len+1);
	memcpy(str,file,len);
	*(str+len)=0;
	return str;
};
//void LnParsefile(char* program, char* filename, char* ext1, char* ext2, char* args){
//	char* argv[5];
//	argv[0]=program;
//	argv[1]=mtString_Join(filename,ext1);
//	argv[2]=mtString_Join(filename,ext2);
//	argv[3]=0;
//	argv[4]=0;
//	LnRunprogram(program,(char**)(&argv));
//	free(argv[1]);
//	free(argv[2]);
//};



void LnInterrupthandler(int signum){
	fprintf(stderr,"L:  [M] Interrupted \n");
	ErfFatal();
};
void LnFailedassertionhandler(int signum){
	fprintf(stderr,"L:  [F] Failed assertion catched! \n");
	ErfFatal();
};
void LnNullpointerhandler(int signum){
	fprintf(stderr,"L:  [F] Segfault catched! \n");
	ErfFatal();
};
void LnCompile(char* file){
	printf("L:  [M] Compiling \"%s\"\n",file);
	GInitializePerfile();
	// preprocess file
	//char* filename = LnTrimextension(file);
	//LnParsefile("cpp",filename,".c",".cpr",0);
	//mtString_Append(&filename,".cpr");
	// Read file
	FILE* srcfile = fopen(file,"r");
	GSourcefile = srcfile;
	if(srcfile==nullptr){
		fprintf(stderr,"L:  [E] Skipping file \"%s\", file opening error: %i•\"%s\" \n",
			file,errno,strerror(errno)
		);
		return;
	};
	// Tokenize
	TkTokenize(srcfile);
	if(0){
		printf("L:  [D] . Printing tList<tToken> GTokenized :\n");
		for(tListnode* i=GTokenized.first;i!=nullptr;i=i->next){
			tToken* j=i->item;
			if(i->next){
				if(mtToken_HasString(j))
				printf("L:  [D] | %p->%p   .- %s \n",i,j,j->string);
				printf("L:  [D] | %p->%p % 4i:%s \n",i,j,j->type,TokenidtoName[j->type]);
			}else{
				printf("L:  [D] ' %p->%p % 4i:%s \n",i,j,j->type,TokenidtoName[j->type]);
			};
		};
	};
	fclose(srcfile);
	// Lexical parsing
	fprintf(stderr,"L:  [M] Lexicalpreparser \n");
	//LxTypesdeclared = mtList_Create();
	LxPreparse(GTokenized.first);
	LxUsedeclaredtypeslist = true;
	if(0){
		printf("L:  [D] . Printing tList<tString> LxTypesdeclared:\n");
		for(tListnode* i=LxTypesdeclared.first;i!=nullptr;i=i->next){
			if(i->next){
				printf("L:  [D] | Type \"%s\"\n",(char*)i->item);
			}else{
				printf("L:  [D] ' Type \"%s\"\n",(char*)i->item);
			};
		};
	};
	fprintf(stderr,"L:  [M] Lexicalparser \n");
	GLexed = LxParse(GTokenized.first);
	if(0){
		fprintf(stderr,"L:  [M] Printing First AST \n");
		LfPrint_LxNode(GLexed);
		fprintf(stderr,"L:  [M] Done printing First AST \n");
	};
	//exit(4);
	// Preparse
	fprintf(stderr,"L:  [M] Lexicalpostparser \n");
	GRootnamespace = mtGNamespace_Create();
	GLexed->name_space=GRootnamespace;
	GLexed = SppPreparse(GLexed,nullptr); // Semanticalpreparser produces 
	                                      // same type of tree that 
	                                      // Lexicalparser does
	if(1){
		fprintf(stderr,"L:  [M] Printing Primary AST \n");
		LfPrint_LxNode(GLexed);
		fprintf(stderr,"L:  [M] Done printing Primary AST \n");
	};
	//exit(4);
	// Symbol generation - includes structs
	fprintf(stderr,"L:  [M] Symbolgen \n");
	SgUnresolvedtypes = mtList_Create();
	SgUnresolvedstructures = mtList_Create();
	SgCompilablestructures = mtList_Create();
	GStructuretypes = mtGNamespace_Create();
	SgParse(GLexed);
	if(1){
		fprintf(stderr,"L:  [M] Printing Rootnamespace \n");
		LfPrint_GNamespace(GRootnamespace);
		fprintf(stderr,"L:  [M] Done printing Rootnamespace \n");
	};
	// Symbolgen second pass
	//SgUnresolvedtypes = mtList_Create();
	fprintf(stderr,"L:  [M] Symbolgen second pass - finding unresolved types\n");
	SgFindunresolvedtypes(GRootnamespace);
	SgFindunresolvedtypes(GStructuretypes);
	SgFindunresolvedtypes_LxNode(GLexed);
	fprintf(stderr,"L:  [M] Symbolgen second pass - resolving unresolved types\n");
	SgResolveunresolvedtypes();
	fprintf(stderr,"L:  [M] Symbolgen second pass - compiling structures\n");
	SgCompilestructures();
	if(1){
		fprintf(stderr,"L:  [M] Printing Rootnamespace \n");
		LfPrint_GNamespace(GRootnamespace);
		fprintf(stderr,"L:  [M] Done printing Rootnamespace \n");
	};
	SgNosearchfortypes = true;
	// Symbolgen struct registration
	//  not done separately anymore!
	// Semantic parsing - compiles structs
	//exit(4);
	fprintf(stderr,"L:  [M] Semanticparser\n");
	GSecondaryast = SpParse(GLexed);
	if(1){
		fprintf(stderr,"L:  [M] Printing Secondary AST \n");
		LfPrint_SpNode(GSecondaryast);
		fprintf(stderr,"L:  [M] Done printing Secondary AST \n");
	};
	//exit(4);
	/// The question is where `struct`s go
	fprintf(stderr,"L:  [M] Semanticoptimizer\n");
	GSecondaryast = SpOptimize(GSecondaryast);
	// Compile
	for(int i=0;i<meGSegment_Count;i++)GCompiled[i]=mtGInstruction_CreateCnop();
	switch(LnIr){
		assert(false);
		FILE* dstfile = nullptr;
		case eLnIr_Legacy:
			fprintf(stderr,"L:  [M] Legacy IR Generator\n");
			IgParse(GSecondaryast);
			//fprintf(stderr,"L:  [M] Printing IR\n");
			//printf("L:  [M] Code segment: \n");             LfPrint_GInstruction(GCompiled[meGSegment_Code]);
			//printf("L:  [M] Data segment: \n");             LfPrint_GInstruction(GCompiled[meGSegment_Data]);
			//printf("L:  [M] Rodata segment: \n");           LfPrint_GInstruction(GCompiled[meGSegment_Readonlydata]);
			//printf("L:  [M] Udata segment: \n");            LfPrint_GInstruction(GCompiled[meGSegment_Udata]);
			//fprintf(stderr,"L:  [M] Done printing IR\n");
			fprintf(stderr,"L:  [M] Emitting IR\n");
			dstfile = fopen(
				LnOutputfile?:mtString_Join(LnTrimextension(file),".ir"),
				"w"
			);
			IgDumpir(GCompiled, dstfile);
			fprintf(stderr,"L:  [M] Done emitting IR\n");
			break;
		case eLnIr_Alternate:
			fprintf(stderr,"L:  [M] Secondary IR Generator\n");
			Ig2Parse(GSecondaryast);
			fprintf(stderr,"L:  [M] Emitting Secondary IR\n");
			dstfile = fopen(
				LnOutputfile?:mtString_Join(LnTrimextension(file),".ir"),
				"w"
			);
			Ig2Dumpir(GCompiled, dstfile);
			fprintf(stderr,"L:  [M] Done emitting Secondary IR\n");
			fclose(dstfile);
			break;
		case eLnIr_Native:
			fprintf(stderr,"L:  [M] Codegen\n");
			// Do native codegen
			// Dump assembly
			ErfError_String2(
				"L:  [E] "
				"LnCompile: "
				"Compile: "
				"Unable to call codegen:"
				"Native codegen not implemented"
				"\n"
			);
			fprintf(stderr,"L:  [M] Done compiling\n");
			break;
		default:
			ErfError_String2("L:  [E] LnCompile: Compile: Internal failure: Unable to decide which kind of codegen to use\n");
	}
	for(int i=0;i<meGSegment_Count;i++)GCompiled[i]=mtGInstruction_CreateCnop();
};
void LnReadarchdef(char* name){
	assert(name);
	name = mtString_Join(
		"/etc/xcc/",
		mtString_Join(
			name,
			"/archdef.ss"
		)
	);
	FILE* archdeffile = fopen(name, "r");
	if(archdeffile==nullptr)
		ErfError_String2(mtString_Format("Ln: [E] No archdef \"%s\"\n",name));
	// Read the archdef
	while(fpeekc(archdeffile)!=EOF){
		char* str = mtString_Create();
		// Read archdef line
		while(fpeekc(archdeffile)!='\n'){mtString_Appendchar(&str, fgetc(archdeffile));} 
		fgetc(archdeffile);
		// Read directive
		char* arg = mtString_FindcharFirst(str,' ');
		*arg++ = 0;
		// Handle directive
		if(strcmp(str,"transform")==0){
			char* newarg;
			int srctype = strtol(arg,&newarg,0);
			while(*newarg && isspace(*newarg)) newarg++;
			assert(*newarg);
			int dsttype = strtol(newarg,nullptr,0);
			LnTypetranslationmap[srctype]=dsttype;
		}else{
			ErfError_String2(mtString_Format(
				"Ln: [E] Unrecognized directive \"%s\"\n",str));
		}
	};
}
int main(int argc,char* argv[]) {
	setvbuf(stdout,null,_IONBF,0);
	signal(SIGINT,LnInterrupthandler);
	signal(SIGSEGV,LnNullpointerhandler);
	signal(SIGABRT,LnFailedassertionhandler);
	printf("L:  [M] xcc Retargetable C Compiler\n");
	printf("L:      Singlestage build - sources to object file\n");
	printf("L:      Version 0.9.13.0.universal.jam1-ir\n");
	printf("L:      ----------------------------------------------------\n");
	int aindex=1; //Command-line options
	printf("L:  [T] Options:\n");
	for(;aindex<argc;aindex++){
		if(argv[aindex][0]!='-')break;
		switch(argv[aindex][1]){
			case '!': // Inversion. Lookahead needed.
				switch(argv[aindex][2]){
					case 'm': // -!m - use old IR
						LnIr = eLnIr_Legacy;
						break;
					default:
						printf(
							"L:  [F] Unrecognized option `-!%c`\n",
							argv[aindex][2]
						);
						exit(2);
				};
				break;
			case 'm': // -m - use new IR
				LnIr = eLnIr_Alternate;
				break;
			case 'n': // -m - use native codegen
				LnIr = eLnIr_Native;
				break;
			case 'a': // Architecture
				LnReadarchdef(argv[++aindex]);
				break;
			case 'o': // Output file
				LnOutputfile=argv[++aindex];
				break;
			case 'f': // Force
				fprintf(stderr,"L:  [W] Force is used! Things may go wrong!\n");
				ErfForce();
				break;
			case 'F': // FORCE
				fprintf(stderr,"L:  [F] -- Full force is used! Except segfaults! --\n");
				ErfFullforce();
				break;
			default:
				printf("L:  [F] Unrecognized option `-%c`\n",argv[aindex][1]);
				exit(2);
		};
		printf("L:  [T]   %s\n",argv[aindex]);
	};
	//Files for compilation
	printf("L:  [M] Compiling \"%s\"\n",argv[aindex]);
	LnCompile(argv[aindex]);
	return 0;
};

