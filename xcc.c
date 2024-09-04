#include "hyperheader.h"

	// For flags
typedef struct {
	bool last;
	bool singlechar;
	char* str;
	bool passed;
	intptr_t defaultvalue;
	intptr_t value;
} tLnFlagdefinition;

tLnFlagdefinition tLfFlags[] = {
//	 last    s-ch.  str   given   def.   assigned  
	{false,  true,  "m",  false,  false, false    }, // Mode - use multistage
	{true,   false, "",   false,  false, false    }
};

char* szLnOutputfile = "";
char* szLnRundirectory = "";

intptr_t tLnArgsGetflag(char* flag){
	//printf("DBG:tLnArgsGetflag(\"%s\")\n",flag);
	for(int i=0;!tLfFlags[i].last;i++){
		//printf("DBG:tLnArgsGetflag(\"%s\"): index %i\n",flag,i);
		//printf("DBG:tLnArgsGetflag(\"%s\"): trying strcmp(\"%s\",\"%s\"):%i\n",flag,
		//	flag,tLfFlags[i].str,strcmp(tLfFlags[i].str,flag)
		//);
		if(strcmp(tLfFlags[i].str,flag)==0){
			// Flag entry found
			//printf("DBG:tLnArgsGetflag(\"%s\"): flag entry found\n",flag);
			//printf("DBG:tLnArgsGetflag(\"%s\"): boolean values: %i, %i, %i\n",flag, 
			//	tLfFlags[i].passed,tLfFlags[i].value,tLfFlags[i].defaultvalue
			//);
			if(tLfFlags[i].passed){
				return tLfFlags[i].value;
			}else{
				return tLfFlags[i].defaultvalue;
			};
		};
	};
	return 0;
};

bool tLnArgsPassed(char* flag){
	for(int i=0;!tLfFlags[i].last;i++){
		if(strcmp(tLfFlags[i].str,flag)){
			// Flag entry found
			return tLfFlags[i].passed;
		};
	};
	return false;
};

bool tLnArgsSetflag(char* flag, intptr_t value){
	//printf("DBG:tLnArgsSetflag(\"%s\",%i)\n",flag,value);
	for(int i=0;!tLfFlags[i].last;i++){
		if(strcmp(tLfFlags[i].str,flag)==0){
			// Flag entry found
			tLfFlags[i].passed = true;
			tLfFlags[i].value = value;
			return true;
		};
	};
	return false;
};



void runprogram(char* program, char** argv){
	printf("Ln: [M] Running: ");	//a test mid-line comment
	for(int i=0;argv[i]!=0;i++){
		printf("%s ",argv[i]);
	};
	printf("\n");
	{
		int pid,err;
		if(!(pid=fork())){
			execv(program,(char * const *)argv);
			//printf("Ln: [M] Exitcode: %i\n",execv(program,(char * const *)argv));
			exit(1);
		}else{
			waitpid(pid,&err,null);
			if(err){
				fprintf(stderr,"Ln: [M] Exitcode [%i: %s∙%i %s] on program \"%s\"\n",err,err>=256?"user":"posix",err>=256?err/256:err,
					 err==  EOVERFLOW?"segfault"
					:err==  ENOTSUP?"failed assertion"
					:err==1*256?"generic user error"
					:err==4*256?"internal debug"
					:err==5*256?"internal debug"
					:"",
					program
				);
			};
			printf("Ln: [M] Exitcode: %i: %s∙%i %s\n",err,err>=256?"user":"posix",err>=256?err/256:err,
				  err==  139?"segfault"
				 :err==  134?"failed assertion"
				 :err==1*256?"generic user error"
				 :err==4*256?"internal debug"
				 :err==5*256?"internal debug"
				 :""
			);
		};
	};
	//printf("Ln: [M] Exitcode: %i\n",spawnv(_P_WAIT,program,(const char * const *)argv));
}
char * /* dynamic */ trimextension(char* /* borrows */ file){
	//Gives you ownership of dynamic memory, borrows argument
	int len=(int)(mtString_FindcharLast(file,'.')-file);
	char* str=malloc(len+1);
	assert(str);
	memcpy(str,file,len);
	*(str+len)=0;
	return str;
}
void parsefile(char* program, char* filename, char* ext1, char* ext2, char* args){
	char* argv[5];
	fflush(stdout);
	
	argv[0]=mtString_Join(szLnRundirectory,program);
	argv[1]=mtString_Join(filename,ext1);
	argv[2]=mtString_Join(filename,ext2);
	argv[3]=0;
	argv[4]=0;
	runprogram(mtString_Join(szLnRundirectory,program),(char**)(&argv));
	free(argv[1]);
	free(argv[2]);
	fflush(stdout);
};

