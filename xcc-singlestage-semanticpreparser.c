// ------------------- Semantic preparser ---------------------

//tListnode* SppParsefunctionarguments(tLxNode* expr){
//	return nullptr;
//};

tGType* SppGeneratetype(tGType* basetype, tLxNode* typeexpr, char* *name);
tListnode /* <tGType> */ * SppParsefunctionarguments(tLxNode* expr){
	if(!expr){
		// wat
		fprintf(stderr,"SG: [E] SppParsefunctionarguments: Null pointer!\n");
		return nullptr;
	}else switch(expr->type){
		case tLexem_Comma:
			// Iterate
			return mtListnode_Merge(
				SppParsefunctionarguments(expr->left),
				SppParsefunctionarguments(expr->right)
			);
			break;
		default: {
			// Assumming we got them, tailcall to SpGeneratetype
			assert(expr);
			switch(expr->type){
				default:
					fprintf(stderr,"SG: [E] SppParsefunctionarguments: Unrecognized node %i:%s\n",expr->type,TokenidtoName[expr->type]);
					printf("SG: [E] SppParsefunctionarguments: Unrecognized node %i:%s\n",expr->type,TokenidtoName[expr->type]);
					printf("SG: [E] SppParsefunctionarguments: Full ast:\n");
					LfPrint_LxNode(expr);
					break;
				case tLexem_Typeexpression:
					return mtListnode_Cons(SppGeneratetype(expr->returnedtype,expr->left,nullptr),nullptr);
					break;
				case tLexem_Nullexpression:
					printf("SG: [W] SppParsefunctionarguments: Thee probably shouldn't use null expression as function arguments \n");				
					return nullptr;
					break;
			};
		};	break;

	};
};

tGType* SppGeneratetype(tGType* basetype, tLxNode* typeexpr, char* *name){
	tGType* temptype = basetype;
	if(typeexpr==nullptr){
		printf("SPP:[E] SppGeneratetype: typeexpr==nullptr\n");
		return nullptr;
	};
	for(tLxNode* i=typeexpr;i!=nullptr;i=i->left){
#ifdef qvGTrace
		printf("SPP:[T] SppGeneratetype: Forloop iteration for node %p:%i•%s\n",
			i,
			i->type,
			TokenidtoName[i->type]
		);
#endif
		switch(i->type){
			case tLexem_Functioncall:
				// Changing `mtGType_CreateFunctioncall_Expr` to `mtGType_CreateFunctioncall` 
				// so I could remove the silly thing known as Semanticpreparser
				temptype=mtGType_CreateFunctioncall(temptype,SppParsefunctionarguments(i->right));
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
			//		printf("SPP:[W] SppGeneratetype: `far` modifier outside pointer declatation \n");
			//	};
			//	break;
			//case tLexem_Nearmodifier:
			//	if(i->left->type==tLexem_Dereference){
			//		// Far pointer
			//		temptype=mtGType_CreateNearpointer(temptype);
			//		i=i->next; // skip `near` modifier
			//	}else{
			//		// Ignore `far`
			//		printf("SPP:[W] SppGeneratetype: `near` modifier outside pointer declatation \n");
			//	};
			//	break;
			//case tLexem_Hugemodifier:
			//	if(i->left->type==tLexem_Dereference){
			//		// Far pointer
			//		temptype=mtGType_CreateHugepointer(temptype);
			//		i=i->next; // skip `huge` modifier
			//	}else{
			//		// Ignore `far`
			//		printf("SPP:[W] SppGeneratetype: `huge` modifier outside pointer declatation \n");
			//	};
			//	break;
			case tLexem_Identifier:
				if(name)*name=i->identifier;
				return temptype;
			case tLexem_Nullexpression:
				if(name)*name=nullptr;
				return temptype;
			default:
				printf("SPP:[E] SppGeneratetype: Internal inconsistency: unexcepted lexem %i:%s while parsing a type\n",i->type,TokenidtoName[i->type]);
				LfPrint_LxNode(i);
				printf("SPP:[E] SppGeneratetype: Full AST:\n");
				LfPrint_LxNode(typeexpr);
				return nullptr;

		};
	};
	printf("SPP:[E] SppGeneratetype: Internal inconsistency: forloop dropped at nullptr\n");
	exit(5);
};




