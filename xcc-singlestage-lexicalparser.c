// ---------------------- Globals ---------------------
tLxNode* /* <nullptr||.type=switch> */ LxCurrentswitch;
tLxNode* /* <nullptr||.type=statement> */ LxCurrentbreak;
// ------------------ Lexical parsing ------------------
tLxNode* mtLxNode_Create(void){
	return calloc(sizeof(tLxNode),1);
};
tLxNode* mtLxNode_Clone(tLxNode* self){
	return memcpy(malloc(sizeof(tLxNode)),self,sizeof(tLxNode));
};
// ----------------- The Lexer -----------------
tLxNode* LxParseLexemtable(){};// todo
// note that lexem tables were abandoned
tLxNode* LxParseDeclarationorstatement(tLxFetcher* fetcher);
tLxNode* LxParseDeclaration(tLxFetcher* fetcher);
tGType* LxParseBasetype(tLxFetcher* fetcher);
tLxNode* LxParseExpression(tLxFetcher*);
tLxNode* LxParseBlockstatement(tLxFetcher* fetcher);

tLxNode* LxParseDeclarationorstatement(tLxFetcher* fetcher){
	tLxNode* temp;
	tLxFetcher localfetcher = *fetcher;
	if(temp=LxParseExpression(fetcher)){
		return temp;
	};
};
tLxNode* LxParseStatement(tLxFetcher* fetcher){ // Consumes semicolon
#ifdef qvGTrace
	printf("LX: [T] LxParseStatement: entered \n");
	mtLxFetcher_Print_Limited(fetcher);
#endif
	// Skip semicolons
	if(0)while(mtLxFetcher_Peek(fetcher)->type==tToken_Semicolon){
		mtLxFetcher_Advance(fetcher); 
	};
	/* // Notes
	 * There's plenty of statement types in C:
	 * ¤ Local variable declaration
	 * ¤ Expression statement
	 * Instead of having statements for assignment, increment, function call 
	 * and so on like some do C just has one for expression.
	 */
	// Blockstatement bypass
	if(mtLxFetcher_Peek(fetcher)->type==tToken_Opencurlybraces){
		return LxParseBlockstatement(fetcher);
	}
	// Fetch to semicolon the statement boundary
	tLxFetcher localfetcher = *fetcher;
	tLxFetcher* localfetcher2 = nullptr; // pointer
	//mtLxFetcher_Advance(fetcher); 
	// Parse everything
	tGType* basetype;
	tLxNode* declaration;
	//if(basetype=LxParseBasetype(&localfetcher)){
	//	// Declaration actually
	//	return LxParseDeclaration(fetcher);
	
	//if(declaration=LxParseDeclaration(fetcher)){
	//	return declaration;
	//}else
	{
		//mtLxFetcher_Print(localfetcher);
		// Not declaration - either one of C's statements or expression statement
		switch(mtLxFetcher_Peek(fetcher)->type){
			case tToken_Opencurlybraces:
				// A blockstatement!
				return LxParseBlockstatement(fetcher);
				break;
			case tToken_Keywordreturn:
				mtLxFetcher_Advance(fetcher);
				localfetcher = *mtLxFetcher_FetchuntilParenthesized(
					fetcher,tToken_Semicolon
				);
				mtLxFetcher_Advance(fetcher); // skip semicolon
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Returnstatement,
						.left=LxParseExpression(&localfetcher)
					}
				);
				break;
			case tToken_Keywordif:
				printf("LX: [D] LxParseStatement: `if` seen \n");
				mtLxFetcher_Advance(fetcher);
				//mtLxFetcher_Print(fetcher);
				if(mtLxFetcher_Fetch(fetcher)->type==tToken_Openparentheses){
					tLxNode* cond = LxParseExpression(
						mtLxFetcher_FetchuntilParenthesized(
							fetcher,
							tToken_Closeparentheses
						)
					);
					mtLxFetcher_Advance(fetcher); // skip `)`
					tLxNode* left = LxParseStatement(fetcher);
					tLxNode* right = nullptr;
					if(mtLxFetcher_Peek(fetcher)->type==tToken_Keywordelse){
						mtLxFetcher_Advance(fetcher); // skip `else`
						right = LxParseStatement(fetcher);
					};
					//mtLxFetcher_Advance(localfetcher); //skip semicolon?
					//mtLxFetcher_Print(localfetcher);
					return mtLxNode_Clone(
						&(tLxNode){
							.type=tLexem_Ifstatement,
							.condition = cond,
							.left = left,
							.right = right
						}
					);
				}else{
					printf("LX: [F] LxParseStatement: Broken conditional \n");
				};
				break;
			case tToken_Keywordwhile:
				mtLxFetcher_Advance(fetcher);
				//mtLxFetcher_Print(fetcher);
				if(mtLxFetcher_Fetch(fetcher)->type==tToken_Openparentheses){
					tLxNode* cond = LxParseExpression(
						mtLxFetcher_FetchuntilParenthesized(
							fetcher,
							tToken_Closeparentheses
						)
					);
					mtLxFetcher_Advance(fetcher); // skip `)`
					tLxNode* left = LxParseStatement(fetcher);
					//mtLxFetcher_Advance(localfetcher);
					//mtLxFetcher_Print(localfetcher);
					return mtLxNode_Clone(
						&(tLxNode){
							.type=tLexem_Whilestatement,
							.condition = cond,
							.left=left
						}
					);
				}else{
					printf("LX: [F] LxParseStatement: Broken while(); \n");
				};
				break;
			case tToken_Keywordfor:
				printf("LX: [D] LxParseStatement: Forloop seen \n");
				mtLxFetcher_Advance(fetcher);
				//mtLxFetcher_Print(fetcher);
				if(mtLxFetcher_Fetch(fetcher)->type==tToken_Openparentheses){
					tLxFetcher* forblock = mtLxFetcher_FetchuntilParenthesized(
						fetcher,
						tToken_Closeparentheses
					);
					mtLxFetcher_Advance(fetcher); // skip `)`
					tLxNode* initializer = LxParseStatement(forblock);
					mtLxFetcher_Print(forblock);
					//exit(5);
					//mtLxFetcher_Advance(forblock); // skip `;`
					tLxNode* cond = LxParseExpression(
						mtLxFetcher_FetchuntilParenthesized(
							forblock,
							tToken_Semicolon
						)
					);
					mtLxFetcher_Advance(forblock); // skip semicolon
					tLxNode* iterator = LxParseExpression(
						forblock
					);
					tLxNode* body = LxParseStatement(fetcher);
					//mtLxFetcher_Advance(localfetcher);
					//mtLxFetcher_Print(localfetcher);
					return mtLxNode_Clone(
						&(tLxNode){
							.type=tLexem_Forstatement,
							.initializer = initializer,
							.condition = cond,
							.left = iterator,
							.right = body
						}
					);
				}else{
					printf("LX: [F] LxParseStatement: Broken for(;;); \n");
				};
				break;
			case tToken_Keywordswitch: {
				tLxNode* LxPreviousswitch = LxCurrentswitch;
				tLxNode* node = mtLxNode_Create();
				LxCurrentswitch = node;

				mtLxFetcher_Advance(fetcher); // skip `switch`
				//mtLxFetcher_Print(fetcher);
				if(mtLxFetcher_Fetch(fetcher)->type==tToken_Openparentheses){
					tLxNode* cond = LxParseExpression(
						mtLxFetcher_FetchuntilParenthesized(
							fetcher,
							tToken_Closeparentheses
						)
					);
					mtLxFetcher_Advance(fetcher); // skip `)`
					tLxNode* left = LxParseStatement(fetcher);
					//mtLxFetcher_Advance(localfetcher);
					//mtLxFetcher_Print(localfetcher);
					
					node->type=tLexem_Switchstatement;
					node->condition = cond;
					node->left=left;
				}else{
					printf("LX: [F] LxParseStatement: Broken switch(); \n");
					node=nullptr;
				};
				LxCurrentswitch = LxPreviousswitch;
				return node;
			};	break;
			case tToken_Keywordcase: {
#ifdef qvGDebug
				printf("LX: [T] LxParseStatement: `case` \n");
#endif
				mtLxFetcher_Advance(fetcher); // skip `case`
				tLxFetcher* localfetcher = mtLxFetcher_FetchuntilParenthesized(fetcher,tToken_Colon);
				mtLxFetcher_Advance(fetcher); // skip `:`
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Switchcase,
						.initializer = LxCurrentswitch,
						.left=LxParseExpression(localfetcher)
					}
				);
			};	break;
			case tToken_Keywordbreak: {
#ifdef qvGDebug
				printf("LX: [T] LxParseStatement: `break` \n");
#endif
				mtLxFetcher_Advance(fetcher); // skip `break`
				mtLxFetcher_Advance(fetcher); // skip `;`
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Switchcase,
						.initializer = LxCurrentbreak
					}
				);
			};	break;
			case tToken_Keyworddefault: {
#ifdef qvGDebug
				printf("LX: [T] LxParseStatement: `default` \n");
#endif
				mtLxFetcher_Advance(fetcher); // skip `default`
				mtLxFetcher_Advance(fetcher); // skip `:`
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Switchdefault,
						.initializer = LxCurrentswitch
					}
				);
			};	break;
			default:
				//if(declaration=LxParseDeclaration(fetcher)){
				//	return declaration;
				//}else
				{
#ifdef qvGDebug
					printf("LX: [T] LxParseStatement: Expression statement \n");
#endif
					// Expression statement
					localfetcher = *mtLxFetcher_FetchuntilParenthesized(
						fetcher,tToken_Semicolon
					);
					mtLxFetcher_Advance(fetcher); // skip semicolon
					return mtLxNode_Clone(
						&(tLxNode){
							.type=tLexem_Expressionstatement,
							.left=LxParseExpression(&localfetcher)
						}
					);
				};
				break;
		}
	};
	return nullptr;
};
tLxNode* LxParseBlockstatement2(tLxFetcher* fetcher){
#ifdef qvGDebug
	printf("LX: [D] LxParseBlockstatement2: printing fetcher \n");
	mtLxFetcher_Print_Limited(fetcher);
#endif
	switch(mtLxFetcher_Peek(fetcher)->type){
		case tToken_Closecurlybraces: {
			mtLxFetcher_Advance(fetcher); 
			return nullptr;
		};	break;
		case tToken_Opencurlybraces: {
			// tLxNode* st1=LxParseBlockstatement(fetcher);
			mtLxFetcher_Advance(fetcher); // skip `{`
			tLxNode* st1=LxParseBlockstatement2(fetcher);
			tLxNode* st2=LxParseBlockstatement2(fetcher);
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Blockstatement,
					.left=st1,
					.right=st2
				}
			);
		};	break;
		default: {
			tLxNode* st1=LxParseStatement(fetcher);
			tLxNode* st2=LxParseBlockstatement2(fetcher);
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Blockstatement,
					.left=st1,
					.right=st2
				}
			);
		};	break; 
	};
};
tLxNode* LxParseBlockstatement(tLxFetcher* fetcher){
	if(mtLxFetcher_Peek(fetcher)->type!=tToken_Opencurlybraces){
		fprintf(stderr,"LX: [E] LxParseBlockstatement: Bad usage - fetcher points to token after `{` \n");
	}else{
		mtLxFetcher_Advance(fetcher);
	};
	return LxParseBlockstatement2(fetcher);
};

