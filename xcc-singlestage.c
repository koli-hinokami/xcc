// Reforming to be multiple files

/*
	Used namespaces:
		G	compiler Global
		Ln	Launcher
		Tk	Tokenizer
		Lx	Lexer
		Sg  Symbolgen
		Sp	Semantic Parser
		Ig	IR Generator
		Cg	Code Generator
		Lf	Logging Facilities
*/

#include "hyperheader.h"

FILE* GSourcefile;
tList GTokenized;
tLxNode* GLexed;
tGNamespace* GRootnamespace;
tSpNode* GSecondaryast;
tList /* <tGIrInstruction> */ GCompiled[/*segment*/meGSegment_Count];

int iLfIndentation = 0;
char* iLfIndentationstring =  "| | | | | | | | | | | | | | | | | | | | ";
char* szLfPrefix = "?:  [?] ";
/*
	Loglevel:
	T	Trace
	D	Debug
	M	Message
	W	Warning
	E	Error
	F	Fatal error
	O	Off - off is the highest in Log4j, shouldn't be used for messages
	     and when used for loglevel it suppresses every message except off
		 which is always not suppressed. Should be used if something *way*
		 worse than fatal error occurs.
*/
void LfWriteline(char* string){
	printf("%8.8s",string);
	for(int i=0;i<iLfIndentation;i++)printf("| ");
	printf("%s",string+8);
	//printf("%8.8s%.*s%s",
	//	string, //lvalue char * casted-> rvalue char *
	//	iLfIndentation*2, iLfIndentationstring,
	//	string+8 // lvalue const char * casted-> rvalue char *
	//);
}
void LfIndent(char* string){
	printf("%8.8s",string);
	for(int i=0;i<iLfIndentation;i++)printf("| ");
	printf(". %s",string+8);
	//printf("%8.8s%.*s. %s",
	//	string, //lvalue char * casted-> rvalue char *
	//	iLfIndentation*2, iLfIndentationstring,
	//	string+8 // lvalue const char * casted-> rvalue char *
	//);
	iLfIndentation++;
};
void LfUnindent(char* string){
	iLfIndentation--;
	printf("%8.8s",string);
	for(int i=0;i<iLfIndentation;i++)printf("| ");
	printf("\' %s",string+8);
	//printf("%8.8s%.*s\' %s",
	//	string, //lvalue char * casted-> rvalue char *
	//	iLfIndentation*2, iLfIndentationstring,
	//	string+8 // lvalue const char * casted-> rvalue char *
	//);
};

