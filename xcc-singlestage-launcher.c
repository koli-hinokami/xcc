// ------------------ Launcher ------------------
//void LnRunprogram(char* program, char** argv){
//	printf("L:  Running: ");	//a test mid-line comment
//	for(int i=0;argv[i]!=0;i++){
//		printf("%s ",argv[i]);
//	};
//	printf("\n");
//	printf("L:   Exitcode: %i\n",spawnv(_P_WAIT,program,(const char * const *)argv));
//};
//char* LnTrimextension(char* file){
//	//Gives you ownership of dynamic memory, borrows argument
//	int len=(int)(mtString_FindcharLast(file,'.')-file);
//	char* str=malloc(len+1);
//	memcpy(str,file,len);
//	*(str+len)=0;
//	return str;
//};
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

void LnCompile(char* file){
	printf("L:  [M] Compiling \"%s\"\n",file);
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
	/*
	printf("L:  [D] . Printing tList<tToken> GTokenized :\n");
	for(tListnode* i=GTokenized.first;i!=nullptr;i=i->next){
		tToken* j=i->item;
		if(i->next){
			if(mtToken_HasString(j))
			printf("L:  [D] | %p->%p  .- %s \n",i,j,j->string);
			printf("L:  [D] | %p->%p % 4i:%s \n",i,j,j->type,TokenidtoName[j->type]);
		}else{
			printf("L:  [D] ' %p->%p % 4i:%s \n",i,j,j->type,TokenidtoName[j->type]);
		};
	};
	*/
	fclose(srcfile);
	// Lexical parsing
	GLexed = LxParse(GTokenized.first);
	//LfPrint_LxNode(GLexed);
	//exit(4);
	// Preparse
	GRootnamespace = mtGNamespace_Create();
	GLexed->name_space=GRootnamespace;
	GLexed = SppPreparse(GLexed,nullptr); // Semanticalpreparser produces 
	                                      // same type of tree that 
					      // Lexicalparser does
	LfPrint_LxNode(GLexed);
	// Symbol generation - includes structs
	SgUnresolvedtypes = mtList_Create();
	SgUnresolvedstructures = mtList_Create();
	SgCompilablestructures = mtList_Create();
	GStructuretypes = mtGNamespace_Create();
	SgParse(GLexed);
	LfPrint_GNamespace(GRootnamespace);
	// Symbolgen second pass
	//SgUnresolvedtypes = mtList_Create();
	SgFindunresolvedtypes(GRootnamespace);
	SgResolveunresolvedtypes();
	SgCompilestructures();
	LfPrint_GNamespace(GRootnamespace);
	// Symbolgen struct registration
	//  not done separately anymore!
	// Semantic parsing - compiles structs
	GSecondaryast = SpParse(GLexed);
	LfPrint_SpNode(GSecondaryast);
	//exit(4);
	/// The question is where `struct`s go
	// Compile
	// Write object file
};
int main(int argc,char* argv[]) {
	setvbuf(stdout,null,_IONBF,0);
	printf("L:  [M] XCC Retargetable C Compiler\n");
	printf("L:      Singlestage build - sources to object file\n");
	printf("L:      Version 1.0.1.0.gcc-x86_64-cygwin." __buildlab__ "." __timestamp__ "0\n");
	printf("L:      ----------------------------------------------------\n");
	int aindex=1; //Command-line options
	printf("L:  [T] Options:\n");
	for(;aindex<argc;aindex++){
		if(argv[aindex][0]!='-')break;
		printf("L:   [T]   %s\n",argv[aindex]);
	};
	//Files for compilation
	printf("L:  [M] Compiling:\n");
	for(;aindex<argc;aindex++){
		if(argv[aindex][0]=='-')break;
		LnCompile(argv[aindex]);
	};

};

