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
			mtGInstruction_GetLast(i)->next=mtGInstruction_CreateSegmented(
				tInstruction_Load,
				meGSegment_Data,
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
					eGAtomictype_Nearpointer,
					self->symbol->allocatedstorage->dynamicpointer
				);
				
			}else{
				assert(self->symbol->allocatedstorage->segment!=meGSegment_Far);
				if(self->symbol->allocatedstorage->segment==meGSegment_Stackframe){
					return mtGInstruction_Join_Modify(
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
			assert(mtGType_IsPointer(self->left->returnedtype));
			assert(mtGType_IsFunction(self->left->returnedtype->complexbasetype));
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
				//printf("IG: [E] IgCompileExpression: Cast: Sizes mismatch: %i•%i \n",
				//	mtGType_Sizeof(self->returnedtype),
				//	mtGType_Sizeof(self->left->returnedtype)
				//);
				//assert(mtGType_Sizeof(self->returnedtype)==mtGType_Sizeof(self->left->returnedtype));
				//ErfFatal();
				return mtGInstruction_Join_Modify(
					mtGInstruction_CreateCast(
						tInstruction_Cast,
						self->left->returnedtype->atomicbasetype,
						self->returnedtype->atomicbasetype
					),
					IgCompileExpression(self->left)
				);
			};
			return IgCompileExpression(self->left);
		case tSplexem_Assign: {
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						self->left->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								self->right->returnedtype->atomicbasetype
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
			return mtGInstruction_Join_Modify(
				IgCompileExpression(self->left),
				mtGInstruction_CreateSegmented(
					tInstruction_Lvalueincrement,
					meGSegment_Data, // TODO: Segmentation: Get segment using
									 //        mtSpNode_GetSegment(self->right)
					self->left->returnedtype->atomicbasetype
				)
			);
		};	break;
		default:
			printf("IG: [E] IgCompileExpression: Unrecognized "
			       "node %i•%s \n",
				self->type,TokenidtoName[self->type]
			);
			printf("IG: [E] IgCompileExpression: Secondary AST snippet: \n");
			LfPrint_SpNode(self);
			printf("IG: [E] IgCompileExpression: (secondary ast snippet end) \n");
			ErfError();
			return nullptr;
	};
	printf("IG: [F] IgCompileExpression: Absolutely incomprehensible control flow occured \n");
	assert(false);
	return nullptr;
};
tGInstruction* IgCompileConditionaljump(
	tSpNode* self,
	tGInstruction* jumptarget
){ // compiles some exprs to compare-jump-condition
	assert(self->returnedtype->atomicbasetype==eGAtomictype_Boolean);
	tGInstruction* expr = nullptr;
	// Check for special cases
	// TODO: Disable some based on configuration file settings
	if(self->type==tSplexem_Lessthan){
		assert(
			  self->left->returnedtype->atomicbasetype
			==self->right->returnedtype->atomicbasetype
		);
		return mtGInstruction_Join_Modify(
			IgCompileExpression(self->left),
			mtGInstruction_Join_Modify(
				mtGInstruction_CreateBasic(
					tInstruction_Pushleft,self->left->returnedtype->atomicbasetype
				),
				mtGInstruction_Join_Modify(
					IgCompileExpression(self->right),
					mtGInstruction_Join_Modify(
						mtGInstruction_CreateBasic(
							tInstruction_Popright,self->right->returnedtype->atomicbasetype
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
	};
	// If no special cases seen, tailcall to IgCompileExpression 
	// and append jump instruction
	expr = IgCompileExpression(SpInsertbooleantointegercast(self));
	mtGInstruction_GetLast(expr)->next = mtGInstruction_CreateCodepointer(
		tInstruction_Jumptrue,eGAtomictype_Void,jumptarget);
	return expr;
}
tGInstruction* IgCompileStatement(tSpNode* self){
#ifdef qvGTrace
	printf("IG: [T] IgCompileStatement: entered %i•%s\n",self->type,TokenidtoName[self->type]);
#endif
	assert(self);
	switch(self->type){
		case tSplexem_Blockstatement: {
			tGInstruction* i;
			if(self->left){
				i=IgCompileStatement(self->left);
			}else{
				i=mtGInstruction_CreateBasic(tInstruction_Cnop,eGAtomictype_Void);
			};
			assert(mtGInstruction_GetLast(i));
			if(self->right)mtGInstruction_GetLast(i)->next=IgCompileStatement(self->right);
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
		case tSplexem_Variabledeclaration: {
			//	forloop: initializer
			//	_loop:   cj##cond _end
			//	         body
			//	         iterator
			//	         jmp _loop
			//	_end:
			//
			//	init -> cond
			//	cond -> body end
			//
			if(self->right==nullptr)return nullptr; // No initializer - no code emmited
			assert(self!=nullptr);
			assert(self->symbol);
			assert(self->symbol->type->valuecategory == eGValuecategory_Leftvalue);
			assert(self->symbol->symbolkind == mtGSymbol_eType_Pointer);
			return mtGInstruction_Join_Modify(
					self->symbol->allocatedstorage->nonconstant 
				?	mtGInstruction_CreateCodepointer(
						tInstruction_Loadaddress,
						eGAtomictype_Nearpointer,
						self->symbol->allocatedstorage->dynamicpointer
					)
				:	mtGInstruction_Join_Modify(
						mtGInstruction_CreateImmediate(
							tInstruction_Constant,
							eGAtomictype_Nearpointer,
							self->symbol->allocatedstorage->offset
						),
						 	self->symbol->allocatedstorage->segment==meGSegment_Stackframe
						?	mtGInstruction_CreateBasic(
								tInstruction_Indexfp,
								eGAtomictype_Nearpointer
							)
						:	nullptr
					)
				,
				mtGInstruction_Join_Modify(
					mtGInstruction_CreateBasic(
						tInstruction_Pushleft,
						self->returnedtype->atomicbasetype
					),
					mtGInstruction_Join_Modify(
						IgCompileExpression(self->right),
						mtGInstruction_Join_Modify(
							mtGInstruction_CreateBasic(
								tInstruction_Popright,
								self->right->returnedtype->atomicbasetype
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
		};
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
				tGInstruction* cond = IgCompileConditionaljump(self->condition,body);
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
	// Find the place to write code to
	tGInstruction* code = self->symbol->allocatedstorage->dynamicpointer;
	// Set external name
	code->label = self->symbol->name;
	if(self->right){ // If implementation provided
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
	}
	return code;


};
tGInstruction* IgCompileVariable(tSpNode* self){
	assert(self->type==tSplexem_Variabledeclaration);
	if(self->returnedtype->atomicbasetype==eGAtomictype_Array){
		// Compile an array
		assert(false);
	}else{
		// Relatively normal variable
		return mtGInstruction_CreateBasic(
			tInstruction_Allocatestorage,
			self->returnedtype->atomicbasetype
		);
		assert(false);
	};
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
			mtGInstruction_GetLast(GCompiled[meGSegment_Data])->next=IgCompileVariable(self);
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
		"; --------------------- Forth-like IR for xcc-ircompiler --------------------\n"
		"\tirc.programprologue\n"
	);
	for(int i=0;i<meGSegment_Count;i++){
		if(i!=meGSegment_Relative){
			fprintf(file,"\tirc.segment %s\n",meGSegment_ToStringTable[i]);
			for(tGInstruction* j=code[i];j!=nullptr;j=j->next){
				fprintf(file,"l_%p:\t",j);
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
				){
					fprintf(
						file,
						"%i",
						j->immediate
					);
				}
				fprintf(file,"\n");
			};
		};
	};
	fprintf(file,"\tirc.programepilogue\n");
};

