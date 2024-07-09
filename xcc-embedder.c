#include "hyperheader.h"

FILE* src;
FILE* dst;

char* EmSourcefilename;
char* EmTargetfilename;
char* EmConfigdir;
char* EmArchitecturename;

typedef struct {
	FILE* handle;
	char* path; // todo
} tPrIncludelistentry;

tList /* <FILE*> */ includelist;

tPrIncludelistentry* PrGetCurrentinclude(){
	return (tPrIncludelistentry*)(mtList_GetFirstitem(&includelist));
};
char fetchcharater2(){
	if(mtList_Isempty(&includelist)){
		// No includes pending
		//printf("EM: [T] Test fetchcharater noincludes\n");
		return fgetc(src);
	}else{
		// Pending include
		//printf("EM: [T] Test fetchcharater includes\n");
		char c=fgetc(PrGetCurrentinclude()->handle);
		if(c==-1){
			//eof, mf!
			mtList_Removefirst(&includelist);
			return fetchcharater2();
		}else{
			//a normal charater
			return c;
		}
		//return fgetc(((tPrIncludebufferentry*)includelist.first->item)->file);
	}
};
char fetchcharater(){
	char c = fetchcharater2();
	if(c==13){ // i hate working with windows \n•\r line termination from cygwin
	          // which excepts unix \n line terminator
		return fetchcharater();	
	}else{
		return c;
	};
};
char peekcharater(){
	if(mtList_Isempty(&includelist)){
		//No includes pending 
		return ungetc(fgetc(src),src);
	}else{
		//Pending include
		char c=fgetc(PrGetCurrentinclude()->handle);
		if(c==-1){
			//eof, mf!
			mtList_Removefirst(&includelist);
			return peekcharater();
		}else{
			//a normal charater
			ungetc(c,PrGetCurrentinclude()->handle);
			return c;
		}
		//return fgetc(((tPrIncludebufferentry*)includelist.first->item)->file);
	}
};
bool PrCheckEndoffile(){
	if(mtList_Isempty(&includelist)){
		//No includes pending 
		return feof(src);
	}else{
		//Pending include
		if(feof(PrGetCurrentinclude()->handle)){
			// End of file on included file, possible pending data
			// on files underneath
			mtList_Removefirst(&includelist);
			return PrCheckEndoffile();
		}else{
			// Pending data on included file
			return false;
		}
	}
	
};
void PrIncludefile(char* file){
	FILE* fhandle = nullptr;
	char* filename = mtString_Trimfirst(mtString_Trimlast(mtString_Clone(file)));
	char* fpath = nullptr;
	if(file[0]=='<'){
		// search for file in standard headers path
		fpath = mtString_Join(
			mtString_Join(
				"/etc/xcc/",
				mtString_Join(
					EmArchitecturename,
					"/"
				)
			),
			filename
		);
		fhandle = fopen(fpath,"r");
		if(!fhandle){
			// File doesn't exist in target arch libc headers -> try 
			// freestanding libc
			fpath = mtString_Join(
				"/etc/xcc/include/",
				filename
			);
			fhandle = fopen(fpath,"r");
		};
	}else if(file[0]=='"'){
		// search for file in preprocessed file path
		fpath = mtString_Join("./",filename);
		fhandle = fopen(fpath,"r");
	}else{
		// wtf
		printf("EM: [E] PrIncludefile: Unknown \"#include\" file starting symbol: %c•%s\n",file[0],file);
		//exit(1);
	}
	if(fhandle==nullptr){
		printf("EM: [E] PrIncludefile(\"%s\"): Failed to open file: %i•%s\n",
			file, errno, strerror(errno)
		);
	}else{
		tPrIncludelistentry* include = malloc(sizeof(tPrIncludelistentry));
		include->handle=fhandle;
		include->path=fpath;
		mtList_Prepend(&includelist,include);
	};
};
void handledirective(){
	//Here I should find a way to get headers for handling `#include <>`.
	char commandbuffer[256];
	char command[256];
	char arguments[256];
	int i;
	int commentongoing = 0;
	for(i=0;(commandbuffer[i++]=fetchcharater())!='\n';commandbuffer[i+1]=0);
	commandbuffer[  i]=0;
	commandbuffer[--i]=0;
	// . Handle comments - PrHandleline() doesn't handle comments in case of a
	// ' preprocessor directive
	for(i=0;commandbuffer[i];i++){
		if(commandbuffer[i]=='/'){
			commentongoing++;
		}else{
			commentongoing=0;
		};
		if(commentongoing==2){
			commandbuffer[i-2]=0;
			commandbuffer[i+1]=0;
			commandbuffer[i]=0;
		};
	};
	// The log
	//printf("EM: [T] Preprocessor directive \"%s\"\n",commandbuffer);
	// Take command
	for(i=0;!isspace(commandbuffer[i])&&commandbuffer[i];i++){
		command[i]=commandbuffer[i];command[i+1]=0;
	};
	//printf("EM: [T]  Command \"%s\"\n",command);
	if(commandbuffer[i]){
		if(0)printf("EM: [T]  Full string \"%.*s[%c]%s\"\n",
			i,
			commandbuffer,
			commandbuffer[i]!='\n'?commandbuffer[i]:'@',
			commandbuffer+i+1
		);
	}else{
		if(0)printf("EM: [T]  Full string \"%.*s\"\n",
			i,
			commandbuffer
		);
	};
	// TODO: Make commands like include work
	if(strcmp(command,"include")==0){
		// Skip whitespaces
		for(;isspace(commandbuffer[i]);i++);
		strcpy(arguments,commandbuffer+i);
		//printf("EM: [T]  Including file \"%s\"\n",arguments);
		PrIncludefile(arguments);
	}else{
		//printf("EM: [T]  Deferring directive as preprocessor's and not embedder's\n");
		fputc('#',dst);
		for(i=-1;commandbuffer[++i]!=0;fputc(commandbuffer[i],dst));
		fputc('\n',dst);
	};
};

