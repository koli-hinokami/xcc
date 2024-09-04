// ------------------- Semantic preparser ---------------------

//tListnode* SppParsefunctionarguments(tLxNode* expr){
//	return nullptr;
//};
tSppNode* mtSppNode_Create(){
	return calloc(sizeof(tSppNode),1);
};



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
					fprintf(stderr,"SPP:[E] SppParsefunctionarguments: Unrecognized node %i:%s\n",expr->type,TokenidtoName[expr->type]);
					printf("SPP:[E] SppParsefunctionarguments: Unrecognized node %i:%s\n",expr->type,TokenidtoName[expr->type]);
					printf("SPP:[E] SppParsefunctionarguments: Full ast:\n");
					LfPrint_LxNode(expr);
					break;
				case tLexem_Typeexpression:
					return mtListnode_Cons(SppGeneratetype(expr->returnedtype,expr->left,nullptr),nullptr);
					break;
				case tLexem_Nullexpression:
					printf("SPP:[W] SppParsefunctionarguments: Thou probably shouldn't use null expression as function arguments \n");				
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

//void SppTransformrawvariabledeclarationsinsidestructurememberlistTraverse(tGType* self){;};
tLxNode* SppPreparse(tLxNode* self,tLxNode* parentnode);
tLxNode* SppPreparse_Lambda(tLxNode* self){
	return SppPreparse(self,nullptr);
};
tGType* SppTransformrawvariabledeclarations(tGType* self){
	if(!self)return self;
	if(
		  (self->atomicbasetype!=eGAtomictype_Structure)
		&&(self->atomicbasetype!=eGAtomictype_Union)
	)return self;
	if(!self->precompiledstructure)return self;
	printf("SPP:[T] SppTransformrawvariabledeclarations: Transforming structunion \n");
	tGType* cloned = mtGType_Clone(self);
	cloned->precompiledstructure=mtList_Transform(cloned->precompiledstructure,(void*(*)(void*))SppPreparse_Lambda);
	return cloned;
};
tLxNode* SppPreparse(tLxNode* self,tLxNode* parentnode){ // Lexicalpostparser
	if(self==nullptr)return nullptr;
#ifdef qvGTrace
	//printf("SPP:[T] SppPreparse(tLxNode* self %10p,tLxNode* parentnode %10p): Entered with node %i•%s\n",self,parentnode,self->type,TokenidtoName[self->type]);
	printf("SPP:[T] SppPreparse: Entered with node %i•%s\n",self->type,TokenidtoName[self->type]);
#endif
	// Propagate namespaces, kill multideclarations
	// Should be callable before Symbolgen is ran
	tLxNode* node = mtLxNode_Clone(self);
	node->parentnode=parentnode;
	if(	// Create and propagate namespaces for Symbolgen and Semanticparser
		  (self->type==tLexem_Blockstatement)
		||(self->type==tLexem_Forstatement)
	){
		// New namespace
		node->name_space=mtGNamespace_CreateInherit(parentnode?parentnode->name_space:GRootnamespace);
	}else{
		// Same namespace
		node->name_space=parentnode?parentnode->name_space:GRootnamespace;
	};
	// Transform tLexem_Rawvariabledeclaration inside structs
	node->returnedtype=SppTransformrawvariabledeclarations(node->returnedtype);
	//if(self->returnedtype){
	//	if(
	//		  (self->returnedtype->atomicbasetype==eGAtomictype_Structure)
	//		||(self->returnedtype->atomicbasetype==eGAtomictype_Structure)
	//	){
	//	};
	//};
	// Kill multideclarations that are currently returned by semanticparser
	if(self->type==tLexem_Rawvariabledeclaration){
#ifdef qvGTrace
		printf("SPP:[T] SppPreparse: Rawvariabledeclaration\n");
#endif
		// Two types possible -
		//  • Multideclaration - tLexem_Comma
		//  • and unparsed initialized declaration - tLexem_Assign
		if(self->left->type==tLexem_Comma){
#ifdef qvGTrace
		printf("SPP:[T] SppPreparse: Rawvariabledeclaration: Split and recurse\n");
#endif
			// Split and recurse
			tLxNode* node1 = mtLxNode_Create();
			node1->type=tLexem_Rawvariabledeclaration; // gotta confirm that it isnt `(declare (assign) ...)`
			node1->returnedtype=node->returnedtype;
			node1->left=self->left->left;
			tLxNode* node2 = mtLxNode_Create();
			node2->type=tLexem_Rawvariabledeclaration;
			node2->returnedtype=node->returnedtype;
			node2->left=self->left->right;
			
			node->type=tLexem_Declarationlist;
			node->left=SppPreparse(node1,node);
			node->right=SppPreparse(node2,node);
			free(node1);
			free(node2);
			return node;
		}else if(self->left->type==tLexem_Assign){
#ifdef qvGTrace
		printf("SPP:[T] SppPreparse: Rawvariabledeclaration: Split initializer\n");
#endif
			// Split typeexpr and initializer
			node->type=tLexem_Variabledeclaration;
			//node->returnedtype=node->returnedtype;
			node->left=self->left->left;
			node->right=self->left->right;
			return node;
		}else {
#ifdef qvGTrace
		printf("SPP:[T] SppPreparse: Rawvariabledeclaration: Confirm\n");
#endif
			// Confirm being Variabledeclaration
			node->type=tLexem_Variabledeclaration;
			//node->returnedtype=self->returnedtype;
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
			node1->type=tLexem_Typedefinition; 
			node1->returnedtype=node->returnedtype;
			node1->left=self->left->left;
			tLxNode* node2 = mtLxNode_Create();
			node2->type=tLexem_Rawvariabledeclaration;
			node2->returnedtype=node->returnedtype;
			node2->left=self->left->right;
			
			node->type=tLexem_Declarationlist;
			node->returnedtype=nullptr;
			node->left=SppPreparse(node1,node);
			node->right=SppPreparse(node2,node);
			return node;
		}else {
			//node->type=tLexem_Typedefinition;
			//node->returnedtype=self->returnedtype;
		};
	};
	// Return cloned node
	node->left=SppPreparse(self->left,node);
	node->right=SppPreparse(self->right,node);
	return node;
};
tSppNode* SppParse(tLxNode /* From SppPreparse! */ * self, tSppNode* parent){ // Semanticpreparser
	// Semanticpreparser. Mostly deep clone so I could have a tree of tSppNode,
	// but there also are some changes for Variabledeclarations and 
	// Typedeclarations - those are kinda like inside Symbolgen and Semanticparser.
	
	// Deepclone struct
	tSppNode* node = mtSppNode_Create();
	node->type         = self->type;
	node->initializer  = SppParse(self->initializer,node);
	node->condition    = SppParse(self->condition,node);
	node->left         = SppParse(self->right,node);
	node->right        = SppParse(self->right,node);

	node->constant     = self->constant;
	node->name_space   = self->name_space;
	node->parent       = parent;
	node->returnedtype = self->returnedtype;
	node->symbol       = nullptr;
	node->identifier   = self->identifier;
	// Convert Variabledefinitions
	if(node->type==tLexem_Variabledeclaration){
		node->returnedtype=SppGeneratetype(
			self->returnedtype,
			self->left,
			&(node->identifier)
		);
	};
	// Return
	return node;	
};