tGType* LxParseBasetype(tLxFetcher* fetcher){
	tGType* type = malloc(sizeof(tGType));
	// Clear structure
	type->atomicbasetype = null;
	type->complexbasetype = nullptr;
	type->valuecategory = null;
	//type->pointerness = nullptr;
	type->typequalifiers = null;
	type->unresolvedsymbol = null;
	type->templatemodifiers = nullptr;
	tToken* token = nullptr;
	// Checking for types
	if((token=mtLxFetcher_Peek(fetcher))->type==tToken_Keywordconst){
		mtLxFetcher_Advance(fetcher);
		type->typequalifiers|=eGTypequalifiers_Const;
	};
	if((token=mtLxFetcher_Peek(fetcher))->type==tToken_Keywordvolatile){
		mtLxFetcher_Advance(fetcher);
		type->typequalifiers|=eGTypequalifiers_Volatile;
	};
	if((token=mtLxFetcher_Peek(fetcher))->type==tToken_Keywordrestrict){
		mtLxFetcher_Advance(fetcher);
		type->typequalifiers|=eGTypequalifiers_Restrict;
	};
	switch((token=mtLxFetcher_Peek(fetcher))->type){
		case tToken_Keywordunsigned:
			mtLxFetcher_Advance(fetcher);
			switch((token=mtLxFetcher_Peek(fetcher))->type){
				case tToken_Keywordchar:
					mtLxFetcher_Advance(fetcher);
					type->atomicbasetype = eGAtomictype_Unsignedshort;
					break;
				case tToken_Keywordshort:
					mtLxFetcher_Advance(fetcher);
					type->atomicbasetype = eGAtomictype_Unsignedshort;
					break;
				case tToken_Keywordint:
					mtLxFetcher_Advance(fetcher);
					type->atomicbasetype = eGAtomictype_Unsigned;
					break;
				case tToken_Keywordlong:
					mtLxFetcher_Advance(fetcher);
					switch((token=mtLxFetcher_Peek(fetcher))->type){
						case tToken_Keywordlong:
							mtLxFetcher_Advance(fetcher);
							type->atomicbasetype = eGAtomictype_Unsignedlonglong;
							break;
						default:
							type->atomicbasetype = eGAtomictype_Unsignedlong;
							break;
					};
					type->atomicbasetype = eGAtomictype_Unsignedshort;
					break;
				default:
					type->atomicbasetype = eGAtomictype_Unsigned;
					break;
			};
		case tToken_Keywordint:
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Int;
			break;
		case tToken_Keywordshort:
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Short;
			break;
		case tToken_Keywordchar:
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Char;
			break;
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Long;
			break;
		case tToken_Keywordbool:
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Boolean;
			break;
		case tToken_Keywordvoid:
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Void;
			break;
		case tToken_Keywordstruct: {
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Structure;
			printf("LX: [T] LxParseBasetype: Parsing structure\n");
			// Try to recognize optional identifier
			char* identifier = nullptr;
			if(mtLxFetcher_Peek(fetcher)->type==tToken_Identifier){
				identifier=mtLxFetcher_Peek(fetcher)->string;
				mtLxFetcher_Advance(fetcher);
			};
			// Try to recognize definition of fields
			tList* declarations = mtList_Create();
			if(mtLxFetcher_Peek(fetcher)->type==tToken_Opencurlybraces){
				mtLxFetcher_Advance(fetcher);
				while(mtLxFetcher_Peek(fetcher)->type!=tToken_Closecurlybraces){
					mtList_Append(declarations,LxParseDeclaration(fetcher));
				};
				mtLxFetcher_Advance(fetcher);
			};
			// Yell at user if both identifier and fields destcription are absent
			if((identifier==nullptr) && (mtList_Count(declarations)==0)){
				fprintf(stderr,"LX: [F] LxParseBasetype: `struct` w/o both identifier and fields declaration \n");
				exit(2);
			};
			// Assemble field
			type->precompiledstructure = declarations;
			type->unresolvedsymbol = identifier;
		};	break;
		case tToken_Keywordenum: {
			mtLxFetcher_Advance(fetcher);
			type->atomicbasetype = eGAtomictype_Enumeration;
			printf("LX: [T] LxParseBasetype: Parsing enumeration \n");
			// Try to recognize optional identifier
			char* identifier = nullptr;
			if(mtLxFetcher_Peek(fetcher)->type==tToken_Identifier){
				identifier=mtLxFetcher_Peek(fetcher)->string;
				mtLxFetcher_Advance(fetcher);
			};
			// Try to recognize definition of fields
			tLxFetcher* enumdefinition;
			type->precompiledenumeration = nullptr;
			if(mtLxFetcher_Peek(fetcher)->type==tToken_Opencurlybraces){
				mtLxFetcher_Advance(fetcher); // skip `{`
				enumdefinition = mtLxFetcher_FetchuntilParenthesized(fetcher,tToken_Closecurlybraces);
				mtLxFetcher_Advance(fetcher); // skip `}`
				type->precompiledenumeration = LxParseExpression(enumdefinition);
			};
			// Yell at user if both identifier and fields destcription are absent
			if((identifier==nullptr) && (type->precompiledenumeration == nullptr)){
				fprintf(stderr,"LX: [F] LxParseBasetype: `enum` w/o both identifier and fields declaration \n");
				exit(2);
			};
			// Assemble field
			type->unresolvedsymbol = identifier;
		};	break;
	
		case tToken_Identifier:
			type->atomicbasetype = eGAtomictype_Unresolved;
			type->unresolvedsymbol = mtLxFetcher_Peek(fetcher)->string;
			mtLxFetcher_Advance(fetcher);
			//fetcher->fetchfrom=fetcher->fetchfrom->next;
			break;
		default:
			printf("LX: [F] LxParseBasetype: Unexcepted token %i:%s \n",token->type,TokenidtoName[token->type]);
			return nullptr;
	};
	return type;
};
tLxNode* LxParseType(tLxFetcher* fetcher){
#ifdef qvGTrace
	printf("LX: [T] LxParseType(): Entered\n");
#endif
	tGType* localtype = LxParseBasetype(fetcher);
	if(localtype){
		tLxNode* expr = LxParseExpression(fetcher);
		if(expr==nullptr)return nullptr;
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Typeexpression,
				.returnedtype=localtype,
				.left=expr
			}
		);
	}else{
		return nullptr;
	};
};
//		C Operators
//	A modified C++ chart.
//	P r e c e d e n c e   i s   n o n - s t a n d a r d . 
//	Specifically you can't use bitwise and/or as logical ones.
//	 This is not K&R C, please use logical operators.
//
//				Associativity
//  Precedence 	opr.	Description 	
//
// 				Left-to-right →
// 	0 	:: 	Scope resolution 	
// 	1 	++ -- 	Suffix/postfix increment and decrement 	
// 		() 	Function call
// 		[] 	Array subscripting
// 		. 	Structure and union member access
// 		-> 	Structure and union member access through pointer
// 		(){} 	Compound literal from our beloved C99
// 				Right-to-left ←
// 	2 	++ --	Prefix increment and decrement
// 		+ - 	Unary plus and minus
// 		! ~	Logical NOT and bitwise NOT
// 		() 	Type cast
// 		* 	Indirection (dereference)
// 		& 	Address-of
// 		sizeof 	Size-of[note 1]
// 			There should've been `alignof`, but there's not much
// 			 reason for alignment on 8-bit machines, so...
// 				Left-to-right →
// 	2.5 	.* ->*	Pointer-to-member 	
// 	3 	* / % 	Multiplication, division, and remainder
// 	4 	+ - 	Addition and subtraction
// 	5 	<< >> 	Bitwise left shift and right shift
// 	6  	&	Bitwise AND
// 	7  	^ 	Bitwise XOR (exclusive or)
// 	8  	| 	Bitwise OR (inclusive or)
// 	8.5 	<=>	Three-way comparison operator (why though)
// 	9	< <=    Comparison
// 		> >=
// 	10 	== !=	More comparison
// 	11 	&& 	Logical AND
// 	12 	|| 	Logical OR
// 				Right-to-left ←
// 	13 	?:	Ternary conditional
// 	14	= 	Simple assignment
// 		+= -= 	Assignment by sum and difference
// 		*= /=  	Assignment by product, quotient, and remainder
// 		%=
// 		<<=  	Assignment by bitwise left shift and right shift
// 		>>=
// 		&= ^=  	Assignment by bitwise AND, XOR, and OR
// 		|=		
//				Left-to-right → 
// 	15 	, 	Comma 	
bool LxiParseExpression_Checkparenthesation(tLxFetcher* fetcher){
	int parenthesation = 0;
	for(
		tListnode /* <tToken> */ * tempptr = fetcher->fetchfrom;
		tempptr!=fetcher->fetchto;
		tempptr=tempptr->next
	){
		printf("LX: [T] LxiParseExpression_Checkparenthesation: %i:%s \n",((tToken*)tempptr->item)->type,TokenidtoName[((tToken*)tempptr->item)->type]); 
		switch(((tToken*)tempptr->item)->type){
			case tToken_Openbrackets:
			case tToken_Opencurlybraces:
			case tToken_Openparentheses:
				parenthesation++;
				break;
			case tToken_Closebrackets:
			case tToken_Closecurlybraces:
			case tToken_Closeparentheses:
				parenthesation--;
				break;
			default:
				break;
		};
		if(parenthesation<0){
			printf("LX: [T] LxiParseExpression_Checkparenthesation: Broken parenthesation - a good thing actually \n");
			return false; // We brokey...
		};
	};
	if(parenthesation>0){
		return false;
	};
	return true;
};
tLxNode* LxParseExpression(tLxFetcher* fetcher){
#ifdef qvGTrace
	printf("LX: [T] LxParseExpression: entered \n");
	mtLxFetcher_Print(fetcher);
#endif
	tListnode* splitpoint = nullptr;
	int parenthesation = 0;
	/* // Explanation details
	 * 'My own' parsing technique that may as well have been implemented
	 * somewhere else already, so mine in a sense I came up with it and 
	 * not just took from Wikipedia or whereever.
	 * . 
	 * | foreach<order=ascending>(i in precedences){
	 * |   foreach(operator in operators[i].operatorblock){
	 * |     foreach<order=reverse(operators[i].precedence)>(token in fetcher.tokenlist){
	 * |       if(token==operator.token){
	 * |         return new Node(
	 * |           .type=operator.lexem,
	 * |           .left=recurse(
	 * |             fetcher.split(
	 * |               .start=fetcher.start,
	 * |               .end=token.location
	 * |             )
	 * |           ),
	 * |           .right=recurse(
	 * |             fetcher.split(
	 * |               .start=token.location.next,
	 * |               .end=fetcher.end
	 * |             )
	 * |           )
	 * |         )
	 * |       }
	 * |     }
	 * |   }
	 * | }
	 * ' 
	 *
	 *  For those whose eyes bleed from reading someone else's code like mine ones,
	 * it is: {
	 *   trim parentheses at start and end
	 *     if not matching exit(1)
	 *     // not present in C pseudocode
	 *   for each precedence * l o w e s t - t o - h i g h e s t *
	 *     // which is actually descending precedence number if look at the charts
	 *     //so + goes before * and ^
	 *   in order of * r e v e r s e * precedence 
	 *     search for possible operators
	 *     when found
	 *       split token sequence at the place
	 *       recusively parse left and right
	 *       merge it all and return as tNode
	 * }
	 *
	 */
	// Null statement
	if(fetcher->fetchfrom==fetcher->fetchto){
		printf("LX: [D] LxParseExpression: Null statement seen \n");
		// Null statement special code
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Nullexpression,
				.returnedtype=mtGType_Clone(
					&(tGType){
						.atomicbasetype=eGAtomictype_Void
					}
				),
				.initializer=nullptr,
				.condition=nullptr,
				.left=nullptr,
				.right=nullptr
			}
		);
		return nullptr;
	};
	// Unparenthesize
	if(mtLxFetcher_Peek(fetcher)->type==tToken_Openparentheses){ 
#ifdef qvGTrace
		printf("LX: [T] LxParseExpression: Unparenthesize\n");
#endif
		if(mtLxFetcher_Peeklast(fetcher)->type==tToken_Closeparentheses){
			// Parenthesized expression
			printf("LX: [T] LxParseExpression: Parenthesized expression \n");
			tLxFetcher localfetcher = *fetcher;
			localfetcher.fetchfrom=localfetcher.fetchfrom->next;
			// Trim ending parentheses
			for(
				splitpoint = localfetcher.fetchfrom;
				splitpoint->next != localfetcher.fetchto;
				splitpoint = splitpoint->next
			);
			localfetcher.fetchto=splitpoint;
			if(LxiParseExpression_Checkparenthesation(&localfetcher)){
				return LxParseExpression(&localfetcher);
			}else{
				printf("LX: [T] LxParseExpression: I said parenthesized expression? Nevermind, it's not\n");
				// Not really parenthesized expression
			};
		}else{
			// Open parentheses at start but no at the end
		};
	}
	splitpoint=nullptr;
	// Const-volatile-restrict bypass
	if(
		  mtLxFetcher_Peek(fetcher)->type==tToken_Keywordconst
		||mtLxFetcher_Peek(fetcher)->type==tToken_Keywordvolatile
		||mtLxFetcher_Peek(fetcher)->type==tToken_Keywordrestrict
	){ 
#ifdef qvGTrace
		printf("LX: [T] LxParseExpression: Const-volatile-restrict bypass\n");
#endif
		
		return mtLxNode_Clone(
			&(tLxNode){
				.type=	
					 mtLxFetcher_Peek(fetcher)->type==tToken_Keywordconst	
					?tLexem_Constmodifier
					:mtLxFetcher_Peek(fetcher)->type==tToken_Keywordrestrict
					?tLexem_Volatilemodifier
					:mtLxFetcher_Peek(fetcher)->type==tToken_Keywordvolatile
					?tLexem_Restrictmodifier
					:null,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	splitpoint=nullptr;
	// Precedence 15 - Comma 
	// Parsed with left-associativity unit
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Comma:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 15 - Comma \n");
#endif
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Comma,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 14 - Assignments
	// return nullptr;
	// Parsed with right-associativity unit
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 14 - Assignments\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Assign: {
							if(i==fetcher->fetchfrom){
								printf("LX: [E] LxParseExpression: No left-hand operand for assignment \n");
								return nullptr;
							};
							return mtLxNode_Clone(
								&(tLxNode){
									.type=tLexem_Assign,
									.left=LxParseExpression(
										&(tLxFetcher){
											.fetchfrom=fetcher->fetchfrom,
											.fetchto=i
										}
									),
									.right=LxParseExpression(
										&(tLxFetcher){
											.fetchfrom=i->next,
											.fetchto=fetcher->fetchto
										}
									)
								}
							);
						};	break;
							// leftassociative -> full break out of forloop
						default: {
						};	break;
					};
				};
			};	break;
		};
	};
	// Precedence 13 - Ternary conditional
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 13 - Ternary conditional\n");
#endif
	// Parsed with special-purpose right-associativity unit cuz 2 split points - `?` and `:`
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Expressionif:
							splitpoint=i;
							{
								tListnode* splitpoint2 = nullptr;
								for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
									switch(((tToken*)i->item)->type){
										case tToken_Openparentheses: {
											parenthesation++;
										};	break;
										case tToken_Closeparentheses: {
											parenthesation--;
										};	break;
										default: {
											if(parenthesation<0){
												fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
												exit(1);
											};
											if(parenthesation==0){
												switch(((tToken*)i->item)->type){
													case tToken_Colon:
														splitpoint2=i;
														return mtLxNode_Clone(
															&(tLxNode){
																.type=tLexem_Expressionif,
																.condition=LxParseExpression(
																	&(tLxFetcher){
																		.fetchfrom=fetcher->fetchfrom,
																		.fetchto=splitpoint
																	}
																),
																.left=LxParseExpression(
																	&(tLxFetcher){
																		.fetchfrom=splitpoint->next,
																		.fetchto=splitpoint2
																	}
																),
																.right=LxParseExpression(
																	&(tLxFetcher){
																		.fetchfrom=splitpoint2->next,
																		.fetchto=fetcher->fetchto
																	}
																)
															}
														);
													default:
														break;
												};
											};
										};	break;
									};
								};
								
							};
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	// Precedence 12 - Logical OR
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 12 - Logical OR\n");
#endif
	// All the way to precedence 2 leftassociative units are used
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Logicalor:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Logicalor,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 11 - Logical AND
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 11 - Logical AND\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Logicaland:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Logicaland,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 10 - `==` and `!=`
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 10 - `==` and `!=`\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Equals:
						case tToken_Notequal:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=(
					((tToken*)splitpoint->item)->type==tToken_Equals
					?tLexem_Equals:tLexem_Notequal
				),
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 9 - Comparison
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 9 - Comparison\n");
#endif
	//  it's associativity shouldn't matter though, like who does operator<=(int,bool)
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Lessthan:
						case tToken_Greaterthan:
						case tToken_Lessequal:
						case tToken_Greaterequal:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=(
					 ((tToken*)splitpoint->item)->type==tToken_Lessthan	
					?tLexem_Lessthan
					:((tToken*)splitpoint->item)->type==tToken_Greaterthan
					?tLexem_Greaterthan
					:((tToken*)splitpoint->item)->type==tToken_Lessequal
					?tLexem_Lessequal
					:((tToken*)splitpoint->item)->type==tToken_Greaterequal
					?tLexem_Greaterequal
					:null
				),
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 8 - Bitwise OR
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 8 - Bitwise OR\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Bitwiseor:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Bitwiseor,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 7 - Bitwise XOR
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 7 - Bitwise XOR\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Bitwisexor:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Bitwisexor,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 6 - Bitwise AND
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 6 - Bitwise AND\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Bitwiseand:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Bitwiseand,
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 5 - Shift left and right
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 5 - Shift left and right\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Shiftleft:
						case tToken_Shiftright:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=(
					((tToken*)splitpoint->item)->type==tToken_Shiftleft
					?tLexem_Shiftleft:tLexem_Shiftright
				),
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 4 - Plus and minus
	splitpoint=nullptr;
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 4 - Plus and minus\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Plus:
						case tToken_Minus:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint && splitpoint==fetcher->fetchfrom){ // Unary minus
		// Unary minus
	}else
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=(
					((tToken*)splitpoint->item)->type==tToken_Plus
					?tLexem_Add:tLexem_Substract
				),
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	// Precedence 3 - Multiply and divide
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 3 - Multiply and divide\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Multiply:
						case tToken_Divide:
						case tToken_Modulo:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint && (splitpoint == fetcher->fetchfrom)){ // Prefix
		// Handled later
	}else
	if(splitpoint){ // Operator found -> recursive parse + return node
		return mtLxNode_Clone(
			&(tLxNode){
				.type=(
					 ((tToken*)splitpoint->item)->type==tToken_Multiply?tLexem_Multiply
					:((tToken*)splitpoint->item)->type==tToken_Divide  ?tLexem_Divide
					:((tToken*)splitpoint->item)->type==tToken_Modulo  ?tLexem_Modulo
					:null
				),
				.left=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseExpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	//  There should've been .& and ->&
	// Precedence 2 - a lot of various things
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 2 - a lot of various things\n");
#endif
	// Only prefix operators, so welcome to the world of dirty hacks
	switch(mtLxFetcher_Peek(fetcher)->type){
		case tToken_Increment:
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Increment,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Decrement:
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Decrement,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Plus:
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Unaryplus,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Minus:
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Negation,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Logicalnot:
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Logicalnot,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Bitwisenot:
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Bitwisenot,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Multiply: // rvalue * T -> lvalue T
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Dereference,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Bitwiseand: // lvalue T -> rvalue * T
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Addressof,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Keywordsizeof: // sizeof ( T )
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Sizeof,
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
			break;
		case tToken_Openparentheses: {
			//Evaulate parenthesation
			tListnode* i;
			tListnode* k=fetcher->fetchfrom;
			int j = 0;
			for(i=fetcher->fetchfrom->next;j>=0;i=i->next){
				switch(((tToken*)i->item)->type){
					case tToken_Openparentheses:
						j++;
						break;
					case tToken_Closeparentheses:
						j--;
						break;
					default:
						break;
				};
				k=i;
			};
			switch(((tToken*)i->item)->type){
				// Ignore Pr.1 operators
				case tToken_Namespace:       // ::
				case tToken_Increment:       // ++
				case tToken_Decrement:       // --
				case tToken_Dot:             // .
				case tToken_Memberbypointer: // ->
				case tToken_Opencurlybraces: // (){} creating a structure
				case tToken_Openbrackets:    // []   array indexing
					break;
				//Chances are it's type cast
				default: {
					tLxFetcher localfetcher = (tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=i
					};
					tLxFetcher localfetcher2 = (tLxFetcher){
						.fetchfrom=i,
						.fetchto=fetcher->fetchto
					};
					tLxNode* type;
					//mtLxFetcher_Advance(fetcher);
					if(type=LxParseType(
						mtLxFetcher_Trimlast(
							mtLxFetcher_Trimfirst(&localfetcher)
						)
					)){
#ifdef qvGDebug
						printf("LX: [D] LxParseExpression: Proper type cast \n");
#endif
						return mtLxNode_Clone(
							&(tLxNode){
								.type = tLexem_Typecast,
								.left = type,
								.right = LxParseExpression(&localfetcher2)
							}
						);
					}else{
#ifdef qvGDebug
						printf("LX: [D] LxParseExpression: Function call or overall invalid type cast \n");
#endif
						break;
					};
				};	break;
			};
		};	break;
		default: {
		};	break;
	};
	// Precedence 1 - postfix operators
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Precedence 1 - postfix operators\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		if(0)printf("LX: [T] LxParseExpression: Debug: %i.%i:%s\n",
			parenthesation,
			(((tToken*)i->item)->type),
			TokenidtoName[(((tToken*)i->item)->type)]
		);
		if((((tToken*)i->item)->type)==tToken_Openparentheses){ // Dirty hack
			if(parenthesation==0)
				splitpoint=i;

		};
		if((((tToken*)i->item)->type)==tToken_Openbrackets){ // Dirty hack 2
			if(parenthesation==0)
				splitpoint=i;
		};
		if((((tToken*)i->item)->type)==tToken_Opencurlybraces){ // Dirty hack 3
			if(parenthesation==0)
				splitpoint=i;
		};
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseExpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Increment:
						case tToken_Decrement:
						case tToken_Openbrackets:
						case tToken_Dot:
						case tToken_Openparentheses:
						case tToken_Memberbypointer:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		switch(((tToken*)splitpoint->item)->type){
			case tToken_Openparentheses:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Functioncall,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseExpression(
							&(tLxFetcher){
								// Reparses parentheses, but it's fine
								.fetchfrom=splitpoint,
								.fetchto=fetcher->fetchto
							}
						)
					}
				);
				break;
			case tToken_Dot:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Structuremember,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=splitpoint->next,
								.fetchto=fetcher->fetchto
							}
						)
					}
				);
				break;
			case tToken_Memberbypointer:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Memberbypointer,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=splitpoint->next,
								.fetchto=fetcher->fetchto
							}
						)
					}
				);
				break;
			case tToken_Opencurlybraces:
				// oh no
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Compoundliteral,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseExpression(
							mtLxFetcher_Trimlast(
								&(tLxFetcher){
									.fetchfrom=splitpoint->next,
									.fetchto=fetcher->fetchto
								}
							)
						)
					}
				);
				break;
			case tToken_Openbrackets:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Arrayindex,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseExpression(
							mtLxFetcher_Trimlast(
								&(tLxFetcher){
									.fetchfrom=splitpoint->next,
									.fetchto=fetcher->fetchto
								}
							)
						)
					}
				);
				break;
			case tToken_Increment:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Postincrement,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						)
					}
				);
				break;
			case tToken_Decrement:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Postdecrement,
						.left=LxParseExpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						)
					}
				);
				break;
			default:
				break;
		};
		/*
			return mtLxNode_Clone(
				&(tLxNode){
					.type=(
						 ((tToken*)splitpoint->item)->type==tToken_Shiftleft
						?tLexem_Shiftleft:tLexem_Shiftright
					),
					.left=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom,
							.fetchto=splitpoint
						}
					),
					.right=LxParseExpression(
						&(tLxFetcher){
							.fetchfrom=splitpoint->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		*/
	};
	// Constants
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseExpression: Constants\n");
#endif
	if(fetcher->fetchfrom->next==fetcher->fetchto){
		switch(mtLxFetcher_Peek(fetcher)->type){
			case tToken_Integerconstant:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Integerconstant,
						.constant = mtLxFetcher_Peek(fetcher)->constant
					}
				);
			case tToken_Identifier:
				//printf("LX: [T] Emitted identfier %s \n",mtLxFetcher_Peek(fetcher)->string);
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Identifier,
						.identifier = mtLxFetcher_Peek(fetcher)->string
					}
				);
			case tToken_String:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Stringconstant,
						.identifier = mtLxFetcher_Peek(fetcher)->string
					}
				);
			case tToken_Keywordnullptr:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Nullpointer
					}
				);
			case tToken_Keywordtrue:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Booleantrue
					}
				);
			case tToken_Keywordfalse:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Booleanfalse
					}
				);
			default: {
				printf("LX: [T] LxParseExpression: Unrecognized constant - passing through to LxParseType\n");
				tLxNode* typeexpr;
				if(typeexpr=LxParseType(fetcher)){
					return typeexpr;
				};
			};	break;
		};
	}else{
		printf("LX: [T] LxParseExpression: Unrecognized constant - passing through to LxParseType\n");
		tLxNode* typeexpr;
		if(typeexpr=LxParseType(fetcher)){
			return typeexpr;
		};

	};
	
	fprintf(stderr,"LX: [E] LxParseExpression: Unrecognized identifier inside expression at lines %i-%i\n",
		mtLxFetcher_Peek(fetcher)->linenumber,
		mtLxFetcher_Peeklast(fetcher)->linenumber
	);
	mtLxFetcher_Print(fetcher);
	return nullptr;
	exit(2);
	;
};
tLxNode* LxParseTypeexpression(tLxFetcher* fetcher){
#ifdef qvGTrace
	printf("LX: [T] LxParseTypeexpression: entered \n");
	mtLxFetcher_Print(fetcher);
#endif
	tListnode* splitpoint = nullptr;
	int parenthesation = 0;
	// Null statement
	if(fetcher->fetchfrom==fetcher->fetchto){
		printf("LX: [D] LxParseTypeexpression: Null statement seen \n");
		// Null statement special code
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Nullexpression,
				.returnedtype=mtGType_Clone(
					&(tGType){
						.atomicbasetype=eGAtomictype_Void
					}
				),
				.initializer=nullptr,
				.condition=nullptr,
				.left=nullptr,
				.right=nullptr
			}
		);
		return nullptr;
	};
	// Unparenthesize
	if(mtLxFetcher_Peek(fetcher)->type==tToken_Openparentheses){ 
#ifdef qvGTrace
		printf("LX: [T] LxParseTypeexpression: Unparenthesize\n");
#endif
		if(mtLxFetcher_Peeklast(fetcher)->type==tToken_Closeparentheses){
			// Parenthesized expression
			printf("LX: [T] LxParseTypeexpression: Parenthesized expression \n");
			tLxFetcher localfetcher = *fetcher;
			localfetcher.fetchfrom=localfetcher.fetchfrom->next;
			// Trim ending parentheses
			for(
				splitpoint = localfetcher.fetchfrom;
				splitpoint->next != localfetcher.fetchto;
				splitpoint = splitpoint->next
			);
			localfetcher.fetchto=splitpoint;
			if(LxiParseExpression_Checkparenthesation(&localfetcher)){
				return LxParseTypeexpression(&localfetcher);
			}else{
				printf("LX: [T] LxParseTypeexpression: I said parenthesized expression? Nevermind, it's not\n");
				// Not really parenthesized expression
			};
		}else{
			// Open parentheses at start but no at the end
		};
	}
	splitpoint=nullptr;
	// Const-volatile-restrict bypass
	if(
		  mtLxFetcher_Peek(fetcher)->type==tToken_Keywordconst
		||mtLxFetcher_Peek(fetcher)->type==tToken_Keywordvolatile
		||mtLxFetcher_Peek(fetcher)->type==tToken_Keywordrestrict
	){ 
#ifdef qvGTrace
		printf("LX: [T] LxParseTypeexpression: Const-volatile-restrict bypass\n");
#endif
		
		return mtLxNode_Clone(
			&(tLxNode){
				.type=	
					 mtLxFetcher_Peek(fetcher)->type==tToken_Keywordconst	
					?tLexem_Constmodifier
					:mtLxFetcher_Peek(fetcher)->type==tToken_Keywordrestrict
					?tLexem_Volatilemodifier
					:mtLxFetcher_Peek(fetcher)->type==tToken_Keywordvolatile
					?tLexem_Restrictmodifier
					:null,
				.left=LxParseTypeexpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	splitpoint=nullptr;
	// Precedence 15 - Comma 
	// Parsed with left-associativity unit
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseTypeexpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Comma:
							splitpoint=i;
							// leftassociative -> full break out of forloop
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseTypeexpression: Precedence 15 - Comma \n");
#endif
		return mtLxNode_Clone(
			&(tLxNode){
				.type=tLexem_Comma,
				.left=LxParseTypeexpression(
					&(tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=splitpoint
					}
				),
				.right=LxParseTypeexpression(
					&(tLxFetcher){
						.fetchfrom=splitpoint->next,
						.fetchto=fetcher->fetchto
					}
				)
			}
		);
	};
	//  There should've been .& and ->&
	// Precedence 2 - a lot of various things
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseTypeexpression: Precedence 2 - a lot of various things\n");
#endif
	// Only prefix operators, so welcome to the world of dirty hacks
	switch(mtLxFetcher_Peek(fetcher)->type){
		case tToken_Multiply: // rvalue * T -> lvalue T
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Dereference,
					.left=LxParseTypeexpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Bitwiseand: // lvalue T -> rvalue * T
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Addressof,
					.left=LxParseTypeexpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		case tToken_Keywordsizeof: // sizeof ( T )
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Sizeof,
					.left=LxParseTypeexpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
			break;
		case tToken_Openparentheses: {
			//Evaulate parenthesation
			tListnode* i;
			tListnode* k=fetcher->fetchfrom;
			int j = 0;
			for(i=fetcher->fetchfrom->next;j>=0;i=i->next){
				switch(((tToken*)i->item)->type){
					case tToken_Openparentheses:
						j++;
						break;
					case tToken_Closeparentheses:
						j--;
						break;
					default:
						break;
				};
				k=i;
			};
			// `i` points to first token after matching `)`
			switch(((tToken*)i->item)->type){
				// Ignore Pr.1 operators
				case tToken_Namespace:       // ::
				case tToken_Increment:       // ++
				case tToken_Decrement:       // --
				case tToken_Dot:             // .
				case tToken_Memberbypointer: // ->
				case tToken_Opencurlybraces: // (){} creating a structure
				case tToken_Openbrackets:    // []   array indexing
					break;
				//Chances are it's type cast
				default: {
					tLxFetcher localfetcher = (tLxFetcher){
						.fetchfrom=fetcher->fetchfrom,
						.fetchto=i
					};
					tLxFetcher localfetcher2 = (tLxFetcher){
						.fetchfrom=i,
						.fetchto=fetcher->fetchto
					};
					tLxNode* type;
					//mtLxFetcher_Advance(fetcher);
					if(type=LxParseType(
						mtLxFetcher_Trimlast(
							mtLxFetcher_Trimfirst(&localfetcher)
						)
					)){
#ifdef qvGDebug
						printf("LX: [D] LxParseTypeexpression: Proper type cast \n");
#endif
						return mtLxNode_Clone(
							&(tLxNode){
								.type = tLexem_Typecast,
								.left = type,
								.right = LxParseTypeexpression(&localfetcher2)
							}
						);
					}else{
#ifdef qvGDebug
						printf("LX: [D] LxParseTypeexpression: Function call or overall invalid type cast \n");
#endif
						break;
					};
				};	break;
			};
		};	break;
		default: {
		};	break;
	};
	// Precedence 1 - postfix operators
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseTypeexpression: Precedence 1 - postfix operators\n");
#endif
	for(tListnode* i=fetcher->fetchfrom;i!=fetcher->fetchto;i=i->next){
		if(0)printf("LX: [T] LxParseTypeexpression: Debug: %i.%i:%s\n",
			parenthesation,
			(((tToken*)i->item)->type),
			TokenidtoName[(((tToken*)i->item)->type)]
		);
		if((((tToken*)i->item)->type)==tToken_Openparentheses){ // Dirty hack
			if(parenthesation==0)
				splitpoint=i;

		};
		if((((tToken*)i->item)->type)==tToken_Openbrackets){ // Dirty hack 2
			if(parenthesation==0)
				splitpoint=i;
		};
		if((((tToken*)i->item)->type)==tToken_Opencurlybraces){ // Dirty hack 3
			if(parenthesation==0)
				splitpoint=i;
		};
		switch(((tToken*)i->item)->type){
			case tToken_Openparentheses: 
			case tToken_Openbrackets: 
			case tToken_Opencurlybraces: 
				parenthesation++;
				break;
			case tToken_Closeparentheses: 
			case tToken_Closebrackets: 
			case tToken_Closecurlybraces: 
				parenthesation--;
				break;
			default: {
				if(parenthesation<0){
					fprintf(stderr,"LX: [F] LxParseTypeexpression: Closed parentheses w/o preceding open parentheses at line %i \n",((tToken*)i->item)->linenumber);
					exit(1);
				};
				if(parenthesation==0){
					switch(((tToken*)i->item)->type){
						case tToken_Increment:
						case tToken_Decrement:
						case tToken_Openbrackets:
						case tToken_Dot:
						case tToken_Openparentheses:
						case tToken_Memberbypointer:
							splitpoint=i;
							break;
						default:
							break;
					};
				};
			};	break;
		};
	};
	if(splitpoint){ // Operator found -> recursive parse + return node
		switch(((tToken*)splitpoint->item)->type){
			case tToken_Openparentheses:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Functioncall,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseTypeexpression(
							&(tLxFetcher){
								// Reparses parentheses, but it's fine
								.fetchfrom=splitpoint,
								.fetchto=fetcher->fetchto
							}
						)
					}
				);
				break;
			case tToken_Dot:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Structuremember,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=splitpoint->next,
								.fetchto=fetcher->fetchto
							}
						)
					}
				);
				break;
			case tToken_Memberbypointer:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Memberbypointer,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=splitpoint->next,
								.fetchto=fetcher->fetchto
							}
						)
					}
				);
				break;
			case tToken_Opencurlybraces:
				// oh no
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Compoundliteral,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseTypeexpression(
							mtLxFetcher_Trimlast(
								&(tLxFetcher){
									.fetchfrom=splitpoint->next,
									.fetchto=fetcher->fetchto
								}
							)
						)
					}
				);
				break;
			case tToken_Openbrackets:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Arrayindex,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						),
						.right=LxParseTypeexpression(
							mtLxFetcher_Trimlast(
								&(tLxFetcher){
									.fetchfrom=splitpoint->next,
									.fetchto=fetcher->fetchto
								}
							)
						)
					}
				);
				break;
			case tToken_Increment:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Postincrement,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						)
					}
				);
				break;
			case tToken_Decrement:
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Postdecrement,
						.left=LxParseTypeexpression(
							&(tLxFetcher){
								.fetchfrom=fetcher->fetchfrom,
								.fetchto=splitpoint
							}
						)
					}
				);
				break;
			default:
				break;
		};
		/*
			return mtLxNode_Clone(
				&(tLxNode){
					.type=(
						 ((tToken*)splitpoint->item)->type==tToken_Shiftleft
						?tLexem_Shiftleft:tLexem_Shiftright
					),
					.left=LxParseTypeexpression(
						&(tLxFetcher){
							.fetchfrom=fetcher->fetchfrom,
							.fetchto=splitpoint
						}
					),
					.right=LxParseTypeexpression(
						&(tLxFetcher){
							.fetchfrom=splitpoint->next,
							.fetchto=fetcher->fetchto
						}
					)
				}
			);
		*/
	};
	// Constants
