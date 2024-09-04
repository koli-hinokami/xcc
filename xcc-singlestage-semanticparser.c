// Semantic(al) parser
tSpNode /* Function declaration */ * SpCurrentfunction;
tSpNode /* `switch();` */ * SpCurrentswitch;
tSpNode /* `for/switch;` */ * SpCurrentbreak;

// -------------------------- Forward declarations --------------------------
tSpNode* SpInsertimpliedrvaluecast(tSpNode* self);
tSpNode* SpParse(tLxNode* self);

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
			// TODO: General retargetability: Implement distinction between
			//                                cdecl/stdcall and pascal
			//                                subroutines
			SpCompilefunctionarguments(typeexpr->left,fextinfo,name_space);
			SpCompilefunctionarguments(typeexpr->right,fextinfo,name_space);
		}else{
			char* name;
			if(typeexpr->type==tLexem_Typeexpression){
				if(
					  typeexpr
					&&typeexpr->returnedtype
					&&typeexpr->returnedtype->atomicbasetype==eGAtomictype_Void
					&&(
						  typeexpr->left==nullptr
						||typeexpr->left->type==tLexem_Nullexpression
					)
				)
					return;
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
					mtGType_Sizeof(
						mtGType_SetValuecategory(
							mtGType_Deepclone(
								type
							),
							eGValuecategory_Rightvalue
						)
					)
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
		ErfError();
	};
};
tSpNode* mtSpNode_Promote(tSpNode* self,tGType* type){ // should've been called Cast
#ifdef qvGTrace
	printf("SP: [T] mtSpNode_Promote: entered\n");
#endif
	assert(self);
	assert(self);
	assert(mtGType_IsCastableto(self->returnedtype,type));
	if(mtGType_Equals(self->returnedtype,type))return self;
	if(self->type==tSplexem_Integerconstant)
		return mtSpNode_Clone(
			&(tSpNode){
				.type=tSplexem_Integerconstant,
				.constant=self->constant,
				.returnedtype=type
			}
		);
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
	
	assert(self->returnedtype->atomicbasetype==eGAtomictype_Boolean);
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
	
	if(self->returnedtype->atomicbasetype!=eGAtomictype_Boolean){
		return mtSpNode_Clone(
			&(tSpNode){
				.type=tSplexem_Casttoboolean,
				.returnedtype=mtGType_CreateAtomic(
					eGAtomictype_Boolean
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
			mtGType_SetValuecategory(newtype,eGValuecategory_Rightvalue);
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
					.returnedtype=mtGType_SetValuecategory(
						mtGType_CreateNearpointer(newtype),
						eGValuecategory_Rightvalue
					),
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
			tSpNode* i;
			if(*argumentslist==nullptr){
				printf("SP: [E] SpiParsefunctionarguments: "
				               "Not enough arguments to function\n");
				ErfError();
				// In case the error is suppressed, cast to `int`
				// TODO: Tuning: `intptr_t` may be better?
				i = mtSpNode_Promote(
					SpInsertimpliedrvaluecast(ast),
					mtGType_Transform(mtGType_CreateAtomic(eGAtomictype_Int))
				);
			};
			if(argumentslist[0]->item==nullptr){
				// Variadic arguments - no cast at all cuz the cast is on
				//  thee as the codewriter
				i = SpInsertimpliedrvaluecast(ast);
			}else{
				// Argument type is present
				i = mtSpNode_Promote(
					SpInsertimpliedrvaluecast(ast),
					(tGType*)((*argumentslist)->item)
				);
				*argumentslist = (*argumentslist)->next;
			};
			return i;
		};
	}
};
tSpNode* SpParsefunctionarguments(tSpNode* ast, tListnode /* <tGType */ * argumentslist){
#ifdef qvGTrace
	printf("SP: [T] SpiParsefunctionarguments(%p,%p): entered \n",ast,argumentslist);
#endif
	if(!argumentslist)return ast;
	tListnode* args = argumentslist;
	return SpiParsefunctionarguments(ast,&args);
};
void SpArithmeticpromotion(tSpNode* *left, tSpNode* *right){
	// If the types are the same, that type is the common type.
	if(mtGType_Equals(left[0]->returnedtype,right[0]->returnedtype))return;
	// { Here comes my tweak - when you add scalar to pointer,
	//   weird things happen:
	if(mtGType_IsPointer(left[0]->returnedtype)){
		*right=mtSpNode_Clone(
			&(tSpNode){ // mul<ptr> cast<ptr>right cast<ptr>sizeof(left[])
				.type=tSplexem_Multiplication,
				.returnedtype=mtGType_SetValuecategory( //ptr
					mtGType_Transform(
						mtGType_CreatePointer(
							mtGType_CreateAtomic(
								eGAtomictype_Void
							)
						)
					),
					eGValuecategory_Rightvalue
				),
				.left=mtSpNode_Clone( // cast<ptr> right
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_Transform(
								mtGType_CreatePointer(
									mtGType_CreateAtomic(
										eGAtomictype_Void
									)
								)
							),
							eGValuecategory_Rightvalue
						),
						.left=*right,
					}
				),
				.right=mtSpNode_Clone( // cast<ptr>sizeof(left[])
					&(tSpNode){
						.type=tSplexem_Integerconstant,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_Transform(
								mtGType_CreatePointer(
									mtGType_CreateAtomic(
										eGAtomictype_Void
									)
								)
							),
							eGValuecategory_Rightvalue
						),
						.constant=mtGType_Sizeof(
							left[0]->returnedtype->complexbasetype
						),
					}
				),
			}
		);
		return;
	};
	// }
	// Else, the types are different:
	{
		// . If the types have the same signedness (both signed or both 
		// | unsigned), the operand whose type has the lesser conversion 
		// ' rank1 is implicitly converted2 to the other type.
		// • Note that conversion rank here is pure size of type.
		// Else, the operands have different signedness:
		{
			// . If the unsigned type has conversion rank greater than or 
			// | equal to the rank of the signed type, then the operand 
			// | with the signed type is implicitly converted to the 
			// ' unsigned type.
			// . Else, the unsigned type has conversion rank less than 
			// ' the signed type: 
			{
				// . If the signed type can represent all values of the 
				// | unsigned type, then the operand with the unsigned 
				// ' type is implicitly converted to the signed type.
				// . Else, both operands undergo implicit conversion to 
				// | the unsigned type counterpart of the signed 
				// ' operand's type.
			}
		}
	}
	printf("SP: [W] SpArithmeticpromotion: Unimplemented code hit!\n");
	ErfWarning();
};
tSpNode* SpCompileinitializer(tGType* type, tLxNode* self){
	// Decide between scalar, array and structunion initialization
	if(mtGType_IsScalar(type)){
		// Scalar type initialization
		{	// Typeset cppreference page just for lulz
			// -
			// ___________________________________
			// \ C / C language / Initialization /
			//  '''''''''''''''''''''''''''''''''
			//   Scalar initialization
			// ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
			// 
			// When initializing an object of scalar type, the initializer must
			// be a single expression
			// 
			// The initializer for a scalar (an object of integer type including
			// booleans and enumerated types, floating type including complex 
			// and imaginary, and pointer type including pointer to function) 
			// must be a single expression, optionally enclosed in braces, or an
			// empty initializer(since C23):
			// ---------------------------------------
			//   = expression         (1)
			// ---------------------------------------
			//   = { expression }     (2)
			// ---------------------------------------
			//   = { }                (3)  (since C23)
			// ---------------------------------------
			// 1,2)  The expression is evaluated, and its value, after conversion
			//      as if by assignment to the type of the object, becomes the
			//      initial value of the object being initialized.
			//   3)  The object is empty-initialized, i.e. initialized to numeric
			//      zero for an object of an arithmetic or enumeration type, or 
			//      null pointer value for an object of a pointer type.
			//
			//   Notes
			// 
			// Because of the rules that apply to conversions as if by 
			// assignment, const and volatile qualifiers on the declared type 
			// are ignored when determining which type to convert the 
			// expression to.
			// 
			// See initialization for the rules that apply when no initializer
			// is used.
			// 
			// As with all other initializations, expression must be a constant
			// expression when initializing objects of static or thread-local
			// storage duration.
			// 
			// The expression cannot be a comma operator (unless parenthesized)
			// because the comma at the top level would be interpreted as the
			// beginning of the next declarator.
			// 
			// When initializing objects of floating-point type, all
			// computations for the objects with automatic storage duration are
			// done as-if at execution time and are affected by the current
			// rounding; floating-point errors are reported as specified in
			// math_errhandling. For objects of static and thread-local storage
			// duration, computations are done as-if at compile time, and no
			// exceptions are raised:
			// ┌────────────────────────────────────────────────────────────┐
			// │ void f(void)                                               │
			// │ {                                                          │
			// │ #pragma STDC FENV_ACCESS ON                                │
			// │     static float v = 1.1e75; // does not raise exceptions: │
			// │                              // static init                │
			// │                                                            │
			// │     float u[] = { 1.1e75 }; // raises FE_INEXACT           │
			// │     float w = 1.1e75;       // raises FE_INEXACT           │
			// │                                                            │
			// │     double x = 1.1e75; // may raise FE_INEXACT (depends    │
			// │                        // on FLT_EVAL_METHOD)              │
			// │     float y = 1.1e75f; // may raise FE_INEXACT (depends    │
			// │                        // on FLT_EVAL_METHOD)              │
			// │                                                            │
			// │     long double z = 1.1e75; // does not raise exceptions   │
			// │                             // (conversion is exact)       │
			// │ }                                                          │
			// └────────────────────────────────────────────────────────────┘
			// 
			//    Example
			// 
			// ┌────────────────────────────────────────────────────────────┐
			// │ #include <stdbool.h>                                       │
			// │ int main(void)                                             │
			// │ {                                                          │
			// │     bool b = true;                                         │
			// │     const double d = 3.14;                                 │
			// │     int k = 3.15; // conversion from double to int         │
			// │     int n = {12}, // optional braces                       │
			// │        *p = &n,   // non-constant expression OK for        │
			// │                   // automatic variable                    │
			// │        (*fp)(void) = main;                                 │
			// │     enum {RED, BLUE} e = RED; // enumerations are scalar   │
			// │                               // types as well             │
			// │ }                                                          │
			// └────────────────────────────────────────────────────────────┘
		};
		return mtSpNode_Promote(SpInsertimpliedrvaluecast(SpParse(self)),type);
	}else{
		// Unrecognized type category
		fprintf(
			stderr,
			"SP: [E] SpCompileinitializer: "
			"Initializing unrecognized type %i•%s (%s)\n",
			type->atomicbasetype,
			meGAtomictype_ToStringTable[type->atomicbasetype],
			mtGType_ToString(type)
		);
		fprintf(
			stdout,
			"SP: [E] SpCompileinitializer: "
			"Initializing unrecognized type %i•%s (%s)\n",
			type->atomicbasetype,
			meGAtomictype_ToStringTable[type->atomicbasetype],
			mtGType_ToString(type)
		);
		ErfError();
		return nullptr;
	};
};
tSpNode* SpParse(tLxNode* self){ // Semantic parser primary driver
	if(!self){
		printf("SP: [W] SpParse: Nullpointer\n");
		ErfWarning();
		return nullptr;
	};
	if(self->type==tLexem_Identifier){
		printf("SP: [T] Entered with node %i:%s:%s\n",self->type,TokenidtoName[self->type],self->identifier);
	}else{
		printf("SP: [T] Entered with node %i:%s\n",self->type,TokenidtoName[self->type]);
	}
	ErfEnter_String(
		mtString_Join(
			mtString_Join("SpParse: ",TokenidtoName[self->type]),
			 self->type==tLexem_Functiondeclaration
			?" (producing function name)"
			:(char[1]){0}
		)
	);
	ErfUpdate_String(
		mtString_Join(
			mtString_Join("SpParse: ",TokenidtoName[self->type]),
			 self->type==tLexem_Functiondeclaration
			?SppGeneratetype_GetName(self->left)
			:(char[1]){0}
		)
	);
	tSpNode* retval;
	switch(self->type){
		{	// Driver
			case tLexem_Declarationlist: {
				// Program root, pretty much
				ErfLeave();
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Declarationlist,
						.left=SpParse(self->left),
						.right=self->right?SpParse(self->right):nullptr
					}
				);
				return retval;
			};	break;
			case tLexem_Nulldeclaration: 
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Nulldeclaration
					}
				);
				ErfLeave();
				return retval;
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
				node->fextinfo->callingconvention = eGCallingconvention_Stdcall;
				node->fextinfo->argumentssize = 0;
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
				ErfLeave();
				return node;
			};	break;
			case tLexem_Blockstatement: {
				ErfLeave();
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Blockstatement,
						.left=self->left?SpParse(self->left):nullptr,
						.right=self->right?SpParse(self->right):nullptr,
					}
				);
				return retval;
			};	break;
			case tLexem_Typedefinition:
				// Typedefs are already handled in symbolgen
				ErfLeave();
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
					// Get type
					tGType* type = mtGType_SetValuecategory(
						SppGeneratetype(
							self->returnedtype,
							self->left,
							&name
						),
						eGValuecategory_Leftvalue
					);
					// Get symbol
					tGSymbol* symbol=mtGNamespace_Findsymbol_NameKind(
						self->name_space,
						name,
						mtGSymbol_eType_Pointer
					);
					// Update type
					type=symbol->type;
					// Allocate storage
					if(SpCurrentfunction){
						// Allocate local variable storage
						assert(symbol->symbolkind==mtGSymbol_eType_Pointer);
						symbol->allocatedstorage=SpAllocatelocalvarstorage(
							SpCurrentfunction->fextinfo,
							mtGType_Sizeof(
								mtGType_SetValuecategory(
									mtGType_Deepclone(
										symbol->type
									),
									eGValuecategory_Rightvalue
								)
							)
						);
					}else{
						// Allocating global variable storage is done in 
						// IR Generator
						symbol->allocatedstorage=nullptr;
						//assert(symbol->symbolkind==mtGSymbol_eType_Pointer);
						//symbol->allocatedstorage=SpAllocateglobalvarstorage(
						//	mtGType_Sizeof(
						//		mtGType_SetValuecategory(
						//			mtGType_Deepclone(
						//				symbol->type
						//			),
						//			eGValuecategory_Rightvalue
						//		)
						//	)
						//);
					};
					// Compile initializer
					tSpNode* right = 
						self->right
						?SpCompileinitializer(type, self->right)
						:nullptr;
					// Return node
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Variabledeclaration,
							.returnedtype=type,
							//.identifier=name,
							.symbol=symbol,
							.right=right,
						}
					);
					ErfLeave();
					return retval;
				};
				//return nullptr;
				break;
			case tLexem_Externaldeclaration:
				// -
				// . The thing is, I need to declare a symbol as external 
				// | reference but not definition of this symbol.
				// ' So ignoring it is!
				// Instruction unclear, ended up needing to do `.extern label` 
				// to request it. Will deal with it here.
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Externaldeclaration,
						.identifier=SppGeneratetype_GetName(
							self->left),
					}
				);
				ErfLeave();
				return retval;
				break;
		};
		{	// Statements
			case tLexem_Returnstatement: {
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Returnstatement,
						.left=mtSpNode_Promote(
							SpInsertimpliedrvaluecast(SpParse(self->left)),
							SpCurrentfunction->returnedtype->complexbasetype
						),
						.fextinfo=SpCurrentfunction->fextinfo
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Expressionstatement: {
				// TODO: Unallocate 'free-after-use' subexpressions
				//  '- Resolved: as a sideeffect, moved onto IR Generator.
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Expressionstatement,
						.left=SpInsertimpliedrvaluecast(SpParse(self->left)),
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Ifstatement: {
				retval = mtSpNode_Clone(
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
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Whilestatement: {
				retval = mtSpNode_Clone(
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
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Forstatement: { //(forloop init cond iter body)
				tSpNode* i = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Forstatement,                 //for(
						.initializer=SpParse(self->initializer),     //	initializer;
						.condition=                                  //	condition
								self->condition->type                //
							==	tLexem_Nullexpression                //
						?	SpParse(self->condition)                 //
						:	SpInsertintegertobooleancast(            //
								SpInsertimpliedrvaluecast(           //
									SpParse(self->condition)         //
								)                                    // 
							),                                       // ;
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
			case tLexem_Switchstatement: {
				tSpNode* prevswitch = SpCurrentswitch;
				tSpNode* prevbreak  = SpCurrentbreak;
				tSpNode* i = mtSpNode_Create();
				SpCurrentswitch = i;
				i->type = tSplexem_Switchstatement;
				i->switchlabels = mtList_Create();
				i->left = SpParse(self->left);
				i->right = SpParse(self->right);
				SpCurrentswitch = prevswitch;
				SpCurrentbreak  = prevbreak;
				ErfLeave();
				return i;
			};	break;
			case tLexem_Switchcase: {
				tSpNode* i = mtSpNode_Create();
				i->type = tSplexem_Switchcase;
				i->left = SpParse(self->left);
				mtList_Append((SpCurrentswitch->switchlabels),i);
				ErfLeave();
				return i;
			};	break;
			case tLexem_Switchdefault: {
				tSpNode* i = mtSpNode_Create();
				i->type = tSplexem_Switchdefault;
				mtList_Append((SpCurrentswitch->switchlabels),i);
				ErfLeave();
				return i;
			};	break;
			case tLexem_Breakstatement: {
				tSpNode* i = mtSpNode_Create();
				i->type = tSplexem_Breakstatement;
				i->initializer = SpCurrentbreak;
				ErfLeave();
				return i;
			};	break;
		};
		{	// Expressions - terms
			case tLexem_Nullpointer:
				{
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=mtGType_SetValuecategory(
								mtGType_Transform(
									mtGType_CreateAtomic(
										eGAtomictype_Pointer
									)
								),
								eGValuecategory_Rightvalue
							),
							.constant=0
						}
					);
					ErfLeave();
					return retval;
				};
			case tLexem_Booleantrue:
				{
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=mtGType_SetValuecategory(
								mtGType_Transform(
									mtGType_CreateAtomic(
										eGAtomictype_Boolean
									)
								),
								eGValuecategory_Rightvalue
							),
							.constant=1
						}
					);
					ErfLeave();
					return retval;
				};
			case tLexem_Booleanfalse:
				{
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=mtGType_SetValuecategory(
								mtGType_Transform(
									mtGType_CreateAtomic(
										eGAtomictype_Boolean
									)
								),
								eGValuecategory_Rightvalue
							),
							.constant=0
						}
					);
					ErfLeave();
					return retval;
				};
			case tLexem_Charaterconstant:
				{
					tGType* type = mtGType_Transform(
						mtGType_CreateAtomic(
							eGAtomictype_Char));
					mtGType_GetBasetype(type)->valuecategory =
						eGValuecategory_Rightvalue;
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=type,
							.constant=self->identifier[0]
						}
					);
					ErfLeave();
					return retval;
				};
			case tLexem_Integerconstant:
				{
					tGType* type = mtGType_Transform(self->returnedtype);
					mtGType_GetBasetype(type)->valuecategory =
						eGValuecategory_Rightvalue;
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Integerconstant,
							.returnedtype=type,
							.constant=self->constant
						}
					);
					ErfLeave();
					return retval;
				};
			case tLexem_Stringconstant:
				{
					tGType* type = mtGType_CreateArray(mtGType_CreateAtomic(eGAtomictype_Char));
					mtGType_GetBasetype(type)->valuecategory=eGValuecategory_Leftvalue;
					retval = mtSpNode_Clone(
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
					ErfLeave();
					return retval;
				};
			case tLexem_Nullexpression:
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Nullexpression,
						.returnedtype=mtGType_CreateAtomic(eGAtomictype_Void)
					}
				);
				ErfLeave();
				return retval;
			case tLexem_Identifier: 
				{
					tGSymbol* symbol = mtGNamespace_Findsymbol_NameKind(
						self->name_space,
						self->identifier,
						mtGSymbol_eType_Pointer
					);
					if(!symbol){
						fprintf(stderr,"SP: [E] SpParse: Undefined symbol \"%s\"\n",
							self->identifier);
						printf("SP: [E] SpParse: Undefined symbol \"%s\"\n",
							self->identifier);
						ErfError();
					};
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Symbol,
							.returnedtype=symbol->type,
							.symbol=symbol,
						}
					);
				};
				ErfLeave();
				return retval;
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
				if(right->type==tSplexem_Nullexpression){
					retval = mtSpNode_Clone( // cast<lvalue T>(self->left);
						&(tSpNode){
							.type=tSplexem_Cast,
							.returnedtype=returnedtype,
							.left=left
						}
					);
				}else{
					retval = mtSpNode_Clone( // cast<lvalue T>(self->left + sizeof(T)*self->right)
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
												mtGType_Transform(
													mtGType_CreatePointer(
														mtGType_CreateAtomic(
															eGAtomictype_Void
														)
													)
												),
												eGValuecategory_Rightvalue
											),
											.left=mtSpNode_Clone(
												&(tSpNode){
													.type=tSplexem_Cast,
													.returnedtype=mtGType_SetValuecategory(
														mtGType_Transform(
															mtGType_CreatePointer(
																mtGType_CreateAtomic(
																	eGAtomictype_Void
																)
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
														mtGType_Transform(
															mtGType_CreatePointer(
																mtGType_CreateAtomic(
																	eGAtomictype_Void
																)
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
				};
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Structuremember: {
				tSpNode* left = SpParse(self->left);
				// Don't parse right yet!
				// ↓ TODO: Allow auto-cast to pointer to structure
				assert(left->returnedtype->atomicbasetype==eGAtomictype_Structure);
				assert(self->right->type==tLexem_Identifier);
				assert(left->returnedtype->structure);
#ifdef qvGDebug
				if(!left->returnedtype->complexbasetype->structure){
					fprintf(stderr,"SP: [E] "
					               "SpParse: "
					               "tLexem_Memberbypointer: "
					               "Structure <%s> not compiled!\n",
					               mtGType_ToString(left->returnedtype)
					);
					ErfError();
					return nullptr;
				};
#endif
				tGSymbol* symbol = mtGNamespace_Findsymbol_NameKind(
					left->returnedtype->structure,
					self->right->identifier,
					mtGSymbol_eType_Pointer
				);
				if(!symbol){
					fprintf(stderr,"SP: [E] Unrecognized structure "
					               "<%s> member \"%s\"\n",
					               mtGType_ToString(left->returnedtype),
					               self->right->identifier
					               );
					ErfError();
					return nullptr;
				};
				assert(symbol->allocatedstorage);
				//assert(symbol->allocatedstorage->nonconstant);
				if(
					  (!symbol->allocatedstorage->nonconstant) // constant
					&&(symbol->allocatedstorage->segment       // and relative
					   ==meGSegment_Relative)
				){
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Structuremember,
							.returnedtype=symbol->type,
							.left=left,
							.constant=symbol->allocatedstorage->offset
						}
					);
				}else{
					// Static structure member
					retval = mtSpNode_Clone( // Dirty hackery to make everything work
						&(tSpNode){
							.type=tSplexem_Symbol,
							.returnedtype=symbol->type,
							.symbol=symbol,
						}
					);
				};
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Memberbypointer: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				// Don't parse right yet!
				// ↓ TODO: Allow auto-cast to structure (not pointer to struct)
				assert(  mtGType_GetValuecategory(left->returnedtype)
				       ==eGValuecategory_Rightvalue);
				assert(mtGType_IsPointer(left->returnedtype));
				assert(   left->returnedtype->complexbasetype->atomicbasetype
				       != eGAtomictype_Unresolved);
				assert(   left->returnedtype->complexbasetype->atomicbasetype
				       == eGAtomictype_Structure);
				assert(self->right->type==tLexem_Identifier);
#ifdef qvGDebug
				if(!left->returnedtype->complexbasetype->structure){
					fprintf(stderr,"SP: [E] "
					               "SpParse: "
					               "tLexem_Memberbypointer: "
					               "Structure <%s> not compiled!\n",
					               mtGType_ToString(left->returnedtype)
					);
					ErfError();
					return nullptr;
				};
#endif
				tGSymbol* symbol = mtGNamespace_Findsymbol_NameKind(
					left->returnedtype->complexbasetype->structure,
					self->right->identifier,
					mtGSymbol_eType_Pointer
				);
				if(!symbol){
					fprintf(stderr,"SP: [E] Unrecognized structure "
					               "<%s> member \"%s\"\n",
					               mtGType_ToString(left->returnedtype),
					               self->right->identifier
					               );
					ErfError();
					return nullptr;
				};
				assert(symbol->allocatedstorage);
				//assert(symbol->allocatedstorage->nonconstant);
				if(
					  (!symbol->allocatedstorage->nonconstant) // constant
					&&(symbol->allocatedstorage->segment       // and relative
					   ==meGSegment_Relative)
				){
					assert(    mtGType_GetValuecategory(left->returnedtype)
					         ==eGValuecategory_Rightvalue
						   &&  mtGType_IsPointer(left->returnedtype));
					assert(  symbol->type->valuecategory 
					       ==eGValuecategory_Leftvalue);
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Structuremember,
							.returnedtype=symbol->type,
							.left=left,
							.constant=symbol->allocatedstorage->offset
						}
					);
				}else{
					// Static structure member
					retval = mtSpNode_Clone( // Dirty hackery to make everything work
						&(tSpNode){
							.type=tSplexem_Symbol,
							.returnedtype=symbol->type,
							.symbol=symbol,
						}
					);
				};
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Addressof: {
				tSpNode* left = SpParse(self->left);
				tGType* returnedtype = mtGType_Deepclone(left->returnedtype);
				// lvalue T -> rvalue T*
				assert(
					mtGType_GetValuecategory(returnedtype)
					==eGValuecategory_Leftvalue
				);
				returnedtype=mtGType_Transform(
					mtGType_CreatePointer(returnedtype));
				mtGType_SetValuecategory(
					returnedtype,eGValuecategory_Rightvalue);
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=returnedtype,
						.left=left,
					}
				);
				ErfLeave();
				return retval;
			};
			case tLexem_Dereference: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tGType* returnedtype = mtGType_Deepclone(left->returnedtype);
				// rvalue T* -> lvalue T
				assert(mtGType_IsPointer(returnedtype));
				returnedtype=returnedtype->complexbasetype;
				mtGType_GetBasetype(returnedtype)->valuecategory=eGValuecategory_Leftvalue;
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=returnedtype,
						.left=left,
					}
				);
				ErfLeave();
				return retval;
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
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Functioncall,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_Deepclone(
								  left
								->returnedtype
								->complexbasetype
								->complexbasetype
							),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Comma: {
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Comma,
						.returnedtype=nullptr,
						.left=SpParse(self->left),
						.right=SpParse(self->right),
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Assign: {
				ErfUpdate_String("SpParse: tLexem_Assign: Left");
				tSpNode* left = SpParse(self->left);
				ErfUpdate_String("SpParse: tLexem_Assign: Right");
				tSpNode* right = mtSpNode_Promote(
					SpInsertimpliedrvaluecast(SpParse(self->right)),
					mtGType_SetValuecategory(
						mtGType_Clone(left->returnedtype),
						eGValuecategory_Rightvalue
					)
				);
				ErfUpdate_String("SpParse: tLexem_Assign: Postprocessing");
				if(mtGType_GetBasetype(left->returnedtype)->valuecategory!=eGValuecategory_Leftvalue){
					printf("SP: [E] SpParse: `=`: Assignment to right value \n");
					ErfError();
					GError();
					assert(false);
					return nullptr;
				};
				retval = mtSpNode_Clone(
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
				ErfLeave();
				return retval;
			};
			case tLexem_Sizeof: {
				//
#ifdef qvGTrace
				//printf("SP: [T] SpParse: Sizeof(%s)\n",
				//	mtLxNode_ToString(self->left)
				//);
#endif
				ErfUpdate_String("SpParse: tLexem_Sizeof: Type");
				assert(self->left);
				if(self->left->type==tLexem_Typeexpression){
					retval = mtSpNode_Clone(
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
					ErfLeave();
					return retval;
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
					ErfUpdate_String("SpParse: tLexem_Sizeof: Expression");
					retval = mtSpNode_Clone(
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
					ErfLeave();
					return retval;
				};
				assert(false);
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
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Cast,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_Deepclone(
								SppForceresolvetype(
									type,
									self->name_space
								)
							),
							eGValuecategory_Rightvalue
						),
						.left=SpInsertimpliedrvaluecast(
							SpParse(
								self->right
							)
						),
					}
				);
				ErfLeave();
				return retval;
			};
			case tLexem_Compoundliteral: {
				assert(self->left->type == tLexem_Typeexpression);
				tGType* type = SppForceresolvetype(
					SppGeneratetype(
						self->left->returnedtype,
						self->left->left,
						null
					),
					self->name_space
				);
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Compoundliteral,
						.returnedtype=type,
						.left=SpCompileinitializer(
							type,
							self->right
						),
					}
				);
				ErfLeave();
				return retval;
			};	break;
		};
		{	// Expressions - arithmetic operators
			case tLexem_Negation: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Negation,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=nullptr,
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Add: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				SpArithmeticpromotion(&left,&right);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: Addition: Types not equal! %s•%s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Addition,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
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
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Substraction,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
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
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Multiplication,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
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
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Division,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
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
					ErfError();
					ErfLeave();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Modulo,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Shiftleft: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = mtSpNode_Promote(
					SpInsertimpliedrvaluecast(SpParse(self->right)),
					mtGType_Transform(
						mtGType_CreateAtomic(
							eGAtomictype_Short
						)
					)
				);
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Shiftleft,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Shiftright: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = mtSpNode_Promote(
					SpInsertimpliedrvaluecast(SpParse(self->right)),
					mtGType_Transform(
						mtGType_CreateAtomic(
							eGAtomictype_Short
						)
					)
				);
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Shiftright,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Bitwiseand: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: Bitwiseand: Types not equal! %s•%s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					ErfError();
					ErfLeave();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Bitwiseand,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return retval;
			};	break;
			case tLexem_Logicalnot: {
				tSpNode* left = SpInsertintegertobooleancast(
					SpInsertimpliedrvaluecast(
						SpParse(self->left)));
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				retval = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Logicalnot,
						.returnedtype=left->returnedtype,
						.left=left,
						.right=nullptr,
					}
				);
				ErfLeave();
				return retval;
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
			case tLexem_Increment: {
				tSpNode* left = SpParse(self->left);
				if(mtGType_GetValuecategory(left->returnedtype)==eGValuecategory_Leftvalue){
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Preincrement,
							.returnedtype=left->returnedtype,
							.left=left,
						}
					);
					ErfLeave();
					return retval;
				}else{
					ErfFatal_String("SP: SpParse: Postincrement: Unrecognized"
					                "value category \n");
					assert(false);
					ErfLeave();
					return nullptr;
				};
			};	break;
			case tLexem_Postincrement: {
				tSpNode* left = SpParse(self->left);
				if(mtGType_GetValuecategory(left->returnedtype)==eGValuecategory_Leftvalue){
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Postincrement,
							.returnedtype=mtGType_SetValuecategory(
								mtGType_Deepclone(
									left->returnedtype
								),
								eGValuecategory_Rightvalue
							),
							.left=left,
						}
					);
					ErfLeave();
					return retval;
				}else if(mtGType_GetValuecategory(left->returnedtype)==eGValuecategory_Rightvalue){
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Rvalueincrement,
							.returnedtype=left->returnedtype,
							.left=left,
						}
					);
					ErfLeave();
					return retval;
				}else{
					ErfFatal_String("SP: SpParse: Postdecrement: Unrecognized"
					                "value category \n");
					assert(false);
					ErfLeave();
					return nullptr;
				};
			};	break;
			case tLexem_Postdecrement: {
				tSpNode* left = SpParse(self->left);
				if(  mtGType_GetValuecategory(left->returnedtype)
				   ==eGValuecategory_Leftvalue){
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Postdecrement,
							.returnedtype=left->returnedtype,
							.left=left,
						}
					);
					ErfLeave();
					return retval;
				}else if(  mtGType_GetValuecategory(left->returnedtype)
				         ==eGValuecategory_Rightvalue){
					retval = mtSpNode_Clone(
						&(tSpNode){
							.type=tSplexem_Rvaluedecrement,
							.returnedtype=left->returnedtype,
							.left=left,
						}
					);
					ErfLeave();
					return retval;
				}else{
					ErfFatal_String("SP: SpParse: Postdecrement: Unrecognized"
					                "value category \n");
					assert(false);
					ErfLeave();
					return nullptr;
				};
			};	break;
		};
		{	// Expressions - comparison
			case tLexem_Lessequal: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				left=SpInsertimpliedrvaluecast(left);
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				right=SpInsertimpliedrvaluecast(right);
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `≤`: Types not equal! %s : %s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				tSpNode* i = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Lessequal,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_CreateAtomic(eGAtomictype_Boolean),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return i;
			};	break;
			case tLexem_Greaterequal: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				left=SpInsertimpliedrvaluecast(left);
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				right=SpInsertimpliedrvaluecast(right);
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `≥`: Types not equal! %s : %s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				tSpNode* i = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Greaterequal,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_CreateAtomic(eGAtomictype_Boolean),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return i;
			};	break;
			case tLexem_Greaterthan: {
				tSpNode* left = SpInsertimpliedrvaluecast(SpParse(self->left));
				left=SpInsertimpliedrvaluecast(left);
				tSpNode* right = SpInsertimpliedrvaluecast(SpParse(self->right));
				right=SpInsertimpliedrvaluecast(right);
				
				//if(mtGType_Sizeof(left->returnedtype)<mtGType_Sizeof(right->returnedtype))
				//	left=mtSpNode_Promote(left,right->returnedtype);
				//if(mtGType_Sizeof(right->returnedtype)<mtGType_Sizeof(left->returnedtype))
				//	right=mtSpNode_Promote(right,left->returnedtype);
				if(!mtGType_Equals(left->returnedtype,right->returnedtype)){
					printf("SP: [E] SpParse: `>`: Types not equal! %s : %s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					ErfError();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				tSpNode* i = mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Greaterthan,
						.returnedtype=mtGType_SetValuecategory(
							mtGType_CreateAtomic(eGAtomictype_Boolean),
							eGValuecategory_Rightvalue
						),
						.left=left,
						.right=right,
					}
				);
				ErfLeave();
				return i;
			};	break;
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
					printf("SP: [E] SpParse: `<`: Types not equal! %s : %s\n",
						mtGType_ToString(left->returnedtype),
						mtGType_ToString(right->returnedtype)
					);
					ErfError();
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
				ErfLeave();
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
					ErfError();
					ErfLeave();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
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
				ErfLeave();
				return retval;
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
					ErfError();
					ErfLeave();
					return nullptr;
				};
				assert(mtGType_Sizeof(right->returnedtype)==mtGType_Sizeof(left->returnedtype));
				retval = mtSpNode_Clone(
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
				ErfLeave();
				return retval;
			};	break;
		};
		default:
			printf("SP: [E] SpParse: unrecognized node %i:%s\n",self->type,TokenidtoName[self->type]);
			GError();
			return nullptr;
	};
};
tSpNode* SpOptimize(tSpNode* self){ // Semanticoptimizer
	assert(self);
	{	// Init
		
	};
	{	// Recurse
		if(self->initializer) self->initializer = SpOptimize(self->initializer);
		if(self->condition)   self->condition   = SpOptimize(self->condition);
		if(self->left)        self->left        = SpOptimize(self->left);
		if(self->right)       self->right       = SpOptimize(self->right);
	};
	{	// Negating constant
		if(
			  self->type==tSplexem_Negation
			&&self->left->type==tSplexem_Integerconstant
		){
			self=self->left;
			self->constant=-self->constant;
		};
	};
	{	// Multiply by 1
		if(
			  self->type==tSplexem_Multiplication
			&&self->right->type==tSplexem_Integerconstant
			&&self->right->constant==1
		){
			self=self->left;
		};
	};
	{	// Multiply constant by constant
		if(
			  self->type==tSplexem_Multiplication
			&&self->left->type==tSplexem_Integerconstant
			&&self->right->type==tSplexem_Integerconstant
		){
			self->left->constant*=self->right->constant;
			self=self->left;
		};
	};
	{	// Greater than constant -> Greaterequal to constant
		if(
			  self->type==tSplexem_Greaterthan
			&&self->right->type==tSplexem_Integerconstant
		){
			self->type=tSplexem_Greaterequal;
			self->left->constant+=1;
		};
	};
	{	// Expressionstatement increment transform - i++ -> ++i
		if(
			  self->type==tSplexem_Expressionstatement
			&&self->left->type==tSplexem_Postincrement
		){
			self->left->type=tSplexem_Preincrement;
		};
	};
	{	// Default - pass node through while recursing (again)
		//if(self->initializer) self->initializer = SpOptimize(self->initializer);
		//if(self->condition)   self->condition   = SpOptimize(self->condition);
		//if(self->left)        self->left        = SpOptimize(self->left);
		//if(self->right)       self->right       = SpOptimize(self->right);
		return self;
	};
};
