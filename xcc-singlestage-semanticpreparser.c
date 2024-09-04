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
char* SppGeneratetype_GetName(tLxNode* typeexpression){
	char* name;
	SppGeneratetype(nullptr,typeexpression,&name);
	return name;
};

tLxNode* SppPreparse(tLxNode* self){
	// TODO: Propagate namespaces, kill multideclarations
	// Should be callable before Symbolgen is ran
	tLxNode* node = mtLxNode_Clone(self);
	if(	// Create and propagate namespaces for Symbolgen and Semanticparser
		  (self->type==tLexem_Blockstatement)
		||(self->type==tLexem_Forstatement)
	){
		node->name_space=mtGNamespace_CreateInherit(self->name_space);
	}else{
		node->name_space=self->name_space;
	};
	// Kill multideclarations that are currently returned by semanticparser
	if(self->type==tLexem_Rawvariabledeclaration){
		// Two types possible -
		//  • Multideclaration - tLexem_Comma
		//  • and unparsed initialized declaration - tLexem_Assign
		if(self->left->type==tLexem_Comma){
			// Split and recurse
			tLxNode* node1 = mtLxNode_Create();
			node1->type=tLexem_Rawvariabledeclaration; // gotta confirm that it isnt `(declare (assign) ...)`
			node1->returnedtype=self->returnedtype;
			node1->left=self->left->left;
			tLxNode* node2 = mtLxNode_Create();
			node2->type=tLexem_Rawvariabledeclaration;
			node2->returnedtype=self->returnedtype;
			node2->left=self->left->right;
			
			node->type=tLexem_Declarationlist;
			node->left=SppPreparse(node1);
			node->right=SppPreparse(node2);
			free(node1);
			free(node2);
			return node;
		}else if(self->left->type==tLexem_Assign){
			// Split typeexpr and initializer
			node->type=tLexem_Variabledeclaration;
			node->returnedtype=self->returnedtype;
			node->left=self->left->left;
			node->right=self->left->right;
			return node;
		}else {
			// Confirm being Variabledeclaration
			node->type=tLexem_Variabledeclaration;
			node->returnedtype=self->returnedtype;
			node->left=self->left;
			node->right=nullptr;
			return node;
		};
	};
	// Also kill multidefinitions for typedefs
	if(self->type==tLexem_Typedefinition){
		if(self->left->type==tLexem_Comma){
			// Split and recurse
			tLxNode* node1 = mtLxNode_Create();
			node1->type=tLexem_Typedefinition; // gotta confirm that it isnt `(declare (assign) ...)`
			node1->returnedtype=self->returnedtype;
			node1->left=self->left->left;
			tLxNode* node2 = mtLxNode_Create();
			node2->type=tLexem_Rawvariabledeclaration;
			node2->returnedtype=self->returnedtype;
			node2->left=self->left->right;
			
			node->type=tLexem_Declarationlist;
			node->returnedtype=self->returnedtype;
			node->left=SppPreparse(node1);
			node->right=SppPreparse(node2);
			return node;
		}else {
			node->type=tLexem_Typedefinition;
			node->returnedtype=self->returnedtype;
			node->left=self->left;
			return node;
		};
	};
};