#ifdef qvGTraceexpressions
	printf("LX: [T] LxParseTypeexpression: Constants\n");
#endif
	if(fetcher->fetchfrom->next==fetcher->fetchto){
		switch(mtLxFetcher_Peek(fetcher)->type){
			case tToken_Integerconstant:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Integerconstant,
						.constant = mtLxFetcher_Peek(fetcher)->constant
					}
				);
			case tToken_Identifier:
				//printf("LX: [T] Emitted identfier %s \n",mtLxFetcher_Peek(fetcher)->string);
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Identifier,
						.identifier = mtLxFetcher_Peek(fetcher)->string
					}
				);
			case tToken_String:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Stringconstant,
						.identifier = mtLxFetcher_Peek(fetcher)->string
					}
				);
			case tToken_Keywordnullptr:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Nullpointer
					}
				);
			case tToken_Keywordtrue:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Booleantrue
					}
				);
			case tToken_Keywordfalse:
				return mtLxNode_Clone(
					&(tLxNode){
						.type = tLexem_Booleanfalse
					}
				);
			default: {
				printf("LX: [T] LxParseTypeexpression: Unrecognized constant - passing through to LxParseType\n");
				tLxNode* typeexpr;
				if(typeexpr=LxParseType(fetcher)){
					return typeexpr;
				};
			};	break;
		};
	}else{
		printf("LX: [T] LxParseTypeexpression: Unrecognized constant - passing through to LxParseType\n");
		tLxNode* typeexpr;
		if(typeexpr=LxParseType(fetcher)){
			return typeexpr;
		};

	};
	
	fprintf(stderr,"LX: [E] LxParseTypeexpression: Unrecognized identifier inside expression at lines %i-%i\n",
		mtLxFetcher_Peek(fetcher)->linenumber,
		mtLxFetcher_Peeklast(fetcher)->linenumber
	);
	mtLxFetcher_Print(fetcher);
	return nullptr;
	exit(2);
	;
};
tLxNode* LxParseDeclaration(tLxFetcher* fetcher){
	/* |> Declaration of whatever that be
	 * semicolon // empty declaration
	 * basetype semicolon // struct <identifier> { } ; // though no one uses the syntax
	 * basetype typeexpr<identifier> semicolon // uninitialized variable declaration
	 * basetype typeexpr<identifier> assign expression semicolon // initialized variable declaration
	 * basetype typeexpr<identifier>{.lastop==functioncall} blockstatement // function declaration
	 * other atomics like typedef
	 */
	tLxFetcher savedfetcher = *fetcher;
#ifdef qvGTrace
	printf("LX: [T] LxParseDeclaration(): Entered\n");
#endif
#ifdef qvGDebug
	printf("LX: [D] LxParseDeclaration(): Printing fetcher \n");
	mtLxFetcher_Print_Limited(fetcher);
#endif
	if(mtLxFetcher_Eof(fetcher))return nullptr;
	printf("LX: [D] LxParseDeclaration(): start token == %i:%s\n",mtLxFetcher_Peek(fetcher)->type,TokenidtoName[mtLxFetcher_Peek(fetcher)->type]);
	switch(mtLxFetcher_Peek(fetcher)->type){
		case tToken_Semicolon:
			mtLxFetcher_Advance(fetcher);
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Nulldeclaration,
					.returnedtype=nullptr,
					.initializer=nullptr,
					.condition=nullptr,
					.left=nullptr,
					.right=nullptr
				}
			);
			break;
		case tToken_Keywordtypedef:
			printf("LX: [D] LxParseDeclaration: Typedef seen \n");
			mtLxFetcher_Advance(fetcher);
			tGType* basetype=LxParseBasetype(fetcher);
			tLxFetcher* typeexprfetcher = mtLxFetcher_FetchuntilParenthesized(fetcher,tToken_Semicolon);
			mtLxFetcher_Print(typeexprfetcher);
			tLxNode* expr = LxParseTypeexpression(typeexprfetcher);
			return mtLxNode_Clone(
				&(tLxNode){
					.type=tLexem_Typedefinition,
					.returnedtype=basetype,
					.left=expr,
					.right=nullptr
				}
			);
			break;
		default: {
			tGType* basetype;
			if(basetype=LxParseBasetype(fetcher)){
				printf("LX: [T] LxParseDeclaration: Type fetched successfully, trying variable declaration\n");
				//mtLxFetcher_Print(fetcher);
				tLxNode* initializer = nullptr;
				tLxFetcher* typeexprfetcher = mtLxFetcher_Fetchuntil_Variadic(fetcher, 
					3,tToken_Assign,tToken_Opencurlybraces,tToken_Semicolon
				);
#ifdef qvGDebug
				printf("LX: [D] LxParseDeclaration: typeexprfetcher.print \n");
				mtLxFetcher_Print(typeexprfetcher);
#endif
				tLxNode* expr = LxParseTypeexpression(typeexprfetcher);
				// If no valid type expression
				if(expr==nullptr){
					*fetcher=savedfetcher;
					return nullptr;
				};
				//fprintf(stderr,"LX: [F] Unfinished code hit! \n");
				//exit(2);
				//printf("LX: [T] %i \n",mtLxFetcher_Peek(fetcher)->type);
				switch(mtLxFetcher_Peek(fetcher)->type){ // Decide on initializer
					case tToken_Opencurlybraces: {
						printf("LX: [T] LxParseDeclaration: Block statement\n");
						initializer = LxParseBlockstatement(fetcher);
						return mtLxNode_Clone(
							&(tLxNode){
								.type=tLexem_Functiondeclaration,
								.returnedtype=basetype,
								.left=expr,
								.right=initializer
							}
						);
						//mtLxFetcher_Advance(fetcher);
						//initializer = LxParseExpression(mtLxFetcher_FetchuntilParenthesized(fetcher,tToken_Closecurlybraces));
					};	break;
					case tToken_Semicolon: {
						printf("LX: [T] LxParseDeclaration: No initializer\n");
						mtLxFetcher_Advance(fetcher);
						return mtLxNode_Clone(
							&(tLxNode){
								.type=tLexem_Variabledeclaration,
								.returnedtype=basetype,
								.left=expr,
								.right=nullptr
							}
						);
						//mtLxFetcher_Advance(fetcher);
						//initializer = LxParseExpression(mtLxFetcher_FetchuntilParenthesized(fetcher,tToken_Closecurlybraces));
					};	break;
					case tToken_Assign: {
						printf("LX: [T] LxParseDeclaration: Expression initializer \n");
						mtLxFetcher_Advance(fetcher);
						initializer = LxParseExpression(mtLxFetcher_FetchuntilParenthesized(fetcher,tToken_Semicolon));
						mtLxFetcher_Advance(fetcher); // skip `;`
						return mtLxNode_Clone(
							&(tLxNode){
								.type=tLexem_Variabledeclaration,
								.returnedtype=basetype,
								.left=expr,
								.right=initializer
							}
						);
					};	break;
					default: {
						printf("LX: [F] LxParseDeclaration: Unrecognized initializer or forgotten semicolon\n");
						mtLxFetcher_Print_Limited(fetcher);
						exit(1);
					};	break;
				};
				return mtLxNode_Clone(
					&(tLxNode){
						.type=tLexem_Variabledeclaration,
						.returnedtype=basetype,
						.left=expr,
						.right=nullptr
					}
				);
			}else{
				printf("LX: [F] LxParseDeclaration: Unrecognized declaration! \n");
				return nullptr;
				//exit(1);
			};
		};	break;
	};
	
	return nullptr;
};
tLxNode* LxParseProgram(tLxFetcher* fetcher){ // either {eof} or {declaration program}
#ifdef qvGTrace
	printf("LX: [T] LxParseProgram(): entered \n");
#endif
	if(mtLxFetcher_Eof(fetcher)){
		return nullptr; // Program end
	}else{
		tLxNode* i = calloc(1,sizeof(tLxNode));
		i->type = tLexem_Declarationlist;
		i->condition = nullptr;
		i->left = LxParseDeclaration(fetcher);
		i->right = LxParseProgram(fetcher);
		return i;
	};
};
tLxNode* LxParse(tListnode* startpoint){
	// Main lexical parsing subroutine.
	/* For ones modifying this CC - parsing sourcefile->list<token> is 
	 * called tokenizing here and is carried out by `TkTokenize()`.
	 * Lexing here is parsing list<token> -> primary ast.
	 * Semantical parsing is primary ast -> secondary ast.
	 * In-between there is symbolgen pass to generate type and symbol lists.
	 */
#ifdef qvGTrace
	printf("LX: [T] Parsing program! \n");
#endif
	tLxNode* i = LxParseProgram(&(tLxFetcher){.fetchfrom=startpoint,.fetchto=nullptr});
	LfPrint_LxNode(i);
	//exit(3);
	return i;
	//return LxParseProgram(&(tLxFetcher){.fetchfrom=startpoint,.fetchto=nullptr});
	
//	tLxNode* node = mtLxNode_Create();
//	tToken* token = startpoint->item; // Templates not yet supported, you know
//	                                  // got to type the list element
//	tLxFetcher* fetcher = mtLxFetcher_Create(startpoint,nullptr);
//	
//	switch(token->type){
//		default: // Apparently type name?
//			{
//				tGType* fetchedbasetype;
//				if(fetchedbasetype=mtLxFetcher_TryfetchType(fetcher)){
//					//Fetch declaration of whatever it be - function or variable
//					// One thing for sure - terminated by either semicolon or 
//					//  open curly brace.
//					tLxFetcher* typeexprfetcher = mtLxFetcher_Fetchuntil_Variadic(fetcher,2,tToken_Semicolon,tToken_Opencurlybraces);
//					tLxNode* typeexprast = LxParseExpression(typeexprfetcher);
//					if(((tToken*)(typeexprfetcher->fetchfrom->item))->type==tToken_Semicolon){ 
//						//Forward function declaration or uninitialized variable declaration
//					}else if(((tToken*)(typeexprfetcher->fetchfrom->item))->type==tToken_Opencurlybraces){
//						//Fetch the block statement
//						// ¤ Treat as an initializer
//					}else{
//						printf(
//							"LX: [F] . Internal inconsistency exception!\n"
//							/*"LX: [F] | Koli, you forgot about legacy code again! \n"
//							"LX: [F] | I have no idea what token %i:%s is supposed to do in a declaration,\n"
//							"LX: [F] |  maybe you decided against for now and forgot later on about the \n"
//							"LX: [F] '  comma in declarations. \n"
//							*/,token->type,TokenidtoName[token->type]
//						);
//					}
//				}else{
//					printf("LX: [F] Fetcher unable to recognize a declaration \n");
//				};
//			};
//			printf("LX: [F] Unexcepted token! %i:%s \n",token->type,TokenidtoName[token->type]);
//			break;
//	};
//	return nullptr;
};
