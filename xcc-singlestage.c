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
		}else if(self->type==tLexem_Identifier){
			sprintf(buffer,"Lf: [M] %2s ∙ ¤ Lexem %i:%s:%s \n",pr,self->type,TokenidtoName[self->type],self->identifier);
			LfWriteline(buffer);
			return;
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
char* mtGType_ToString_Embeddable(tGType* /* MfCcMmDynamic */ self){
	char *s1;
	char *s2;
	char *s3;
	if(self==nullptr){
		return mtString_Clone("totally_invalid");
	}else if(self->atomicbasetype==eGAtomictype_Unresolved){
		// Unresolved
		//sprintf(buffer,"unresolved<%s> ",self->unresolvedsymbol);
		//return self->unresolvedsymbol;
		s1 = mtString_Join(self->unresolvedsymbol,">");
		s2 = mtString_Join("unresolved<",s1);
		free(s1);
		return s2;
	}else if(self->atomicbasetype==eGAtomictype_Enumeration){ 
		// Enumeration
		s1 = mtString_Join(self->unresolvedsymbol,"");
		s2 = mtString_Join("enum ",s1);
		free(s1);
		return s2;
		//LfiPrint_LxNode("p:",self->precompiledenumeration);
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Structure){ 
		s1 = mtString_Clone("struct");
		if(self->unresolvedsymbol){
			mtString_Append(&s1," ");
			mtString_Append(&s1,self->unresolvedsymbol);
		};
		if(self->precompiledstructure){
			//mtString_Append(&s1," { ... }");
			mtString_Append(&s1," ( ");
			mtString_Append(&s1,
				mtString_FromInteger(
					mtList_Count(self->precompiledstructure)
				)
			);
			mtString_Append(&s1," fields )");
		};
		//if(!self->unresolvedsymbol){
		//	// Occasionally structures are defined only by their fields
		//	return mtString_Clone("struct");
		//}else{
		//	s1 = mtString_Join(self->unresolvedsymbol,"");
		//	s2 = mtString_Join("struct ",s1);
		//};
		return s1;
		free(s1);
		return s2;
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Pointer){ 
		return mtString_Join(
			mtGType_ToString_Embeddable(self->complexbasetype),
			"*"
		);
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Array){ 
		return mtString_Join(
			mtGType_ToString_Embeddable(self->complexbasetype),
			"[]"
		);
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Function){ 
		return mtString_Join(
			mtGType_ToString_Embeddable(self->complexbasetype),
			"()"
		);
		//LfPrint_GNamespace(self->structure);
	}else if(self->atomicbasetype==eGAtomictype_Void){ 
		return mtString_Clone("void");
		printf("void ",self->unresolvedsymbol);
		//LfPrint_GNamespace(self->structure);
	}else{
		// Resolve atomic type to string
		s1 = "unknown<";
		s1 = mtString_Join(s1,mtString_FromInteger(self->atomicbasetype));
		s2 = mtString_Join(s1,">");
		free(s1);
		for(
			GAtomictypetostring_Entry * ptr = GAtomictypetostring;
			ptr->str;
			ptr++
		){
			if(ptr->atomictype==self->atomicbasetype){
				s2 = mtString_Clone(ptr->str);
			};
		};
		return s2;
	};
};
void LfPrint_GSymbol(tGSymbol* self){
	char buffer[512];
	if(self==nullptr){
		sprintf(buffer,"Lf: [M] ¤ `(tGSymbol*)nullptr` found it's way into namespace!\n",self,mtGType_ToString_Embeddable(self->type),self->symbolkind,self->name);
	}else{
		sprintf(buffer,"Lf: [M] ¤ Symbol %p:%s %i•%s\n",self,mtGType_ToString_Embeddable(self->type),self->symbolkind,self->name);
	};
	LfWriteline(buffer);
};
void LfPrint_GNamespace(tGNamespace* self){
	char buffer[512];
	sprintf(buffer,"Lf: [M] ¤ Namespace %p\n",self);
	LfIndent(buffer);
	// Print stuff
	mtList_Foreach(&(self->symbols),(void(*)(void*))(&LfPrint_GSymbol));
	// Finalize
	LfUnindent("Lf: [M]   \n");
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

tGType* mtGType_Create(){
	return calloc(sizeof(tGType),1);
}
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
tGSymbol* mtGSymbol_Create(){
	return calloc(sizeof(tGSymbol),1);
};
tGNamespace* mtGNamespace_Create(){
	return calloc(sizeof(tGNamespace),1);
};
tGSymbol* mtGNamespace_Findsymbol_NameKind(tGNamespace* self, char* name, enum mtGSymbol_eType kind){
#ifdef qvGTrace
	printf("ss: [T] mtGNamespace_Findsymbol_NameKind: Entered \n");
#endif
	// Empty namespace
	if(self==nullptr)return nullptr;
	// Search in list
	for(tListnode* ptr=self->symbols.first;ptr!=nullptr;ptr=ptr->next){
		tGSymbol* symbol = ptr->item;
		if(symbol->symbolkind==kind){
			if(strcmp(symbol->name,name)==0){
				return symbol;
			};
		};
	};
	// Search in parent namespace
	return mtGNamespace_Findsymbol_NameKind(self->parentnamespace,name,kind);
};
tGSymbol* mtGNamespace_FindsymbolNoparent_NameKind(tGNamespace* self, char* name, enum mtGSymbol_eType kind){
#ifdef qvGTrace
	printf("ss: [T] mtGNamespace_FindsymbolNoparent_NameKind: Entered \n");
#endif
	// Empty namespace
	if(self==nullptr)return nullptr;
	// No string
	if(name==nullptr)return nullptr;
	// Search in list
	if(self->symbols.first==nullptr){
		// Not going to find anything in an empty list
	}else{
		for(tListnode* ptr=self->symbols.first;ptr!=nullptr;ptr=ptr->next){
			tGSymbol* symbol = ptr->item;
			if(symbol==nullptr){
				printf("ss: [W] mtGNamespace_FindsymbolNoparent_NameKind: `(tGSymbol*)nullptr` sneaked it's way into namespace somehow \n");
			}else if(symbol->symbolkind==kind){
				if(symbol->name==nullptr){
					printf("ss: [W] mtGNamespace_FindsymbolNoparent_NameKind: null string inside symbol \n");
				}else if(strcmp(symbol->name,name)==0){
					return symbol;
				};
			};
		};
	};
	// Don't search in parent namespace!
	return nullptr;
};
void mtGNamespace_Add(tGNamespace* namespace, tGSymbol* symbol){
#ifdef qvGTrace
	printf("ss: [T] mtGNamespace_Add: Entered \n");
#endif
	// Obviously check if namespace is valid
	if(namespace==nullptr){
		printf("ss: [E] mtGNamespace_Add: namespace==nullptr \n");
		return;
	};
	// Check if we are adding correct symbol in the first place
	if(symbol==nullptr){
		printf("ss: [E] mtGNamespace_Add: symbol==nullptr \n");
		return;
	};
	// Check for no string
	if(symbol->name==nullptr){
		printf("ss: [E] mtGNamespace_Add: symbol->name==nullptr \n");
		return;
	};
	// Check for duplicate symbol
	if(
		mtGNamespace_FindsymbolNoparent_NameKind(
			namespace,symbol->name,symbol->symbolkind
		)!=nullptr
	){
		// TODO: add line numbers
		printf(
			"ss: [W] mGNamespace_Add: Adding duplicate symbol %i∙%s \n",
			symbol->symbolkind,
			symbol->name
		);
	};
	// Add symbol to namespace
	mtList_Prepend(&(namespace->symbols),symbol);
};

// --------------------- Tokenizer ---------------------
#include "xcc-singlestage-tokenizer.c"
#include "xcc-singlestage-fetcher.c"
#include "xcc-singlestage-lexicalparser.c"
//#include "xcc-singlestage-semanticpreparser.c"
tGType* mtGType_CreatePointer(tGType* self){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Pointer;
	temp->complexbasetype = self;
	// TODO: Far pointers
	return temp;
};
tGType* mtGType_CreateFunctioncall(tGType* self, tListnode /* <tGType> */ * args){
	tGType* temp = mtGType_Create();
	// Function
	temp->atomicbasetype = eGAtomictype_Function;
	// Returning:
	temp->complexbasetype = self;
	// And taking:
	temp->functionarguments = args;
	return temp;
};
//tGType* mtGType_CreateFunctioncall_Expr(tGType* self, tLxNode* expr){
//	tGType* temp = mtGType_Create();
//	// Function
//	temp->atomicbasetype = eGAtomictype_Function;
//	// Returning:
//	temp->complexbasetype = self;
//	// And taking:
//	temp->functionarguments = SppParsefunctionarguments(expr);
//	return temp;
//};
tGType* mtGType_CreateArray_Expr(tGType* self, tLxNode* expr){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Array;
	temp->complexbasetype = self;
	// TODO: Far pointers
	return temp;
};
tGSymbol* mtGSymbol_CreatePointer(char* name, tGType* type, tGTargetPointer* ptr){
	tGSymbol* temp = mtGSymbol_Create();
	temp->name = name;
	temp->type = type;
	temp->symbolkind = mtGSymbol_eType_Pointer;
	temp->allocatedstorage = ptr;
	return temp;
};
tGSymbol* mtGSymbol_CreateDeferred(char* name, tGType* type, tLxNode* expr){
	tGSymbol* temp = mtGSymbol_Create();
	temp->name = name;
	temp->type = type;
	temp->symbolkind = mtGSymbol_eType_Deferredevaulation;
	temp->deferredexpression = expr;
	return temp;
};
tGSymbol* mtGSymbol_CreateTypedef(char* name, tGType* type){
	tGSymbol* temp = mtGSymbol_Create();
	temp->name = name;
	temp->type = type;
	temp->symbolkind = mtGSymbol_eType_Typedef;
	return temp;
};
#include "xcc-singlestage-symbolgen.c"
#include "xcc-singlestage-launcher.c"
// the end
