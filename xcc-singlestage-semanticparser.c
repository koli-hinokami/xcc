// Semantic(al) parser

tSpNode* mtSpNode_Create(void){
	return calloc(sizeof(tSpNode),1);
};
tSpNode* mtSpNode_Clone(tSpNode* self){
	return memcpy(malloc(sizeof(tSpNode)),self,sizeof(tSpNode));
};

tSpNode* SpParse(tLxNode* self){ // Semantic parser primary driver
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
						.right=SpParse(self->right)
					}
				);
			//case tLexem_Nullexpression:
			//	return mtSpNode_Clone(
			//		&(tSpNode){
			//			.type=tSplexem_Nullexpression,
			//			.returnedtype=mtGType_Void
			//		}
			//	);
			//case tLexem_Constant:
			//	return mtSpNode_Clone(
			//		&(tSpNode){
			//			.type=tSplexem_Constant,
			//			.returnedtype=mtGType_Integer,
			//			.constant=self->constant
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

