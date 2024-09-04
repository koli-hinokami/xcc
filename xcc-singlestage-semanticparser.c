// Semantic(al) parser
tSpNode /* Function declaration */ * SpCurrentfunction;


tSpNode* mtSpNode_Create(void){
	return calloc(sizeof(tSpNode),1);
};
tSpNode* mtSpNode_Clone(tSpNode* self){
	return memcpy(malloc(sizeof(tSpNode)),self,sizeof(tSpNode));
};
tSpFunctionextinfo* mtSpFunctionextinfo_Create(){
	return calloc(sizeof(tSpFunctionextinfo),1);
};
tGTargetPointer* SpAllocatelocalvarstorage(tSpFunctionextinfo* fextinfo, tGTargetSizet size){
	fextinfo->localssize-=size;
	return mtGTargetPointer_CreateStatic(meGSegment_Stackframe,fextinfo->localssize);
};
tGTargetPointer* SpAllocateglobalvarstorage(tGTargetSizet size){
	tGInstruction* i = mtGInstruction_CreateAllocatestorage(size);
	mtGInstruction_GetLast(GCompiled[meGSegment_Data])->next=i;
	return mtGTargetPointer_CreateDynamic(i);
};
void SpCompilefunctionarguments(
	tLxNode /* comma-separated list of variable declarations */ * typeexpr,
	tSpFunctionextinfo* fextinfo, 
	tGNamespace* name_space
){
#ifdef qvGTrace
	printf("SP: [T] SpCompilefunctionarguments: entered \n");
#endif
	// if(comma){
	// 	recurse(self->left,,);recurse(->right,,);
	// else 
	// 	namespace.find(typeexpr->name)->pointer=
	// 		new Pointer(
	// 			segment_stackframe,
	// 			fextinfo->argumentssize-=sizeof(typeexpr->type)
	// 		);
	if(typeexpr){
		if(typeexpr->type==tLexem_Comma){
			SpCompilefunctionarguments(typeexpr->left,fextinfo,name_space);
			SpCompilefunctionarguments(typeexpr->right,fextinfo,name_space);
		}else{
			char* name;
			if(typeexpr->type==tLexem_Typeexpression){
				tGType* type = SppGeneratetype(typeexpr->returnedtype,typeexpr->left,&name);
				mtGNamespace_Findsymbol_NameKind(
					name_space,
					name,
					mtGSymbol_eType_Pointer
				)->allocatedstorage=(
					mtGTargetPointer_CreateStatic(
						meGSegment_Stackframe,
						fextinfo->argumentssize-=mtGType_Sizeof(type)
					)
				);
			}else{
				printf("SP: [F] SpCompilefunctionarguments: Unrecognized node %i•%s \n",typeexpr->type,TokenidtoName[typeexpr->type]);
				LfPrint_LxNode(typeexpr);
				GFinalize();
			};
		}
	}else{
		printf("SP: [E] SpCompilefunctionarguments: typeexpr==nullptr \n");
	};
};
tSpNode* SpParse(tLxNode* self){ // Semantic parser primary driver
	if(self)printf("SP: [T] Entered with node %i:%s\n",self->type,TokenidtoName[self->type]);
	if(!self){
		printf("SP: [E] SpParse: Nullpointer\n");
		return nullptr;
	}else{
		switch(self->type){
			case tLexem_Declarationlist:
				// Program root, pretty much
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Declarationlist,
						.left=SpParse(self->left),
						.right=self->right?SpParse(self->right):nullptr
					}
				);
			case tLexem_Nulldeclaration: 
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Nulldeclaration
					}
				);
				break;
			case tLexem_Functiondeclaration: {
				// Function
				tSpNode* node = mtSpNode_Create();
				node->type = tSplexem_Functiondeclaration;
				SpCurrentfunction = node;
				// Generate base type and get name
				char* name = nullptr;
				node->returnedtype = SgGeneratetype(self->returnedtype,self->left,&name);
				// Bind symbol
				node->symbol=mtGNamespace_Findsymbol_NameKind(GRootnamespace,name,mtGSymbol_eType_Pointer);
				 // And insert compilernop inside
				 node->symbol->allocatedstorage=mtGTargetPointer_CreateDynamic(mtGInstruction_CreateCnop());
				// Compile function
				node->fextinfo = mtSpFunctionextinfo_Create();
				node->fextinfo->argumentssize = GTargetStackframeArgumentsstart;
				assert(self->left);
				assert(self->left->type==tLexem_Functioncall);
				SpCompilefunctionarguments(self->left->right,node->fextinfo,self->right->name_space);
				node->right=SpParse(self->right);
				SpCurrentfunction = nullptr;
				return node;
			};	break;
			case tLexem_Blockstatement: {
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Blockstatement,
						.left=SpParse(self->left),
						.right=SpParse(self->right)
					}
				);
			};	break;
			case tLexem_Returnstatement: {
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Returnstatement,
						.left=SpParse(self->left),
						.fextinfo=SpCurrentfunction->fextinfo
					}
				);
			};	break;
			case tLexem_Integerconstant:
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Integerconstant,
						.returnedtype=mtGType_CreateAtomic(eGAtomictype_Int),
						.constant=self->constant
					}
				);
			case tLexem_Typedefinition:
				// Typedefs are already handled in symbolgen
				return nullptr;
				break;
			case tLexem_Variabledeclaration:
				// Do I even need to handle those?
				//  Well, I think so, so those will live for now
				//  Crap, initialization. Locals *must* live but
				//   I'll need to do something with globals

				//return nullptr;
				{
					char* name;
					tGType* type = SppGeneratetype(
						self->returnedtype,
						self->left,
						&name
					);
					tGSymbol* symbol=mtGNamespace_Findsymbol_NameKind(
						self->name_space,
						name,
						mtGSymbol_eType_Pointer
					);
					if(SpCurrentfunction){
						assert(symbol->symbolkind=mtGSymbol_eType_Pointer);
						symbol->allocatedstorage=SpAllocatelocalvarstorage(SpCurrentfunction->fextinfo,mtGType_Sizeof(type));
					}else{
						assert(symbol->symbolkind=mtGSymbol_eType_Pointer);
						symbol->allocatedstorage=SpAllocateglobalvarstorage(mtGType_Sizeof(type));
					};
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Variabledeclaration,
							.returnedtype=type,
							//.identifier=name,
							.symbol=symbol
						}
					);
				};
				//return nullptr;
				break;
			case tLexem_Nullexpression:
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Nullexpression,
						.returnedtype=mtGType_CreateAtomic(eGAtomictype_Void)
					}
				);
			case tLexem_Identifier:
				tGSymbol* symbol = mtGNamespace_Findsymbol_NameKind(
					self->name_space,
					self->identifier,
					mtGSymbol_eType_Pointer
				);
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Symbol,
						.returnedtype=symbol->type,
						.symbol=symbol,
					}
				);
			//case tLexem_Variabledeclaration: {
			//	char* name = nullptr;
			//	if(self->left->type==tLexem_Assign){
			//		return mtSpNode_Clone(
			//			&(tSpNode){
			//				.type=tSplexem_Symbol,
			//				.returnedtype=SgGeneratetype(self->returnedtype,self->left->left,&name),
			//				.symbol=symbol,
			//				.right=SgParse(self->left->right), // if no multideclarations - self->right
			//			}
			//		);
			//	}else{
			//	};
			//	
			//};	break;
			//case tLexem_Substraction: {
			//	tSpNode* left = SgParse(self->left);
			//	tSpNode* right = SgParse(self->right);
			//	return mtSpNode_Clone(
			//		&(tSpNode){
			//			.type=tSplexem_Substraction,
			//			.returnedtype=mtGType_Biggest(left->returnedtype,right->returnedtype),
			//			.left=left,
			//			.right=right,
			//		}
			//	);
			//};	break;
			//case tLexem_Dereference: {
			//	tSpNode* left = SgParse(self->left);
			//	return mtSpNode_Clone(
			//		&(tSpNode){
			//			.type=tSplexem_Dereference,
			//			.returnedtype=mtGType_Dereference(left->returnedtype),
			//			.left=left,
			//		}
			//	);
			//};
			default:
				printf("SP: [E] SpParse: unrecognized node %i:%s\n",self->type,TokenidtoName[self->type]);
				return nullptr;
		};
	};
};

