//      --== Koli's Lisp compiler for xcc crossdevelopment suite ==--      //
// • Aiming for Interlisp compat.

#include "hyperheader.h"
// -- Types --
typedef struct tGCons { 
	union { struct tGCons *car,*left,*node; };
	union { struct tGCons *cdr,*right,*next; char* atom; };
} tGCons, *ptGCons;
// -- Preprocessor macroses --
// -- Preprocessor constants --
// -- Forward declarations --
// -- Globals --
FILE* srcfile;
FILE* dstfile;
// -- Auxilirally functions --
int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};
// -- Lispmachine --
// typedef struct tGCons { 
// 	union { struct tGCons car,left,node; };
// 	union { struct tGCons cdr,right,next; char* atom; };
// } tGCons;
// -- Classes --
// -- Reader --
ptGCons UReadtree(){ // Global for lcom
#ifdef qvGTrace
	printf("U:  [T] UReadtree: Entered\n");
#endif
	// Skip whitespace
	while(isspace(fpeekc(srcfile)))if(fgetc(srcfile)==EOF)return nullptr;
	ErfFatal_String("L:  [F] lcom: UReadtree: Unimplemented code hit!\n");
	return nullptr;
};
// -- Symbolgen --
// -- Compiler --
void UCompile(ptGCons tree){ // Compile a tree into compiled lisp code
	ErfFatal_String("L:  [F] lcom: UCompile: Unimplemented code hit!\n");
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
					case 'a': // Architecture. lcom (so far) doesn't care, so skip.
						++i;
						goto argpunwind;
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
	exit(4);
	return 0;
};
