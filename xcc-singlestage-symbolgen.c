// ----------- Symbol list generator ------------
tList /* <tGType> */ * SgUnresolvedtypes;
tList /* <tGType> */ * SgUnresolvedstructures;
tList /* <tGType> */ * SgCompilablestructures;
tGNamespace* GStructuretypes;

void SgRegisterstructure(tGType* type){
#ifdef qvGTrace
	printf("SG: [T] SgRegisterstructure: entered \n");
#endif
	assert(type);
	assert(type->atomicbasetype==eGAtomictype_Structure);
	mtList_Append(SgCompilablestructures,type); 
	if(type->unresolvedsymbol){
		// Do we have fields declared?
		if(type->precompiledstructure){
			// Register as definition
#ifdef qvGTrace
			printf("SG: [T] SgRegisterstructure: registering structure definition \"%s\"\n",mtGType_ToString_Embeddable(type));
#endif
			mtGNamespace_Add(GStructuretypes,
				mtGSymbol_CreateTypedef(type->unresolvedsymbol,type)
			);
		}else{
			// We actually should register it as a structure to be resolved
			printf("SG: [T] SgRegisterstructure: registering structure usage \"%s\"\n",type->unresolvedsymbol);
			mtList_Append(SgUnresolvedstructures,type); 
		};
	}else{
		// No hope of registering this structure
	};
	//if mtGNamespace_Find_NameKind(type-> // someone remind me what I thought about
};
void SgRegisterstructureTraverse(tGType* type){
#ifdef qvGTrace
	printf("SG: [T] SgRegisterstructureTraverse: entered \n");
#endif
	// Take type and traverse it
	if(type==nullptr){
		printf("SG: [W] SgCompileandregisterstructs: `type==nullptr` \n");
	}else switch(type->atomicbasetype){
		case eGAtomictype_Structure:
			if(type->precompiledstructure){
				//mtList_Foreach(type->precompiledstructure,(void(*)(void*))SgRegisterstructureTraverse);
			};
			SgRegisterstructure(type);
			break;
		case eGAtomictype_Unresolved:
			printf("SG: [W] SgRegisterstructureTraverse(): Unresolved type seen \n");
			// do nothing
			break;
		case eGAtomictype_Function:
			mtListnode_Foreach(type->functionarguments,(void(*)(void*))SgRegisterstructureTraverse);
		case eGAtomictype_Pointer:
		case eGAtomictype_Array:
			SgRegisterstructureTraverse(type->complexbasetype);
			break;
		default:
			printf("SG: [E] SgRegisterstructureTraverse: Unrecognized atomic type %i:%s \n",
				type->atomicbasetype,mtGType_ToString_Embeddable(type)
			);
			break;
	};
};
void SgRegisterstructures(tLxNode* ast){
	switch(ast->type){
		case tLexem_Typedefinition:
			SgRegisterstructureTraverse(ast->returnedtype);
			break;
		case tLexem_Variabledeclaration:
			SgRegisterstructureTraverse(ast->returnedtype);
			break;
		default:
	};
};
void SgResolvestructure(tGType* type){
	printf("SG: [T] SgResolvestructure(%s): entered\n",mtGType_ToString_Embeddable(type));
	*type = *(
		mtGNamespace_Findsymbol_NameKind(
			GStructuretypes,
			type->unresolvedsymbol,
			mtGSymbol_eType_Typedef
		)->type
	);
};
void SgResolvestructures(){
#ifdef qvGTrace
	printf("SG: [T] SgResolvestructures: entered \n");
#endif	
	mtList_Foreach(SgUnresolvedstructures,(void(*)(void*))SgResolvestructure);
};
tListnode /* <tGType> */ * SppParsefunctionarguments(tLxNode* expr);
tGType* SgGeneratetype(tGType* basetype, tLxNode* typeexpr, char* *name){
	printf("SG: [E] SgGeneratetype: Tailcalling to SppGeneratetype\n");
	return SppGeneratetype(basetype, typeexpr, name);
//	tGType* temptype = basetype;
//	if(typeexpr==nullptr){
//		printf("SG: [E] SgGeneratetype: typeexpr==nullptr\n");
//		return nullptr;
//	};
//	for(tLxNode* i=typeexpr;i!=nullptr;i=i->left){
//#ifdef qvGTrace
//		printf("SG: [T] SgGeneratetype: Forloop iteration for node %p:%i•%s\n",
//			i,
//			i->type,
//			TokenidtoName[i->type]
//		);
//#endif
//		switch(i->type){
//			case tLexem_Functioncall:
//				// Changing `mtGType_CreateFunctioncall_Expr` to `mtGType_CreateFunctioncall` 
//				// so I could remove the silly thing known as Semanticpreparser
//				temptype=mtGType_CreateFunctioncall(temptype,SppParsefunctionarguments(i->right));
//				break;
//			case tLexem_Arrayindex:
//				temptype=mtGType_CreateArray_Expr(temptype,i->right);
//				break;
//			case tLexem_Dereference:
//				temptype=mtGType_CreatePointer(temptype);
//				break;
//			// v For dereference specifically:
//			//case tLexem_Farmodifier:
//			//	if(i->left->type==tLexem_Dereference){
//			//		// Far pointer
//			//		temptype=mtGType_CreateFarpointer(temptype);
//			//		i=i->next; // skip `far` modifier
//			//	}else{
//			//		// Ignore `far`
//			//		printf("SG: [W] SgGeneratetype: `far` modifier outside pointer declatation \n");
//			//	};
//			//	break;
//			//case tLexem_Nearmodifier:
//			//	if(i->left->type==tLexem_Dereference){
//			//		// Far pointer
//			//		temptype=mtGType_CreateNearpointer(temptype);
//			//		i=i->next; // skip `near` modifier
//			//	}else{
//			//		// Ignore `far`
//			//		printf("SG: [W] SgGeneratetype: `near` modifier outside pointer declatation \n");
//			//	};
//			//	break;
//			//case tLexem_Hugemodifier:
//			//	if(i->left->type==tLexem_Dereference){
//			//		// Far pointer
//			//		temptype=mtGType_CreateHugepointer(temptype);
//			//		i=i->next; // skip `huge` modifier
//			//	}else{
//			//		// Ignore `far`
//			//		printf("SG: [W] SgGeneratetype: `huge` modifier outside pointer declatation \n");
//			//	};
//			//	break;
//			case tLexem_Identifier:
//				if(name)*name=i->identifier;
//				return temptype;
//			case tLexem_Nullexpression:
//				if(name)*name=nullptr;
//				return temptype;
//			default:
//				printf("SG: [E] SgGeneratetype: Internal inconsistency: unexcepted lexem %i:%s while parsing a type\n",i->type,TokenidtoName[i->type]);
//				LfPrint_LxNode(i);
//				printf("SG: [E] SgGeneratetype: Full AST:\n");
//				LfPrint_LxNode(typeexpr);
//				return nullptr;
//
//		};
//	};
//	printf("SG: [E] SgGeneratetype: Internal inconsistency: forloop dropped at nullptr\n");
//	exit(5);
};
void SgAutoiteratecommas(tLxNode* ast, void(*lambda)(tLxNode* ast)){
	if(!ast){
		printf("sg: [E] SgAutoiteratecommas: Invalid expression, ignoring \n");
	}else if(ast->left->type==tLexem_Nulldeclaration){
		// Ignore lol
	}else if(ast->left->type==tLexem_Comma){
		// Handle commas
		SgAutoiteratecommas(ast->left,lambda);
		SgAutoiteratecommas(ast->left,lambda);
	}else{
		// Handle whatever it is, probably something important
		lambda(ast);
	};
};
void SgParseTypedefInternal(tLxNode* ast){
};
void SgRegisterfunctionarguments(tLxNode* self,tGNamespace* namespace){
#ifdef qvGTrace
	printf("SG: [T] SgRegisterfunctionarguments: entered \n");
#endif
	if(self){
		if(self->type==tLexem_Comma){
			SgRegisterfunctionarguments(self->left,namespace);
			SgRegisterfunctionarguments(self->right,namespace);
		}else if(self->type==tLexem_Typeexpression){
			char* name;
			tGType* type = SppGeneratetype(self->returnedtype,self->left,&name);
			mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Leftvalue;
			if(name){
				mtGNamespace_Add(
					namespace,
					mtGSymbol_CreatePointer(
						name,
						type,
						nullptr
					)
				);
			};
		}else if(self->type==tLexem_Ellipsis){
			// Variadic
			mtGNamespace_Add(
				namespace,
				mtGSymbol_CreatePointer(
					"__xcc_variadic",
					mtGType_CreateAtomic(eGAtomictype_Void),
					nullptr
				)
			);
		}else if(self->type==tLexem_Nullexpression){
			// Nothing here
		}else {
			// Someone, put an error message here!
			printf("SG: [E] SgRegisterfunctionarguments: Unrecognized node %i•%s \n",self->type,TokenidtoName[self->type]);
			assert(false);
		};
	}else{
		assert(false);
	};
};
void SgParse(tLxNode* ast){
	if(ast==nullptr){
		printf("SG: [W] SgParse: Null AST protection triggered \n");
		return;
	};
#ifdef qvGTrace
	printf("SG: [T] SgParse: Entered with node %i∙%s\n",ast->type,TokenidtoName[ast->type]);
	//if(ast->type!=tLexem_Declarationlist)LfPrint_LxNode(ast);
#endif
	switch(ast->type){
		case tLexem_Nulldeclaration:
			// Speaks for itself
			break;
		//case tLexem_Namespace: // should work for `name_space std { name_space io { ... } }`
		//	mtGNamespace_Add(
		//		ast->name_space,
		//		mtGSymbol_CreateNamespace(
		//			ast->left->identifier,
		//			ast->right->name_space
		//		)
		//	);
		//	break;
		case tLexem_Declarationlist:
			// Program root, pretty much
			SgParse(ast->left);
			if(ast->right)SgParse(ast->right);
			break;
		case tLexem_Blockstatement: {
			SgParse(ast->left);
			SgParse(ast->right);
		};
		case tLexem_Returnstatement:
		case tLexem_Expressionstatement:
			// Skip both
			break;
		case tLexem_Functiondeclaration: {
			char* name = nullptr;
			tGType* type = SgGeneratetype(
				ast->returnedtype,ast->left,&name
			);
			assert(type);
			assert(mtGType_GetBasetype(type));
			mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Leftvalue;
			mtGNamespace_Add( // Register function itself
				ast->parentnode?ast->parentnode->name_space:GRootnamespace,
				mtGSymbol_CreatePointer(
					name,
					type,
					nullptr
				)
			);
			if(ast->right){
				mtGNamespace_Add( // And it's namespace... you need to
				                  // take care of locals, after all...
					ast->parentnode->name_space,
					mtGSymbol_CreateNamespace(
						name,
						ast->right->name_space
					)
				);
				tLxNode* functionbase = ast->left;
				while(functionbase->left->type!=tLexem_Identifier){
					assert(functionbase);
					assert(functionbase->left);
					functionbase = functionbase->left;
				};
				assert(ast->left);
				assert(functionbase->type==tLexem_Functioncall);
				SgRegisterfunctionarguments(functionbase->right,ast->right->name_space);
			};
			SgParse(ast->right); // functions and their locals...
		};	break;
		case tLexem_Variabledeclaration: {
#ifdef qvGTrace
			printf("SG: [T] SgParse: Variable declaration \n");
#endif
			// Variable initialized decls, possibly with initializer
			SgRegisterstructureTraverse(ast->returnedtype);
			// Traverse declaration list
			if(!ast->left){
				printf("SG: [E] SgParse: Variable declaration: Invalid type expression, ignoring \n");
			}else{
				if(ast->left->type==tLexem_Nulldeclaration){
					// Ignore lol
					// Not entirely though, you still got to register structure types
				}else{
					// Handle a declaration
					char* name = nullptr;
					tGType* type = SgGeneratetype(ast->returnedtype,ast->left,&name);
					mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Leftvalue;
					//if(ast->right){
					//	mtGNamespace_Add(
					//		ast->name_space,
					//		mtGSymbol_CreateDeferred(
					//			name,
					//			type,
					//			ast->right
					//		)
					//	);
					//}else{
					if(name){ // You can still write a 
						  // declaration that declares
						  // no variables, just type
						// Uninitialized
						mtGNamespace_Add(
							ast->name_space,
							mtGSymbol_CreatePointer(
								name,
								type,
								nullptr
							)
						);
					};
					//};
				};
			};
		};	break;
		case tLexem_Externaldeclaration: {
#ifdef qvGTrace
			printf("SG: [T] SgParse: Externaldeclaration \n");
#endif
			// External declaration.
			// Known no initializer.
			SgRegisterstructureTraverse(ast->returnedtype);
			// Traverse declaration list
			if(!ast->left){
				printf("SG: [E] SgParse: Externaldeclaration: Invalid type expression, ignoring \n");
				ErfError();
			}else{
				if(ast->left->type==tLexem_Nulldeclaration){
					// Ignore lol
					// Not entirely though, you still got to register structure types
				}else{
					// Handle a declaration
					char* name = nullptr;
					tGType* type = SgGeneratetype(ast->returnedtype,ast->left,&name);
					mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Leftvalue;
					tGInstruction* instr = 
						mtGInstruction_CreateBasic(
							tInstruction_Cnop,
							eGAtomictype_Void
						);
					instr->label = name;
					tGTargetPointer* ptr = 
						mtGTargetPointer_CreateDynamic(
							instr
						);
					tGSymbol* sym = 
						mtGSymbol_CreatePointer(
							name,
							type,
							ptr
						);
					mtGNamespace_Add(ast->name_space,sym);
				};
			};
		};	break;
		case tLexem_Typedefinition:
			//
#ifdef qvGTrace
			printf("SG: [T] SgParse: Type declaration \n");
#endif
			// Type definition
			SgRegisterstructureTraverse(ast->returnedtype);
			// Traverse declaration list
			if(!ast->left){
				printf("SG: [E] SgParse: typedef: Invalid type expression \n");
			}else if(ast->left->type==tLexem_Nulldeclaration){
				// Ignore lol
			}else{
				// Handle a declaration
				char* name = nullptr;
				tGType* type = SgGeneratetype(ast->returnedtype,ast->left,&name);
				mtGNamespace_Add(
					ast->name_space,
					mtGSymbol_CreateTypedef(
						name,
						type
					)
				);
			};
			break;
		case tLexem_Forstatement:
			SgParse(ast->initializer);
		case tLexem_Ifstatement:
			SgParse(ast->condition);
			SgParse(ast->left);
			SgParse(ast->right);
			break;
		{	// Ignorable lexems
			case tLexem_Equals:
			case tLexem_Identifier:
			case tLexem_Nullexpression:
			case tLexem_Decrement:
			case tLexem_Postdecrement:
			case tLexem_Assign:
			case tLexem_Logicalnot:
			case tLexem_Notequal:
			case tLexem_Whilestatement:
			case tLexem_Functioncall:
			case tLexem_Postincrement:
			case tLexem_Greaterequal:
			case tLexem_Arrayindex:
			case tLexem_Dereference:
			case tLexem_Integerconstant:
			case tLexem_Logicaland:
			case tLexem_Multiply:
			case tLexem_Lessthan:
			case tLexem_Memberbypointer:
			case tLexem_Switchcase:
			case tLexem_Logicalor:
			case tLexem_Switchstatement:
			case tLexem_Bitwiseand:
			case tLexem_Lessequal:
			case tLexem_Greaterthan:
			case tLexem_Increment:
				break;
		};
		default:
			fprintf(stderr,"SG: [E] SgParse: Unrecognized node type %i:%s\n",ast->type,TokenidtoName[ast->type]);
			GError();
			break;
	};
};
void SgRegisterunresolvedtype(tGType* type){
	printf("SG: [D] Registering unresolved type `%s`\n",type->unresolvedsymbol);
	mtList_Append(SgUnresolvedtypes,type);
};
void SgFindunresolvedtypes_Type(tGType* type);
void SgFindunresolvedtypes_LxNode(tLxNode* node){
	assert(node);
	printf("SG: [T] SgFindunresolvedtypes_LxNode: Entered with node %i•%s\n",
		node->type,TokenidtoName[node->type]
	);
	if(
		  node->type!=tLexem_Switchcase
		&&node->type!=tLexem_Switchdefault
	)
		if(node->initializer)
			SgFindunresolvedtypes_LxNode(node->initializer);
	if(node->condition)  SgFindunresolvedtypes_LxNode(node->condition);
	if(node->left)       SgFindunresolvedtypes_LxNode(node->left);
	if(node->right)      SgFindunresolvedtypes_LxNode(node->right);
	if(node->returnedtype)SgFindunresolvedtypes_Type(node->returnedtype);
	//else{
	//	printf("SG: [E] SgFindunresolvedtypes_LxNode: Unexcepted node type %i:%s inside node %s \n",
	//		node->type,TokenidtoName[node->type],
	//		mtLxNode_ToString(node)
	//	);
	//	ErfError();
	//};
};
void SgFindunresolvedtypes_Type(tGType* type){
#ifdef qvGTrace
	//printf("SG: [T] SgFindunresolvedtypes_Type(%p:%s): entered \n",type,mtGType_ToString_Embeddable(type));
#endif
	if(type==nullptr){
		printf("SG: [W] SgFindunresolvedtypes_Type: `type==nullptr` \n");
	}else switch(type->atomicbasetype){
		case eGAtomictype_Unresolved:
			SgRegisterunresolvedtype(type);
			break;
		case eGAtomictype_Function:
		case eGAtomictype_Nearfunction:
		case eGAtomictype_Farfunction:
			mtListnode_Foreach(type->functionarguments,(void(*)(void*))SgFindunresolvedtypes_Type);
		case eGAtomictype_Pointer:
		case eGAtomictype_Array:
			SgFindunresolvedtypes_Type(type->complexbasetype);
			break;
		case eGAtomictype_Structure: // I still need to compile structures
		case eGAtomictype_Union:     
			mtList_Foreach(type->precompiledstructure,(void(*)(void*))SgFindunresolvedtypes_LxNode);
			//SgRegistercompilablestructure(type);
		default:
			
	};
};
void SgFindunresolvedtypes(tGNamespace* name_space);
void SgFindunresolvedtypes_Symbol(tGSymbol* symbol){
#ifdef qvGTrace
	//printf("SG: [T] SgFindunresolvedtypes_Symbol(%p): entered \n",symbol);
#endif	
	// Traverse type and check for undefined types
	switch(symbol->symbolkind){
		case mtGSymbol_eType_Constant:
		case mtGSymbol_eType_Pointer:
		case mtGSymbol_eType_Deferredevaulation:
		case mtGSymbol_eType_Typedef:
			SgFindunresolvedtypes_Type(symbol->type);
			break;
		case mtGSymbol_eType_Namespace:
			SgFindunresolvedtypes(symbol->name_space);
			break;
		case mtGSymbol_eType_Null:
			assert(false);
			return;
	};
};
void SgFindunresolvedtypes(tGNamespace* name_space){
#ifdef qvGTrace
	printf("SG: [T] SgFindunresolvedtypes: entered \n");
#endif	
	// Traverse and register
	mtList_Foreach(&name_space->symbols,(void(*)(void*))SgFindunresolvedtypes_Symbol);
};
tGType* SgResolvetype(tGNamespace* name_space, char* name){
	if(!name){
		// wat
		printf("SG: [E] SgResolvetype(tGNamespace* `%p`,nullptr): What do you mean 'find type idk which one cuz i didn't provided type's name'?\n",name_space);
		return nullptr;
	};
	for(tListnode* i=name_space->symbols.first;i!=nullptr;i=i->next){
		tGSymbol* symbol = i->item;
		if(!symbol){
			// wat
		}else if(symbol->symbolkind!=mtGSymbol_eType_Typedef){
			// wronge node type
		}else if(strcmp(symbol->name,name)){
			// wronge name
		}else {
			// finally! I found thee!
			assert(symbol->type->atomicbasetype!=eGAtomictype_Unresolved);
			return symbol->type;
		}
	};
	// I was unable to find anything. Try again?
	printf("SG: [E] SgResolvetype(tGNamespace* `%p`,\"%s\"): Sadly, no such type was found. Try again? \n",name_space,name);
	return nullptr;
};
void SgResolveunresolvedtypes_Resolvetype(tGType* type){
	printf("SG: [T] SgResolveunresolvedtypes_Resolvetype: Resolving type \"%s\"\n",type->unresolvedsymbol);
	eGValuecategory valcat=type->valuecategory;
	if(type->atomicbasetype!=eGAtomictype_Unresolved)return;
	tGType* temp = SgResolvetype(GRootnamespace,type->unresolvedsymbol);
	if(temp==nullptr){
		fprintf(stderr,"SG: [E] SgResolveunresolvedtypes_Resolvetype: Unable to resolve type \"%s\"! \n",type->unresolvedsymbol);
		ErfError();
	}else{
		*type = *temp;
		type->valuecategory=valcat;
	}
};
void SgResolveunresolvedtypes(){
#ifdef qvGTrace
	printf("SG: [T] SgResolveunresolvedtypes: entered \n");
#endif	
	mtList_Foreach(SgUnresolvedtypes,(void(*)(void*))SgResolveunresolvedtypes_Resolvetype);
	// Additionally resolve structs
	SgResolvestructures();
};
void SgCompilestructures(){
	printf("SP: [D] SgCompilestructures: entered \n");
	mtList_Foreach(SgCompilablestructures,(void(*)(void*))SppCompilestructure);
};
