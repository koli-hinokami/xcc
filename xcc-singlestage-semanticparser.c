// Semantic(al) parser
tSpNode* SpCurrentfunction;


tSpNode* mtSpNode_Create(void){
	return calloc(sizeof(tSpNode),1);
};
tSpNode* mtSpNode_Clone(tSpNode* self){
	return memcpy(malloc(sizeof(tSpNode)),self,sizeof(tSpNode));
};
tSpFunctionextinfo* mtSpFunctionextinfo_Create(){
	return calloc(sizeof(tSpFunctionextinfo),1);
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
				// Compile function
				node->fextinfo = mtSpFunctionextinfo_Create();
				node->right=SpParse(self->right);
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
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Variabledeclaration,
							.returnedtype=type,
							//.identifier=name,
							.symbol=mtGNamespace_Findsymbol_NameKind(self->name_space,name,mtGSymbol_eType_Pointer)
						}
					);
				};
				//return nullptr;
				break;
			//case tLexem_Nullexpression:
			//	return mtSpNode_Clone(
			//		&(tSpNode){
			//			.type=tSplexem_Nullexpression,
			//			.returnedtype=mtGType_Void
			//		}
			//	);
			//case tLexem_Identifier:
			//	tGSymbol* symbol = mtGNamespace_Find_NameKind(GRootnamespace,self->identifier,mtGSymbol_eType_Pointer);
			//	return mtSpNode_Clone(
			//		&(tSpNode){
			//			.type=tSplexem_Symbol,
			//			.returnedtype=symbol->type,
			//			.symbol=symbol,
			//		}
			//	);
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

