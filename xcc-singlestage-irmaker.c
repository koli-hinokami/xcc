// XCC Intermediate representation creator
// Developed *simultaneously* with Semanticparser

tGInstruction* IgCompileExpression(tSpNode* self);
tGInstruction* IgCompileFunctionarguments(tSpNode* self){
	assert(self);
#ifdef qvGTrace
	printf("IG: [T] IgCompileFunctionarguments: entered %i•%s\n",self->type,TokenidtoName[self->type]);
	LfPrint_SpNode(self);
#endif
	switch(self->type){
		case tSplexem_Comma:
			// TODO: Make sure order is proper and introduce 
			//  retargeting #ifdef's and runtime flags to 
			//  differenciate `pascal` and `cdecl`/`stdcall`
			//  functions
			return mtGInstruction_Join_Modify(
				IgCompileFunctionarguments(self->left),
				IgCompileFunctionarguments(self->right)
			);
		default:
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self),
				mtGInstruction_CreateBasic(
					tInstruction_Pushargument,
					self->returnedtype->atomicbasetype
				)
			);
			assert(false);
	};
};
tGInstruction* IgCompileExpression(tSpNode* self){
	assert(self);
#ifdef qvGTrace
	printf("IG: [T] IgCompileExpression: entered %i•%s\n",self->type,TokenidtoName[self->type]);
	LfPrint_SpNode(self);
#endif
	switch(self->type){
		case tSplexem_Integerconstant:
			return mtGInstruction_CreateImmediate(
				tInstruction_Constant,
				self->returnedtype->atomicbasetype,
				self->constant
			);
		case tSplexem_Dereference: {
			tGInstruction* i;
			i = IgCompileExpression(self->left);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(
				tInstruction_Loadindirect,
				self->returnedtype->atomicbasetype
			);
			return i;
		};	break;
		case tSplexem_Addition: {
			tGInstruction* i;
			i=IgCompileExpression(self->left);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Pushleft,self->left->returnedtype->atomicbasetype);
			mtGInstruction_GetLast(i)->next=IgCompileExpression(self->right);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Popright,self->right->returnedtype->atomicbasetype);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Add,self->right->returnedtype->atomicbasetype);
			return i;
		};	break;
		case tSplexem_Substraction: {
			tGInstruction* i;
			i=IgCompileExpression(self->left);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Pushleft,self->left->returnedtype->atomicbasetype);
			mtGInstruction_GetLast(i)->next=IgCompileExpression(self->right);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Popright,self->right->returnedtype->atomicbasetype);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Substract,self->right->returnedtype->atomicbasetype);
			return i;
		};	break;
		case tSplexem_Symbol:
			assert(mtGType_GetBasetype(self->returnedtype)->valuecategory==eGValuecategory_Leftvalue);
			if(self->symbol->allocatedstorage->nonconstant){
				return mtGInstruction_CreateCodepointer(
					tInstruction_Loadaddress,
					eGAtomictype_Pointer,
					self->symbol->allocatedstorage->dynamicpointer
				);
				
			}else{
				assert(self->symbol->allocatedstorage->segment!=meGSegment_Far);
				if(self->symbol->allocatedstorage->segment==meGSegment_Stackframe){
					return mtGInstruction_Join_Modify(
						mtGInstruction_CreateImmediate(
							tInstruction_Constant,
							eGAtomictype_Pointer,
							self->symbol->allocatedstorage->offset
						),
						mtGInstruction_CreateBasic(
							tInstruction_Indexfp,
							eGAtomictype_Pointer
						)
					);
				}else{
					return mtGInstruction_CreateImmediate(
						tInstruction_Constant,
						self->returnedtype->atomicbasetype,
						self->symbol->allocatedstorage->offset
					);
				};
			};
		case tSplexem_Functioncall: {
			tGInstruction* i = mtGInstruction_CreateCnop();
			//i=IgCompileExpression(self->left);
			//mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Pushleft,self->left->returnedtype->atomicbasetype);
			//mtGInstruction_GetLast(i)->next=IgCompileExpression(self->right);
			//mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Popright,self->right->returnedtype->atomicbasetype);
			//mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Add,self->right->returnedtype->atomicbasetype);

			assert(self->left->returnedtype->atomicbasetype==eGAtomictype_Pointer);
			assert(self->left->returnedtype->complexbasetype->atomicbasetype==eGAtomictype_Function);
			mtGInstruction_GetLast(i)->next=IgCompileFunctionarguments(
				self->right
			);
			mtGInstruction_GetLast(i)->next=IgCompileExpression(self->left);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Call,self->left->returnedtype->atomicbasetype);
			return i;
		};	break;
		case tSplexem_Nullexpression: {
			return mtGInstruction_CreateImmediate(tInstruction_Constant,eGAtomictype_Void,0);
		};	break;
		case tSplexem_Stringconstant: {
			assert(mtGType_GetBasetype(self->returnedtype)->valuecategory==eGValuecategory_Leftvalue);
			if(self->symbol->allocatedstorage->nonconstant){
				return mtGInstruction_CreateCodepointer(
					tInstruction_Loadaddress,
					eGAtomictype_Pointer,
					self->symbol->allocatedstorage->dynamicpointer
				);
				
			}else{
				assert(self->symbol->allocatedstorage->segment!=meGSegment_Far);
				if(self->symbol->allocatedstorage->segment==meGSegment_Stackframe){
					return mtGInstruction_Join_Modify(
						mtGInstruction_CreateImmediate(
							tInstruction_Constant,
							eGAtomictype_Pointer,
							self->symbol->allocatedstorage->offset
						),
						mtGInstruction_CreateBasic(
							tInstruction_Indexfp,
							eGAtomictype_Pointer
						)
					);
				}else{
					return mtGInstruction_CreateImmediate(
						tInstruction_Constant,
						self->returnedtype->atomicbasetype,
						self->symbol->allocatedstorage->offset
					);
				};
			};
		};	break;
		case tSplexem_Cast:
			if(mtGType_Sizeof(self->returnedtype)!=mtGType_Sizeof(self->left->returnedtype)){
				printf("IG: [E] IgCompileExpression: Cast: Sizes mismatch: %i•%i \n",
					mtGType_Sizeof(self->returnedtype),
					mtGType_Sizeof(self->left->returnedtype)
				);
				assert(mtGType_Sizeof(self->returnedtype)==mtGType_Sizeof(self->left->returnedtype));
			};
			return IgCompileExpression(self->left);
		default:
			assert(false);
	};
	printf("IG: [F] IgCompileExpression: Absolutely incomprehensible control flow occured \n");
	assert(false);
	return nullptr;
};
tGInstruction* IgCompileStatement(tSpNode* self){
#ifdef qvGTrace
	printf("IG: [T] IgCompileStatement: entered %i•%s\n",self->type,TokenidtoName[self->type]);
#endif
	assert(self);
	switch(self->type){
		case tSplexem_Blockstatement: {
			tGInstruction* i;
			i=IgCompileStatement(self->left);
			assert(mtGInstruction_GetLast(i));
			if(self->right)mtGInstruction_GetLast(i)->next=IgCompileStatement(self->right);
			return i;
		};	break;
		case tSplexem_Returnstatement: {
			tGInstruction* i;
			i=IgCompileExpression(self->left);
			assert(mtGInstruction_GetLast(i));
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(
				tInstruction_Return,
				self->left->returnedtype->atomicbasetype
			);
			return i;
		};	break;
		case tSplexem_Expressionstatement: {
			tGInstruction* i;
			i=IgCompileExpression(self->left);
			assert(mtGInstruction_GetLast(i));
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(
				tInstruction_Drop,
				self->left->returnedtype->atomicbasetype
			);
			return i;
		};	break;
		default:
			assert(false);
	};
	printf("IG: [F] IgCompileStatement: Absolutely incomprehensible control flow occured \n");
	assert(false);
	return nullptr;
};
tGInstruction* IgCompileFunction(tSpNode* self){
#ifdef qvGTrace
	printf("IG: [T] IgCompileFunction: entered \n");
#endif
	assert(self);
	// Find the place to write code to
	tGInstruction* code = self->symbol->allocatedstorage->dynamicpointer;
	// Prologue
	mtGInstruction_GetLast(code)->next=mtGInstruction_CreateBasic(tInstruction_Enterframe,eGAtomictype_Void);
	// Compile function itself
	mtGInstruction_GetLast(code)->next=IgCompileStatement(self->right);
	// Implied return statement
	mtGInstruction_GetLast(code)->next=mtGInstruction_CreateImmediate(tInstruction_Constant,self->returnedtype->complexbasetype->atomicbasetype,0);
	mtGInstruction_GetLast(code)->next=mtGInstruction_CreateBasic(tInstruction_Prereturn,self->returnedtype->complexbasetype->atomicbasetype);
	// Epilogue
	mtGInstruction_GetLast(code)->next=mtGInstruction_CreateBasic(tInstruction_Leaveframe,eGAtomictype_Void);
	mtGInstruction_GetLast(code)->next=mtGInstruction_CreateBasic(tInstruction_Return,self->returnedtype->complexbasetype->atomicbasetype);
	return code;


};
void IgParse(tSpNode* self){
	assert(self);
	printf("IG: [T] Entered with node %i•%s \n",self->type,TokenidtoName[self->type]);
	switch(self->type){
		case tSplexem_Nulldeclaration:
			break;
		case tSplexem_Declarationlist:
			if(self->left) IgParse(self->left);
			if(self->right)IgParse(self->right);
			break;
		case tSplexem_Functiondeclaration:
			mtGInstruction_GetLast(GCompiled[meGSegment_Code])->next=IgCompileFunction(self);
			break;
		default:
			assert(false);
	};
};
// void IgDumpir(ptGInstruction code[passedbypointer meGSegment_Count] /* The code */, FILE* targetfile)
void IgDumpir(tGInstruction** code, FILE* file){ 
	// Print IR as text into file given as argument
	fprintf(
		file,
		"; --- XCC Retargetable C Crosscompiler - Intermediate representation dump ---\n"
		"; --------------------------- CAsm format on Jam-1 --------------------------\n"
		"\t.architecture 8BitPipeline-xcc-ir\n"
		"\t.include lib\\ircompiler.inc\n"
		"\t.include lib\\libc.inc\n"
		//"\tircompiler_pre\n"
	);
	for(int i=0;i<meGSegment_Count;i++){
		if(i!=meGSegment_Relative){
			fprintf(file,"\t.segment %s\n",meGSegment_ToStringTable[i]);
			for(tGInstruction* j=code[i];j!=nullptr;j=j->next){
				fprintf(
					file,
					"l_%p:\tv.%s.%s.%s %i,l_%p \n",
					j,
					TokenidtoName_Compact[j->opcode.opr],
					meGSegment_ToStringTable[j->opcode.segment],
					meGAtomictype_ToStringTable[j->opcode.isize],
					j->immediate,
					j->jumptarget
				);
			};
		};
	};
	//fprintf(file,"\tircompiler_post\n");
};

