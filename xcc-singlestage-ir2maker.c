// xcc Secondary Intermediate Representation creator
// . Developed with Semanticparser and Primary Intermediate Representation 
// ' creator already in place
#define Ig2Todoproto(name) tGInstruction* name (tSpNode* self){assert(false);};
tGInstruction* Ig2CompileFunctionarguments(tSpNode* self){assert(false);};
tGInstruction* Ig2CompileExpression(tSpNode* self){assert(false);};
tGInstruction* Ig2CompileInvertedconditionaljump(){assert(false);};
tGInstruction* Ig2CompileConditionaljump(){assert(false);};
tGInstruction* Ig2Compileglobalvariable(tGType* type, tSpNode* self){
	assert(false);
};
tGInstruction* Ig2Compilelocalvariable(){assert(false);};
tGInstruction* Ig2CompileStatement(tSpNode* self){
	tGInstruction* retval = nullptr;
	ErfEnter_String("Ig2CompileStatement");
	if(!self){
		ErfError_String2("IG2:[E] Ig2CompileStatement: Nullpointer as `self`\n");
	}else{
		switch(self->type){
			case tLexem_Blockstatement:
				retval = mtGInstruction_Join_Modify(
					self->left?Ig2CompileStatement(self->left):nullptr,
					self->right?Ig2CompileStatement(self->right):nullptr
				);
				break;
			default:
				ErfError_String2(
					mtString_Format(
						"IG2:[E] Ig2CompileStatement: Unrecognized node type %i•%s\n",
						self->type,TokenidtoName[self->type]
					)
				);
				break;
		};
	};
	ErfLeave();
	return retval;

};
tGInstruction* Ig2CompileVariable(tSpNode* self){
	ErfEnter_String("Ig2CompileVariable");
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
	ErfLeave();
	return retval;
};
tGInstruction* Ig2CompileFunction(tSpNode* self){
	assert(self);
	assert(self->fextinfo);
	assert(  self->returnedtype->atomicbasetype==eGAtomictype_Nearfunction
	       ||self->returnedtype->atomicbasetype==eGAtomictype_Farfunction);
	ErfEnter_String("Ig2CompileFunction: root");
	ErfUpdate_String(mtString_Join("Ig2CompileFunction: ",self->symbol->name));
	// Find the place to write code to
	assert(self->symbol);
	assert(self->symbol->allocatedstorage);
	assert(self->symbol->allocatedstorage->dynamicpointer);
	tGInstruction* code = self->symbol->allocatedstorage->dynamicpointer;
	// Set external name
	code->opcode.opr=tInstruction_Global;
	code->label = self->symbol->name;
	if(self->right){ // If implementation provided
		// Prologue
		mtGInstruction_GetLast(code)->next=mtGInstruction_Clone(
			&(tGInstruction){
				.opcode={
					.opr= tInstruction_Enterframe,
					.isize=0,
				},
				.label=0,
				.next=nullptr,
				.jumptarget=nullptr,
				.immediate = self->fextinfo->localssize,
			}
		);
		// Compile function itself
		mtGInstruction_GetLast(code)->next=Ig2CompileStatement(self->right);
		// Epilogue
		mtGInstruction_GetLast(code)->next=mtGInstruction_Clone(
			&(tGInstruction){
				.opcode={
					.opr=
							self->returnedtype->atomicbasetype==eGAtomictype_Nearfunction
						?	tInstruction_Nearreturn
						:	self->returnedtype->atomicbasetype==eGAtomictype_Nearfunction
						?	tInstruction_Farreturn
						:	tInstruction_Return,
					.isize=self->returnedtype->complexbasetype->atomicbasetype,
				},
				.label=0,
				.next=nullptr,
				.jumptarget=nullptr,
				.immediate = self->fextinfo->argumentssize,
				.source=(tIr2Operand){
					.segment = meGSegment_Immediate,
					.constant = 0
				}
			}
		);
	};
	ErfLeave();
	return code;
};
void Ig2Parse(tSpNode* self){
	assert(self);
#ifdef qvGTrace
	printf("IG2:[T] Entered with node %i•%s \n",self->type,TokenidtoName[self->type]);
#endif
	switch(self->type){
		case tSplexem_Nulldeclaration:
			break;
		case tSplexem_Declarationlist:
			if(self->left)Ig2Parse(self->left);
			if(self->right)return Ig2Parse(self->right);
			return;
			break;
		case tSplexem_Functiondeclaration:
			assert(mtGInstruction_GetLast(GCompiled[meGSegment_Code]));
			mtGInstruction_GetLast(GCompiled[meGSegment_Code])->next=Ig2CompileFunction(self);
			break;
		case tSplexem_Variabledeclaration:
			ErfEnter_String("Ig2Parse: tSplexem_Variabledeclaration");
			mtGInstruction_GetLast(GCompiled[meGSegment_Data])->next=Ig2CompileVariable(self);
			ErfLeave();
			break;
		case tSplexem_Externaldeclaration:
			ErfEnter_String("Ig2Parse: tSplexem_Externaldeclaration");
			mtGInstruction_GetLast(GCompiled[meGSegment_Code])->next
				=mtGInstruction_CreateExtern(self->identifier);
			ErfLeave();
			break;
		default:
			assert(false);
	};
};
void Ig2Dumpir_Writeop(tIr2Operand *self, FILE* file){ 
	if(self->segment==meGSegment_Immediate){
		fprintf(file, "const:%i,", (int) self->constant);
	}else if(self->segment==meGSegment_Cstack){
		fprintf(file, "cstack,");
	}else{
		fprintf(file, "%s:", meGSegment_ToStringTable[self->segment]);
		if(self->do_deref){
			if(self->deref_disp!=0)
				fprintf(file, "%i!", (int) self->deref_disp);
			assert(self->deref_symbol->symbolkind==mtGSymbol_eType_Pointer);
			// TODO: Extract segment from dynamicpointer
			if(self->deref_symbol->allocatedstorage->nonconstant)
				if(self->deref_symbol->allocatedstorage->dynamicpointer->label)
					fprintf(file, "%s@", 
						self->deref_symbol->allocatedstorage->dynamicpointer->label
					);
				else
					fprintf(file, "l_%p@", 
						self->deref_symbol->allocatedstorage->dynamicpointer
					);
			else
				assert(false);
		};
		if(self->index_size){
			// TODO: Extract segment from dynamicpointer
			if(self->index_symbol)
				if(self->index_symbol->allocatedstorage->nonconstant)
					if(self->index_symbol->allocatedstorage->dynamicpointer->label)
						fprintf(file, "(%.0i)%s@", 
							(int)self->index_disp,
							self->index_symbol->allocatedstorage->dynamicpointer->label
						);
					else
						fprintf(file, "(%.0i)l_%p", 
							(int)self->index_disp,
							self->index_symbol->allocatedstorage->dynamicpointer
						);
				else
					assert(false);
			else fprintf(file, "(%.0i)", 
					(int)self->index_disp
				);
		};
		fprintf(file, "[%.0i]", self->displacement);
		if(self->base)
			if(self->base->allocatedstorage->nonconstant)
				if(self->base->allocatedstorage->dynamicpointer->label)
					fprintf(file, "%s@", 
						self->base->allocatedstorage->dynamicpointer->label
					);
				else
					fprintf(file, "l_%p", 
						self->base->allocatedstorage->dynamicpointer
					);
			else
				assert(false);
		;
	};
};
void Ig2Dumpir(tGInstruction** code, FILE* file){ 
	// Print IR as text into file given as argument
	fprintf(
		file,
		";-- xcc Retargetable C Crosscompiler - Sec. Intermediate representation dump --\n"
		";--------- Secondary IR for xcc-ircompiler and linking assembler ----------\n"
		"\tv2.programprologue\n"
	);
	for(int i=0;i<meGSegment_Count;i++){
		if(	// the segment should be emmited
			  i==meGSegment_Code
			||i==meGSegment_Data
			||i==meGSegment_Readonlydata
			||i==meGSegment_Udata
		){;}else continue;
		fprintf(file,"\tirc.segment %i\n", // set segment
			 i== meGSegment_Code         ? 7
			:i== meGSegment_Data         ? 8
			:i== meGSegment_Readonlydata ? 9
			:i== meGSegment_Udata        ?10
			:(assert(false),0)
		);
		for(tGInstruction* j=code[i];j!=nullptr;j=j->next){ //foreach(instructions)
			fprintf(file,"l_%p:\t",j);
			if(j->opcode.opr == tInstruction_Extern){ // externing bypass
				fprintf(file,"v2.extern %s\n",j->label);
				continue;
			}
			if(j->label)fprintf(file,"%s:\t",j->label); // emit label
			fprintf(file,"v2.%s.%s\t",
				TokenidtoName_Compact[j->opcode.opr],
				meGAtomictype_ToStringTable[j->opcode.isize]
			);
			if(	false // Legacy constant
				|| j->opcode.opr == tInstruction_Definevalue
			)
				fprintf(file,"%i,\t",(int)j->immediate);
			if(	false// Print destination
				
			){
				Ig2Dumpir_Writeop(&(j->dest),file);
			};
			if(	false// Print source
				|| j->opcode.opr == tInstruction_Return
			){
				Ig2Dumpir_Writeop(&(j->source),file);
			};
			if(	false// Print second source
			){
				Ig2Dumpir_Writeop(&(j->source2),file);
			};
			fprintf(file,"\n");
		};

	};
};
