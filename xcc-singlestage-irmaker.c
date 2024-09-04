// XCC Intermediate representation creator
// Developed *simultaneously* with Semanticparser

tGInstruction* IgCompileExpression(tSpNode* self){
	assert(self);
#ifdef qvGTrace
	printf("IG: [T] IgCompileExpression: entered %i•%s\n",self->type,TokenidtoName[self->type]);
	LfPrint_SpNode(self);
#endif
	switch(self->type){
		case tSplexem_Integerconstant:
			assert(self->constant);
			return mtGInstruction_CreateImmediate(
				tInstruction_Constant,
				self->returnedtype->atomicbasetype,
				self->constant
			);
		default:
			assert(false);
	};
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
		default:
			assert(false);
	};
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
	printf("IG: [T] Entered with node %i•%s \n",self->type,TokenidtoName[self->type]);
	switch(self->type){
		case tSplexem_Nulldeclaration:
			break;
		case tSplexem_Declarationlist:
			IgParse(self->left);
			if(self->right)IgParse(self->right);
			break;
		case tSplexem_Functiondeclaration:
			mtGInstruction_GetLast(GCompiled[meGSegment_Code])->next=IgCompileFunction(self);
			break;
		default:
			assert(false);
	};
};
