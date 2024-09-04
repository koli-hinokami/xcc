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
tGInstruction* GCompiled[/*segment*/meGSegment_Count];

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

char* mtGType_ToString_Embeddable(tGType *);
void LfiPrint_LxNode(char* pr,tLxNode* self);
void LfiPrint_GType(char* pr, tGType* self){
	LfWriteline(mtString_Join("Lf: [M] • t: ¤ type \"",mtString_Join(mtGType_ToString_Embeddable(self),"\"\n")));
	return;
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
		//for(tListnode* i=self->precompiledstructure->first;i!=nullptr;i=i->next){
		//	LfiPrint_LxNode("f:",(tLxNode*)i->item);
		//}
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
		if(self->complexbasetype)LfiPrint_GType("b:",self->complexbasetype);
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
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%i \n",pr,self->type,TokenidtoName[self->type],self->constant);
			LfWriteline(buffer);
			return;
		}else if(self->type==tLexem_Identifier){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%s \n",pr,self->type,TokenidtoName[self->type],self->identifier);
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
char* mtGType_ToString_Embeddable(tGType *);
char* mtGType_ToString(tGType * self){return mtGType_ToString_Embeddable(self);};
char* mtGSymbol_ToString(tGSymbol* self){
	char buffer[4096];
	if(self==nullptr){
		return mtString_Clone("nullptr");
	}else{
		//snprintf(buffer,4096,"Lf: [M] ¤ Symbol %p:%s %i•%s\n",self,mtGType_ToString_Embeddable(self->type),self->symbolkind,self->name);
		// Some conversions
		return mtString_Join(
			 self->symbolkind==mtGSymbol_eType_Constant?          "const "
			:self->symbolkind==mtGSymbol_eType_Namespace?         "namespace "
			:self->symbolkind==mtGSymbol_eType_Pointer?           "declare "
			:self->symbolkind==mtGSymbol_eType_Deferredevaulation?"declareexpr "
			:self->symbolkind==mtGSymbol_eType_Typedef?           "typedef "
			:"unknown",
			mtString_Join(
				mtGType_ToString(self->type),
				mtString_Join(
					" ",
					mtString_Join(
						self->name,
						";"
					)
				)
				
			)
		);
	};
	LfWriteline(buffer);

};
char* mtLxNode_ToString(tLxNode* self){
	char buffer[512];
	char* bufptr=buffer;
	if(self==nullptr){
		//exit(5);
		//sprintf(buffer,"Lf: [M] · %2s ¤ Nullpointer \n",pr);
		return mtString_Clone("nullptr");
		//LfWriteline(buffer);
	}else switch(self->type){
		case tLexem_Declarationlist:
			return mtString_Join(
				mtLxNode_ToString(self->left),
				mtLxNode_ToString(self->right)
			);
			break;
		case tLexem_Integerconstant:
			sprintf(buffer,"intconstant(%i)",self->type,TokenidtoName[self->type],self->constant);
			return mtString_Clone(buffer);
		case tLexem_Identifier:
			sprintf(buffer,"identifier\"%s\"",self->identifier);
			return mtString_Clone(buffer);
		default:
			sprintf(buffer,"lexem %i:%s",self->type,TokenidtoName[self->type]);
			//exit(6);
			bufptr=mtString_Clone(buffer);
			if(self->returnedtype)mtString_Append(&bufptr,mtGType_ToString_Embeddable(self->returnedtype));
			if(
				  (self->type==tLexem_Switchcase)
				||(self->type==tLexem_Switchdefault)
			){
				//LfWriteline("Lf: [M] · i: ¤ Folded bound switch \n");
				mtString_Append(&bufptr," (i:boundswitch)");
			}else if(self->type==tLexem_Breakstatement){
				//LfWriteline("Lf: [M] · i: ¤ Folded bound break target \n");
				mtString_Append(&bufptr," (i:breaktarget)");
			}else if(self->initializer){
				//LfiPrint_LxNode("i:",self->initializer);
				mtString_Append(&bufptr," (i:");
				mtString_Append(&bufptr,mtLxNode_ToString(self->initializer));
				mtString_Append(&bufptr,")");
			};
			//LfiPrint_LxNode("c:",self->condition);
			//LfiPrint_LxNode("l:",self->left);
			//LfiPrint_LxNode("r:",self->right);
			if(self->condition){
				mtString_Append(&bufptr," (c:");
				mtString_Append(&bufptr,mtLxNode_ToString(self->condition));
				mtString_Append(&bufptr,")");
			};
			if(self->left){
				mtString_Append(&bufptr," (l:");
				mtString_Append(&bufptr,mtLxNode_ToString(self->left));
				mtString_Append(&bufptr,")");
			};
			if(self->right){
				mtString_Append(&bufptr," (r:");
				mtString_Append(&bufptr,mtLxNode_ToString(self->right));
				mtString_Append(&bufptr,")");
			};
			return bufptr;
			break;
	};
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
			mtString_Append(&s1,"/* ");
			mtString_Append(&s1,
				mtString_FromInteger(
					mtList_Count(self->precompiledstructure)
				)
			);
			mtString_Append(&s1," fields */");

			// Inlined List.foreach cuz no capturing lambdas *yet*
			//for(tListnode* i=self->precompiledstructure->first;i!=nullptr;i=i->next){
			//	mtString_Append(&s1,mtLxNode_ToString((tLxNode*)i->item));
			//	mtString_Append(&s1,"; ");
			//}
			mtString_Append(&s1," )");
		};
		if(self->structure){
			//mtString_Append(&s1," { ... }");
			mtString_Append(&s1," { ");
			mtString_Append(&s1,"/* ");
			mtString_Append(&s1,
				mtString_FromInteger(
					self->structsize
				)
			);
			mtString_Append(&s1," bytes */ ");

			// No display of member locations right now
			// Member locations
			for(tListnode* i=self->structure->symbols.first;i!=nullptr;i=i->next){
				mtString_Append(&s1,mtGSymbol_ToString((tGSymbol*)i->item));
				mtString_Append(&s1," ");
			}
			mtString_Append(&s1,"}");
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
		//printf("void ",self->unresolvedsymbol);
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
		return mtString_Join(
			 self->valuecategory==eGValuecategory_Leftvalue?"lvalue "
			:self->valuecategory==eGValuecategory_Rightvalue?"rvalue "
			:"novalue ",
			s2
		);
	};
};
void LfiPrint_SpNode(char* pr,tSpNode* self){
	char buffer[512];
	char* bufptr=buffer;
	if(self==nullptr){
		//exit(6);
		sprintf(buffer,"Lf: [M] · %2s ¤ Nullpointer \n",pr);
		//LfWriteline(buffer);
	}else if(self->type==tSplexem_Declarationlist){
		LfiPrint_SpNode("l:",self->left);
		LfiPrint_SpNode("r:",self->right);
	}else{
		if(self->type==tSplexem_Integerconstant){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%i \n",pr,self->type,TokenidtoName[self->type],(int)self->constant);
			LfWriteline(buffer);
			return;
		}else if(self->type==tSplexem_Symbol){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%s \n",pr,self->type,TokenidtoName[self->type],mtGSymbol_ToString(self->symbol));
			LfWriteline(buffer);
			return;
		}else{
			//exit(6);
			sprintf(buffer,"Lf: [M] %2s ¤ Lexem %i:%s \n",pr,self->type,TokenidtoName[self->type]);
		};
		//exit(6);
		LfIndent(buffer);
		if(self->returnedtype)LfiPrint_GType("t:",self->returnedtype);
		if(self->symbol){
			sprintf(buffer,
				"Lf: [M] ∙ s: ¤ Symbol \"%s\" \n",
				mtGSymbol_ToString(self->symbol)
			);
			LfWriteline(buffer);
		};
		if(
			  (self->type==tLexem_Switchcase)
			||(self->type==tLexem_Switchdefault)
		){
			LfWriteline("Lf: [M] · i: ¤ Folded bound switch \n");
		}else if(self->type==tLexem_Breakstatement){
			LfWriteline("Lf: [M] · i: ¤ Folded bound break target \n");
		}else{
			LfiPrint_SpNode("i:",self->initializer);
		};
		LfiPrint_SpNode("c:",self->condition);
		LfiPrint_SpNode("l:",self->left);
		LfiPrint_SpNode("r:",self->right);
		if(
			self->type==tSplexem_Structuremember
		){
			sprintf(buffer,
				"Lf: [M] ∙ n: ¤ Offset %i \n",
				self->constant
			);
			LfWriteline(buffer);
		};
		LfUnindent("Lf: [M]   \n");
	};
};
void LfPrint_SpNode(tSpNode* self){
	LfiPrint_SpNode("",self);
	//exit(4);
};
void LfPrint_GNamespace(tGNamespace* self);
void LfPrint_GSymbol(tGSymbol* self){
	// TODO: Rewrite to use dynamic buffers instead of static 
	//  cuz I already got one buffer overflow and don't want any more
	char buffer[4096];
	if(self==nullptr){
		snprintf(buffer,4096,"Lf: [M] ¤ `(tGSymbol*)nullptr` found it's way into namespace!\n",self,mtGType_ToString_Embeddable(self->type),self->symbolkind,self->name);
		LfWriteline(buffer);
	}else if(self->symbolkind==mtGSymbol_eType_Namespace){
		snprintf(buffer,4096,"Lf: [M] ¤ Childnamespace %p:%s\n",self,self->name);
		LfWriteline(buffer);
		LfPrint_GNamespace(self->name_space);
		return;
	}else{
		snprintf(buffer,4096,"Lf: [M] ¤ Symbol %p:%s %i•%s\n",self,mtGType_ToString_Embeddable(self->type),self->symbolkind,self->name);
		LfWriteline(buffer);
	};
};
void LfPrint_GNamespace(tGNamespace* self){
	char buffer[512];
	if(self){
		sprintf(buffer,"Lf: [M] ¤ Namespace %p:%s\n",self,self->name);
		LfIndent(buffer);
		// Print stuff
		mtList_Foreach(&(self->symbols),(void(*)(void*))(&LfPrint_GSymbol));
		// Finalize
		LfUnindent("Lf: [M]   \n");
	}else{
		LfWriteline("Lf: [M] • ¤ Nullnamespace \n");
	}
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
tGType* mtGType_CreateAtomic(eGAtomictype type){
	tGType* i = mtGType_Create();
	i->atomicbasetype = type;
	return i;
}
tGType* mtGType_Clone(tGType* self){
	return memcpy(malloc(sizeof(tGType)),self,sizeof(tGType));
}
tGType* mtGType_Deepclone(tGType* self){
	if(!self)return nullptr;
	tGType* i = mtGType_Clone(self);
	i->complexbasetype=mtGType_Deepclone(i->complexbasetype);
	return i;
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
tGType* mtGType_GetBasetype(tGType* self){
#ifdef qvGTrace
	printf("ss: [T] mtGType_GetBasetype: entered \n");
#endif
	assert(self);
	if(
		  (self->atomicbasetype==eGAtomictype_Pointer)
		||(self->atomicbasetype==eGAtomictype_Array)
		||(self->atomicbasetype==eGAtomictype_Function)
	){
#ifdef qvGTrace
	printf("ss: [T] mtGType_GetBasetype: got it \n");
#endif
		return mtGType_GetBasetype(self->complexbasetype);
	}else{
		return self;
	};
	exit(2);
};
tGSymbol* mtGSymbol_Create(){
	return calloc(sizeof(tGSymbol),1);
};
tGNamespace* mtGNamespace_Create(){
	return calloc(sizeof(tGNamespace),1);
};
tGNamespace* mtGNamespace_CreateInherit(tGNamespace* parent){
	tGNamespace* i = mtGNamespace_Create();
	i->parentnamespace=parent;
	return i;
};
tGSymbol* mtGNamespace_Findsymbol_NameKind(tGNamespace* self, char* name, enum mtGSymbol_eType kind){
#ifdef qvGTrace
	printf("ss: [T] mtGNamespace_Findsymbol_NameKind: Entered \n");
#endif
	// Empty namespace
	if(self==nullptr){
		printf("ss: [E] mtGNamespace_Findsymbol_NameKind(%p): Symbol %i•%s not found \n",self,kind,name);
		return nullptr;
	};
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
	printf("ss: [T] mtGNamespace_Add(namespace %p,symbol %s): Entered \n",
		namespace,mtGSymbol_ToString(symbol)
	);
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
tGSymbol* mtGSymbol_CreateNamespace(char* name, tGNamespace* name_space){
	tGSymbol* temp = mtGSymbol_Create();
	temp->name = name;
	temp->type = nullptr;
	temp->symbolkind = mtGSymbol_eType_Namespace;
	temp->name_space = name_space;
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
tGTargetPointer* mtGTargetPointer_Create(){
	return calloc(sizeof(tGTargetPointer),1);
};
tGTargetPointer* mtGTargetPointer_Clone(tGTargetPointer* self){
	return memcpy(malloc(sizeof(tGTargetPointer)),self,sizeof(tGTargetPointer));
};
tGTargetPointer* mtGTargetPointer_CreateDynamic(tGInstruction* instr){
	tGTargetPointer* i=mtGTargetPointer_Create();
	i->nonconstant=true;
	i->dynamicpointer=instr;
	return i;
};
tGTargetPointer* mtGTargetPointer_CreateStatic(eGSegment segment, tGTargetNearpointer offset){
	tGTargetPointer* i=mtGTargetPointer_Create();
	i->nonconstant=false;
	i->segment=segment;
	i->offset=offset;
	return i;
};
tGInstruction* mtGInstruction_Create(){
	return calloc(sizeof(tGInstruction),1);
};
tGInstruction* mtGInstruction_CreateAllocatestorage(tGTargetSizet size){
	tGInstruction* i=mtGInstruction_Create();
	i->opcode.opr=tInstruction_Allocatestorage;
	i->opcode.isize=eGAtomictype_Uint8;
	i->immediate=size;
	return i;
};
tGInstruction* mtGInstruction_CreateCnop(){
	tGInstruction* i=mtGInstruction_Create();
	i->opcode.opr=tInstruction_Cnop;
	i->opcode.isize=eGAtomictype_Void;
	return i;
};
tGInstruction* mtGInstruction_GetLast(tGInstruction* self){
	// Normally I'd use a forloop `for(T i = self;i;i=i->next);` but I'm
	// lazy here so recursion it is.
	if(self->next==nullptr){
		return self;
	}else{
		return mtGInstruction_GetLast(self->next);
	}
};
// --------------------- Tokenizer ---------------------
void GInitializePerfile(){
	for(int i=0;i<meGSegment_Count;i++)GCompiled[i]=mtGInstruction_CreateCnop();
};
void GFinalize(){
};
#include "xcc-singlestage-tokenizer.c"
#include "xcc-singlestage-fetcher.c"
#include "xcc-singlestage-lexicalparser.c"
#include "xcc-singlestage-semanticpreparser.c"
#include "xcc-singlestage-symbolgen.c"
#include "xcc-singlestage-semanticparser.c"
#include "xcc-singlestage-launcher.c"
// the end