void compile(char* file){
	fflush(stdout);
	printf("Ln: [M] Compiling \"%s\"\n",file);
	char* filename = trimextension(file);
	parsefile("xcc-embedder"   ,filename,".c"  ,".cem",0);
	parsefile("xcc-preprocess" ,filename,".cem",".cpr",0);
	if(!tLnArgsGetflag("m")){
		//Use singlestage compiler
		parsefile("xcc-singlestage",filename,".cpr",".obj",0);
	}else{
		//Use multistage compiler
		parsefile("xcc-tokenizer"  ,filename,".cpr",".ctk",0);
		parsefile("xcc-lexer"      ,filename,".ctk",".clx",0);
		parsefile("xcc-symbolgen"  ,filename,".clx",".sym",0);
		parsefile("xcc-untypedef"  ,filename,".clx",".cut",0);
		parsefile("xcc-unstruct"   ,filename,".cut",".blx",0);
		parsefile("xcc-irmaker"    ,filename,".blx",".ir" ,0);
	}
	// IR Compiler and assembler currently are used by Singlestage compiler
	// as well
	parsefile("xcc-ircompiler" ,filename,".ir" ,".asm",0); 
	parsefile("xcc-asm"        ,filename,".asm",".obj",0);
	parsefile("xcc-ld"        ,filename,".obj",".exe",0);
	
	free(filename);
	
}

int main (int argc,char* argv[]) {
	//setvbuf(stdout,null,_IONBF,0);
	//printf("\n\n");
	printf("Ln: [M] XCC Retargetable C Compiler\n");
	printf("Ln:     The frontend\n");
	printf("Ln:     Version 0.9.12.0.universal.jam1-ir\n");
	// ^ if you are not defining the directives in your Makefile: 
	//printf("Ln: Version 1.0.1.0.undefined.homelab.160101-0800\n");
	printf("Ln:     ----------------------------------------------------\n");
	//The xcc(.exe) frontend is responsible for NO compile-related
	//tasks - it only launches:
	// ¤ xcc-embedder    *.c  ->*.cem  Embedder - #include and alike
	// ¤ xcc-preprocess  *.cem->*.cpr  Preprocessor for stuff like #ifdef
	//                                 and #define'd macroses
	// ¤ xcc-singlestage *.cpr->*.obj  Singlestage compiler
	// ¤ xcc-tokenizer   *.cpr->*.ctk  Tokenizer
	// ¤ xcc-lexer       *.ctk->*.clx  Lexer - for things such as 
	//                                 recognizing whether an identifier 
	//                                 is typedef'd, struct, localvar or
	//                                 label - globalvariable/function,
	//                                 as well as expression compilation
	// ¤ xcc-symbolgen   *.clx->*.sym  Symbol list generator
	// ¤ xcc-untypedef   *.clx->*.cut  Untypedef'er - bakes in typedef's
	// ¤ xcc-unstruct    *.cut->*.blx  Unstructer - decompiles structs
	// ¤ xcc-irmaker     *.blx->*.ir   Compiles into .ir
	// ¤ xcc-ircompiler  *.ir ->*.asm  Compiles .ir into .asm sources -
	//                                 pretty much the only target-dependent
	//                                 part of XCC
	// ¤ xcc-asm         *.asm->*.obj  Assembles .asm sources into .obj
	// ¤ xcc-ld          *.obj->*.exe  Links into executable - obviously
	//                                 might need to be changed for other
	//                                 targets
	int aindex=1;
	//Command-line options
	printf("Ln: [T] Program name is: %s\n", argv[0]);
	szLnRundirectory = mtString_Clone(argv[0]);
	if(mtString_FindcharLast(szLnRundirectory,'/')){
		mtString_FindcharLast(szLnRundirectory,'/')[1]=0;
	}else{
		free(szLnRundirectory);
		szLnRundirectory=mtString_Clone("");
	};
	printf("Ln: [T] Options:\n");
	for(;aindex<argc;aindex++){
		if(argv[aindex][0]!='-')break;
		printf("Ln: [T]    %s \n",argv[aindex]);
		if(argv[aindex][1]!='-'){
			//Single charater flag
			for(int i=1;argv[aindex][i]!=0;i++){
				if(argv[aindex][i]=='!'){ // Force flag negation
					i++;
					tLnArgsSetflag((char*)(&(char[2]){argv[aindex][i],0}),false);
				}else if(argv[aindex][i]=='o'){
					szLnOutputfile = argv[++aindex];
				}else{
					tLnArgsSetflag((char*)(&(char[2]){argv[aindex][i],0}),true);
				}
				
			};
		}else{
			//Multicharater flag
			tLnArgsSetflag(argv[aindex]+2,true);
		}
	};
	//Files for compilation
	printf("Ln: [M] Compiling:\n");
	for(;aindex<argc;aindex++){
		if(argv[aindex][0]=='-')break;
		compile(argv[aindex]);
	};
	free(szLnRundirectory);
};
