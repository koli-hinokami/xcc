// ----------- Symbol list generator ------------

tGType* SgGeneratetype(tGType* basetype, tLxNode* typeexpr, char* *name){
	tGType* temptype = basetype;
	if(typeexpr==nullptr){
		printf("SG: [E] SgGeneratetype: typeexpr==nullptr\n");
		return nullptr;
	};
	for(tLxNode* i=typeexpr;i!=nullptr;i=i->left){
#ifdef qvGTrace
		if(0)printf("SG: [T] SgGeneratetype: Forloop iteration for node %p:%i•%s\n",
			i,
			i->type,
			TokenidtoName[i->type]
		);
#endif
		switch(i->type){
			case tLexem_Functioncall:
				temptype=mtGType_CreateFunctioncall_Expr(temptype,i->right);
				break;
			case tLexem_Arrayindex:
				temptype=mtGType_CreateArray_Expr(temptype,i->right);
				break;
			case tLexem_Dereference:
				temptype=mtGType_CreatePointer(temptype);
				break;
			// v For dereference specifically:
			//case tLexem_Farmodifier:
			//	if(i->left->type==tLexem_Dereference){
			//		// Far pointer
			//		temptype=mtGType_CreateFarpointer(temptype);
			//		i=i->next; // skip `far` modifier
			//	}else{
			//		// Ignore `far`
			//		printf("SG: [W] SgGeneratetype: `far` modifier outside pointer declatation \n");
			//	};
			//	break;
			//case tLexem_Nearmodifier:
			//	if(i->left->type==tLexem_Dereference){
			//		// Far pointer
			//		temptype=mtGType_CreateNearpointer(temptype);
			//		i=i->next; // skip `near` modifier
			//	}else{
			//		// Ignore `far`
			//		printf("SG: [W] SgGeneratetype: `near` modifier outside pointer declatation \n");
			//	};
			//	break;
			//case tLexem_Hugemodifier:
			//	if(i->left->type==tLexem_Dereference){
			//		// Far pointer
			//		temptype=mtGType_CreateHugepointer(temptype);
			//		i=i->next; // skip `huge` modifier
			//	}else{
			//		// Ignore `far`
			//		printf("SG: [W] SgGeneratetype: `huge` modifier outside pointer declatation \n");
			//	};
			//	break;
			case tLexem_Identifier:
				if(name)*name=i->identifier;
				return temptype;
			default:
				printf("SG: [E] SgGeneratetype: Internal inconsistency: unexcepted lexem %i:%s while parsing a type\n",i->type,TokenidtoName[i->type]);
				return nullptr;

		};
	};
	printf("SG: [E] SgGeneratetype: Internal inconsistency: forloop dropped at nullptr\n");
	exit(5);
	
};
void SgParse(tGNamespace* namespace, tLxNode* ast){
#ifdef qvGTrace
	printf("SG: [T] SgParse: Entered with node %i∙%s\n",ast->type,TokenidtoName[ast->type]);
#endif
	if(ast==nullptr){
		printf("SG: [T] SgParse: Null AST protection triggered \n");
		return;
	};
	if(namespace==nullptr){
		printf("SG: [T] SgParse: Null namespace protection triggered \n");
		return;
	};
	switch(ast->type){
		case tLexem_Declarationlist:
			// Program root, pretty much
			SgParse(namespace,ast->left);
			if(ast->right)SgParse(namespace,ast->right);
			break;
		case tLexem_Variabledeclaration: {
			// Variable declaration, possibly with initializer
			char* name;
			//printf("SG: [T] Debug: %i\n",ast->returnedtype->atomicbasetype);
			tGType* type = SgGeneratetype(ast->returnedtype,ast->left,&name);
			mtGNamespace_Add(
				namespace,
				mtGSymbol_CreateDeferred(
					name,
					type,
					ast->right
				)
			);
		};	break;
		default:
			fprintf(stderr,"SG: [E] SgParse: Unrecognized node type %i:%s\n",ast->type,TokenidtoName[ast->type]);
			break;
	};
};

