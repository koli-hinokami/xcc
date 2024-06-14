#include "hyperheader.h"
// -- Types --
typedef struct {
	char* name;
	tList /* char* */ arguments;
	char* resolved;
} tPrMacrodefinition;
// -- Globals --
FILE* src;
FILE* dst;

tList /* tPrMacrodefinition */ PrMacroses;
tList /* bool */ PrPendingifs;
// -- Forwarddeclarations --
bool mtChar_PrIsTokenterminator(char self);

// -- Source/destination file interface --
char PrFetchcharater(){
	char c = fgetc(src);
	if(c==13){ // i hate working with windows \r•\n line termination from cygwin
	           // which excepts unix \n line terminators
		printf("PR: [F]  N o   W i n d o w s   l i n e   e n d i n g s !  \n");
		exit(1);
		return fgetc(src);	
	}else{
		return c;
	};
};
char PrPeekcharater(){
	return ungetc(fgetc(src),src);
};
bool PrEndoffile(){
	int i = ungetc(fgetc(src),src);
	if(i==EOF){
		return true;
	};
	return false;
};
void PrEmitcharater(char ch){
	fputc(ch,dst);
};
// -- Support primitives --
bool mtChar_PrIsTokenterminator(char self){
	if(self=='_')return false;
	if(isspace(self))return true;
	if(ispunct(self))return true;
	return false;
};
char* /* __needs_free */ PrGettoken(){
	//
#ifdef qvGTrace
	//printf("PR: [T] PrGettoken: Entered\n");
#endif
	char* buffer = mtString_Clone("");
	// Special cases like line merging
	if(PrPeekcharater()=='\\'){
		PrFetchcharater();
		if(PrPeekcharater()=='\n'){
			// You know what? I think we'll return escaped 
			// line break as a token.
			PrFetchcharater();
			mtString_Appendchar(&buffer,'\n'); 
			return buffer;
		};
		// Nothing special. Pass backslash through as a token.
		mtString_Appendchar(&buffer,'\\'); 
		return buffer;
	};
	// Skip whitespaces
#ifndef __CYGWIN__
	while(
		  (PrPeekcharater()==' ')
		||(PrPeekcharater()=='\t')
	)PrEmitcharater(PrFetchcharater());
#endif
	// A token is at least one charater
	mtString_Appendchar(&buffer,PrFetchcharater()); 
	// Cygwin skip whitespaces
#ifdef __CYGWIN__
	while(
		  (buffer[0]==' ')
		||(buffer[0]=='\t')
	){
		PrEmitcharater(buffer[0]);
		mtString_Trimfirst(buffer);
	};
#endif
	// And other charaters until an operator is seen
	while(!mtChar_PrIsTokenterminator(PrPeekcharater())){
		mtString_Appendchar(&buffer,PrFetchcharater()); 
	};
	return buffer;
};
// -- Conditionals --
bool PriIsDefined_Internalclojure(char* macroname, tPrMacrodefinition* macrodef){
	return strcmp(macrodef->name,macroname)==0;
};
bool PrIsDefined(char* macroname){
	return mtList_Find_Clojure(
		&PrMacroses,
		(bool(*)(void*,void*))PriIsDefined_Internalclojure,
		macroname
	)!=nullptr;
};
bool PrGetIgnorelines(){
	for(                                       // For
		tListnode* i = PrPendingifs.first;     //  Pending ifs
		i;                                     //  To the very top
		i=i->next                              //  Sequentially
	) 
		if(!(bool)i->item)                     //  If said if block evaulated to false
			return true;                       //   Suppress lines
	return false ;                             //   As a default, let lines through.
};
bool PrEvaulatecondition(char* condstring){
	printf("PR: [F] Unimplemented function PrEvaulatecondition \n");
	ErfError();
	return true;
};
bool PrMacrofinderclojure(
	char* token,
	tPrMacrodefinition* attemptedmacro
){
	if(attemptedmacro)
		if(attemptedmacro->name)
			if(
				strcmp(
					attemptedmacro->name,
					token
				)==0
			)
				return true;
	return false;
};
// -- Directives --
void PrDefine(char* definition){
	// Define a macro
	if(mtString_Contains(definition,' ')){
		// oh god a real macro
		if(0)printf("PR: [D] Defining macro: \"%s\"\n",definition);
		// Define some variables
		char* macroname = mtString_Clone(definition);
		char* arguments2= mtString_FindcharFirst_Lambda(macroname,mtChar_PrIsTokenterminator);
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
		if(0)if(parametrized){
			printf("PR: [T] Parsed macro definition: %s•%s•%s•%s\n",
				parametrized?"true":"false",
				macroname,
				arguments,
				value
			);
		}else{
			printf("PR: [T] Parsed macro definition: %s•%s•%s\n",
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
			mtList_Prepend(&PrMacroses,macrodef);
		}else{
			// Plain parameterless macro
			tPrMacrodefinition* macrodef = malloc(sizeof(tPrMacrodefinition));
			macrodef->name = macroname;
			macrodef->arguments.first=nullptr;
			macrodef->arguments.last=nullptr;
			macrodef->resolved=value;
			mtList_Prepend(&PrMacroses,macrodef);
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
		mtList_Prepend(&PrMacroses,macrodef);
	};
};
bool PriPreprocessdirective(char* command,char* arguments){
#ifdef qvGTrace
	//printf("PR: [T] PriPreprocessdirective(%s,%s): entered\n",command,arguments);
#endif
	if(strcmp(command,"define")==0){
		// Skip whitespaces
		//printf("PR: [T]  Define issued: \"%s\"\n",arguments);
		PrDefine(arguments);
		return true;
	};
	return false;
};
bool PriPreprocesscrucialdirective(char* command,char* arguments){
#ifdef qvGTrace
	//printf("PR: [T] PriPreprocesscrucialdirective(%s,%s): entered\n",command,arguments);
#endif
	if(strcmp(command,"if")==0){
		mtList_Prepend(&PrPendingifs,(void*)PrEvaulatecondition(arguments));
		return true;
	};
	if(strcmp(command,"ifdef")==0){
		mtList_Prepend(&PrPendingifs,(void*)PrIsDefined(arguments));
		return true;
	}
	if(strcmp(command,"ifndef")==0){
		mtList_Prepend(&PrPendingifs,(void*)!PrIsDefined(arguments));
		return true;
	}
	if(strcmp(command,"else")==0){
		PrPendingifs.first->item=(void*)(!(bool)PrPendingifs.first->item);
		return true;
	};
	if(strcmp(command,"endif")==0){
		mtList_Removefirst(&PrPendingifs);
		return true;
	};
	//if(strcmp(command,"define")==0){
	//	//printf("PR: [T]  Define issued: \"%s\"\n",arguments);
	//	PrDefine(arguments);
	//	// todo
	//};
	return false;
};
void PrPreprocesscrucialdirective(char* /* modifies */ directive){
#ifdef qvGTrace
	//printf("PR: [T] PrPreprocesscrucialdirective(%s): Entered\n",directive);
#endif
	char command[256];
	char arguments[256];
	memset(command,0,sizeof(command));
	memset(arguments,0,sizeof(arguments));
	char* i = directive;
	// Skip all spaces
	while((*i!=0) && (isspace(*i)))i++;
	// Get command
	for(char* j = command;(*i!=0) && (!isspace(*i));*j++=*i++)j[1]=0;
	// Skip all spaces
	while((*i!=0) && (isspace(*i)))i++;
	// Get arguments
	for(char* j = arguments;*i!=0;*j++=*i++)j[1]=0;
	// Handle commands
	if(PriPreprocesscrucialdirective(command,arguments))return;
};
void PrPreprocessdirective(char* /* modifies */ directive){
#ifdef qvGTrace
	//printf("PR: [T] PrPreprocessdirective(%s): Entered\n",directive);
#endif
	char command[256];
	char arguments[256];
	memset(command,0,sizeof(command));
	memset(arguments,0,sizeof(arguments));
	char* i = directive;
	// Skip all spaces
	while((*i!=0) && (isspace(*i)))i++;
	// Get command
	for(char* j = command;(*i!=0) && (!isspace(*i));*j++=*i++)j[1]=0;
	// Skip all spaces
	while((*i!=0) && (isspace(*i)))i++;
	// Get arguments
	for(char* j = arguments;*i!=0;*j++=*i++)j[1]=0;
	// Handle commands
	if(PriPreprocesscrucialdirective(command,arguments))return;
	if(PriPreprocessdirective(command,arguments))return;
	printf("PR: [W] Unrecognized directive %s•%s\n",command,arguments);
};
// -- Macro expansion --
char* PrTransformtoken(char* token){
	//
#ifdef qvGTrace
	//printf("PR: [T] PrTransformtoken(%s): Entered\n",token);
#endif
	tPrMacrodefinition * matchingmacro = mtList_Find_Clojure(
		&PrMacroses,
		(bool(*)(void*,void*))PrMacrofinderclojure,
		(void*)token
	);
	if(matchingmacro){
#ifdef qvGTrace
		if(0)printf("PR: [T] PrTransformtoken: A token happily transformed (%s)->(%s)\n",
			token,matchingmacro->resolved
		);
#endif
		return mtString_Clone(matchingmacro->resolved);
	};
	return nullptr;
};
void PrEmittoken(char* str){
	mtString_Foreach_Clojure2_Precasttoint(str,(void(*)(int,void*))fputc,dst);
	//fputc(' ',dst);
};
void PrIgnoretoken(){
	//
#ifdef qvGTrace
	//printf("PR: [T] PrIgnoretoken: Entered\n");
#endif
	// PrHandletoken() but with all of emitting commented out
	char* token = PrGettoken();
	//char* macroedtoken = PrTransformtoken(token);
	//if(macroedtoken){
	//	free(token);
	//	token = macroedtoken;
	//};
	//PrEmittoken(token);
	free(token);
};
void PrHandletoken(){
	//
#ifdef qvGTrace
	//printf("PR: [T] PrHandletoken: Entered\n");
#endif
	char* token = PrGettoken();
	char* macroedtoken = PrTransformtoken(token);
	if(macroedtoken){
#ifdef qvGTrace
		printf("PR: [T] PrHandletoken: Macro-transformed token\n");
#endif
		free(token);
		token = macroedtoken;
	};
	PrEmittoken(token);
	free(token);
};

// -- Main loop --
void PrHandleline(){
	//
#ifdef qvGTrace
	//printf("PR: [T] PrHandleline: Entered\n");
#endif
	if(PrPeekcharater()=='#'){
		PrFetchcharater();
		// Preprocessor directive
		char* directive = mtString_Clone("");
		while(PrPeekcharater()!='\n'){
			mtString_Appendchar(
				&directive,
				PrFetchcharater()
			);
		};
		PrFetchcharater();
		if(PrGetIgnorelines()){
			PrPreprocesscrucialdirective(directive);
		}else{
			PrPreprocessdirective(directive);
		};
		free(directive);
	}else{
		// Normal line
		if(PrGetIgnorelines()){
			while(PrPeekcharater()!='\n'){
				PrIgnoretoken();
			};
			PrEmitcharater(PrFetchcharater());
		}else{
			while(PrPeekcharater()!='\n'){
				PrHandletoken();
			};
			PrEmitcharater(PrFetchcharater());
		};
	};
};
void PrHandlefile(){
	while(!PrEndoffile())PrHandleline();
}
// -- Launcher --
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
	mtList_Foreach(&PrMacroses,(void(*)(void*))PriLogmacroses2);
	printf("PR: [M] ' (end) %i macroses defined \n",
		mtList_Count(&PrMacroses)
	);
};
int main(int argc,char** argv){
	printf("PR: [M] xcc Preprocessor: \"%s\"->\"%s\"\n",argv[1],argv[2]);
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
	PrHandlefile();
	PrLogmacroses();
	fclose(src);
	fclose(dst);
};