tGTargetSizet mtGType_Sizeof(tGType* self){
	assert(self);
	switch(self->atomicbasetype){
		case eGAtomictype_Void:
			return 0;
		case eGAtomictype_Structure:
			return self->structsize;
		case eGAtomictype_Enumeration:
			// TODO: Enumeration with base type other that `int`
			return 2;
		case eGAtomictype_Unresolved:
			assert(false);
		// Internal types
		case eGAtomictype_Union: // Temporary type - gets converted to struct later on
			return self->structsize;
		case eGAtomictype_Pointer:
			return 2;
		case eGAtomictype_Array:
			return mtGType_Sizeof(self->complexbasetype);
		case eGAtomictype_Function:
			assert(false);
		// IR-side types (and optionally C-side)
		case eGAtomictype_Int8:
			return 1;
		case eGAtomictype_Uint8:
			return 1;
		case eGAtomictype_Int16:
			return 2;
		case eGAtomictype_Uint16:
			return 2;
		case eGAtomictype_Int32:
			return 4;
		case eGAtomictype_Uint32:
			return 4;
		case eGAtomictype_Int64:
			return 8;
		case eGAtomictype_Uint64:
			return 8;
		case eGAtomictype_Int80:
			return 10;
		case eGAtomictype_Uint80:
			return 10;
		case eGAtomictype_Float32:
			return 4;
		case eGAtomictype_Float64:
			return 8;
		case eGAtomictype_Float80:
			return 10;
		// C-side types
		case eGAtomictype_Char:
			return 1;
		case eGAtomictype_Signedchar:
			return 1;
		case eGAtomictype_Unsignedchar:
			return 1;
		case eGAtomictype_Short:
			return 2;
		case eGAtomictype_Unsignedshort:
			return 2;
		case eGAtomictype_Int:
			return 2;
		case eGAtomictype_Unsigned:
			return 2;
		case eGAtomictype_Long:
			return 4;
		case eGAtomictype_Unsignedlong:
			return 4;
		case eGAtomictype_Longlong:
			return 8;
		case eGAtomictype_Unsignedlonglong:
			return 8;
		case eGAtomictype_Boolean:
			return 1;
		case eGAtomictype_Float:
			return 4;
		case eGAtomictype_Double:
			return 8;
		case eGAtomictype_Longdouble:
			return 10;
		default:
			assert(false);
	};
};
void SppCompileanonymousstructure(tGType* self, tGTargetSizet *offset, tGNamespace* name_space){
	assert(self);
	if(self->atomicbasetype==eGAtomictype_Structure){
		for(
			tListnode /* <tLxNode> */ * i = self->precompiledstructure->first;
			i;
			i=i->next
		){
			tLxNode* node = i->item;
			char* name = nullptr;
			assert(node->type==tLexem_Variabledeclaration);
			tGType* type = SppGeneratetype(node->returnedtype,node->left,&name);
			if(name){ // If we actually got a symbol
				// Create symbol
				mtGNamespace_Add(
					name_space,
					mtGSymbol_CreatePointer(
						name,
						type,
						mtGTargetPointer_Clone(
							&(tGTargetPointer){
								.nonconstant=false,
								.segment=meGSegment_Relative,
								.offset=*offset
							}
						)
					)
				);
				*offset+=mtGType_Sizeof(type);
			}else{
				// Anonymous structunion
				SppCompileanonymousstructure(type,offset,self->structure);
			};
			// Advance position
		};
		//self->structsize=offset;
	}else if(self->atomicbasetype==eGAtomictype_Union){
		tGTargetSizet unionsize;
		for(
			tListnode /* <tLxNode> */ * i = self->precompiledstructure->first;
			i;
			i=i->next
		){
			tLxNode* node = i->item;
			char* name = nullptr;
			assert(node->type==tLexem_Variabledeclaration);
			tGType* type = SppGeneratetype(node->returnedtype,node->left,&name);
			if(name){ // If we actually got a symbol
				// Create symbol
				mtGNamespace_Add(
					name_space,
					mtGSymbol_CreatePointer(
						name,
						type,
						mtGTargetPointer_Clone(
							&(tGTargetPointer){
								.nonconstant=false,
								.segment=meGSegment_Relative,
								.offset=*offset
							}
						)
					)
				);
				if(mtGType_Sizeof(type)>unionsize)unionsize=mtGType_Sizeof(type);
			}else{
				// Anonymous structunion
				SppCompileanonymousstructure(type,offset,self->structure);
			};
			// Advance position
		};
		*offset+=unionsize;
		//self->structsize=offset;
	}else{
		assert(false);	
	};

};
tGType* SppCompilestructure(tGType* self){
	assert(self);
	if(self->atomicbasetype==eGAtomictype_Structure){
		tGTargetNearpointer offset=0;
		self->structure = mtGNamespace_Create();
		for(
			tListnode /* <tLxNode> */ * i = self->precompiledstructure->first;
			i;
			i=i->next
		){
			tLxNode* node = i->item;
			char* name = nullptr;
			if(node->type==tLexem_Variabledeclaration){
				tGType* type = SppGeneratetype(node->returnedtype,node->left,&name);
				if(name){ // If we actually got a symbol
					// Create symbol
					mtGNamespace_Add(
						self->structure,
						mtGSymbol_CreatePointer(
							name,
							type,
							mtGTargetPointer_Clone(
								&(tGTargetPointer){
									.nonconstant=false,
									.segment=meGSegment_Relative,
									.offset=offset
								}
							)
						)
					);
					// Advance position
					offset+=mtGType_Sizeof(type);
				}else{
					// Anonymous structunion
					SppCompileanonymousstructure(type,&offset,self->structure);

				};
			}else if(node->type==tLexem_Nulldeclaration){
			}else{
				printf("SPP:[E] SppCompilestructure: Invalid declaration inside structure! \n");
			};
		};
		self->structsize=offset;
	}else if(self->atomicbasetype==eGAtomictype_Union){
		tGTargetNearpointer offset=0;
		self->structure = mtGNamespace_Create();
		for(
			tListnode /* <tLxNode> */ * i = self->precompiledstructure->first;
			i;
			i=i->next
		){
			tLxNode* node = i->item;
			char* name = nullptr;
			assert(node->type==tLexem_Variabledeclaration);
			tGType* type = SppGeneratetype(node->returnedtype,node->left,&name);
			if(name){ // If we actually got a symbol
				// Create symbol
				mtGNamespace_Add(
					self->structure,
					mtGSymbol_CreatePointer(
						name,
						type,
						mtGTargetPointer_Clone(
							&(tGTargetPointer){
								.nonconstant=false,
								.segment=meGSegment_Relative,
								.offset=offset
							}
						)
					)
				);
				// Recalculate union size
				if(mtGType_Sizeof(type)>offset)offset=mtGType_Sizeof(type);
			}else{
				// Anonymous structunion
				SppCompileanonymousstructure(type,&offset,self->structure);
			};
		};
		self->structsize=offset;
	}else{
		assert(false);	
	};
	return self;
};
