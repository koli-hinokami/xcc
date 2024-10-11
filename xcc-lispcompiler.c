//      --== Koli's Lisp compiler for xcc crossdevelopment suite ==--      //
// • Aiming for Interlisp compat.

#include "hyperheader.h"
// -- Types --
typedef struct tGCons { 
	bool isatom;
	struct tGCons *car;
	struct tGCons *cdr;
	char* atom;
} tGCons, *ptGCons;

typedef struct {
	char* name;
	enum {
		mtSgSymbol_Frame = 1, // fp+%i
		mtSgSymbol_Code  = 2, // c_%i
		mtSgSymbol_Data  = 3, // d_%i
		mtSgSymbol_Named = 4, // %s
	} segment;
	char* sym;
	int offset;
} tSgSymbol;
// -- Preprocessor macroses --
#define takeowns
#define borrows
#define mut
// -- Preprocessor constants --
#define qvSgMaxsize 2000 // Should be enough
// -- Forward declarations --
ptGCons mtGCons_Print(ptGCons self);
ptGCons mtGCons_PrintI(ptGCons self);
// -- Globals --
FILE* srcfile;
FILE* dstfile;

tSgSymbol SgSymbols[qvSgMaxsize];
int SgLastsymbolid; // Also symbol count
int ULabels;
// -- Auxilirally functions --
int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};
bool mtChar_IsTokenseparator(char ch){
	if(ch=='_')return false;
	if(ch=='\n')return true;
	if(ch==')')return true;
	if(ch=='(')return true;
	return isblank(ch);
};
// -- Lispmachine --
// typedef struct tGCons { 
// 	union { struct tGCons car,left,node; };
// 	union { struct tGCons cdr,right,next; char* atom; };
// } tGCons;
ptGCons mtGCons_Clone(ptGCons self){return memcpy(malloc(sizeof(tGCons)),self,sizeof(tGCons));};
ptGCons mtGCons_CreateCons(ptGCons car,ptGCons cdr){return mtGCons_Clone(&(tGCons){.isatom=false,.car=car,.cdr=cdr,.atom="(cons)"});};
ptGCons mtGCons_CreateAtom(char takeowns * str){return mtGCons_Clone(&(tGCons){.isatom=true,.car=nullptr,.cdr=nullptr,.atom=str});};
ptGCons mtGCons_PrintI(ptGCons self){
	if(!self){
		printf("/* internal inconsistency */");
	}else if(self->isatom){
		printf(" . ");
		mtGCons_Print(self);
	}else{
		mtGCons_Print(self->car);
		if(self->cdr){
			printf(" ");
			mtGCons_PrintI(self->cdr);
		}else{
		};
	}
	return self;
};
ptGCons mtGCons_Print(ptGCons self){
	if(!self){
		printf("NIL");
	}else if(self->isatom){
		printf("%s",self->atom);
	}else{
		printf("(");
		mtGCons_PrintI(self);
		printf(")");
	}
	return self;
};
// -- Classes --
// -- Reader --
ptGCons UReadtree(){ // Global for lcom
#ifdef qvGTrace
	//printf("U:  [T] UReadtree: Entered\n");
#endif
	// Skip whitespace
	while(isspace(fpeekc(srcfile)))if(fgetc(srcfile)==EOF)return nullptr;
	if(fpeekc(srcfile)=='('){fgetc(srcfile);
		// Open parenthesis -> read a list
		ptGCons temp = nullptr;
		for(;;){
			// Skip whitespace
			while(isspace(fpeekc(srcfile)))
				if(fgetc(srcfile)==EOF) {
					ErfError_String2("U:  [E] UReadtree: Reading list: No closing parentheses\n");
					return nullptr;
				};
			// Terminate if list terminator ')' is seen
			if(fpeekc(srcfile)==')'){fgetc(srcfile);return temp;};
			// Read an atom
			ptGCons atom = UReadtree();
			// Insert it into list
			if(!temp){
				temp=mtGCons_CreateCons(atom,nullptr);
			}else{
				for(ptGCons i=temp; i; i=i->cdr){
					assert(i);
					if(i->cdr==nullptr){
						i->cdr=mtGCons_CreateCons(atom,nullptr);
						break;
					};
				};
			};
		};
	}else{
		// Atom
		char* str = mtString_Create();
		while(!mtChar_IsTokenseparator(fpeekc(srcfile))) mtString_Appendchar(&str,fgetc(srcfile));
		return mtGCons_CreateAtom(str);
	};
};
// -- Symbolgen --
tSgSymbol* SgFindsymbol(char* name){
	for(int i=0;i<SgLastsymbolid;i++)
		if(strcmp(name,SgSymbols[i].name)==0)
			return &(SgSymbols[i]);
	return nullptr;
};
tSgSymbol* SgCreatesymbol(char* name){
	SgSymbols[SgLastsymbolid++].name=name;
	SgSymbols[SgLastsymbolid-1].segment = 0;
	SgSymbols[SgLastsymbolid-1].offset = 0;
	SgSymbols[SgLastsymbolid-1].sym = nullptr;
};
// -- Compiler --
void CgCompilearguments(ptGCons self){
	ErfError_String2("U:  [E] CgCompilearguments: Unimplemented code hit\n");
};
void CgImpliedprogn(ptGCons self){
	ErfError_String2("U:  [E] CgImpliedprogn: Unimplemented code hit\n");
};
void UCompile(ptGCons tree){ // Compile a tree into compiled lisp code
	assert(!tree->isatom);
	if(!tree->car->isatom)
		ErfError_String2("U:  [E] UCompile: Composite declaration kind\n");
	if(strcmp(tree->car->atom,"rpaqq")==0){
		ErfError_String2("U:  [E] UCompile: Who gave me Interlisp sources?!\n");
	}else if(strcmp(tree->car->atom,"defineq")==0){
		ErfError_String2("U:  [E] UCompile: Who gave me Interlisp sources?!\n");
	}else if(strcmp(tree->car->atom,"*")==0){
		// Intentional - Interlisp-style inside-progn comments.
	}else if(strcmp(tree->car->atom,"defun")==0){
		printf("U:  [D] UCompile: Compiling a defun\n");
		// Create symbol
		{
			tSgSymbol* sym = SgCreatesymbol(tree->cdr->car->atom);
			sym->segment = mtSgSymbol_Code;
			sym->offset = ULabels++;
		}
		// Get locals unwinding point
		int sgunwind = SgLastsymbolid;
		// Parse arguments
		CgCompilearguments(tree->cdr->car);
		// Compile implied progn
		CgImpliedprogn(tree->cdr->cdr);
		// Unwind symbols
		SgLastsymbolid = sgunwind;
	}else{
		ErfError_String2("U:  [E] UCompile: Unrecognized declaration kind\n");
	};
};
// -- Main loop --
void UCompilefile(char* file){
#ifdef qvGTrace
	printf("U:  [T] UCompilefile: Entered\n");
#endif
	srcfile = fopen(file,"r");
	while(fpeekc(srcfile)!=EOF){
		// Read-eval-print loop is here!
		// Except it's read-compile-dump loop.
		ptGCons tree = UReadtree();
#ifdef qvGTrace
		printf("U:  [D] UCompilefile: Compiling tree { ");
		mtGCons_Print(tree);
		printf(" } \n");
#endif
		if(!tree)break;
		UCompile(tree);
	};
};
// -- Launcher --
int main(int argc, char** argv, char** envp){
	printf("L:  [M] %s: xcc Lisp compiler\n", argv[0]);
	for(int i=1;i<argc;i++){assert(argv[i]);
		// Parse an argument
#ifdef qvGTrace
		printf("U:  [D] main: Parsing argument \"%s\"\n",argv[i]);
#endif
		if(argv[i][0]=='-'){ // Argument
			for(unsigned j=1;argv[i][j];j++){
				switch(argv[i][j]){
					case 'o': // Output file.
						if(dstfile) ErfError_String2("L:  [E] lcom: Output file specified when output file was already specified\n");
						dstfile = fopen(argv[++i],"w");
						goto argpunwind;
						break;
					case 'a': // Architecture. lcom (so far) doesn't care, so skip.
						++i;
						goto argpunwind;
						break;
					case '-':
						ErfError_String2(mtString_Format(
							"L:  [E] lcom: Argument \"%s\": Unrecognized multichar argument '--%s'\n",argv[i],&(argv[i][j+1])
						));
						break;
					default:
						ErfError_String2(mtString_Format(
							"L:  [E] lcom: Argument \"%s\": Unrecognized argument '-%c' \n",argv[i],argv[i][j]
						));
						//todo
				};
			};
			argpunwind:
		}else{
			UCompilefile(argv[i]);
		};
	};
	return 0;
};