void LfiPrint_LxNode(char* pr,tLxNode* self);
void LfiPrint_GType(char* pr, tGType* self){
	char buffer[512];
	sprintf(buffer,"Lf: [M] %2s ¤ Type: \n",pr);
	LfIndent(buffer);
	if(self->atomicbasetype==eGAtomictype_Unresolved){
		sprintf(buffer,"Lf: [M]      ¤ Unresolved: \"%s\" \n",self->unresolvedsymbol);
		LfWriteline(buffer);
	}else if(self->atomicbasetype==eGAtomictype_Enumeration){ 
		sprintf(buffer,"Lf: [M]      ¤ Enumeration \n");
		LfWriteline(buffer);
		//LfiPrint_LxNode("p:",self->precompiledenumeration);
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Structure){ 
		sprintf(buffer,"Lf: [M]      ¤ Structunion \n");
		LfWriteline(buffer);
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Void){ 
		sprintf(buffer,"Lf: [M]      ¤ Atomictype: %i:%s \n",self->atomicbasetype,"void");
		LfWriteline(buffer);
		//LfPrint_GNamespace(self->structure);
	}else{
		// Resolve atomic type to string
		char* str = "unknown";
		for(
			GAtomictypetostring_Entry * ptr = GAtomictypetostring;
			ptr->str;
			ptr++
		){
			if(ptr->atomictype==self->atomicbasetype){
				str = ptr->str;
				break;
			};
		};
		sprintf(buffer,"Lf: [M]      ¤ Atomictype: %i:%s \n",self->atomicbasetype,str);
		LfWriteline(buffer);
	};
	LfUnindent("Lf: [M]   \n");
};
void LfiPrint_LxNode(char* pr,tLxNode* self){
	char buffer[512];
	char* bufptr=buffer;
	if(self==nullptr){
		//exit(5);
		sprintf(buffer,"Lf: [M] · %2s ¤ Nullpointer \n",pr);
		//LfWriteline(buffer);
	}else if(self->type==tLexem_Declarationlist){
		LfiPrint_LxNode("l:",self->left);
		LfiPrint_LxNode("r:",self->right);
	}else{
		if(self->type==tLexem_Integerconstant){
			sprintf(buffer,"Lf: [M] %2s ∙ ¤ Lexem %i:%s:%i \n",pr,self->type,TokenidtoName[self->type],self->constant);
			LfWriteline(buffer);
			return;
		};
		if(self->type==tLexem_Identifier){
			sprintf(buffer,"Lf: [M] %2s ¤ Lexem %i:%s:%s \n",pr,self->type,TokenidtoName[self->type],self->identifier);
		}else{
			sprintf(buffer,"Lf: [M] %2s ¤ Lexem %i:%s \n",pr,self->type,TokenidtoName[self->type]);
		};
		//exit(6);
		LfIndent(buffer);
		if(self->returnedtype)LfiPrint_GType("t:",self->returnedtype);
		if(
			  (self->type==tLexem_Switchcase)
			||(self->type==tLexem_Switchdefault)
		){
			LfWriteline("Lf: [M] · i: ¤ Folded bound switch \n");
		}else if(self->type==tLexem_Breakstatement){
			LfWriteline("Lf: [M] · i: ¤ Folded bound break target \n");
		}else{
			LfiPrint_LxNode("i:",self->initializer);
		};
		LfiPrint_LxNode("c:",self->condition);
		LfiPrint_LxNode("l:",self->left);
		LfiPrint_LxNode("r:",self->right);
		LfUnindent("Lf: [M]   \n");
	};
	
};
void LfPrint_LxNode(tLxNode* self){
	LfWriteline("DBG:[M] LfPrint_LxNode: Printing primary AST \n");
	LfiPrint_LxNode("",self);
	//exit(4);
};

bool mtToken_HasString(tToken* self){
	if(self->type&0x0100){
		return true;
	};
	return false;
};
size_t mtToken_Sizeof(tToken* self){
#ifndef qvmtToken_Usestringvla
	return sizeof(tToken);
#else
	if(mtToken_HasString(self)){
		return sizeof(tToken)+mtString_Length(self->string);
	}else{
		return sizeof(tToken);
	};
#endif

};
tToken* mtToken_Clone(tToken* self){
	tToken* i = malloc(mtToken_Sizeof(self));
	return memcpy(i,self,mtToken_Sizeof(self));
};

tGType* mtGType_Clone(tGType* self){
	return memcpy(malloc(sizeof(tGType)),self,sizeof(tGType));
}
tGType* mtGType_Create_String(char* str){
	return mtGType_Clone(
		&(tGType){
			.atomicbasetype=eGAtomictype_Unresolved,
			.unresolvedsymbol=str
		}
	);
}
tGType* mtGType_Compose(tGType* t1,tGType* t2){
	fprintf(stderr,"G:  [F] mtGType_Compose: Unfinished code hit! \n");
	exit(2);
};
// --------------------- Tokenizer ---------------------
#include "xcc-singlestage-tokenizer.c"

