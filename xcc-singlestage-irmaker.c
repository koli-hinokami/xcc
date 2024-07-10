// XCC Intermediate representation creator
// Developed *simultaneously* with Semanticparser

// -- Globals --
tSpNode* IgCurrentfunction;
tList /* <tInstruction*> */ IgPendingsideeffects;
// -- Forward declarations --
tGInstruction* IgCompileExpression(tSpNode* self);
tGInstruction* IgCompileConditionaljump(tSpNode* self, tGInstruction* jumptarget);
// -- Functions --
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
				IgCompileFunctionarguments(self->right),
				IgCompileFunctionarguments(self->left)
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
	tGInstruction* retval = nullptr;
	ErfEnter_String(
		mtString_Format(
			"IgCompileExpression: %i•%s",
			self->type,
			TokenidtoName[self->type]
		)
	);
	switch(self->type){
		case tSplexem_Integerconstant: {
			retval = mtGInstruction_CreateImmediate(
				tInstruction_Constant,
				self->returnedtype->atomicbasetype,
				self->constant
			);
		};	break;
		case tSplexem_Dereference: {
			retval = mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_CreateSegmented(
					tInstruction_Load,
					meGSegment_Data,
					self->returnedtype->atomicbasetype
				)
			);
		};	break;
		case tSplexem_Negation: {
			retval = IgCompileExpression(self->left);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Negation,
				self->returnedtype->atomicbasetype
			);
		};	break;
		case tSplexem_Bitwiseand: {
			retval=IgCompileExpression(self->left);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Pushleft,
				self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=IgCompileExpression(self->right);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Popright,
				self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Bitwiseand,
				self->left->returnedtype->atomicbasetype
			);
		};	break;
		case tSplexem_Addition: {
			retval=IgCompileExpression(self->left);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Pushleft,
				self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=IgCompileExpression(self->right);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Popright,
				self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Add,
				self->left->returnedtype->atomicbasetype
			);
		};	break;
		case tSplexem_Substraction: {
			retval=IgCompileExpression(self->left);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Pushleft,self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=IgCompileExpression(self->right);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Popright,self->right->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Substract,self->right->returnedtype->atomicbasetype
			);
		};	break;
		case tSplexem_Symbol: {
			assert(mtGType_GetBasetype(self->returnedtype)->valuecategory==eGValuecategory_Leftvalue);
			assert(self->symbol->allocatedstorage); // by now storage should be allocated...
			if(self->symbol->allocatedstorage->nonconstant){
				retval = mtGInstruction_CreateCodepointer(
					tInstruction_Loadaddress,
					eGAtomictype_Nearpointer,
					self->symbol->allocatedstorage->dynamicpointer
				);
			}else{
				assert(self->symbol->allocatedstorage->segment!=meGSegment_Far);
				if(self->symbol->allocatedstorage->segment==meGSegment_Stackframe){
					retval = mtGInstruction_Join_Modify(
						mtGInstruction_CreateImmediate(
							tInstruction_Constant,
							eGAtomictype_Nearpointer,
							self->symbol->allocatedstorage->offset
						),
						mtGInstruction_CreateBasic(
							tInstruction_Indexfp,
							eGAtomictype_Nearpointer
						)
					);
				}else{
					retval = mtGInstruction_CreateImmediate(
						tInstruction_Constant,
						self->returnedtype->atomicbasetype,
						self->symbol->allocatedstorage->offset
					);
				};
			};
		};	break;
		case tSplexem_Functioncall: {
			retval = mtGInstruction_CreateCnop();
			//i=IgCompileExpression(self->left);
			//mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Pushleft,self->left->returnedtype->atomicbasetype);
			//mtGInstruction_GetLast(i)->next=IgCompileExpression(self->right);
			//mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Popright,self->right->returnedtype->atomicbasetype);
			//mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(tInstruction_Add,self->right->returnedtype->atomicbasetype);
			assert(mtGType_IsPointer(self->left->returnedtype));
			assert(mtGType_IsFunction(self->left->returnedtype->complexbasetype));
			// TODO: General: Find a way to get fextinfo by symbol
			//                and assert that we're calling an 
			//                stdcall subroutine
			mtGInstruction_GetLast(retval)->next=IgCompileFunctionarguments(
				self->right
			);
			mtGInstruction_GetLast(retval)->next=IgCompileExpression(self->left);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Call,self->left->returnedtype->atomicbasetype);
		};	break;
		case tSplexem_Nullexpression: {
			retval = mtGInstruction_CreateImmediate(tInstruction_Constant,eGAtomictype_Void,0);
		};	break;
		case tSplexem_Stringconstant: {
			assert(mtGType_GetBasetype(self->returnedtype)->valuecategory==eGValuecategory_Leftvalue);
			if(self->symbol->allocatedstorage->nonconstant){
				retval = mtGInstruction_CreateCodepointer(
					tInstruction_Loadaddress,
					eGAtomictype_Nearpointer,
					self->symbol->allocatedstorage->dynamicpointer
				);
			}else{
				assert(self->symbol->allocatedstorage->segment!=meGSegment_Far);
				if(self->symbol->allocatedstorage->segment==meGSegment_Stackframe){
					retval = mtGInstruction_Join_Modify(
						mtGInstruction_CreateImmediate(
							tInstruction_Constant,
							eGAtomictype_Nearpointer,
							self->symbol->allocatedstorage->offset
						),
						mtGInstruction_CreateBasic(
							tInstruction_Indexfp,
							eGAtomictype_Nearpointer
						)
					);
				}else{
					retval = mtGInstruction_CreateImmediate(
						tInstruction_Constant,
						self->returnedtype->atomicbasetype,
						self->symbol->allocatedstorage->offset
					);
				};
			};
		};	break;
		case tSplexem_Cast: {
			if(mtGType_Sizeof(self->returnedtype)!=mtGType_Sizeof(self->left->returnedtype)){
				//printf("IG: [D] IgCompileExpression: Cast: Sizes mismatch: %i•%i \n",
				//	mtGType_Sizeof(self->returnedtype),
				//	mtGType_Sizeof(self->left->returnedtype)
				//);
				//assert(mtGType_Sizeof(self->returnedtype)!=mtGType_Sizeof(self->left->returnedtype));
				//ErfFatal();
				retval = mtGInstruction_Join_Modify(
					IgCompileExpression(self->left),
					mtGInstruction_CreateCast(
						tInstruction_Cast,
						self->left->returnedtype->atomicbasetype,
						   self->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->returnedtype->atomicbasetype
					)
				);
			}else{
				retval = IgCompileExpression(self->left);
			};
		};	break;
		case tSplexem_Assign: {
			retval = mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateSegmented(
								tInstruction_Store,
								meGSegment_Data, // TODO: Segmentation: Get segment using
												 //        mtSpNode_GetSegment(self->right)
								self->right->returnedtype->atomicbasetype
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Preincrement: {
			assert(  self->left->returnedtype->valuecategory
			       ==eGValuecategory_Leftvalue);
			retval = mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_CreateSegmented(
					tInstruction_Lvalueincrement,
					meGSegment_Data, // TODO: Segmentation: Get segment using
									 //        mtSpNode_GetSegment(self->right)
					self->left->returnedtype->atomicbasetype
				)
			);
		};	break;
		case tSplexem_Shiftleft: {
			if(self->right->type == tSplexem_Integerconstant){
				retval = mtGInstruction_Join_Modify(
					IgCompileExpression(self->left),
					mtGInstruction_CreateImmediate(
						tInstruction_Constantshiftleft,
						self->left->returnedtype->atomicbasetype,
						self->right->constant
					)
				);
			}else{
				assert(false);
			};
		};	break;
		case tSplexem_Shiftright: {
			if(self->right->type == tSplexem_Integerconstant){
				retval = mtGInstruction_Join_Modify(
					IgCompileExpression(self->left),
					mtGInstruction_CreateImmediate(
						tInstruction_Constantshiftright,
						self->left->returnedtype->atomicbasetype,
						self->right->constant
					)
				);
			}else{
				assert(false);
			};
		};	break;
		case tSplexem_Multiplication: {
			retval=IgCompileExpression(self->left);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Pushleft,self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=IgCompileExpression(self->right);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Popright,self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(retval)->next=mtGInstruction_CreateBasic(
				tInstruction_Multiply,self->left->returnedtype->atomicbasetype
			);
		};	break;
		case tSplexem_Structuremember: {
			retval = mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_CreateImmediate(
					tInstruction_Index,
							self->left->returnedtype->valuecategory
						==	eGValuecategory_Leftvalue
					?	eGAtomictype_Nearpointer
					:	self->left->returnedtype->atomicbasetype,
					self->constant
				)
			);
		};	break;
		default: {
			printf("IG: [E] IgCompileExpression: Unrecognized "
			       "node %i•%s \n",
				self->type,TokenidtoName[self->type]
			);
			printf("IG: [E] IgCompileExpression: Secondary AST snippet: \n");
			LfPrint_SpNode(self);
			printf("IG: [E] IgCompileExpression: (secondary ast snippet end) \n");
			ErfError();
			retval = nullptr;
		};	break;
	};
	ErfLeave();
	return retval;
};
tGInstruction* IgCompileInvertedconditionaljump(
	tSpNode* self,
	tGInstruction* jumptarget
){ // compiles some exprs to compare-jump-condition
	assert(self);
	assert(self->left);
	assert(self->returnedtype);
	assert(self->returnedtype->atomicbasetype==eGAtomictype_Boolean);
	tGInstruction* expr = nullptr;
	// Check for special cases
	// TODO: Disable some based on configuration file settings
	switch(self->type){
		case tSplexem_Logicalnot: {
			return IgCompileConditionaljump(self->left,jumptarget);
		};	break;
		case tSplexem_Equality: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								tInstruction_Comparejumpnotequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Nonequality: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								tInstruction_Comparejumpequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Greaterequal: {
			assert(self->right);
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedlessthan
								:tInstruction_Comparejumpunsignedlessthan,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Lessequal: {
			assert(self->right);
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedgreaterthan
								:tInstruction_Comparejumpunsignedgreaterthan,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Greaterthan: {
			assert(self->right);
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedlessequal
								:tInstruction_Comparejumpunsignedlessequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Lessthan: {
			assert(self->right);
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedgreaterequal
								:tInstruction_Comparejumpunsignedgreaterequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Casttoboolean: {
			expr = IgCompileExpression(self->left);
			assert(self->left->returnedtype->valuecategory==eGValuecategory_Rightvalue);
			mtGInstruction_GetLast(expr)->next = 
				mtGInstruction_CreateCodepointer(
					tInstruction_Jumpfalse,
					self->left->returnedtype->atomicbasetype,
					jumptarget
				);
		};	break;
		default:
			assert(false);
	};
	// If no special cases seen, tailcall to IgCompileExpression 
	// and append jump instruction
	return expr;
}
tGInstruction* IgCompileConditionaljump(
	tSpNode* self,
	tGInstruction* jumptarget
){ // compiles some exprs to compare-jump-condition
	assert(self);
	assert(self->left);
	assert(self->returnedtype);
	assert(self->returnedtype->atomicbasetype==eGAtomictype_Boolean);
	tGInstruction* expr = nullptr;
	// Check for special cases
	// TODO: Disable some based on configuration file settings
	switch(self->type){
		case tSplexem_Logicalnot: {
			return IgCompileInvertedconditionaljump(self->left,jumptarget);
		};	break;
		case tSplexem_Equality: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								tInstruction_Comparejumpequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Nonequality: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								tInstruction_Comparejumpnotequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Greaterequal: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedgreaterequal
								:tInstruction_Comparejumpunsignedgreaterequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Lessequal: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedlessequal
								:tInstruction_Comparejumpunsignedlessequal,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Greaterthan: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedgreaterthan
								:tInstruction_Comparejumpunsignedgreaterthan,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Lessthan: {
			assert(
				  self->left->returnedtype->atomicbasetype
				==self->right->returnedtype->atomicbasetype
			);
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						   self->left->returnedtype->valuecategory
						 ==eGValuecategory_Leftvalue
						?eGAtomictype_Nearpointer
						:self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								   self->left->returnedtype->valuecategory
								 ==eGValuecategory_Leftvalue
								?eGAtomictype_Nearpointer
								:self->left->returnedtype->atomicbasetype
							),
							mtGInstruction_CreateCodepointer(
								 mtGType_IsSigned(self->left->returnedtype)
								?tInstruction_Comparejumpsignedlessthan
								:tInstruction_Comparejumpunsignedlessthan,
								self->left->returnedtype->atomicbasetype,
								jumptarget
							)
						)
					)
				)
			);
		};	break;
		case tSplexem_Casttoboolean: {
			expr = IgCompileExpression(self->left);
			assert(self->left->returnedtype->valuecategory==eGValuecategory_Rightvalue);
			mtGInstruction_GetLast(expr)->next = 
				mtGInstruction_CreateCodepointer(
					tInstruction_Jumptrue,
					self->left->returnedtype->atomicbasetype,
					jumptarget
				);
		};	break;
		default:
			printf("IG: [E] IgCompileConditionaljump: Unrecognized node %s",
				TokenidtoName[self->type]
			);
			ErfError();
			return nullptr;
	};
	return expr;
}
tGInstruction* IgEvaulaterelocateableconstantexpression(eGAtomictype size, tSpNode*self){
	ErfEnter_String(
		mtString_Format(
			"IgEvaulaterelocateableconstantexpression size:%s node:%i•%s",
			meGAtomictype_ToStringTable[size],
			self->type,
			TokenidtoName[self->type]
		)
	);
	tGInstruction* retval = nullptr;
	switch(self->type){
		case tSplexem_Integerconstant:
			retval = mtGInstruction_CreateImmediate(
				tInstruction_Definevalue,
				size,
				self->constant
			);
			break;
		default:
			fprintf(
				stderr,
				"IG: [E] IgEvaulaterelocateableconstantexpression: "
				"Unrecognized node type %i•%s\n",
				self->type,
				TokenidtoName[self->type]
			);
			fprintf(
				stderr,
				"IG: [E] IgEvaulaterelocateableconstantexpression: "
				"Unrecognized node type %i•%s\n",
				self->type,
				TokenidtoName[self->type]
			);
			printf(
				"IG: [E]                                           "
				"Secondary AST snippet:\n"
			);
			LfPrint_SpNode(self);
			ErfError();
			retval = nullptr;
	};
	ErfLeave();
	return retval;
};
tGInstruction* IgCompileglobalvariable(
	tGType* type, 
	tSpNode* self
){ // Compile global variable's initializer
	ErfEnter_String(
		mtString_Format("IgCompileglobalvariable type:<%s> node:%p",
			mtGType_ToString(type),self)
	);
	tGInstruction* retval = nullptr;
	if(mtGType_IsScalar(type)){
		// Scalar type. Compile as initializer
		{	// Typeset cppreference page just for lulz
			// -
			// ___________________________________
			// \ C / C language / Initialization /
			//  '''''''''''''''''''''''''''''''''
			//   Scalar initialization
			// ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
			// 
			// When initializing an object of scalar type, the initializer must
			// be a single expression
			// 
			// The initializer for a scalar (an object of integer type including
			// booleans and enumerated types, floating type including complex 
			// and imaginary, and pointer type including pointer to function) 
			// must be a single expression, optionally enclosed in braces, or an
			// empty initializer(since C23):
			// ---------------------------------------
			//   = expression         (1)
			// ---------------------------------------
			//   = { expression }     (2)
			// ---------------------------------------
			//   = { }                (3)  (since C23)
			// ---------------------------------------
			// 1,2)  The expression is evaluated, and its value, after conversion
			//      as if by assignment to the type of the object, becomes the
			//      initial value of the object being initialized.
			//   3)  The object is empty-initialized, i.e. initialized to numeric
			//      zero for an object of an arithmetic or enumeration type, or 
			//      null pointer value for an object of a pointer type.
			//
			//   Notes
			// 
			// Because of the rules that apply to conversions as if by 
			// assignment, const and volatile qualifiers on the declared type 
			// are ignored when determining which type to convert the 
			// expression to.
			// 
			// See initialization for the rules that apply when no initializer
			// is used.
			// 
			// As with all other initializations, expression must be a constant
			// expression when initializing objects of static or thread-local
			// storage duration.
			// 
			// The expression cannot be a comma operator (unless parenthesized)
			// because the comma at the top level would be interpreted as the
			// beginning of the next declarator.
			// 
			// When initializing objects of floating-point type, all
			// computations for the objects with automatic storage duration are
			// done as-if at execution time and are affected by the current
			// rounding; floating-point errors are reported as specified in
			// math_errhandling. For objects of static and thread-local storage
			// duration, computations are done as-if at compile time, and no
			// exceptions are raised:
			// ┌────────────────────────────────────────────────────────────┐
			// │ void f(void)                                               │
			// │ {                                                          │
			// │ #pragma STDC FENV_ACCESS ON                                │
			// │     static float v = 1.1e75; // does not raise exceptions: │
			// │                              // static init                │
			// │                                                            │
			// │     float u[] = { 1.1e75 }; // raises FE_INEXACT           │
			// │     float w = 1.1e75;       // raises FE_INEXACT           │
			// │                                                            │
			// │     double x = 1.1e75; // may raise FE_INEXACT (depends    │
			// │                        // on FLT_EVAL_METHOD)              │
			// │     float y = 1.1e75f; // may raise FE_INEXACT (depends    │
			// │                        // on FLT_EVAL_METHOD)              │
			// │                                                            │
			// │     long double z = 1.1e75; // does not raise exceptions   │
			// │                             // (conversion is exact)       │
			// │ }                                                          │
			// └────────────────────────────────────────────────────────────┘
			// 
			//    Example
			// 
			// ┌────────────────────────────────────────────────────────────┐
			// │ #include <stdbool.h>                                       │
			// │ int main(void)                                             │
			// │ {                                                          │
			// │     bool b = true;                                         │
			// │     const double d = 3.14;                                 │
			// │     int k = 3.15; // conversion from double to int         │
			// │     int n = {12}, // optional braces                       │
			// │        *p = &n,   // non-constant expression OK for        │
			// │                   // automatic variable                    │
			// │        (*fp)(void) = main;                                 │
			// │     enum {RED, BLUE} e = RED; // enumerations are scalar   │
			// │                               // types as well             │
			// │ }                                                          │
			// └────────────────────────────────────────────────────────────┘
		};
		if(!self){
			retval = mtGInstruction_CreateBasic(
				tInstruction_Allocatestorage,
				type->atomicbasetype
			);
		}else{
			retval = IgEvaulaterelocateableconstantexpression(
				type->atomicbasetype,self);
		};
	}else switch(type->atomicbasetype){
		case eGAtomictype_Array:
			if(!self){
				for(int n=0;n<type->arraysize;n++){
					retval=mtGInstruction_Join_Modify(retval,
						IgCompileglobalvariable(
							type->complexbasetype,
							nullptr
						)
					);
				};
			}else{
				tSpNode* j = self;
				assert(j->type==tSplexem_Comma); // Semanticparser provides a 
				                                 // comma-separated list of initializers
				assert(!type->dynamicarraysize);
				for(int n=0;n<type->arraysize;n++){
					retval=mtGInstruction_Join_Modify(retval,
						IgCompileglobalvariable(
							type->complexbasetype,
							j?j->left:nullptr
						)
					);
					if(j)j=j->right;
				};
			};
			break;
		default:
			fprintf(
				stderr,
				"IG: [E] IgCompileglobalvariable: "
				"Unrecognized type %i•%s <%s>\n",
				type->atomicbasetype,
				meGAtomictype_ToStringTable[type->atomicbasetype],
				mtGType_ToString(type)
			);
			fprintf(
				stdout,
				"IG: [E] IgCompilelocalvariable: "
				"Unrecognized type %i•%s <%s>\n",
				type->atomicbasetype,
				meGAtomictype_ToStringTable[type->atomicbasetype],
				mtGType_ToString(type)
			);
			ErfError();
			break;
	};
	ErfLeave();
	return retval;
};
tGInstruction* IgCompilelocalvariable(
	tGTargetPointer* var,
	tGTargetSizet offset, 
	tGType* type, 
	tSpNode* self
){ // Compiles local variable's initializer
	ErfEnter_String(
		mtString_Format(
			"IgCompilelocalvariable var:%p offset:%i type:<%s> node:%p",
			var,
			offset,
			mtGType_ToString(type),
			self
		)
	);
	assert(var);
	assert(type);
	assert(self);
	if(mtGType_IsScalar(type)){
		// Scalar type. Compile as initializer
		tGInstruction* i = mtGInstruction_Join_Modify(
				// Load pointer to variable
				var->nonconstant 
			?	mtGInstruction_Join_Modify(
					// In case of a global
					mtGInstruction_CreateCodepointer(
						tInstruction_Loadaddress,
						eGAtomictype_Nearpointer,
						var->dynamicpointer
					),
					mtGInstruction_CreateImmediate(
						tInstruction_Index,
						eGAtomictype_Nearpointer,
						offset
					)
				)
			:	mtGInstruction_Join_Modify(
					// In case of a local
					mtGInstruction_CreateImmediate(
						tInstruction_Constant,
						eGAtomictype_Nearpointer,
						var->offset+offset
					),
						  var->segment
						==meGSegment_Stackframe
					?	mtGInstruction_CreateBasic(
							tInstruction_Indexfp,
							eGAtomictype_Nearpointer
						)
					:	nullptr
				)
			,
			mtGInstruction_Join_Modify(
				// Glue
				mtGInstruction_CreateBasic(
					tInstruction_Pushleft,
					   self->returnedtype->valuecategory
					 ==eGValuecategory_Leftvalue
					?eGAtomictype_Nearpointer
					:self->returnedtype->atomicbasetype
				),
				mtGInstruction_Join_Modify(
					// Initializer
					IgCompileExpression(self),
					mtGInstruction_Join_Modify(
						// Glue
						mtGInstruction_CreateBasic(
							tInstruction_Popright,
							   self->returnedtype->valuecategory
							 ==eGValuecategory_Leftvalue
							?eGAtomictype_Nearpointer
							:self->returnedtype->atomicbasetype
						),
						// Write it
						mtGInstruction_CreateSegmented(
							tInstruction_Store,
							meGSegment_Data, // TODO: Segmentation: Get segment using
											 //        mtSpNode_GetSegment(self->right)
							self->returnedtype->atomicbasetype
						)
					)
				)
			)
		);
		return i;
	}else switch(type->atomicbasetype){
		default:
			fprintf(
				stderr,
				"IG: [E] IgCompilelocalvariable: "
				"Unrecognized type %i•%s <%s>\n",
				type->atomicbasetype,
				meGAtomictype_ToStringTable[type->atomicbasetype],
				mtGType_ToString(type)
			);
			fprintf(
				stdout,
				"IG: [E] IgCompilelocalvariable: "
				"Unrecognized type %i•%s <%s>\n",
				type->atomicbasetype,
				meGAtomictype_ToStringTable[type->atomicbasetype],
				mtGType_ToString(type)
			);
			ErfError();
			return nullptr;
			break;
	};
};
tGInstruction* IgCompileStatement(tSpNode* self){
	assert(self);
#ifdef qvGTrace
	printf("IG: [T] IgCompileStatement: entered %i•%s\n",self->type,TokenidtoName[self->type]);
#endif
	switch(self->type){
		case tSplexem_Declarationlist: {
			ErfEnter_String("IgCompileStatement: Declarationlist");
			tGInstruction* i;
			if(self->left){
				i=IgCompileStatement(self->left);
				if(!i)i=mtGInstruction_CreateBasic(tInstruction_Cnop,eGAtomictype_Void);
				assert(i);
			}else{
				i=mtGInstruction_CreateBasic(tInstruction_Cnop,eGAtomictype_Void);
				assert(i);
			};
			assert(i);
			assert(mtGInstruction_GetLast(i));
			if(self->right)mtGInstruction_GetLast(i)->next=IgCompileStatement(self->right);
			ErfLeave();
			return i;
		};	break;
		case tSplexem_Blockstatement: {
			ErfEnter_String("IgCompileStatement: Blockstatement");
			tGInstruction* i = mtGInstruction_CreateCnop();
			ErfUpdate_String("IgCompileStatement: Blockstatement - left");
			if(self->left)  mtGInstruction_GetLast(i)->next=IgCompileStatement(self->left);
			ErfUpdate_String("IgCompileStatement: Blockstatement - right");
			if(self->right) {
				assert(i);
				assert(mtGInstruction_GetLast(i));
				mtGInstruction_GetLast(i)->next=IgCompileStatement(self->right);
			};
			ErfUpdate_String("IgCompileStatement: Blockstatement - post");
			ErfLeave();
			return i;
		};	break;
		case tSplexem_Returnstatement: {
			tGInstruction* i;
			i=IgCompileExpression(self->left);
			assert(mtGInstruction_GetLast(i));
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(
				tInstruction_Prereturn,
				self->left->returnedtype->atomicbasetype
			);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(
				tInstruction_Leaveframe,
				eGAtomictype_Void
			);
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateImmediate(
				tInstruction_Return,
				self->left->returnedtype->atomicbasetype,
				   IgCurrentfunction->fextinfo->callingconvention
				 ==eGCallingconvention_Stdcall
				?IgCurrentfunction->fextinfo->argumentssize
				:-1
			);
			return i;
		};	break;
		case tSplexem_Expressionstatement: {
			ErfEnter_String("IgCompileStatement: Expressionstatement");
			tGInstruction* i;
			i=IgCompileExpression(self->left);
			assert(i);
			assert(mtGInstruction_GetLast(i));
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateBasic(
				tInstruction_Drop,
				self->left->returnedtype->atomicbasetype
			);
			ErfLeave();
			return i;
		};	break;
		case tSplexem_Variabledeclaration: {
			ErfEnter_String("IgCompileStatement: Variabledeclaration");
			tGInstruction* i;
			assert(self);
			assert(self->symbol);
			if(self->right)
				i = IgCompilelocalvariable(
					self->symbol->allocatedstorage,
					0,
					self->returnedtype,
					self->right
				);
			else
				i=nullptr;
			ErfLeave();
			return i;
			//if(self->right==nullptr){ // No initializer - no code emmited
			//	ErfLeave();
			//	return nullptr; 
			//}
			//assert(self!=nullptr);
			//assert(self->symbol);
			//assert(self->symbol->type->valuecategory == eGValuecategory_Leftvalue);
			//assert(self->symbol->symbolkind == mtGSymbol_eType_Pointer);
			//tGInstruction* i = mtGInstruction_Join_Modify(
			//		self->symbol->allocatedstorage->nonconstant 
			//	?	mtGInstruction_CreateCodepointer(
			//			tInstruction_Loadaddress,
			//			eGAtomictype_Nearpointer,
			//			self->symbol->allocatedstorage->dynamicpointer
			//		)
			//	:	mtGInstruction_Join_Modify(
			//			mtGInstruction_CreateImmediate(
			//				tInstruction_Constant,
			//				eGAtomictype_Nearpointer,
			//				self->symbol->allocatedstorage->offset
			//			),
			//				  self->symbol->allocatedstorage->segment
			//				==meGSegment_Stackframe
			//			?	mtGInstruction_CreateBasic(
			//					tInstruction_Indexfp,
			//					eGAtomictype_Nearpointer
			//				)
			//			:	nullptr
			//		)
			//	,
			//	mtGInstruction_Join_Modify(
			//		mtGInstruction_CreateBasic(
			//			tInstruction_Pushleft,
			//			   self->returnedtype->valuecategory
			//			 ==eGValuecategory_Leftvalue
			//			?eGAtomictype_Nearpointer
			//			:self->returnedtype->atomicbasetype
			//		),
			//		mtGInstruction_Join_Modify(
			//			IgCompileExpression(self->right),
			//			mtGInstruction_Join_Modify(
			//				mtGInstruction_CreateBasic(
			//					tInstruction_Popright,
			//					   self->returnedtype->valuecategory
			//					 ==eGValuecategory_Leftvalue
			//					?eGAtomictype_Nearpointer
			//					:self->returnedtype->atomicbasetype
			//				),
			//				mtGInstruction_CreateSegmented(
			//					tInstruction_Store,
			//					meGSegment_Data, // TODO: Segmentation: Get segment using
			//									 //        mtSpNode_GetSegment(self->right)
			//					self->right->returnedtype->atomicbasetype
			//				)
			//			)
			//		)
			//	)
			//);
		};	break;
		case tSplexem_Forstatement: {
			//	forloop: initializer
			//	         jmp   _cond
			//	_loop:   body
			//	         iterator
			//	_cond:   cj##cond  _loop
			//	_end:
			ErfEnter_String("IgCompileStatement: Forloop");
				tGInstruction* end  = mtGInstruction_CreateBasic(tInstruction_Cnop,eGAtomictype_Void);
				ErfEnter_String("IgCompileStatement: Body");
				tGInstruction* body = IgCompileStatement(self->right);
				ErfLeave();
				ErfEnter_String("IgCompileStatement: Initializer");
				tGInstruction* init = IgCompileStatement(self->initializer);
				ErfLeave();
				ErfEnter_String("IgCompileStatement: Condition");
				tGInstruction* cond = 
					 self->condition->type==tSplexem_Nullexpression
					?mtGInstruction_CreateCodepointer(
						tInstruction_Jump,
						eGAtomictype_Void,
						body
					)
					:IgCompileConditionaljump(self->condition,body)
					;
				ErfLeave();
				ErfEnter_String("IgCompileStatement: Iterator");
				tGInstruction* iter = IgCompileStatement(self->left);
				ErfLeave();
				ErfEnter_String("IgCompileStatement: Binding it together");
				mtGInstruction_GetLast(init)->next = 
					mtGInstruction_CreateCodepointer(
						tInstruction_Jump,
						eGAtomictype_Void,
						cond
					);
				mtGInstruction_GetLast(init)->next = body;
				mtGInstruction_GetLast(init)->next = iter;
				mtGInstruction_GetLast(init)->next = cond;
				//mtGInstruction_GetLast(init)->next = 
				//	mtGInstruction_CreateCodepointer(
				//		tInstruction_Jump,
				//		eGAtomictype_Void,
				//		body
				//	);
				mtGInstruction_GetLast(init)->next = end;
				ErfLeave();
			ErfLeave();
			return init;
		};
		case tSplexem_Whilestatement: {
			//	while_l: jmp   _cond
			//	_loop:   body
			//	_cond:   cj##cond  _loop
			tGInstruction* retval = nullptr;
			ErfEnter_String("IgCompileStatement: Whileloop");
				ErfUpdate_String("IgCompileStatement: Whileloop: Body");
				tGInstruction* body = IgCompileStatement(self->left);
				ErfUpdate_String("IgCompileStatement: Whileloop: Condition");
				tGInstruction* cond = 
					 self->condition->type==tSplexem_Nullexpression
					?mtGInstruction_CreateCodepointer(
						tInstruction_Jump,
						eGAtomictype_Void,
						body
					)
					:IgCompileConditionaljump(self->condition,body)
					;
				ErfUpdate_String("IgCompileStatement: Whileloop: Binding together");
				retval = mtGInstruction_CreateCodepointer(
					tInstruction_Jump,
					eGAtomictype_Void,
					cond
				);
				mtGInstruction_GetLast(retval)->next = body;
				mtGInstruction_GetLast(retval)->next = cond;
			ErfLeave();
			return retval;
		};
		case tSplexem_Ifstatement: {
			//		ifstatement
			//
			//		cj	_t
			//		false
			//		jmp	_e
			//_t	true
			//_e	
			//
			//		cjn	_e
			//		true
			//_e
			tGInstruction* cond;
			ErfEnter_String("IgCompileStatement: `if`");
			tGInstruction* i = mtGInstruction_CreateCnop();
			ErfUpdate_String("IgCompileStatement: `if` - then?");
			assert(self);
			assert(self->left);
			tGInstruction* thenbranch = IgCompileStatement(self->left);
			if(self->right){
				ErfUpdate_String("IgCompileStatement: `if` - else");
				tGInstruction* elsebranch = IgCompileStatement(self->right);
				ErfUpdate_String("IgCompileStatement: `if` - condition");
				cond = IgCompileConditionaljump(self->condition,thenbranch);
				ErfUpdate_String("IgCompileStatement: `if` - tying together");
				mtGInstruction_GetLast(cond)->next=elsebranch;
				mtGInstruction_GetLast(cond)->next=
					mtGInstruction_CreateCodepointer(
						tInstruction_Jump,
						eGAtomictype_Void,
						i
					);
				mtGInstruction_GetLast(cond)->next=thenbranch;
				mtGInstruction_GetLast(cond)->next=i;
				i=cond;
			}else{
				ErfUpdate_String("IgCompileStatement: `if` - condition");
				cond = IgCompileInvertedconditionaljump(self->condition,i);
				ErfUpdate_String("IgCompileStatement: `if` - tying together");
				mtGInstruction_GetLast(cond)->next=thenbranch;
				mtGInstruction_GetLast(cond)->next=i;
				i=cond;
			};
			ErfLeave();
			return i;
		};	break;
		default:
			printf("IG: [E] IgCompileStatement: Unrecognized statement "
			       "node %i•%s \n",
				self->type,TokenidtoName[self->type]
			);
			printf("IG: [E]                     Secondary AST snippet: \n");
			LfPrint_SpNode(self);
			printf("IG: [E] IgCompileStatement: (secondary ast snippet end) \n");
			ErfError();
			return nullptr;
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
	assert(self->fextinfo);
	assert(  self->returnedtype->atomicbasetype==eGAtomictype_Nearfunction
	       ||self->returnedtype->atomicbasetype==eGAtomictype_Function);
	ErfEnter_String("IgCompileFunction: root");
	ErfUpdate_String(mtString_Join("IgCompileFunction: ",self->symbol->name));
	IgCurrentfunction = self;
	// Find the place to write code to
	tGInstruction* code = self->symbol->allocatedstorage->dynamicpointer;
	// Set external name
	code->opcode.opr=tInstruction_Global;
	code->label = self->symbol->name;
	if(self->right){ // If implementation provided
		// Prologue
		mtGInstruction_GetLast(code)->next=mtGInstruction_CreateImmediate(
			tInstruction_Enterframe,eGAtomictype_Void,
			self->fextinfo->localssize+2);
		// Compile function itself
		mtGInstruction_GetLast(code)->next=IgCompileStatement(self->right);
		// Implied return statement
		mtGInstruction_GetLast(code)->next=mtGInstruction_CreateImmediate(tInstruction_Constant,self->returnedtype->complexbasetype->atomicbasetype,0);
		mtGInstruction_GetLast(code)->next=mtGInstruction_CreateBasic(tInstruction_Prereturn,self->returnedtype->complexbasetype->atomicbasetype);
		// Epilogue
		mtGInstruction_GetLast(code)->next=mtGInstruction_CreateBasic(tInstruction_Leaveframe,eGAtomictype_Void);
		mtGInstruction_GetLast(code)->next=mtGInstruction_CreateImmediate(
			tInstruction_Return,
			self->returnedtype->complexbasetype->atomicbasetype,
			   IgCurrentfunction->fextinfo->callingconvention
			 ==eGCallingconvention_Stdcall
			?IgCurrentfunction->fextinfo->argumentssize
			:-1
		);
	};
	ErfLeave();
	return code;
};
tGInstruction* IgCompileVariable(tSpNode* self){
	assert(self);
	tGInstruction* retval;
	retval = mtGInstruction_Join_Modify(
		mtGInstruction_SetLabel(
			mtGInstruction_CreateBasic(
				tInstruction_Global,
				eGAtomictype_Void
			),
			self->symbol->name
		),
		IgCompileglobalvariable(self->returnedtype,self->right)
	);
	self->symbol->allocatedstorage=mtGTargetPointer_CreateDynamic(retval);
	return retval;
	//// Old code
	//if(self->returnedtype->atomicbasetype==eGAtomictype_Array){
	//	// Compile an array
	//	assert(!self->returnedtype->dynamicarraysize);
	//	assert(self->returnedtype->arraysize);
	//	retval = mtGInstruction_Join_Modify(
	//		mtGInstruction_SetLabel(
	//			mtGInstruction_CreateBasic(tInstruction_Global,eGAtomictype_Void),
	//			mtString_Clone(self->symbol->name)
	//		),
	//		mtGInstruction_CreateImmediate(
	//			tInstruction_Allocatestorage2,
	//			eGAtomictype_Uint8,
	//			mtGType_Sizeof(
	//				mtGType_SetValuecategory(
	//					mtGType_Deepclone(
	//						self->returnedtype
	//					),
	//					eGValuecategory_Rightvalue
	//				)
	//			)
	//		)
	//	);
	//}else{
	//	// Relatively normal variable
	//	retval = mtGInstruction_Join_Modify(
	//		mtGInstruction_SetLabel(
	//			mtGInstruction_CreateBasic(tInstruction_Global,eGAtomictype_Void),
	//			mtString_Clone(self->symbol->name)
	//		),
	//		mtGInstruction_CreateBasic(
	//			tInstruction_Allocatestorage,
	//			self->returnedtype->atomicbasetype
	//		)
	//	);
	//};
	//assert(self);
	//assert(self->symbol);
	//assert(self->symbol->allocatedstorage);
	//assert(self->symbol->allocatedstorage->dynamicpointer);
	//*self->symbol->allocatedstorage->dynamicpointer=*retval;
	//retval=self->symbol->allocatedstorage->dynamicpointer;
	//return retval;
};
void IgParse(tSpNode* self){
	assert(self);
#ifdef qvGTrace
	printf("IG: [T] Entered with node %i•%s \n",self->type,TokenidtoName[self->type]);
#endif
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
		case tSplexem_Variabledeclaration:
			ErfEnter_String("IgParse: tSplexem_Variabledeclaration");
			mtGInstruction_GetLast(GCompiled[meGSegment_Data])->next=IgCompileVariable(self);
			ErfLeave();
			break;
		case tSplexem_Externaldeclaration:
			ErfEnter_String("IgParse: tSplexem_Externaldeclaration");
			mtGInstruction_GetLast(GCompiled[meGSegment_Code])->next
				=mtGInstruction_CreateExtern(self->identifier);
			ErfLeave();
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
		"; --- xcc Retargetable C Crosscompiler - Intermediate representation dump ---\n"
		"; --------- Forth-like IR for xcc-ircompiler and linking assembler ----------\n"
		"\tirc.programprologue\n"
	);
	for(int i=0;i<meGSegment_Count;i++){
		if(
			  i==meGSegment_Code
			||i==meGSegment_Data
			||i==meGSegment_Readonlydata
			||i==meGSegment_Udata
		){
			fprintf(file,"\tirc.segment %i\n",
				 i== meGSegment_Code         ? 7
				:i== meGSegment_Data         ? 8
				:i== meGSegment_Readonlydata ? 9
				:i== meGSegment_Udata        ?10
				:(assert(false),0)
			);
			for(tGInstruction* j=code[i];j!=nullptr;j=j->next){
				fprintf(file,"l_%p:\t",j);
				if(j->opcode.opr == tInstruction_Extern){
					fprintf(file,"v.%s.%s.%s %s\t",
						TokenidtoName_Compact[j->opcode.opr],
						meGSegment_ToStringTable[j->opcode.segment],
						meGAtomictype_ToStringTable[j->opcode.isize],
						j->label
					);
				}else{
					if(j->label)fprintf(file,"%s:\t",j->label);
					if(j->opcode.opr == tInstruction_Cast){
						fprintf(file,"v.%s.%s.%s\t",
							TokenidtoName_Compact[j->opcode.opr],
							meGAtomictype_ToStringTable[j->opcode.isize],
							meGAtomictype_ToStringTable[j->opcode.altsize]
						);
					}else
						fprintf(file,"v.%s.%s.%s\t",
							TokenidtoName_Compact[j->opcode.opr],
							meGSegment_ToStringTable[j->opcode.segment],
							meGAtomictype_ToStringTable[j->opcode.isize]
						);
					if(	// Instruction that need a label
						   j->opcode.opr==tInstruction_Jump
						|| j->opcode.opr==tInstruction_Jumptrue
						|| j->opcode.opr==tInstruction_Jumpfalse
						|| j->opcode.opr==tInstruction_Loadaddress
						|| j->opcode.opr==tInstruction_Comparejumpequal               
						|| j->opcode.opr==tInstruction_Comparejumpnotequal            
						|| j->opcode.opr==tInstruction_Comparejumpsignedlessthan      
						|| j->opcode.opr==tInstruction_Comparejumpsignedlessequal     
						|| j->opcode.opr==tInstruction_Comparejumpsignedgreaterthan   
						|| j->opcode.opr==tInstruction_Comparejumpsignedgreaterequal  
						|| j->opcode.opr==tInstruction_Comparejumpunsignedlessthan    
						|| j->opcode.opr==tInstruction_Comparejumpunsignedlessequal   
						|| j->opcode.opr==tInstruction_Comparejumpunsignedgreaterthan 
						|| j->opcode.opr==tInstruction_Comparejumpunsignedgreaterequal
					){
						if(j->jumptarget->label){
							fprintf(
								file,
								"%s ",
								j->jumptarget->label
							);
						}else{
							fprintf(
								file,
								"l_%p ",
								j->jumptarget
							);
						}
					}else if( // Immediate-operand commands
						  (j->opcode.opr==tInstruction_Constant)
						||(j->opcode.opr==tInstruction_Enterframe) // It needs the size of stackframe
						||(j->opcode.opr==tInstruction_Definevalue)// For obvious reasons
						||(j->opcode.opr==tInstruction_Return)     // To deallocate passed parameters
						||(j->opcode.opr==tInstruction_Constantshiftleft)
						||(j->opcode.opr==tInstruction_Constantshiftright)
						||(j->opcode.opr==tInstruction_Allocatestorage2)
					){
						fprintf(
							file,
							"%i",
							j->immediate
						);
					}
				};
				fprintf(file,"\n");
			};
		};
	};
	fprintf(file,"\tirc.programepilogue\n");
};

