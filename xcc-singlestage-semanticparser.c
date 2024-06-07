// Semantic(al) parser
tSpNode /* Function declaration */ * SpCurrentfunction;

// -------------------------- Forward declarations --------------------------
tSpNode* SpInsertimpliedrvaluecast(tSpNode* self);

// ------------------------------- Functions -------------------------------
tSpNode* mtSpNode_Create(void){
	return calloc(sizeof(tSpNode),1);
};
tSpNode* mtSpNode_Clone(tSpNode* self){
	return memcpy(malloc(sizeof(tSpNode)),self,sizeof(tSpNode));
};
tSpFunctionextinfo* mtSpFunctionextinfo_Allocate(){
	return calloc(sizeof(tSpFunctionextinfo),1);
};
tSpFunctionextinfo* mtSpFunctionextinfo_Create(){
	tSpFunctionextinfo* i = mtSpFunctionextinfo_Allocate();
	i->argumentssize = 0;
	i->localssize = 0;
	return i;
};
tGTargetPointer* SpAllocateargumentstorage(tSpFunctionextinfo* fextinfo, tGTargetSizet size){
	fextinfo->argumentssize+=size;
	return mtGTargetPointer_CreateStatic(
		meGSegment_Stackframe,
		GTargetStackframeArgumentsstart
		+fextinfo->argumentssize-size
	);
};
tGTargetPointer* SpAllocatelocalvarstorage(tSpFunctionextinfo* fextinfo, tGTargetSizet size){
	fextinfo->localssize+=size;
	return mtGTargetPointer_CreateStatic(
		meGSegment_Stackframe,
		GTargetStackframeLocalsstart
		-fextinfo->localssize
	);
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
				tGType* type = SppGeneratetype(
					typeexpr->returnedtype,
					typeexpr->left,
					&name
				);
				mtGType_Destroy(type);
				type=mtGNamespace_Findsymbol_NameKind(
					name_space,
					name,
					mtGSymbol_eType_Pointer
				)->type;
				mtGNamespace_Findsymbol_NameKind(
					name_space,
					name,
					mtGSymbol_eType_Pointer
				)->allocatedstorage=SpAllocateargumentstorage(
					fextinfo,
					mtGType_Sizeof(type)
				);
				mtGType_GetBasetype(
					mtGNamespace_Findsymbol_NameKind(
						name_space,
						name,
						mtGSymbol_eType_Pointer
					)->type
				)->valuecategory=eGValuecategory_Leftvalue;
			}else if(typeexpr->type==tLexem_Ellipsis){
				//tGType* type = SppGeneratetype(typeexpr->returnedtype,typeexpr->left,&name);
				mtGNamespace_Add(
					name_space,
					mtGSymbol_CreatePointer(
						"__xcc_variadic",
						mtGType_CreateAtomic(eGAtomictype_Void),
						mtGTargetPointer_CreateStatic(
							meGSegment_Stackframe,
							fextinfo->argumentssize
						)
					)
				);
				//mtGNamespace_Findsymbol_NameKind(
				//	name_space,
				//	"__xcc_variadic",
				//	mtGSymbol_eType_Pointer
				//)->allocatedstorage=(
				//	mtGTargetPointer_CreateStatic(
				//		meGSegment_Stackframe,
				//		fextinfo->argumentssize
				//	)
				//);
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
tSpNode* mtSpNode_Promote(tSpNode* self,tGType* type){
#ifdef qvGTrace
	printf("SP: [T] mtSpNode_Promote: entered\n");
#endif
	assert(mtGType_IsCastableto(self->returnedtype,type));
	if(mtGType_Equals(self->returnedtype,type))return self;
	return mtSpNode_Clone(
		&(tSpNode){
			.type=tSplexem_Cast,
			.returnedtype=mtGType_SetValuecategory(
				mtGType_Deepclone(type),
				eGValuecategory_Rightvalue
			),
			.left=SpInsertimpliedrvaluecast(
				self
			),
		}
	);
};
tSpNode* SpInsertbooleantointegercast(tSpNode* self){
	if(!self)return self;
	if(!self->returnedtype)return self;
	if(!mtGType_GetBasetype(self->returnedtype))return self;
	
	//if(self->returnedtype->atomicbasetype==eGAtomictype_Boolean){
		return mtSpNode_Clone(
			&(tSpNode){
				.type=tSplexem_Castfromboolean,
				.returnedtype=mtGType_Transform(
					mtGType_CreateAtomic(
						eGAtomictype_Boolean
					)
				),
				.left=self,
			}
		);
	//};
	return self;
};
tSpNode* SpInsertintegertobooleancast(tSpNode* self){
	if(!self)return self;
	if(!self->returnedtype)return self;
	if(!mtGType_GetBasetype(self->returnedtype))return self;
	
	if(self->returnedtype->atomicbasetype==eGAtomictype_Boolean){
		return mtSpNode_Clone(
			&(tSpNode){
				.type=tSplexem_Casttoboolean,
				.returnedtype=mtGType_Transform(
					mtGType_CreateAtomic(
						eGAtomictype_Boolean
					)
				),
				.left=self,
			}
		);
	};
	return self;
};
tSpNode* SpInsertimpliedrvaluecast(tSpNode* self){
	if(!self)return self;
	if(!self->returnedtype)return self;
	if(!mtGType_GetBasetype(self->returnedtype))return self;
	
	if(mtGType_GetBasetype(self->returnedtype)->valuecategory==eGValuecategory_Leftvalue){
		tGType* newtype = mtGType_Deepclone(self->returnedtype);
		mtGType_GetBasetype(newtype)->valuecategory=eGValuecategory_Rightvalue;
		if(newtype->atomicbasetype==eGAtomictype_Array){
			// lvalue T[] -> rvalue T*
			newtype->atomicbasetype=eGAtomictype_Pointer;
			mtGType_Transform(newtype);
			return mtSpNode_Clone(
				&(tSpNode){
					.type=tSplexem_Cast,
					.returnedtype=newtype,
					.left=self,
				}
			);
		}else if(newtype->atomicbasetype==eGAtomictype_Function){
			assert(false);
		}else if(
			  (newtype->atomicbasetype==eGAtomictype_Nearfunction)
			||(newtype->atomicbasetype==eGAtomictype_Farfunction)
		){
			return mtSpNode_Clone(
				&(tSpNode){
					.type=tSplexem_Cast,
					.returnedtype=mtGType_CreateNearpointer(newtype),
					.left=self,
				}
			);
		}else{
			// lvalue T -> dereference rvalue T
			return mtSpNode_Clone(
				&(tSpNode){
					.type=tSplexem_Dereference,
					.returnedtype=newtype,
					.left=self,
				}
			);
		};
	};
	return self;
};
tSpNode* SpiParsefunctionarguments(tSpNode* ast, tListnode /* <tGType> */ ** argumentslist){
#ifdef qvGTrace
	printf("SP: [T] SpiParsefunctionarguments: entered \n");
#endif
	switch(ast->type){
		case tSplexem_Comma:
			return mtSpNode_Clone(
				&(tSpNode){
					.type=tSplexem_Comma,
					.left=SpiParsefunctionarguments(ast->left,argumentslist),
					.right=SpiParsefunctionarguments(ast->right,argumentslist),
				}
			);
		default: {
			assert(argumentslist);
			assert(*argumentslist);
			tSpNode* i = mtSpNode_Promote(
				SpInsertimpliedrvaluecast(ast),
				(tGType*)((*argumentslist)->item)
			);
			*argumentslist = (*argumentslist)->next;
			return i;
		};
	}
};
tSpNode* SpParsefunctionarguments(tSpNode* ast, tListnode /* <tGType */ * argumentslist){
#ifdef qvGTrace
	printf("SP: [T] SpiParsefunctionarguments(%p,%p): entered \n",ast,argumentslist);
#endif
	assert(argumentslist);
	tListnode* args = argumentslist;
	return SpiParsefunctionarguments(ast,&args);
};
tSpNode* SpParse(tLxNode* self){ // Semantic parser primary driver
	if(!self){
		printf("SP: [E] SpParse: Nullpointer\n");
		return nullptr;
	};
	if(self->type==tLexem_Identifier){
		printf("SP: [T] Entered with node %i:%s:%s\n",self->type,TokenidtoName[self->type],self->identifier);
	}else{
		printf("SP: [T] Entered with node %i:%s\n",self->type,TokenidtoName[self->type]);
	}
	switch(self->type){
		{	// Driver
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
				// Get it's name
				char* name = nullptr;
				//node->returnedtype = SppGeneratetype(self->returnedtype,self->left,&name);
				mtGType_Destroy(SppGeneratetype(self->returnedtype,self->left,&name));
				// Bind symbol
				node->symbol=mtGNamespace_Findsymbol_NameKind(self->name_space,name,mtGSymbol_eType_Pointer);
				node->returnedtype = node->symbol->type;
				mtGType_GetBasetype(node->returnedtype)->valuecategory = eGValuecategory_Leftvalue;
				 // And insert compilernop inside
				 assert(node->symbol);
				 node->symbol->allocatedstorage=mtGTargetPointer_CreateDynamic(mtGInstruction_CreateCnop());
				// Compile function
				node->fextinfo = mtSpFunctionextinfo_Create();
				node->fextinfo->argumentssize = GTargetStackframeArgumentsstart;
				assert(self->left);
				tLxNode* functionbase = self->left;
				while(functionbase->left->type!=tLexem_Identifier){
					assert(functionbase);
					assert(functionbase->left);
					functionbase = functionbase->left;
				};
				assert(functionbase->type==tLexem_Functioncall);
				if(self->right)SpCompilefunctionarguments(
					functionbase->right,
					node->fextinfo,
					self->right?self->right->name_space:nullptr
				);
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
					tGType* type = mtGType_SetValuecategory(
						SppGeneratetype(
							self->returnedtype,
							self->left,
							&name
						),
						eGValuecategory_Leftvalue
					);

					tGSymbol* symbol=mtGNamespace_Findsymbol_NameKind(
						self->name_space,
						name,
						mtGSymbol_eType_Pointer
					);
					if(SpCurrentfunction){
						assert(symbol->symbolkind==mtGSymbol_eType_Pointer);
						symbol->allocatedstorage=SpAllocatelocalvarstorage(SpCurrentfunction->fextinfo,mtGType_Sizeof(type));
					}else{
						assert(symbol->symbolkind==mtGSymbol_eType_Pointer);
						symbol->allocatedstorage=SpAllocateglobalvarstorage(mtGType_Sizeof(type));
					};
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Variabledeclaration,
							.returnedtype=type,
							//.identifier=name,
							.symbol=symbol,
							.right=SpInsertimpliedrvaluecast(SpParse(self->right)),
						}
					);
				};
				//return nullptr;
				break;
			};
		{	// Statements
			case tLexem_Returnstatement: {
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Returnstatement,
						.left=mtSpNode_Promote(
							SpInsertimpliedrvaluecast(SpParse(self->left)),
							SpCurrentfunction->returnedtype->complexbasetype
						),
						.fextinfo=SpCurrentfunction->fextinfo
					}
				);
			};	break;
			case tLexem_Expressionstatement: {
				// TODO: Unallocate 'free-after-use' subexpressions
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Expressionstatement,
						.left=SpInsertimpliedrvaluecast(SpParse(self->left)),
					}
				);
			};	break;
			case tLexem_Ifstatement: {
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Ifstatement,
						.condition=SpInsertintegertobooleancast(
							SpInsertimpliedrvaluecast(
								SpParse(self->condition)
							)
						),
						.left=SpParse(self->left),
						.right=SpParse(self->right),
					}
				);
			};	break;
			case tLexem_Whilestatement: {
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Whilestatement,
						.condition=SpInsertintegertobooleancast(
							SpInsertimpliedrvaluecast(
								SpParse(self->condition)
							)
						),
						.left=SpParse(self->left),
						.right=nullptr,
					}
				);
			};	break;
			//(forloop init cond iter body)
			case tLexem_Forstatement: {
				ErfEnter_String("SpParse: Forloop");
				tSpNode* i = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Forstatement,                 //for(
						.initializer=SpParse(self->initializer),     //	initializer;
						.condition=SpInsertintegertobooleancast(     //	condition
							SpInsertimpliedrvaluecast(               //
								SpParse(self->condition)             //
							)                                        // 
						),                                           // ;
						.left=mtSpNode_Clone(                        // iterator
							&(tSpNode){                              //
								.type=tSplexem_Expressionstatement,  //
								.left=SpInsertimpliedrvaluecast(     //
									SpParse(self->left)              //
								),                                   //
							}                                        //
						),                                           // ;
						.right=SpParse(self->right),                 //)body;
					}
				);
				ErfLeave();
				return i;
			};	break;
		};
		{	// Expressions - terms
			case tLexem_Integerconstant:
				{
					tGType* type = mtGType_Transform(mtGType_CreateAtomic(eGAtomictype_Int));
					mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Rightvalue;
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=type,
							.constant=self->constant
						}
					);
				};
			case tLexem_Stringconstant:
				{
					tGType* type = mtGType_CreateArray(mtGType_CreateAtomic(eGAtomictype_Char));
					mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Leftvalue;
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Stringconstant,
							.returnedtype=type,
							.symbol=mtGSymbol_CreatePointer(
								"(anonymous)",
								type,
								mtGTargetPointer_CreateDynamic(
									GAddinstructiontosegment(
										meGSegment_Readonlydata,
										mtGInstruction_CreateStringdefinition(
											self->identifier
										)
									)
								)
							),
						}
					);
				};
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
		};
		{	// Expressions - special operators
			case tLexem_Arrayindex: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				tGType* returnedtype = mtGType_Deepclone(left->returnedtype);
				// rvalue T* -> lvalue T
				if(!mtGType_IsPointer(returnedtype)){
					printf("SP: [F] Indexing type <%s> failed - unable to convert to pointer\n",mtGType_ToString(returnedtype));
					GFatal();
				};
				returnedtype=returnedtype->complexbasetype;
				mtGType_GetBasetype(returnedtype)->valuecategory=eGValuecategory_Leftvalue;
				return mtSpNode_Clone( // cast<lvalue T>(self->left + sizeof(T)*self->right)
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=returnedtype,
						.left=mtSpNode_Clone(
							&(tSpNode){
								.type=tSplexem_Addition,
								.returnedtype=left->returnedtype,
								.left=left,
								.right=mtSpNode_Clone(
									&(tSpNode){
										.type=tSplexem_Multiplication,
										.returnedtype=mtGType_SetValuecategory(
											mtGType_CreatePointer(
												mtGType_CreateAtomic(
													eGAtomictype_Void
												)
											),
											eGValuecategory_Rightvalue
										),
										.left=mtSpNode_Clone(
											&(tSpNode){
												.type=tSplexem_Cast,
												.returnedtype=mtGType_SetValuecategory(
													mtGType_CreatePointer(
														mtGType_CreateAtomic(
															eGAtomictype_Void
														)
													),
													eGValuecategory_Rightvalue
												),
												.left=right,
											}
										),
										.right=mtSpNode_Clone(
											&(tSpNode){
												.type=tSplexem_Integerconstant,
												.returnedtype=mtGType_SetValuecategory(
													mtGType_CreatePointer(
														mtGType_CreateAtomic(
															eGAtomictype_Void
														)
													),
													eGValuecategory_Rightvalue
												),
												.constant=mtGType_Sizeof(left->returnedtype->complexbasetype),
											}
										),
									}
								),
							}
						),
					}
				);
			};	break;
			case tLexem_Structuremember: {
				tSpNode* left = SpParse(self->left);
				// Don't parse right yet!
				// ↓ TODO: Allow auto-cast to pointer to structure
				assert(left->returnedtype->atomicbasetype==eGAtomictype_Structure);
				assert(self->right->type==tLexem_Identifier);
				tGSymbol* symbol = mtGNamespace_Findsymbol_NameKind(
					left->returnedtype->structure,
					self->right->identifier,
					mtGSymbol_eType_Pointer
				);
				assert(symbol->allocatedstorage);
				//assert(symbol->allocatedstorage->nonconstant);
				if(
					  (!symbol->allocatedstorage->nonconstant) // constant
					&&(symbol->allocatedstorage->segment       // and relative
					   ==meGSegment_Relative)
				){
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Structuremember,
							.returnedtype=symbol->type,
							.left=left,
							.constant=symbol->allocatedstorage->offset
						}
					);
				}else{
					// Static structure member
					return mtSpNode_Clone( // Dirty hackery to make everything work
						&(tSpNode){
							.type=tSplexem_Identifier,
							.returnedtype=symbol->type,
							.symbol=symbol,
						}
					);
				};
			};	break;
			case tLexem_Dereference: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tGType* returnedtype = mtGType_Deepclone(left->returnedtype);
				// rvalue T* -> lvalue T
				assert(mtGType_IsPointer(returnedtype));
				returnedtype=returnedtype->complexbasetype;
				mtGType_GetBasetype(returnedtype)->valuecategory=eGValuecategory_Leftvalue;
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=returnedtype,
						.left=left,
					}
				);
			};
			case tLexem_Functioncall: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				if(left->returnedtype->atomicbasetype!=eGAtomictype_Nearpointer){
					printf("SP: [F] Unexcepted type when calling a function: %i instead of %i\n",
						left->returnedtype->atomicbasetype,
						eGAtomictype_Nearpointer
					);
					assert(false);
				}
				assert(mtGType_IsPointer(left->returnedtype));
				assert(mtGType_IsFunction(left->returnedtype->complexbasetype));
				tSpNode* right = SpParsefunctionarguments(
					SpParse(self->right),
					left->returnedtype->complexbasetype->functionarguments
				);
				
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Functioncall,
						.returnedtype=left->returnedtype->complexbasetype->complexbasetype,
						.left=left,
						.right=right,
					}
				);
			};	break;
			case tLexem_Comma: {
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Comma,
						.returnedtype=nullptr,
						.left=SpParse(self->left),
						.right=SpParse(self->right),
					}
				);
			};	break;
			case tLexem_Assign: {
				tSpNode* left = SpParse(self->left);
				tSpNode* right = mtSpNode_Promote(
					SpInsertimpliedrvaluecast(SpParse(self->right)),
					mtGType_SetValuecategory(
						mtGType_Clone(left->returnedtype),
						eGValuecategory_Rightvalue
					)
				);
				if(mtGType_GetBasetype(left->returnedtype)->valuecategory!=eGValuecategory_Leftvalue){
					printf("SP: [E] SpParse: `=`: Assignment to right value \n");
					GError();
					assert(false);
				};
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Assign,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_Clone(left->returnedtype),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
				assert(false);
			};
			case tLexem_Sizeof: {
				//
#ifdef qvGTrace
				//printf("SP: [T] SpParse: Sizeof(%s)\n",
				//	mtLxNode_ToString(self->left)
				//);
#endif
				assert(self->left);
				if(self->left->type==tLexem_Typeexpression){
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=mtGType_Transform(
								mtGType_SetValuecategory(
									mtGType_CreateAtomic(
										eGAtomictype_Sizet
									),
									eGValuecategory_Rightvalue
								)
							),
							.constant=mtGType_Sizeof(
								SppGeneratetype(
									self->left->returnedtype,
									self->left->left,
									nullptr
								)
							)
						}
					);
				}else{
					// Expression
#ifdef qvGTrace
					//printf("SP: [T] SpParse: Sizeof(%s)\n",
					//	mtGType_ToString(
					//		SpParse(
					//			self->left
					//		)->returnedtype
					//	)
					//);
#endif
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=mtGType_Transform(
								mtGType_SetValuecategory(
									mtGType_CreateAtomic(
										eGAtomictype_Sizet
									),
									eGValuecategory_Rightvalue
								)
							),
							.constant=mtGType_Sizeof(
								mtGType_SetValuecategory(
									SpParse(
										self->left
									)->returnedtype,
									eGValuecategory_Rightvalue
								)
							)
						}
					);
				};
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Integerconstant,
						.returnedtype=mtGType_Transform(
							mtGType_SetValuecategory(
								mtGType_CreateAtomic(
									eGAtomictype_Sizet
								),
								eGValuecategory_Rightvalue
							)
						),
						.constant=mtGType_Sizeof(
							mtGType_SetValuecategory(
								SppGeneratetype(
									self->left->returnedtype,
									self->left->left,
									nullptr
								),
								eGValuecategory_Rightvalue
							)
						)
					}
				);
			};
			case tLexem_Typecast: {
				assert(self->left->type == tLexem_Typeexpression);
				tGType* type = SppGeneratetype(
					self->left->returnedtype,
					self->left->left,
					null
				);
				tSpNode* right = SpParse(self->right);
				assert(mtGType_IsCastableto(right->returnedtype,type));
				//if(mtGType_Equals(self->returnedtype,type))return self;
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_Deepclone(type),
							eGValuecategory_Rightvalue
						),
						.left=SpInsertimpliedrvaluecast(
							SpParse(
								self->right
							)
						),
					}
				);
			};
		};
		{	// Expressions - arithmetic operators
			case tLexem_Add: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: Addition: Types not equal! \n");
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Addition,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
			};	break;
			case tLexem_Substract: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: Substraction: Types not equal! \n");
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Substraction,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
			};	break;
			case tLexem_Multiply: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `*`: Types not equal! %s•%s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Multiplication,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
			};	break;
			case tLexem_Divide: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `/`: Types not equal! \n");
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Division,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
			};	break;
			case tLexem_Modulo: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `%%`: Types not equal! %s•%s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Modulo,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
			};	break;
		};
		{	// Expressions - increment/decrement
			//TODO: Handle both lvalues and rvalues
			//@(Documentation snippet){
			//	++i
			//	Preincrement
			//	ld.	i32	i
			//	lea.	nearptr	i
			//	inc_modify.nearptr.i32	
			//	
			//	++1
			//	Rvalueincrement
			//	ld.	i32	i
			//	inc.	i32	i
			//	
			//	i++
			//	Postincrement
			//	lea.	nearptr	i
			//	inc_modify.nearptr.i32	
			//	ld.	i32	i
			//	
			//	1++
			//	(ignore)
			//}
			case tLexem_Postdecrement: {
				tSpNode* left = SpParse(self->left);
				if(mtGType_GetValuecategory(left->returnedtype)==eGValuecategory_Rightvalue){
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Postdecrement,
							.returnedtype=left->returnedtype,
							.left=left,
						}
					);
				}else if(mtGType_GetValuecategory(left->returnedtype)==eGValuecategory_Leftvalue){
					return mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Rvaluedecrement,
							.returnedtype=left->returnedtype,
							.left=left,
						}
					);
				}else{
					ErfFatal_String("SP: SpParse: Postdecrement: Unrecognized value category \n");
					assert(false);
				};
			};	break;
		};
		{	// Expressions - comparison
			case tLexem_Lessthan: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				left=SpInsertimpliedrvaluecast(left);
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				right=SpInsertimpliedrvaluecast(right);
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `<`: Types not equal! \n");
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				tSpNode* i = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Lessthan,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_CreateAtomic(eGAtomictype_Boolean),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
				return i;
			};	break;
			case tLexem_Equals: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				left=SpInsertimpliedrvaluecast(left);
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				right=SpInsertimpliedrvaluecast(right);
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: Equality: Types not equal! \n");
					return nullptr;
				};

				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Equality,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_CreateAtomic(eGAtomictype_Boolean),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
			};	break;
			case tLexem_Notequal: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				left=SpInsertimpliedrvaluecast(left);
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				right=SpInsertimpliedrvaluecast(right);
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: Equality: Types not equal! \n");
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Nonequality,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_CreateAtomic(eGAtomictype_Boolean),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
			};	break;
		};
		default:
			printf("SP: [E] SpParse: unrecognized node %i:%s\n",self->type,TokenidtoName[self->type]);
			GError();
			return nullptr;
	};
};