//void checkeof(FILE* handle){
//	if(feof(handle)){
//		exit;
//	};
//};
//void PrHandleline2(){ // todo
//	// Alternative linepump - reads the string first
//	char buf;
//	char stringbuffer[1024];
//	if(feof(src))return;
//	buf=fetchcharater();
//	if(feof(src))return;
//	if(buf=='\n')return;
//	for(
//		int i=0;
//		(buf!='\n')&&(!feof(src));
//		buf=fetchcharater()
//	){
//		stringbuffer[i]=buf;
//	};
//	
//};
void PrHandleline(){
	char buf1;
	if(feof(src))return;
	//printf("EM: [T] Test \n");
	switch(buf1=fetchcharater()){
		case '#':
			handledirective();
			break;
		case '\n':
			fputc('\n',dst);
			break;
		case -1:
			break;
		default:
			//printf("EM: [T] Char \'%c\'\n",buf1);
			// Handle comments
			while((buf1!='\n')&&(!feof(src))){
				if(buf1=='/'){
					if(peekcharater()=='/'){
						//Single-line comment
						while((buf1!='\n')&&(!feof(src)))buf1=fetchcharater();
						if(buf1!=-1)fputc(buf1,dst);
						return;
					}else if(peekcharater()=='*'){
						// Multiline comment
						do{
							while((fetchcharater()!='*')&&(!feof(src)));
						}while((fetchcharater()!='/')&&(!feof(src)));
						fputc(' ',dst); // standard compliance!
						if(peekcharater()=='\n')fputc('\n',dst);
						return;
					}else{	
						// do nothing - fputc is issued below
					};
				}
				fputc(buf1,dst);
				buf1=fetchcharater();
			};
			fputc('\n',dst);
			//for(fputc(buf1,dst);buf1!='\n';fputc(buf1,dst))buf1=fgetc(src);
			break;
	};
};

void preprocess(){
	char buf1,buf2;
	ungetc('\n',src);
	while(!feof(src))PrHandleline();
	if(0){
		buf1=fgetc(src);
		if(feof(src))return;
		switch(buf1){
			case '\n':
				switch(buf2=fgetc(src)){
					case '#': // Preprocessor directives
						handledirective();
						break;
					default:
						fputc('\n',dst);
						ungetc(buf2,src);
				}
				break;
		case '/':
				switch(buf2=fgetc(src)){
					case '/': // Single-line comments
						while(fgetc(src)!='\n');
						fputc('\n',dst);
						break;
					case '*': // Multiline comments
						for(bool done=true;done;){
							switch(fgetc(src)){
								case '*':
									switch(fgetc(src)){
										case '/':
											done=false;
											break;
										default:
											break;
									};
									break;
								default:
									break;
							};
						}
						break;
					default:
						fputc('/',dst);
						fputc(buf2,dst);
				}
				break;
			case -1:
				break;
			default:
				fputc(buf1,dst);
		};
	};
};

