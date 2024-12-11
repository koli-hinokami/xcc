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
eGAtomictype LnTypetranslationmap[eGAtomictype_Count];
tGTargetSizet LnSizeofmap[eGAtomictype_Count];
tGTargetSizet LnNearpointersize = 2;
tGTargetSizet LnFarpointersize = 4;
char* LnOutputfile;
enum{eLnIr_Legacy=1,eLnIr_Alternate=2,eLnIr_Native=3}LnIr = 1; //IR to use

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


// ------------------------- Forward declarations -------------------------

char* mtGType_ToString_Embeddable(tGType *);
char* mtGType_ToString(tGType * self);
void LfPrint_GNamespace(tGNamespace* self);
void LfiPrint_LxNode(char* pr,tLxNode* self);
void LfiPrint_GType(char* pr, tGType* self);
tGType* mtGType_Clone(tGType* self);
char* mtGSymbol_ToString(tGSymbol* self);
void GError();
tGType* mtGType_Transform(tGType /* modifies */ * self);

// ------------------------- Auxiliary functions --------------------------

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};

// -------------------------- Logging facilities --------------------------

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
void LfiPrint_SpNode(char* pr,tSpNode* self){
	char buffer[512];
	if(self==nullptr){
		//exit(6);
		sprintf(buffer,"Lf: [M] · %2s ¤ Nullpointer \n",pr);
		//LfWriteline(buffer);
	}else if(self->type==tSplexem_Declarationlist){
		LfiPrint_SpNode("l:",self->left);
		LfiPrint_SpNode("r:",self->right);
	}else{
		if(self->type==tSplexem_Integerconstant){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%s:%i \n",pr,self->type,TokenidtoName[self->type],mtGType_ToString(self->returnedtype),(int)self->constant);
			LfWriteline(buffer);
			return;
		}else if(self->type==tSplexem_Symbol){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%s:%s \n",pr,self->type,TokenidtoName[self->type],mtGType_ToString(self->returnedtype),mtGSymbol_ToString(self->symbol));
			LfWriteline(buffer);
			return;
		}else{
			//exit(6);
			sprintf(buffer,"Lf: [M] %2s ¤ Lexem %i:%s \n",pr,self->type,TokenidtoName[self->type]);
			LfIndent(buffer);
		};
		//exit(6);
		if(self->returnedtype)LfiPrint_GType("t:",self->returnedtype);
		if(self->symbol){
			sprintf(buffer,
				"Lf: [M] ∙ s: ¤ Symbol \"%s\" \n",
				mtGSymbol_ToString(self->symbol)
			);
			LfWriteline(buffer);
		};
		if(self->switchlabels){
			mtList_Foreach_Clojure(
				self->switchlabels,
				(void(*)(void*,void*))LfiPrint_SpNode,
				"sc"
			);
		};
		if(self->fextinfo){
			sprintf(buffer,
				"Lf: [M] ∙ f: \"%s\" args:%i bytes  locals:%i bytes \n",
				 self->fextinfo->callingconvention==eGCallingconvention_Stdcall
				?"stdcall"
				:self->fextinfo->callingconvention==eGCallingconvention_Cdecl
				?"cdecl"
				:"(unk. calling convention)",
				self->fextinfo->argumentssize,
				self->fextinfo->localssize
			);
			LfWriteline(buffer);
		};
		if(
			  (self->type!=tSplexem_Breakstatement)
			&&(self->type!=tSplexem_Continuestatement)
		) LfiPrint_SpNode("i:",self->initializer);
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
void LfPrint_GSymbol(tGSymbol* self){
	// TODO: Rewrite to use dynamic buffers instead of static 
	//  cuz I already got one buffer overflow and don't want any more
	char buffer[4096];
	if(self==nullptr){
		snprintf(buffer,4096,"Lf: [M] ¤ `(tGSymbol*)nullptr` found it's way into namespace!\n");
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
void LfiPrint_LxNode_Commafolding(char* pr,tLxNode* self){
	char buffer[512];
	if(self==nullptr){
		//exit(5);
		sprintf(buffer,"Lf: [M] · %2s ¤ Nullpointer \n",pr);
		//LfWriteline(buffer);
	}else if(self->type==tLexem_Declarationlist){
		LfiPrint_LxNode("l:",self->left);
		LfiPrint_LxNode("r:",self->right);
	}else if(self->type==tLexem_Comma){
		LfiPrint_LxNode_Commafolding("  ",self->left);
		LfiPrint_LxNode_Commafolding("  ",self->right);
	}else{
		LfiPrint_LxNode(pr,self);
	};
};
void LfiPrint_LxNode(char* pr,tLxNode* self){
	char buffer[512];
	if(self==nullptr){
		//exit(5);
		sprintf(buffer,"Lf: [M] · %2s ¤ Nullpointer \n",pr);
		//LfWriteline(buffer);
	}else if(self->type==tLexem_Declarationlist){
		LfiPrint_LxNode_Commafolding("  ",self->left);
		LfiPrint_LxNode_Commafolding("  ",self->right);
	}else if(self->type==tLexem_Comma){
		sprintf(buffer,"Lf: [M] %2s ¤ Lexem %i:%s \n",pr,self->type,TokenidtoName[self->type]);
		LfIndent(buffer);
		LfiPrint_LxNode_Commafolding("  ",self->left);
		LfiPrint_LxNode_Commafolding("  ",self->right);
		LfUnindent("Lf: [M]   \n");
	}else{
		if(self->type==tLexem_Integerconstant){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%i \n",pr,self->type,TokenidtoName[self->type],self->constant);
			LfWriteline(buffer);
			return;
		}else if(self->type==tLexem_Identifier){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:%s \n",pr,self->type,TokenidtoName[self->type],self->identifier);
			LfWriteline(buffer);
			return;
		}else if(self->type==tLexem_Stringconstant){
			sprintf(buffer,"Lf: [M] ∙ %2s ¤ Lexem %i:%s:\"%s\" \n",pr,self->type,TokenidtoName[self->type],self->identifier);
			LfWriteline(buffer);
			return;
		}else{
			sprintf(buffer,"Lf: [M] %2s ¤ Lexem %i:%s \n",pr,self->type,TokenidtoName[self->type]);
		};
		//exit(6);
		LfIndent(buffer);
		if(self->returnedtype)LfiPrint_GType("t:",self->returnedtype);
		if(self->name_space){
			sprintf(buffer,"Lf: [M] ∙ n: ¤ Namespace %p•%s ->%p \n",self->name_space,self->name_space->name,self->name_space->parentnamespace);
			LfWriteline(buffer);
		};
		if(
			  (self->type==tLexem_Switchcase)
			||(self->type==tLexem_Switchdefault)
		){
			LfWriteline("Lf: [M] · i: ¤ Folded bound switch \n");
		}else if(self->type==tLexem_Breakstatement){
			LfWriteline("Lf: [M] · i: ¤ Folded bound break target \n");
		}else if(self->type==tLexem_Continuestatement){
			LfWriteline("Lf: [M] · i: ¤ Folded bound continue target \n");
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

// ------------------------------- class tGType -------------------------------
//   Constructors
tGType* mtGType_Create(){
	return calloc(sizeof(tGType),1);
}
tGType* mtGType_Clone(tGType* self){
	ErfEnter_String("mtGType_Clone");
	assert(self);
	ErfLeave();
	return memcpy(malloc(sizeof(tGType)),self,sizeof(tGType));
}
tGType* mtGType_CreateAtomic(eGAtomictype type){
	tGType* i = mtGType_Create();
	i->atomicbasetype = type;
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
tGType* mtGType_CreatePointer(tGType* self){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Pointer;
	temp->complexbasetype = mtGType_Clone(self);
	// TODO: Far pointers
	return temp;
};
tGType* mtGType_CreateNearpointer(tGType* self){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Nearpointer;
	temp->complexbasetype = mtGType_Clone(self);
	// TODO: Far pointers
	return temp;
};
tGType* mtGType_CreateFarpointer(tGType* self){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Farpointer;
	temp->complexbasetype = mtGType_Clone(self);
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
tGType* mtGType_CreateArray(tGType* self){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Array;
	temp->complexbasetype = self;
	// TODO: Far pointers
	return temp;
};
tGType* mtGType_CreateArray_Expr(tGType* self, tLxNode* expr){
	tGType* temp = mtGType_Create();
	temp->atomicbasetype = eGAtomictype_Array;
	temp->complexbasetype = self;
	// Parse declarations
	tLxNode* i = expr;
	for(bool cont = true;cont;){
		switch(i->type){
			// Array modifiers
			//case tLexem_Farmodifier:
			//case tLexem_Nearmodifier:
			//case tLexem_Boundcheckmodifier:

			default:
				cont=false;
		};
	};
	if(i->type==tLexem_Nullexpression){
		temp->arraysizepresent = false;
	}else if(i->type==tLexem_Integerconstant){
		temp->arraysizepresent = true;
		temp->arraysize = i->constant;
	//}else if(i->type==tLexem_){
	}else{
		printf("ss: [E] mtGType_CreateArray_Expr: Unrecognized array size node %i•%s\n",
			i->type,
			TokenidtoName[i->type]
		);
		GError();
	};
	//temp->arraysize = SppEvalconstexpr(expr);
	// TODO: Far pointers
	return temp;
};
tGType* mtGType_Deepclone(tGType* self){
	ErfEnter_String("mtGType_Deepclone");
	if(!self){
		ErfWarning();
		return nullptr;
	};
	tGType* i = mtGType_Clone(self);
	if(i->complexbasetype)
		i->complexbasetype=mtGType_Deepclone(i->complexbasetype);
	ErfLeave();
	return i;
}
void mtGType_Deallocate(tGType* self){
	free(self);
};
void mtGType_Destroy(tGType* self){
	if(self->complexbasetype){
		mtGType_Destroy(self->complexbasetype);
	};
	mtGType_Deallocate(self);
};
bool mtGType_IsPointer(tGType* self){
	if(
		  (self->atomicbasetype == eGAtomictype_Pointer)
		||(self->atomicbasetype == eGAtomictype_Nearpointer)
		||(self->atomicbasetype == eGAtomictype_Farpointer)
	)return true;
	return false;
};
bool mtGType_IsFunction(tGType* self){
	if(
		  (self->atomicbasetype == eGAtomictype_Function)
		||(self->atomicbasetype == eGAtomictype_Nearfunction)
		||(self->atomicbasetype == eGAtomictype_Farfunction)
	)return true;
	return false;
};
bool mtGType_IsArray(tGType* self){
	if(
		  self->atomicbasetype == eGAtomictype_Array
		//||self->atomicbasetype == eGAtomictype_Neararray
		//||self->atomicbasetype == eGAtomictype_Fararray
	)return true;
	return false;
};
bool mtGType_IsScalar(tGType* self){
	switch(self->atomicbasetype){
		case eGAtomictype_Char            :return true;
		case eGAtomictype_Signedchar      :return true;
		case eGAtomictype_Unsignedchar    :return true;
		case eGAtomictype_Short           :return true;
		case eGAtomictype_Unsignedshort   :return true;
		case eGAtomictype_Int             :return true;
		case eGAtomictype_Unsigned        :return true;
		case eGAtomictype_Long            :return true;
		case eGAtomictype_Unsignedlong    :return true;
		case eGAtomictype_Longlong        :return true;
		case eGAtomictype_Unsignedlonglong:return true;
		case eGAtomictype_Float           :return true;
		case eGAtomictype_Double          :return true;
		case eGAtomictype_Longdouble      :return true;
		case eGAtomictype_Pointer         :return true;
		case eGAtomictype_Sizet           :return true;
		case eGAtomictype_Intptr          :return true;
		case eGAtomictype_Intnearptr      :return true;
		case eGAtomictype_Intfarptr       :return true;
		case eGAtomictype_Nearpointer     :return true;
		// IR-side types
		case eGAtomictype_Int8            :return true;
		case eGAtomictype_Uint8           :return true;
		case eGAtomictype_Int16           :return true;
		case eGAtomictype_Uint16          :return true;
		case eGAtomictype_Int32           :return true;
		case eGAtomictype_Uint32          :return true;
		// TODO: : ptrdiff_t is signed but nearpointer is not
		case eGAtomictype_Array           :return false;
		case eGAtomictype_Structure       :return false;
		default: 
			printf("ss: [E] mtGType_IsScalar: Unrecognized atomic type %i∙%s\n",
				self->atomicbasetype,
				meGAtomictype_ToStringTable[self->atomicbasetype]
			);
			ErfError();
			return false;
			break;
	};
};
bool mtGType_IsSigned(tGType* self){
	switch(self->atomicbasetype){
		case eGAtomictype_Char            :return false;
		case eGAtomictype_Signedchar      :return true;
		case eGAtomictype_Unsignedchar    :return false;
		case eGAtomictype_Short           :return true;
		case eGAtomictype_Unsignedshort   :return false;
		case eGAtomictype_Int             :return true;
		case eGAtomictype_Unsigned        :return false;
		case eGAtomictype_Long            :return true;
		case eGAtomictype_Unsignedlong    :return false;
		case eGAtomictype_Longlong        :return true;
		case eGAtomictype_Unsignedlonglong:return false;
		case eGAtomictype_Float           :return true;
		case eGAtomictype_Double          :return true;
		case eGAtomictype_Longdouble      :return true;
		case eGAtomictype_Pointer         :return true;
		case eGAtomictype_Sizet           :return false;
		case eGAtomictype_Intptr          :return true;
		case eGAtomictype_Intnearptr      :return true;
		case eGAtomictype_Intfarptr       :return true;
		case eGAtomictype_Nearpointer     :return false; 
		// IR-side types
		case eGAtomictype_Int8            :return true;
		case eGAtomictype_Uint8           :return false;
		case eGAtomictype_Int16           :return true;
		case eGAtomictype_Uint16          :return false;
		case eGAtomictype_Int32           :return true;
		case eGAtomictype_Uint32          :return false;
		// TODO: : ptrdiff_t is signed but nearpointer is not
		default: 
			printf("ss: [E] mtGType_IsSigned: Unrecognized atomic type %i∙%s\n",
				self->atomicbasetype,
				meGAtomictype_ToStringTable[self->atomicbasetype]
			);
			ErfError();
			return false;
			break;
	};
};
tGType* mtGType_Compose(tGType* t1,tGType* t2){
	fprintf(stderr,"G:  [F] mtGType_Compose: Unfinished code hit! \n");
	exit(2);
};
tGType* mtGType_GetBasetype(tGType* self){
#ifdef qvGTrace
	//printf("ss: [T] mtGType_GetBasetype: entered \n");
#endif
	assert(self);
	if(
		  (self->atomicbasetype==eGAtomictype_Pointer)
		||(self->atomicbasetype==eGAtomictype_Array)
		||(self->atomicbasetype==eGAtomictype_Function)
	){
#ifdef qvGTrace
	//printf("ss: [T] mtGType_GetBasetype: got it \n");
#endif
		return mtGType_GetBasetype(self->complexbasetype);
	}else{
		return self;
	};
	exit(2);
};
bool mtGType_IsCastableto(tGType* self,tGType* type){
	assert(self);
	assert(type);
	if(
		  (self->atomicbasetype==eGAtomictype_Pointer)
		&&(
			  (self->atomicbasetype!=eGAtomictype_Pointer)
			&&(self->atomicbasetype!=eGAtomictype_Array)
		)
	)return false;
	if(
		(
			  (self->atomicbasetype==eGAtomictype_Structure)
			||(self->atomicbasetype==eGAtomictype_Union)
		)&&(
			  (type->atomicbasetype!=eGAtomictype_Structure)
			&&(type->atomicbasetype!=eGAtomictype_Union)
		)
	)return false;
	if(
		(
			  (type->atomicbasetype==eGAtomictype_Structure)
			||(type->atomicbasetype==eGAtomictype_Union)
		)&&(
			  (self->atomicbasetype!=eGAtomictype_Structure)
			&&(self->atomicbasetype!=eGAtomictype_Union)
		)
	)return false;
	return true;
};
bool mtGType_Equals(tGType* self,tGType* type){
	assert(self);
	assert(type);
	if(
		  (self->atomicbasetype==eGAtomictype_Function)
		&&(type->atomicbasetype==eGAtomictype_Function)
	){
		for(
			tListnode *i=self->functionarguments, *j=type->functionarguments;
			(i!=nullptr)&&(j!=nullptr);
			i=i->next,j=j->next
		)
			if(!mtGType_Equals(i->item,i->item))return false;
	};
	if(
		(
			  (self->atomicbasetype==eGAtomictype_Pointer)
			&&(type->atomicbasetype==eGAtomictype_Pointer)
		)||(
			  (self->atomicbasetype==eGAtomictype_Array)
			&&(type->atomicbasetype==eGAtomictype_Array)
		)||(
			  (self->atomicbasetype==eGAtomictype_Function)
			&&(type->atomicbasetype==eGAtomictype_Function)
		)
	){
		return mtGType_Equals(self->complexbasetype,type->complexbasetype);
	};
	if(self->valuecategory!=type->valuecategory)return false;
	return self->atomicbasetype==type->atomicbasetype;
};
eGValuecategory mtGType_GetValuecategory(tGType /* modifies */ * self){
#ifdef qvGTrace
	//printf("ss: [T] mtGType_GetValuecategory: entered \n");
#endif
	assert(mtGType_GetBasetype(self));
	return 
			mtGType_GetBasetype(self)
		?	mtGType_GetBasetype(self)->valuecategory
		:	0;
}
tGType* mtGType_SetValuecategory(tGType /* modifies */ * self, eGValuecategory val){
#ifdef qvGTrace
	printf("ss: [T] mtGType_SetValuecategory: entered \n");
#endif
	mtGType_GetBasetype(self)->valuecategory=val;
	return self;
}
tGType* mtGType_Transform_Ignorenullptr(tGType /* modifies */ * self){
	if(self==nullptr)return self;
	return mtGType_Transform(self);
};
tGType* mtGType_Transform(tGType /* modifies */ * self){ // Transform type from C-side types to IR-side types
#ifdef qvGTrace
	printf("ss: [T] mtGType_Transform: entered \n");
#endif
	if(!self){
		printf("ss: [E] mtGType_Transform: nullptr \n");
		ErfError();
		return nullptr;
	};
	if(self->complexbasetype)mtGType_Transform(self->complexbasetype);
	if(
		  (self->atomicbasetype==eGAtomictype_Function)
		||(self->atomicbasetype==eGAtomictype_Nearfunction)
		||(self->atomicbasetype==eGAtomictype_Farfunction)
	){
		mtListnode_Foreach(
			self->functionarguments,
			(void(*)(void*))mtGType_Transform_Ignorenullptr
		);
	};
	//if(self->atomicbasetype==eGAtomictype_Enumeration){
	//	*self=*(self->complexbasetype);
	//};
	switch(self->atomicbasetype){
		case eGAtomictype_Char            :self->atomicbasetype=eGAtomictype_Uint8       ;break;
		case eGAtomictype_Signedchar      :self->atomicbasetype=eGAtomictype_Int8        ;break;
		case eGAtomictype_Unsignedchar    :self->atomicbasetype=eGAtomictype_Uint8       ;break;
		case eGAtomictype_Short           :self->atomicbasetype=eGAtomictype_Int16       ;break;
		case eGAtomictype_Unsignedshort   :self->atomicbasetype=eGAtomictype_Uint16      ;break;
		case eGAtomictype_Int             :self->atomicbasetype=eGAtomictype_Int16       ;break;
		case eGAtomictype_Unsigned        :self->atomicbasetype=eGAtomictype_Uint16      ;break;
		case eGAtomictype_Long            :self->atomicbasetype=eGAtomictype_Int32       ;break;
		case eGAtomictype_Unsignedlong    :self->atomicbasetype=eGAtomictype_Uint32      ;break;
		case eGAtomictype_Longlong        :self->atomicbasetype=eGAtomictype_Int64       ;break;
		case eGAtomictype_Unsignedlonglong:self->atomicbasetype=eGAtomictype_Uint64      ;break;
		case eGAtomictype_Boolean         :self->atomicbasetype=eGAtomictype_Uint8       ;break;
		case eGAtomictype_Float           :self->atomicbasetype=eGAtomictype_Float32     ;break;
		case eGAtomictype_Double          :self->atomicbasetype=eGAtomictype_Float64     ;break;
		case eGAtomictype_Longdouble      :self->atomicbasetype=eGAtomictype_Float80     ;break;

		case eGAtomictype_Pointer         :self->atomicbasetype=eGAtomictype_Nearpointer ;break;
		//case eGAtomictype_Array           :self->atomicbasetype=eGAtomictype_Neararray   ;break;
		case eGAtomictype_Function        :self->atomicbasetype=eGAtomictype_Nearfunction;break;
		case eGAtomictype_Sizet           :self->atomicbasetype=eGAtomictype_Uint16      ;break;
		case eGAtomictype_Intptr          :self->atomicbasetype=eGAtomictype_Uint16      ;break;
		case eGAtomictype_Intnearptr      :self->atomicbasetype=eGAtomictype_Uint16      ;break;
		case eGAtomictype_Intfarptr       :self->atomicbasetype=eGAtomictype_Uint32      ;break;
		default: break;
	};
	return self;
};
tGType /* gives ownership */ * mtGType_Warp(tGType /* takeown */ * self){
	ErfEnter_String("mtGType_Warp");
	tGType* temp = mtGType_GetBasetype(self);
	tGType* base = mtGType_GetBasetype(self);
	for(tGType* i = self;i!=base;){
		tGType* j = i->complexbasetype;
		i->complexbasetype=temp;
		temp = i;
		i = j;
	};
	ErfLeave();
	return temp;
};
char* mtGType_ToString(tGType * self){
	char* s1;
	if(!self) return mtString_Clone("(nil)");
	switch(mtGType_GetValuecategory(self)){ // Value category
		case eGValuecategory_Leftvalue:       s1="lvalue ";   break;
		case eGValuecategory_Rightvalue:      s1="rvalue ";   break;
		case eGValuecategory_Novalue:         s1="novalue ";  break;
		default:                              s1="unkvalue "; break;
	}
	return mtString_Join(s1,mtGType_ToString_Embeddable(self));
};
//char* mtGType_ToString_Embeddable_Legacy(tGType* /* MfCcMmDynamic */ self){
//	char *s1;
//	char *s2;
//	if(self==nullptr){
//		return mtString_Clone("totally_invalid");
//	}else if(self->atomicbasetype==eGAtomictype_Unresolved){
//		// Unresolved
//		//sprintf(buffer,"unresolved<%s> ",self->unresolvedsymbol);
//		//return self->unresolvedsymbol;
//		assert(self->unresolvedsymbol);
//		s1 = mtString_Join(self->unresolvedsymbol,">");
//		s2 = mtString_Join("unresolved<",s1);
//		free(s1);
//		s1 = mtString_Join(
//			 self->valuecategory==eGValuecategory_Leftvalue?"lvalue "
//			:self->valuecategory==eGValuecategory_Farleftvalue?"farlvalue "
//			:self->valuecategory==eGValuecategory_Rightvalue?"rvalue "
//			:self->valuecategory==eGValuecategory_Novalue?"novalue "
//			:"unkvalue ",
//			s2
//		);
//		free(s2);
//		return s1;
//	}else if(self->atomicbasetype==eGAtomictype_Enumeration){ 
//		// Enumeration
//		assert(self->unresolvedsymbol);
//		s1 = mtString_Join(self->unresolvedsymbol,"");
//		s2 = mtString_Join("enum ",s1);
//		free(s1);
//		return s2;
//		//LfiPrint_LxNode("p:",self->precompiledenumeration);
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Structure){ 
//		s1 = mtString_Clone("struct");
//		if(self->unresolvedsymbol){
//			mtString_Append(&s1," ");
//			mtString_Append(&s1,self->unresolvedsymbol);
//		};
//		if(self->precompiledstructure){
//			//mtString_Append(&s1," { ... }");
//			mtString_Append(&s1," ( ");
//			mtString_Append(&s1,"/* ");
//			mtString_Append(&s1,
//				mtString_FromInteger(
//					mtList_Count(self->precompiledstructure)
//				)
//			);
//			mtString_Append(&s1," fields */");
//			// Inlined List.foreach cuz no capturing lambdas *yet*
//			//for(tListnode* i=self->precompiledstructure->first;i!=nullptr;i=i->next){
//			//	mtString_Append(&s1,mtLxNode_ToString((tLxNode*)i->item));
//			//	mtString_Append(&s1,"; ");
//			//}
//			mtString_Append(&s1," )");
//		};
//		if(self->structure){
//			//mtString_Append(&s1," { ... }");
//			mtString_Append(&s1," { ");
//			mtString_Append(&s1,"/* ");
//			mtString_Append(&s1,
//				mtString_FromInteger(
//					self->structsize
//				)
//			);
//			mtString_Append(&s1," bytes */ ");
//			//// No display of member locations right now
//			//// Member locations
//			//for(tListnode* i=self->structure->symbols.first;i!=nullptr;i=i->next){
//			//	mtString_Append(&s1,mtGSymbol_ToString((tGSymbol*)i->item));
//			//	mtString_Append(&s1," ");
//			//}
//			mtString_Append(&s1,"}");
//		};
//		//if(!self->unresolvedsymbol){
//		//	// Occasionally structures are defined only by their fields
//		//	return mtString_Clone("struct");
//		//}else{
//		//	s1 = mtString_Join(self->unresolvedsymbol,"");
//		//	s2 = mtString_Join("struct ",s1);
//		//};
//		return s1;
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Pointer){ 
//		return mtString_Join(
//			mtGType_ToString_Embeddable(self->complexbasetype),
//			"*"
//		);
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Nearpointer){ 
//		return mtString_Join(
//			mtGType_ToString_Embeddable(self->complexbasetype),
//			"*near"
//		);
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Farpointer){ 
//		return mtString_Join(
//			mtGType_ToString_Embeddable(self->complexbasetype),
//			"*far"
//		);
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Array){ 
//		return mtString_Join(
//			mtGType_ToString_Embeddable(self->complexbasetype),
//			mtString_Join(
//				"[",
//				mtString_Join(
//					mtString_FromInteger(
//						self->arraysize
//					),
//					"]"
//				)
//			)
//		);
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Function){ 
//		char* s1 = mtString_Clone("");
//		for(tListnode* i=self->functionarguments;i!=nullptr;i=i->next){
//			mtString_Append(&s1,mtGType_ToString((tGType*)i->item));
//			mtString_Append(&s1,",");
//		}
//		return mtString_Join(
//			mtGType_ToString_Embeddable(self->complexbasetype),
//			mtString_Join(
//				"(",
//				mtString_Join(
//					s1,
//					")"
//				)
//			)
//		);
//		//LfPrint_GNamespace(self->structure);
//	}else if(self->atomicbasetype==eGAtomictype_Void){ 
//		return mtString_Join(
//			 self->valuecategory==eGValuecategory_Leftvalue?"lvalue "
//			:self->valuecategory==eGValuecategory_Farleftvalue?"farlvalue "
//			:self->valuecategory==eGValuecategory_Rightvalue?"rvalue "
//			:self->valuecategory==eGValuecategory_Novalue?"novalue "
//			:"unkvalue ",
//			"void"
//		);
//		//printf("void ",self->unresolvedsymbol);
//		//LfPrint_GNamespace(self->structure);
//	}else{
//		// Resolve atomic type to string
//		s1 = "unknown<";
//		s1 = mtString_Join(s1,mtString_FromInteger(self->atomicbasetype));
//		s2 = mtString_Join(s1,">");
//		free(s1);
//		for(
//			GAtomictypetostring_Entry * ptr = GAtomictypetostring;
//			ptr->str;
//			ptr++
//		){
//			if(ptr->atomictype==self->atomicbasetype){
//				s2 = mtString_Clone(ptr->str);
//			};
//		};
//		return mtString_Join(
//			 self->valuecategory==eGValuecategory_Leftvalue?"lvalue "
//			:self->valuecategory==eGValuecategory_Farleftvalue?"farlvalue "
//			:self->valuecategory==eGValuecategory_Rightvalue?"rvalue "
//			:self->valuecategory==eGValuecategory_Novalue?"novalue "
//			:"unkvalue ",
//			s2
//		);
//	};
//};
char* mtGType_ToString_Embeddable(tGType* /* MfCcMmDynamic */ self){
	char* s1 = mtString_Create();
	if(self==nullptr){
		mtString_Append(&s1,"(nil)");
	}else if(self->atomicbasetype==eGAtomictype_Unresolved){
		assert(self->unresolvedsymbol);
		mtString_Append(&s1,"unresolved<");
		mtString_Append(&s1,self->unresolvedsymbol);
		mtString_Append(&s1,">");
	}else if(self->atomicbasetype==eGAtomictype_Enumeration){ 
		assert(self->unresolvedsymbol);
		mtString_Append(&s1,"enum ");
		mtString_Append(&s1,self->unresolvedsymbol);
		mtString_Append(&s1," : ");
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		if(self->precompiledenumeration)
			mtString_Append(&s1," ( ... )");
		if(self->structure)
			mtString_Append(&s1," { ... }");
	}else if(self->atomicbasetype==eGAtomictype_Structure){ 
		mtString_Append(&s1,"struct ");
		mtString_Append(&s1,self->unresolvedsymbol?:"(anonymous)");
		if(self->precompiledstructure){
			char* s2 = mtString_Format(" ( /* %i fields */ )",
				mtList_Count(self->precompiledstructure));
			mtString_Append(&s1,s2);
			mtString_Destroy(s2);
		};
		if(self->structure){
			char* s2 = mtString_Format(" { /* %i bytes */ }",
				(int)self->structsize);
			mtString_Append(&s1,s2);
			mtString_Destroy(s2);
		};
	}else if(self->atomicbasetype==eGAtomictype_Pointer){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		mtString_Append(&s1,"*");
	}else if(self->atomicbasetype==eGAtomictype_Nearpointer){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		mtString_Append(&s1,"*near");
	}else if(self->atomicbasetype==eGAtomictype_Farpointer){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		mtString_Append(&s1,"*far");
	}else if(self->atomicbasetype==eGAtomictype_Array){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		char* s2 = mtString_Format("[%i]",(int)self->arraysize);
		mtString_Append(&s1,s2);
		mtString_Destroy(s2);
	}else if(self->atomicbasetype==eGAtomictype_Function){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		mtString_Append(&s1,"(");
		for(tListnode* i=self->functionarguments;i!=nullptr;i=i->next){
			mtString_Append(&s1,mtGType_ToString((tGType*)i->item));
			mtString_Append(&s1,",");
		}
		mtString_Trimlast(s1);
		mtString_Append(&s1,")");
	}else if(self->atomicbasetype==eGAtomictype_Nearfunction){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		mtString_Append(&s1,"(");
		if(self->functionarguments){
			for(tListnode* i=self->functionarguments;i!=nullptr;i=i->next){
				mtString_Append(&s1,mtGType_ToString((tGType*)i->item));
				mtString_Append(&s1,",");
			};
			//mtString_Trimlast(s1);
		}else{
			mtString_Append(&s1,"void");
		};
		mtString_Append(&s1,")near");
	}else if(self->atomicbasetype==eGAtomictype_Farfunction){ 
		mtString_Append(&s1,mtGType_ToString_Embeddable(self->complexbasetype));
		mtString_Append(&s1,"(");
		if(self->functionarguments){
			for(tListnode* i=self->functionarguments;i!=nullptr;i=i->next){
				mtString_Append(&s1,mtGType_ToString((tGType*)i->item));
				mtString_Append(&s1,",");
			};
			mtString_Trimlast(s1);
		}else{
			mtString_Append(&s1,"void");
		};
		mtString_Append(&s1,")far");
	}else{
		mtString_Append(&s1,meGAtomictype_ToStringTable[self->atomicbasetype]);
	};
	return s1;
};

// ------------------------------ class tGSymbol ------------------------------

tGSymbol* mtGSymbol_Create(){
	return calloc(sizeof(tGSymbol),1);
};
tGSymbol* mtGSymbol_CreateConstant(char* name, tGType* type, tGTargetUintmax val){
	tGSymbol* temp = mtGSymbol_Create();
	temp->name = name;
	temp->symbolkind = mtGSymbol_eType_Constant;
	temp->type = type;
	temp->value = val;
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
char* mtGSymbol_ToString(tGSymbol* self){
	char buffer[4096];
	if(self==nullptr){
		return mtString_Clone("nullptr");
	}else{
		//snprintf(buffer,4096,
		//	"Lf: [M] ¤ Symbol %p:%s %i•%s\n",
		//	self,
		//	mtGType_ToString_Embeddable(self->type),
		//	self->symbolkind,
		//	self->name
		//);
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
void mtGType_Verifycast(tGType* from, tGType* to){ // Throws warnings/errors on invalid casts
};

// ------------------------------ class tLxNode ------------------------------
tLxNode* mtLxNode_Create(void){
	return calloc(sizeof(tLxNode),1);
};
tLxNode* mtLxNode_Clone(tLxNode* self){
	return memcpy(malloc(sizeof(tLxNode)),self,sizeof(tLxNode));
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
			sprintf(buffer,"intconstant(%i)",self->constant);
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
// ------------------------------ class tToken ------------------------------
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
// ---------------------------- class tGNamespace ----------------------------
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
	//printf("ss: [T] mtGNamespace_Findsymbol_NameKind: Entered \n");
	printf("ss: [T] mtGNamespace_Findsymbol_NameKind(namespace %p, symbol %i•%s): Entered \n",self,kind,name);
#endif
	assert(name);
	// Empty namespace
	if(self==nullptr){
		printf("ss: [E] mtGNamespace_Findsymbol_NameKind(%p): Symbol %i•%s not found: No namespace \n",self,kind,name);
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
// -------------------------- class tGTargetPointer --------------------------
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
// --------------------------- class tGInstruction ---------------------------
tGInstruction* mtGInstruction_Create(void){
	return calloc(sizeof(tGInstruction),1);
};
tGInstruction* mtGInstruction_Clone(tGInstruction* self){
	return memcpy(malloc(sizeof(tGInstruction)),self,sizeof(tGInstruction));
};
tGInstruction* mtGInstruction_Deepclone(tGInstruction* self){
	assert(self);
	//assert(self->jumptarget==nullptr); // Cloning trees is all nice and great,
	//                                   // it's just recursive.
	//                                   // Cloning directed graphs on the other
	//                                   // hand gets painful quickly.
	for(tGInstruction* i=self;i;i=i->next) 
		for(tGInstruction* j=self;j;j=j->next) 
			assert(j->jumptarget!=i);
	// ↑ For now I'll just bang in a check for jump target not being 
	//   referenced by other nodes here.
	//   Yes, it's cubic time complexity.
	//   > TODO: get a proper cyclic (acyclic?) directed graph cloning here
	//   I don't care about it *much* as of now, but we will see.
	return mtGInstruction_Clone(&(tGInstruction){
		                                              // // Common
		.opcode = self->opcode,                       // tGOpcode opcode; 
		.label = self->label,                         // char* label;
		.comment = self->comment,                     // char* comment;
		.next = self->next                            // struct tGInstruction* next;
		        ?mtGInstruction_Deepclone(self->next):nullptr,
		.jumptarget = self->jumptarget,               // struct tGInstruction* jumptarget;
		                                              // // Primary IR
		.immediate = self->immediate,                 // tGTargetUintmax immediate;
		                                              // // Secondary IR
		                                              // tIr2Operand dest, source, source2;
		.dest = self->dest,
		.source = self->source,
		.source2 = self->source2,
	});
};
tGInstruction* mtGInstruction_CreateAllocatestorage(tGTargetSizet size){
	tGInstruction* i=mtGInstruction_Create();
	i->opcode.opr=tInstruction_Allocatestorage2;
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
tGInstruction* mtGInstruction_CreateDefinecharater(char ch){
	tGInstruction* i=mtGInstruction_Create();
	i->opcode.opr=tInstruction_Definevalue;
	i->opcode.isize=eGAtomictype_Uint8;
	i->immediate=ch;
	return i;
};
tGInstruction* mtGInstruction_CreateStringdefinition(char* str){
	assert(str);
	tGInstruction* i=mtGInstruction_CreateDefinecharater(*str);
	if(*str){
		i->next=mtGInstruction_CreateStringdefinition(str+1);
	}else{
		i->next=nullptr;
	};
	return i;
};
tGInstruction* mtGInstruction_CreateBasic(short opcode,enum eGAtomictype size){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=opcode;
	i->opcode.isize=size;
	return i;
};
tGInstruction* mtGInstruction_CreateSegmented(short opcode,enum eGSegment segment,enum eGAtomictype size){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=opcode;
	i->opcode.segment=segment;
	i->opcode.isize=size;
	return i;
};
tGInstruction* mtGInstruction_CreateImmediateCodepointer(int opcode,int size,int val, tGInstruction* jumptarget){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=opcode;
	i->opcode.isize=size;
	i->immediate=val;
	i->jumptarget=jumptarget;
	return i;
};
tGInstruction* mtGInstruction_CreateImmediate(int opcode,int size,int val){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=opcode;
	i->opcode.isize=size;
	i->immediate=val;
	return i;
};
tGInstruction* mtGInstruction_CreateCodepointer(int opcode,int size,tGInstruction* val){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=opcode;
	i->opcode.isize=size;
	i->jumptarget=val;
	return i;
};
tGInstruction* mtGInstruction_CreateCast(int opcode,int fromsize,int tosize){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=opcode;
	i->opcode.isize=fromsize;
	i->opcode.altsize=tosize;
	return i;
};
tGInstruction* mtGInstruction_CreateExtern(char* /* takeown */ name){
	tGInstruction* i = mtGInstruction_Create();
	i->opcode.opr=tInstruction_Extern;
	i->opcode.isize=eGAtomictype_Void;
	i->label = name;
	return i;
};
tGInstruction* mtGInstruction_SetLabel(tGInstruction* self,char* /* takeown */ name){
	self->label = name;
	return self;
};
tGInstruction* mtGInstruction_GetLast(tGInstruction* self){
#ifdef qvGTrace
	printf("ss: [T] mtGInstruction_GetLast(%p): Entered\n",self);
#endif
	// Normally I'd use a forloop `for(T i = self;i;i=i->next);` but I'm
	// lazy here so recursion it is.
	assert(self);
	//if(self->next==nullptr){
	//	return self;
	//}else{
	//	return mtGInstruction_GetLast(self->next);
	//}
	for(tGInstruction* i = self; i; i=i->next) {
		tList* visited = mtList_Create();
		if(!i->next) {
			mtList_Destroy(visited);
			printf("ss: [T] mtGInstruction_GetLast(%p): Leaving: Found base\n",self);
			return i;
		};
		if(mtList_Find(visited,i)){
			printf("ss: [T] mtGInstruction_GetLast(%p): %p: Cycle found\n",self,i);
			ErfError();
		};
		mtList_Append(visited,i);
	};
	printf("ss: [T] mtGInstruction_GetLast(%p): Leaving: Weird case\n",self);
	return self;
};
tGInstruction* mtGInstruction_Join_Modify(tGInstruction /* modifies */ * self, tGInstruction* /* takeowns */ val){
	if(!self)return val;
	mtGInstruction_GetLast(self)->next=val;
	return self;
};
char* mtGInstruction_ToString(tGInstruction* self){
	char buffer[512];
	// Resolve atomic type to string
	char* size = "?";
	for(
		GAtomictypetostring_Entry * ptr = GAtomictypetostring;
		ptr->str;
		ptr++
	){
		if(ptr->atomictype==self->opcode.isize){
			size = ptr->str;
			break;
		};
	};
	
	snprintf(
		buffer,512,"%-20p: %-17s %-10i %-20p",
		self,
		mtString_Join(
			TokenidtoName_Compact[self->opcode.opr],
			mtString_Join(".",size)
		),
		(int)self->immediate,
		self->jumptarget
	);
	return mtString_Clone(buffer);
};
void LfPrint_GInstruction(tGInstruction* self){
	char buffer[512];
	snprintf(buffer,512,"Lf: [M] Printing instruction %p\n",self);
	LfIndent(buffer);
	for(tGInstruction* i = self;i;i=i->next){
		char* s1=mtGInstruction_ToString(i);
		snprintf(buffer,512,"Lf: [M] %s\n",s1);
		LfWriteline(buffer);
		free(s1);
	};
	LfUnindent("Lf: [M]   \n");
};
// --------------------------------- Global ---------------------------------
tGInstruction* GAddinstructiontosegment(eGSegment segment, tGInstruction* instr){
	return mtGInstruction_GetLast(GCompiled[segment])->next=instr;
};
void GInitializePerfile(){
	for(int i=0;i<meGSegment_Count;i++)GCompiled[i]=mtGInstruction_CreateCnop();
};
void GError(){
	fprintf(stderr,"SS: [E] Error occured, terminating \n");
	ErfError();
	//exit(1);
};
void GFatal(){
	ErfFatal();
	//exit(1);
};
void GFinalize(){
};
// -------------------------------- Unsorted --------------------------------
// ------------------------------- Submodules -------------------------------
#include "xcc-singlestage-tokenizer.c"
#include "xcc-singlestage-fetcher.c"
#include "xcc-singlestage-lexicalparser.c"
#include "xcc-singlestage-semanticpreparser.c"
#include "xcc-singlestage-symbolgen.c"
#include "xcc-singlestage-semanticparser.c"
#include "xcc-singlestage-irmaker.c"
#include "xcc-singlestage-ir2maker.c"
#include "xcc-singlestage-launcher.c"
// the end