/*
struct tLxFetcher {
	tListnode<tToken> fetchto,fetchfrom;
};
	Fetcher is needed because things like {
		int, short;
		unsigned, signed char;
		struct T, enum T;
		unsigned long long;
		T<unsigned>
		tDictionary<unsigned long long,tDictionary<int,char*> >;
	} exist. Okay last one was a tiniest bit too much, but just in case.
*/
// ---------------- Fetcher ----------------
typedef struct {
	tListnode* fetchfrom;
	tListnode* fetchto;
	// `fetchto` points to point of stopping. Look into self.eof for more details.
} tLxFetcher;
tLxFetcher* mtLxFetcher_Create(tListnode* startpoint, tListnode* endpoint){
	tLxFetcher* ptr = malloc(sizeof(tLxFetcher));
	ptr->fetchfrom=startpoint;
	ptr->fetchto=endpoint;
	return ptr;
};
tLxFetcher* mtLxFetcher_Clone(tLxFetcher* self){
	return memcpy(malloc(sizeof(tLxFetcher)),self,sizeof(tLxFetcher));
};
bool mtLxFetcher_Eof(tLxFetcher* self){
	if(self->fetchfrom==self->fetchto){
		return true;
	}else{
		return false;
	}
};
tToken* mtLxFetcher_Peek(tLxFetcher* self){
	return self->fetchfrom->item;
}
tToken* mtLxFetcher_Peeklast(tLxFetcher* self){
	tListnode* i;
	for(
		i=self->fetchfrom;
		i->next!=self->fetchto;
		i=i->next
	);
	return i->item;
}
tLxFetcher* mtLxFetcher_Trimfirst(tLxFetcher* self){ // Recreates!
	return mtLxFetcher_Clone(
		&(tLxFetcher){
			.fetchfrom=self->fetchfrom->next,
			.fetchto=self->fetchto
		}
	);
};
tLxFetcher* mtLxFetcher_Trimlast(tLxFetcher* self){ // Recreates!
	tListnode* i;
	if(self->fetchfrom->next==self->fetchto){
		return mtLxFetcher_Clone(
			&(tLxFetcher){
				.fetchfrom=nullptr,
				.fetchto=nullptr
			}
		);
	};
	for(
		i=self->fetchfrom;
		i->next!=self->fetchto;
		i=i->next
	);
	return mtLxFetcher_Clone(
		&(tLxFetcher){
			.fetchfrom=self->fetchfrom,
			.fetchto=i
		}
	);
};
tToken* mtLxFetcher_Fetch(tLxFetcher* self){
	tToken* i = mtLxFetcher_Peek(self);
	if(mtLxFetcher_Eof(self)){
		//eof!
		return nullptr;
	}else{
		self->fetchfrom=self->fetchfrom->next;
	};
	return i;
};
bool mtLxFetcher_Advance(tLxFetcher* self){
	if(mtLxFetcher_Eof(self)){
		return false; // eof
	}else if(self->fetchfrom==nullptr){
		fprintf(stderr,"LX: [E] Advancing fetcher not at EOF with fetchfrom being nullptr \n");
		return false;
	}else{
		self->fetchfrom=self->fetchfrom->next;
		return true;
	};
};
tLxFetcher* mtLxFetcher_Fetchuntil_Variadic(tLxFetcher* fetcher, int tokenidcount,/*variadic tTokentype tokenid[tokenidcount]*/...){
	bool found = false;
	tLxFetcher* targetfetcher = malloc(sizeof(tLxFetcher));
	targetfetcher->fetchfrom=fetcher->fetchfrom;
	tListnode/*<tToken>*/* tempptr;
	for(
		tempptr = targetfetcher->fetchfrom;
		tempptr!=null;
		tempptr=tempptr->next
	){
		va_list args;
		va_start(args,tokenidcount);
		for(int index=0;index<tokenidcount;index++){
			tTokentype arg = va_arg(args,/*tTokentype*/int);
			if(arg==((tToken*)tempptr->item)->type){
				found = true;
			};
			if(found)break;
		};
		va_end(args);
		if(found){
			//found->((tToken*)tempptr->item)->type==va_arg(args,tTokentype)
			fetcher->fetchfrom=tempptr;
			targetfetcher->fetchto=tempptr;
			return targetfetcher;
		};
	};
	printf("DBG:[W] Unimplemented function mtLxFetcher_Fetchuntil_Variadic \n");
};
void mtLxFetcher_Print(tLxFetcher* self){
	printf("DBG:[T] . Fetcher %p print request \n",self);
	int j=0; 
	for(tListnode* i=self->fetchfrom;i!=self->fetchto;i=i->next){
		tToken* t=i->item;
		if(i->next==self->fetchto){
			printf("DBG:[T] ' Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		}else{
			printf("DBG:[T] | Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		};
		j++;
	};
};
void mtLxFetcher_Print_Limited(tLxFetcher* self){
	printf("DBG:[T] . Fetcher %p print request \n",self);
	int j=0; 
	for(tListnode* i=self->fetchfrom;(i!=self->fetchto)&&(j<10);i=i->next){
		tToken* t=i->item;
		if((i->next==self->fetchto)||(j>=10)){
			printf("DBG:[T] ' Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		}else{
			printf("DBG:[T] | Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		};
		j++;
	};
};
tLxFetcher* mtLxFetcher_FetchuntilParenthesized(tLxFetcher* fetcher, tTokentype token){
#ifdef qvGTrace
	printf("LX: [T] mtLxFetcher_FetchuntilParenthesized: entered \n");
#endif
#ifdef qvGTraceexpressions
	printf("LX: [T] mtLxFetcher_FetchuntilParenthesized: printing fetcher \n");
	mtLxFetcher_Print_Limited(fetcher);
#endif
	bool found = false;
	tLxFetcher* targetfetcher = malloc(sizeof(tLxFetcher));
	targetfetcher->fetchfrom=fetcher->fetchfrom;
	tListnode /* <tToken> */ * tempptr = nullptr;
	int parenthesation = 0;
	for(
		tempptr = targetfetcher->fetchfrom;
		tempptr!=null;
		tempptr=tempptr->next
	){
		//printf("123: %i:%s ",((tToken*)tempptr->item)->type,token); 
		if(parenthesation<=0){
			if(((tToken*)tempptr->item)->type==token){
				found = true;
			};
		};
		switch(((tToken*)tempptr->item)->type){
			case tToken_Openbrackets:
			case tToken_Opencurlybraces:
			case tToken_Openparentheses:
				parenthesation++;
				break;
			case tToken_Closebrackets:
			case tToken_Closecurlybraces:
			case tToken_Closeparentheses:
				parenthesation--;
				break;
			default:
				break;
		};
		if(found){
			//found->((tToken*)tempptr->item)->type==va_arg(args,tTokentype)
			fetcher->fetchfrom=tempptr;
			targetfetcher->fetchto=tempptr;
			return targetfetcher;
		};
		if(0)if(parenthesation<0){
			printf("LX: [W] mtLxFetcher_FetchuntilParenthesized: Broken parenthesation! \n");
			//break;
		};
	};
	printf("LX: [W] mtLxFetcher_FetchuntilParenthesized: Target token not found\n");
	return nullptr;
};

#include "xcc-singlestage-lexicalparser.c"
// ------------------ Launcher ------------------
void LnRunprogram(char* program, char** argv){
	printf("L:  Running: ");	//a test mid-line comment
	for(int i=0;argv[i]!=0;i++){
		printf("%s ",argv[i]);
	};
	printf("\n");
	printf("L:   Exitcode: %i\n",spawnv(_P_WAIT,program,(const char * const *)argv));
};
char* LnTrimextension(char* file){
	//Gives you ownership of dynamic memory, borrows argument
	int len=(int)(mtString_FindcharLast(file,'.')-file);
	char* str=malloc(len+1);
	memcpy(str,file,len);
	*(str+len)=0;
	return str;
};
void LnParsefile(char* program, char* filename, char* ext1, char* ext2, char* args){
	char* argv[5];
	argv[0]=program;
	argv[1]=mtString_Join(filename,ext1);
	argv[2]=mtString_Join(filename,ext2);
	argv[3]=0;
	argv[4]=0;
	LnRunprogram(program,(char**)(&argv));
	free(argv[1]);
	free(argv[2]);
};

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
	
	// Symbol generation - includes structs
	// Semantic parsing - compiles structs
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
	int aindex=1;
	//Command-line options
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
// the end