int main(int argc, char* argv[], char** envp){
	printf("EM: [M] xcc Embedder\n");
	printf("EM: [M] Usage: xcc-embedder -a <arch> <arguments> <src> <dst> \n");
	int aindex=1;
	// Help and usage
	if(strcmp(argv[1],"--help")==0){
		printf("EM: [M]  Usage:   xcc -a archname -o outfile sourcefile \n");
		printf("EM: [M]   \n");
		printf("EM: [M]  Options:\n");
		printf("EM: [M]   -a=archname            Architecture to compile for - what did \n");
		printf("EM: [M]                           you except from a crosscompiler? \n");
		printf("EM: [M]   -o=outfile             Output file. Ignored for now.\n");
		printf("EM: [M]   -c=configdir           Directory with configuration files.\n");
		printf("EM: [M]                           Default isn't specified here but should be \n");
		printf("EM: [M]                           /etc/xcc/. \n");
		printf("EM: [M]  \n");
	}else if(strcmp(argv[1],"--usage")==0){
		printf("EM: [M]  Usage:   xcc -a archname -o outfile sourcefile \n");
		printf("EM: [M]   \n");
		printf("EM: [M]  Options:\n");
		printf("EM: [M]   -a=archname            Architecture to compile for - what did \n");
		printf("EM: [M]                           you except from a crosscompiler? \n");
		printf("EM: [M]   -o=outfile             Output file. Ignored for now.\n");
		printf("EM: [M]   -c=configdir           Directory with configuration files.\n");
		printf("EM: [M]                           Default isn't specified here but should be \n");
		printf("EM: [M]                           /etc/xcc. \n");
	};
	//Command-line options
	EmSourcefilename=0;
	for(;aindex<argc;aindex++){
		assert(aindex<argc);
		printf("Em: [T] argp: parsing option %s \n",argv[aindex]);
		if(argv[aindex][0]!='-'){
			if(!EmSourcefilename){
				printf("EM: [T] argp: catched sourcefile %s\n",argv[aindex]);
				EmSourcefilename=argv[aindex];
			}else if(!EmTargetfilename){
				printf("EM: [T] argp: catched outfile %s\n",argv[aindex]);
				EmTargetfilename=argv[aindex];
			}else{
				printf("EM: [T] argp: unrecognized argument %s\n",argv[aindex]);
				assert(false);
				exit(2);
			}
		}else{
			if(argv[aindex][1]=='c'){       // Configdir
				EmConfigdir = argv[++aindex];
			}else if(argv[aindex][1]=='a'){ // Architecture
				printf("EM: [T] argp: catched architecture %s\n",argv[aindex+1]);
				EmArchitecturename = argv[aindex+1];
				++aindex;
			}else if(argv[aindex][1]=='o'){ // Outfile
				if(EmTargetfilename)
					printf("EM: [F] Two output file names - %s and %s\n",
						EmTargetfilename,argv[aindex+1]);
				printf("EM: [T] argp: catched outfile %s\n",argv[aindex+1]);
				EmTargetfilename=argv[aindex+1];
				++aindex;
			}else if(argv[aindex][1]!='-'){ // Others
				printf("Em: [E] Unrecognized option %s \n",argv[aindex]);
				exit(2);
			}else{
				//Multicharater flag
				assert(false);
				exit(2);
			}
		}
	};
	printf("EM: [D] Compiling %s->%s for %s\n",
		EmSourcefilename,EmTargetfilename,EmArchitecturename);
	src = fopen(EmSourcefilename,"r");
	dst = fopen(EmTargetfilename,"w");
	//Do the main preprocessing job
	preprocess( /* src,dst */ );
	/*
	char shortbuffer,shortbuffer2;
	char buffer[16];
	while(!feof(src)){
		if(feof(src))break;
		switch(shortbuffer=fgetc(src)){
			case '#':
				//!TODO: Handle preprocessor directives!
				//skip them for now
				while(fgetc(src)!='\n');
				continue;
			default:
				if(feof(src))goto finalize;
				fputc(shortbuffer,dst);
				break;
		};
		if(feof(src))break;
		for(shortbuffer=' ';shortbuffer!='\n';){
			shortbuffer=fgetc(src);
			if(feof(src))goto finalize;
			if(shortbuffer=='/'){
				if((shortbuffer2=fgetc(src))=='/'){//if a one-line comment
					while(fgetc(src)!='\n');//go until end of line
					fputc('\n',dst);//put the newline back
				}else{
					if(feof(src))goto finalize;
					fputc(shortbuffer,dst);
					fputc(shortbuffer2,dst);
				};
			}else{
				fputc(shortbuffer,dst);
			};
		};
	};
	*/
	while(!feof(src)){
		
	};
//finalize:;
	//while((shortbuffer=fgetc(src))!='\n')fputc(shortbuffer,out);
	//while(!feof(src))fwrite(&buffer,1,fread(&buffer,1,16,src),dst);
	//Finalize
	fclose(src);
	fclose(dst);
};
