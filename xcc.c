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

char* szLnRundirectory = "";
char* szLnConfigdir;
char* szLnArchitecture;
char* szLnOutfile;
tList /* <char*> */ LnObjectfiles;
enum{eLnIr_Legacy=1,eLnIr_Alternate=2,eLnIr_Native=3}bLnIr; //IR to use

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};

intptr_t LnArgsGetflag(char* flag){
	//printf("DBG:LnArgsGetflag(\"%s\")\n",flag);
	for(int i=0;!tLfFlags[i].last;i++){
		//printf("DBG:LnArgsGetflag(\"%s\"): index %i\n",flag,i);
		//printf("DBG:LnArgsGetflag(\"%s\"): trying strcmp(\"%s\",\"%s\"):%i\n",flag,
		//	flag,tLfFlags[i].str,strcmp(tLfFlags[i].str,flag)
		//);
		if(strcmp(tLfFlags[i].str,flag)==0){
			// Flag entry found
			//printf("DBG:LnArgsGetflag(\"%s\"): flag entry found\n",flag);
			//printf("DBG:LnArgsGetflag(\"%s\"): boolean values: %i, %i, %i\n",flag, 
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

bool LnArgsSetflag(char* flag, intptr_t value){
	//printf("DBG:LnArgsSetflag(\"%s\",%i)\n",flag,value);
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
		if(szLnRundirectory){
			program = mtString_Join(szLnRundirectory,program);
		};
		if(!(pid=fork())){
			if(szLnRundirectory) execv (program,(char * const *)argv);
			                else execvp(program,(char * const *)argv);
			fprintf(
				stderr,
				"Ln: [E] Unable to launch %s: %i•%s\n",
				program,
				errno,
				strerror(errno)
			);
			fprintf(
				stdout,
				"Ln: [E] Unable to launch %s: %i•%s\n",
				program,
				errno,
				strerror(errno)
			);
			exit(1);
		}else{
#ifdef qvGTrace
			printf("Ln: [T] Waiting on process %i\n",pid);
#endif
			waitpid(pid,&err,null);
			if(err){
				fprintf(stderr,"Ln: [M] Exitcode [%i: %s∙%i %s] on program \"%s\"\n",err,err>=256?"user":"posix",err>=256?err/256:err,
					 err==  128+SIGSEGV?"segfault"
					:err==  128+SIGABRT?"failed assertion"
					:err==1*256?"generic user error"
					:err==4*256?"internal debug"
					:err==5*256?"internal debug"
					:"",
					program
				);
			};
			printf("Ln: [M] Exitcode: %i: %s∙%i %s\n",err,err>=256?"user":"posix",err>=256?err/256:err,
				  err==  128+SIGSEGV?"segfault"
				 :err==  128+SIGABRT?"sigabort • failed assertion"
				 :err==1*256?"generic user error"
				 :err==4*256?"internal debug"
				 :err==5*256?"internal debug"
				 :""
			);
			if(err)exit(1);
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
void parsefile2(char* program, char* filename, char* ext1, char* ext2, char* args){
	// For the tools that actually properly parse arguments
	char* argv[12];
	fflush(stdout);
	
	unsigned i = 0;
	argv[i++]=mtString_Join(szLnRundirectory?:"",program);
	if(strcmp(program,"xcc-singlestage")==0){
		if(bLnIr==eLnIr_Alternate) argv[i++]="-m";
		if(bLnIr==eLnIr_Legacy)    argv[i++]="-!m";
		if(bLnIr==eLnIr_Native)    argv[i++]="-n";
	};
	argv[i++]="-a";
	argv[i++]=szLnArchitecture;
	argv[i++]="-o";
	argv[i++]=mtString_Join(filename,ext2);
	argv[i++]=mtString_Join(filename,ext1);
	argv[i++]=szLnConfigdir?"-c":0;
	argv[i++]=szLnConfigdir?:0;
	argv[i++]=0;
	argv[i++]=0;
	runprogram(program,(char**)(&argv));
	//free(argv[4]);
	//free(argv[5]);
	fflush(stdout);
};
void parsefile(char* program, char* filename, char* ext1, char* ext2, char* args){
	char* argv[5];
	fflush(stdout);
	
	argv[0]=mtString_Join(szLnRundirectory?:"",program);
	argv[1]=mtString_Join(filename,ext1);
	argv[2]=mtString_Join(filename,ext2);
	argv[3]=0;
	argv[4]=0;
	runprogram(program,(char**)(&argv));
	free(argv[1]);
	free(argv[2]);
	fflush(stdout);
};

void LnLink(tList /* <char*> */ * objfiles){
	char* argv[30];
	memset(argv,0,sizeof(argv));
	assert(mtList_Count(objfiles)<20);
	int i = 0;
	argv[i++]=mtString_Join(szLnRundirectory?:"","xcc-ld");
	if(szLnConfigdir){
		argv[i++]="-c";
		argv[i++]=szLnConfigdir;
	};
	argv[i++]="-a";
	argv[i++]=szLnArchitecture;
	argv[i++]="-o";
	argv[i++]=szLnOutfile;
	for(tListnode /* <char*> */ * j = objfiles->first;j;j=j->next)
		argv[i++]=mtString_Clone(j->item);
	runprogram("xcc-ld",argv);
};
void compile(char* file){ // Downs whatever was given to it to object file
	fflush(stdout);
	printf("Ln: [M] Compiling \"%s\"\n",file);
	char* filename = trimextension(file);
	char* extension = mtString_FindcharLast(file,'.')+1;
	assert(extension-1!=file);
	if(strcmp(extension,"c")==0){
		parsefile2("xcc-embedder"   ,filename,".c"  ,".cem",0);
		extension="cem";
	};
	if(strcmp(extension,"cem")==0){
		parsefile("xcc-preprocess" ,filename,".cem",".cpr",0);
		extension="cpr";
	};
	if(!LnArgsGetflag("m")){
		//Use singlestage compiler
		if(strcmp(extension,"cpr")==0){
			parsefile2("xcc-singlestage",filename,".cpr",".obj",0);
			extension="ir";
		};
	}else{
		//Use multistage compiler
		if(strcmp(extension,"cpr")==0){
			parsefile("xcc-tokenizer"  ,filename,".cpr",".ctk",0);
			extension="ctk";
		};
		if(strcmp(extension,"ctk")==0){
			parsefile("xcc-lexer"      ,filename,".ctk",".clx",0);
			extension="clx";
		};
		if(strcmp(extension,"clx")==0){
			parsefile("xcc-symbolgen"  ,filename,".clx",".sym",0);
			extension="clx";
		};
		if(strcmp(extension,"clx")==0){
			parsefile("xcc-semanticparser"   ,filename,".clx",".blx",0);
			extension="blx";
		};
		if(strcmp(extension,"blx")==0){
			parsefile("xcc-irmaker"    ,filename,".blx",".ir" ,0);
			extension="ir";
		};
	}
	// IR Compiler and assembler currently are used by Singlestage compiler
	// as well
	if(strcmp(extension,"ir")==0){
		parsefile2("xcc-ircompiler" ,filename,".ir" ,".asm",0); 
		extension="asm";
	};
	if(strcmp(extension,"asm")==0){
		parsefile2("xcc-assembler"  ,filename,".asm",".obj",0);
		extension="obj";
	};
	file=mtString_Clone(file);
	if(mtString_FindcharLast(file,'.')!=file)
		mtString_FindcharLast(file,'.')[0]='\0';
	mtList_Append(&LnObjectfiles,mtString_Join(file,".obj"));
}

void LnNullpointerhandler(int signum){
	fprintf(stderr,"Ln: [F] Segfault catched! \n");
	ErfFatal();
};
void LnReadarchdef(char* fname){
	ErfEnter_String("LnReadarchdef");
	assert(fname);
	fname = mtString_Join(
		"/etc/xcc/",
		mtString_Join(
			fname,
			"/archdef.ln"
		)
	);
	FILE* archdeffile = fopen(fname,"r");
	if(archdeffile==nullptr)
		ErfError_String2(mtString_Format("Ln: [E] No archdef \"%s\"\n",fname));
	// Read the archdef
	unsigned line = 0;
	while(fpeekc(archdeffile)!=EOF){
		ErfUpdate_String(mtString_Format("LnReadarchdef: Line %u",++line));
		char* str = mtString_Create();
		// Read archdef line
		while(fpeekc(archdeffile)!='\n'){
			if(fpeekc(archdeffile)==EOF)
				if(errno){
					ErfError_String2(mtString_Format(
						"Ln: [E] Unable to read archdef: "
						"Unexcepted file read error %i"
						"\n",
						(int)errno
					));
				}else{
					ErfError_String2("Ln: [E] Unable to read archdef: Unexcepted end of file: Excepted a charater or newline, got end of file\n");
				};
			mtString_Appendchar(&str, fgetc(archdeffile));
		};
		fgetc(archdeffile);
		// Read directive
		char* arg = mtString_FindcharFirst(str,' ');
		if(arg){
			// Arguments are passed
			*arg++ = 0;
		}else{
			// No arguments passed
			//arg=nullptr;
		};
		// Handle directive
		if(!*str){
			// Empty line/directive
		}else if(strcmp(str,"usealtir")==0){
			bLnIr = eLnIr_Alternate;
		}else if(strcmp(str,"useoldir")==0){
			bLnIr = eLnIr_Legacy;
		}else if(strcmp(str,"usenativeir")==0){
			bLnIr = eLnIr_Native;
		}else{
			ErfError_String2(mtString_Format(
				"Ln: [E] Unrecognized directive \"%s\"\n",str));
		}
	};
	ErfLeave();
};
int main (int argc,char* argv[]) {
	signal(SIGSEGV,LnNullpointerhandler);
	//setvbuf(stdout,null,_IONBF,0);
	//printf("\n\n");
	printf("Ln: [M] xcc Retargetable C Crosscompiler\n");
	printf("Ln:     Version 0.9.13.0.universal.jam1-ir\n");
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
	//                                 part of xcc
	// ¤ xcc-asm         *.asm->*.obj  Assembles .asm sources into .obj
	// ¤ xcc-ld          *.obj->*.exe  Links into executable - obviously
	//                                 might need to be changed for other
	//                                 targets
	int aindex=1;
	// Help and usage
	if(
		  argv[1]==null
		||strcmp(argv[1],"--usage")==0
	){
		printf("Ln: [M]  Usage:   xcc -a archname -o outfile sourcefile \n");
		printf("Ln: [M]   \n");
		printf("Ln: [M]  Options:\n");
		printf("Ln: [M]   -a=archname            Architecture to compile for - what did \n");
		printf("Ln: [M]                           you except from a crosscompiler? \n");
		printf("Ln: [M]   -o=outfile             Output file. Ignored for now.\n");
		printf("Ln: [M]   -c=configdir           Directory with configuration files.\n");
		printf("Ln: [M]                           Default isn't specified here but should be \n");
		printf("Ln: [M]                           /etc/xcc. \n");
		return 0;
	}else if(strcmp(argv[1],"--help")==0){
		printf("Ln: [M]  Usage:   xcc -a archname -o outfile sourcefile \n");
		printf("Ln: [M]   \n");
		printf("Ln: [M]  Options:\n");
		printf("Ln: [M]   -a=archname            Architecture to compile for - what did \n");
		printf("Ln: [M]                           you except from a crosscompiler? \n");
		printf("Ln: [M]   -o=outfile             Output file. Ignored for now.\n");
		printf("Ln: [M]   -c=configdir           Directory with configuration files.\n");
		printf("Ln: [M]                           Default isn't specified here but should be \n");
		printf("Ln: [M]                           /etc/xcc/. \n");
		printf("Ln: [M]  \n");
		return 0;
	};
	//Command-line options
	printf("Ln: [T] Program name is: %s\n", argv[0]);
	szLnRundirectory = mtString_Clone(argv[0]);
	if(mtString_FindcharLast(szLnRundirectory,'/')){
		mtString_FindcharLast(szLnRundirectory,'/')[1]=0;
	}else{
		free(szLnRundirectory);
		szLnRundirectory=null; // ran from path
	};
	printf("Ln: [T] Options:\n");
	for(;aindex<argc;aindex++){
		assert(argv[aindex]);
		if(argv[aindex][0]!='-')break;
		printf("Ln: [T]    %s \n",argv[aindex]);
		if(argv[aindex][1]=='c'){       // Configdir
			szLnConfigdir = argv[++aindex];
		}else if(argv[aindex][1]=='a'){ // Architecture
			szLnArchitecture = argv[++aindex];
#ifdef qvGTrace
			printf("Ln: [T] szLnArchitecture==\"%s\"\n",szLnArchitecture);
#endif
			LnReadarchdef(szLnArchitecture);
		}else if(argv[aindex][1]=='o'){ // Outfile
			szLnOutfile = argv[++aindex];
		}else if(argv[aindex][1]!='-'){ // Others
			//Single charater flag
			for(int i=1;argv[aindex][i]!=0;i++){
				if(argv[aindex][i]=='!'){ // Force flag negation
					i++;
					LnArgsSetflag((char*)(&(char[2]){argv[aindex][i],0}),false);
				}else if(argv[aindex][i]=='o'){
					szLnOutfile = argv[++aindex];
				}else{
					LnArgsSetflag((char*)(&(char[2]){argv[aindex][i],0}),true);
				}
			};
		}else{
			//Multicharater flag
			LnArgsSetflag(argv[aindex]+2,true);
		}
	};
	if(!szLnArchitecture){
		printf("Ln: [F] No architecture specified! \n");
		exit(1);
	};
	//Implied output file name
	if(!szLnOutfile){
		assert(argv[aindex]);
		szLnOutfile=mtString_Clone(argv[aindex]);
		if(mtString_FindcharLast(szLnOutfile,'.')!=szLnOutfile)
			mtString_FindcharLast(szLnOutfile,'.')[0]='\0';
		mtString_Append(&szLnOutfile,".bin");
	};
	//Files for compilation
	printf("Ln: [M] Compiling:\n");
	for(;aindex<argc;aindex++){
		assert(argv[aindex]);
		if(argv[aindex][0]=='-')break;
		compile(mtString_Clone(argv[aindex]));
	};
	//Link it all
	printf("Ln: [M] Linking:\n");
	LnLink(&LnObjectfiles);
	printf("Ln: [M] Done compiling\n");
	if(szLnRundirectory)free(szLnRundirectory);
};
