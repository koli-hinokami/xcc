#include "hyperheader.h"

FILE* src;
FILE* dst;

char** PrEnvironment;
typedef struct {
	char* name;
	tList /* char* */ arguments;
	char* resolved;
} tPrMacrodefinition;
tList /* tPrMacrodefinition */ tPrMacroses;

char fetchcharater2(){
	return fgetc(src);
};
char fetchcharater(){
	char c = fetchcharater2();
	if(c==13){ // i hate working with windows \r•\n line termination from cygwin
	           // which excepts unix \n line terminator
		return fetchcharater();	
	}else{
		return c;
	};
};
char peekcharater(){
	return ungetc(fgetc(src),src);
};
bool PrCheckEndoffile(){
	return feof(src);
};
bool mtChar_PrIsautowhitespace(char self){
	if(self=='_')return false;
	if(isspace(self))return true;
	if(ispunct(self))return true;
	return false;
};
void PrDefine(char* definition){
	// Define a macro
	if(mtString_Contains(definition,' ')){
		// oh god a real macro
		if(0)printf("PR: [D] Defining macro: \"%s\"\n",definition);
		// Define some variables
		char* macroname = mtString_Clone(definition);
		char* arguments2= mtString_FindcharFirst_Lambda(macroname,mtChar_PrIsautowhitespace);
		char* arguments = mtString_FindcharFirst(macroname,'(');
		char* value     = mtString_FindcharFirst(macroname,')');
		bool parametrized;
		// If parameters are absent
		if(arguments2[0]=='('){
			parametrized=true;
		}else{
			parametrized=false;
			value=arguments2;
		};
		//printf("PR: [T] Pointers: %p|%p|%p\n",macroname,arguments,value);
		// Logging
		if(0){
			if(parametrized){
				printf("PR: [T] Macro definition: \"%.*s[%c]%.*s[%c]%s\"\n",
					(int)((arguments-macroname)/sizeof(char)),
					macroname,
					arguments[0],
					(int)((value-arguments-1)/sizeof(char)),
					arguments+1,
					*value,
					value+1
				);
			}else{
				printf("PR: [T] Macro definition: \"%.*s[%c]%s\"\n",
					(int)((arguments2-macroname)/sizeof(char)),
					macroname,
					*arguments2,
					arguments2+1
				);
			};
		};
		// Some adjusting
		if(parametrized){
			*arguments++=0; *value++=0; // split into substrings
			mtString_Trimtrailingspace(macroname);
		}else{
			*value++=0;
		};
		while(*value==' ')value++;
		// More logging
		if(parametrized){
			if(0)printf("PR: [T] Parsed macro definition: %s•%s•%s•%s\n",
				parametrized?"true":"false",
				macroname,
				arguments,
				value
			);
		}else{
			if(0)printf("PR: [T] Parsed macro definition: %s•%s•%s\n",
				parametrized?"true":"false",
				macroname,
				value
			);
		};
		// Decisionmaking
		if(parametrized){
			// Macro with arguments
			if(0)printf("PR: [T] Defining parametrized macro \"%s\" with arguments \"%s\" and value \"%s\"\n",
				macroname,arguments,value
			);
			// todo
			tPrMacrodefinition* macrodef = malloc(sizeof(tPrMacrodefinition));
			macrodef->name = macroname;
			macrodef->arguments=*mtString_Splitat_Modify(arguments,',');
			macrodef->resolved=value;
			mtList_Prepend(&tPrMacroses,macrodef);
		}else{
			// Plain parameterless macro
			tPrMacrodefinition* macrodef = malloc(sizeof(tPrMacrodefinition));
			macrodef->name = macroname;
			macrodef->arguments.first=nullptr;
			macrodef->arguments.last=nullptr;
			macrodef->resolved=value;
			mtList_Prepend(&tPrMacroses,macrodef);
			if(0)printf("PR: [T] Parameterless macro %s•%s \n",
				macroname,value
			);
		};
		// Take parameters list
		// Take name
		// Take definition
		// Record all this mess
	}else{
		// got away, definitionless macro
		if(0)printf("PR: [T] Defining definitionless macro \"%s\"\n",definition);
		tPrMacrodefinition* macrodef = malloc(sizeof(tPrMacrodefinition));
		macrodef->name = mtString_Clone(definition); // revisit later
		macrodef->arguments.first=nullptr;
		macrodef->arguments.last=nullptr;
		macrodef->resolved=nullptr;
		mtList_Prepend(&tPrMacroses,macrodef);
	};
};
void handledirective(){
	char commandbuffer[256];
	char command[256];
	char arguments[256];
	int i;
	for(i=0;(commandbuffer[i++]=fetchcharater())!='\n';commandbuffer[i+1]=0);
	commandbuffer[  i]=0;
	commandbuffer[--i]=0;
	// Logging
	//printf("PR: [T] Preprocessor directive \"%s\"\n",commandbuffer);
	// Take command
	for(i=0;!isspace(commandbuffer[i])&&commandbuffer[i];i++){
		command[i]=commandbuffer[i];command[i+1]=0;
	};
	//printf("PR: [T]  Command \"%s\"\n",command);
	if(0){
		if(commandbuffer[i]){
			printf("PR: [T]  Full string \"%.*s[%c]%s\"\n",
				i,
				commandbuffer,
				commandbuffer[i]!='\n'?commandbuffer[i]:'@',
				commandbuffer+i+1
			);
		}else{
			printf("PR: [T]  Full string \"%.*s\"\n",
				i,
				commandbuffer
			);
		};
	};
	// Arguments
	for(;isspace(commandbuffer[i]);i++);
	strcpy(arguments,commandbuffer+i);
	// Commands
	if(strcmp(command,"define")==0){
		// Skip whitespaces
		//printf("PR: [T]  Define issued: \"%s\"\n",arguments);
		PrDefine(arguments);
		// todo
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
	//printf("PR: [T] Test \n");
	switch(buf1=fetchcharater()){
		case '#':
			handledirective();
			break;
		case '\n':
			break;
		case -1:
			break;
		default:
			//printf("PR: [T] Char \'%c\'\n",buf1);
			// Handle comments
			while((buf1!='\n')&&(!feof(src))){
				if(buf1=='/'){
					if(peekcharater()=='/'){
						//Single-line comment
						while((buf1!='\n')&&(!feof(src)))buf1=fetchcharater();
						if(buf1!=-1)fputc(buf1,dst);
						return;
					}else{
						
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
void PriLogmacroses2(tPrMacrodefinition* definition){
	char* packedarguments = mtString_Clone("{");
	if(mtList_Count(&definition->arguments)){
		for(
			tListnode /* <char* */ * ptr = definition->arguments.first;
			ptr;
			ptr=ptr->next
		){
			mtString_Append(&packedarguments, ptr->item);
			mtString_Append(&packedarguments, ",");
		};
		mtString_Trimlast(packedarguments);
	};
	mtString_Append(&packedarguments, "}");
	printf("PR: [M] |  Macro %s•%s•%s \n",
		definition->name,
		packedarguments,
		definition->resolved
	);
};
void PrLogmacroses(){ // print defined macroses
	printf("PR: [M] . Printing defined macroses \n");
	mtList_Foreach(&tPrMacroses,(void(*)(void*))PriLogmacroses2);
	printf("PR: [M] ' (end) %i macroses defined \n",
		mtList_Count(&tPrMacroses)
	);
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
						printf("PR: [T] test \n");
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
									};
									break;
								default:
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
	PrEnvironment=envp;
	printf("PR: [M] XCC Preprocessor: \"%s\"->\"%s\"\n",argv[1],argv[2]);
	printf("PR: [M] Usage: s t r i c t l y  xcc-preprocess <src> <dst> <switches>\n");
	if(argc>3){
		assert(argv[3][0]=='-');
		for(char* i = argv[3]+1;i[0];i++){
			if(i[0]=='m'){ // 'Mode' - use system-wide preprocessor instead
				char** arguments = calloc(sizeof(char*),argc-4+4+1);
				memcpy(arguments+4,argv+4,sizeof(char*)*(argc-4));
				arguments[0] = "cpp";
				arguments[1] = "-o";
				arguments[2] = argv[2];
				arguments[3] = argv[1];
				execvp(
					"cpp", // The system-wide preprocessor
					arguments
				);
			};
		};
	};
	src = fopen(argv[1],"r");
	dst = fopen(argv[2],"w");
	//Do the main preprocessing job
	preprocess( /* src,dst */ );
	PrLogmacroses();
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
	//while((shortbuffer=fgetc(src))!='\n')fputc(shortbuffer,out);
	//while(!feof(src))fwrite(&buffer,1,fread(&buffer,1,16,src),dst);
	//Finalize
	fclose(src);
	fclose(dst);
};
