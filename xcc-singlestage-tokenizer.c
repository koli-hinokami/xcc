// Tokenizer for `xcc-singlestage` compiler
unsigned TkLinenumber = 1;
void TkEmittoken(tToken token){
	if(0){
		if(mtToken_HasString(&token)){
			printf("TK: [T] Emitted token %6hu:%s, string:%s\n",
				token.type,
				TokenidtoName[token.type],
				token.string
			);
		}else{
			printf("TK: [T] Emitted token %6hu:%s \n",
				token.type,
				TokenidtoName[token.type]
			);
		};
	};
	tToken* heaptoken = mtToken_Clone(&token);
	heaptoken->linenumber=TkLinenumber;
	//printf("TK: [T] Debug: %s \n",heaptoken->string);
	//heaptoken->string=mtString_Clone(heaptoken->string);
	mtList_Append(&GTokenized,heaptoken);
};
bool TkIscspecialcharater(char c){
	if(isalnum(c)){return false;};	//Alphanumeric - usual charater
	if(c=='_')return false;		//Underscore - allowed in tokens
	return true;		//The rest are 'auto-whitespace' chars that begin another token
}
char TkGetc(){
	char buf = fgetc(GSourcefile);
	return buf;
};
void TkUngetc(char ch){
	ungetc(ch,GSourcefile);
};
tGTargetUintmax TkStringtonumber(char* str){
	char* strtol_tail = nullptr;
	long num = strtol(str,&strtol_tail,0);
	assert(strtol_tail);
	assert(strtol_tail[0]==0);
	return num;
};
void TkTokenize(FILE* srcfile){
	char buffer,buffer2;

	while(!feof(srcfile)){
		switch(buffer=TkGetc()){
			case '!':switch(buffer=TkGetc()){
				case '=':	TkEmittoken((tToken){.type=tToken_Notequal});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Logicalnot});	break;
				};break;
			case '%':switch(buffer=TkGetc()){
				case '=':	TkEmittoken((tToken){.type=tToken_Moduloassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Modulo});	break;
				};break;
			case '&':switch(buffer=TkGetc()){
				case '&':	TkEmittoken((tToken){.type=tToken_Logicaland});	break;
				case '=':	TkEmittoken((tToken){.type=tToken_Bitwiseandassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Bitwiseand});	break;
				};break;
			case '(':	TkEmittoken((tToken){.type=tToken_Openparentheses});	break;
			case ')':	TkEmittoken((tToken){.type=tToken_Closeparentheses});	break;
			case '*':switch(buffer=TkGetc()){
				case '=':	TkEmittoken((tToken){.type=tToken_Multiplyassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Multiply});	break;
				};break;
			case '+':switch(buffer=TkGetc()){
				case '+':	TkEmittoken((tToken){.type=tToken_Increment});	break;
				case '=':	TkEmittoken((tToken){.type=tToken_Addassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Plus});	break;
				};break;
			case ',':	TkEmittoken((tToken){.type=tToken_Comma});	break;
			case '-':switch(buffer=TkGetc()){
				case '-':	TkEmittoken((tToken){.type=tToken_Decrement});	break;
				case '>':	TkEmittoken((tToken){.type=tToken_Memberbypointer});	break;
				case '=':	TkEmittoken((tToken){.type=tToken_Substractassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Minus});	break;
				};break;
			case '.':switch(buffer=TkGetc()){
				case '.':	switch(buffer=TkGetc()){
					case '.':	TkEmittoken((tToken){.type=tToken_Ellipsis});	break;
					default:	TkUngetc(buffer);	
							TkUngetc('.');
							TkEmittoken((tToken){.type=tToken_Dot});	break;
						};break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Dot});	break;
				};break;
			case '/':switch(buffer=TkGetc()){
				case '=':	TkEmittoken((tToken){.type=tToken_Divideassign});	break;
				case '/': {	//Comment
						while(TkGetc()!=10);
						TkUngetc(10);
					};
					break;
				case '*': { //Multilinecomment
						while(true){
							while(TkGetc()!='*');
							if(TkGetc()=='/'){break;}else{continue;};
						};
					}; break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Divide});	break;
				};break;
			case ':':switch(buffer=TkGetc()){
				case ':':	TkEmittoken((tToken){.type=tToken_Namespace});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Colon});	break;
				};break;
			case '<':switch(buffer=TkGetc()){
				case '<':	switch(buffer=TkGetc()){
					case '=':	TkEmittoken((tToken){.type=tToken_Shiftleftassign});	break;
					default:	TkUngetc(buffer);
							TkEmittoken((tToken){.type=tToken_Shiftleft});	break;
					};break;
				case '=':	switch(buffer=TkGetc()){
					case '>':	TkEmittoken((tToken){.type=tToken_Threewaycomparison});	break;
					default:	TkUngetc(buffer);
							TkEmittoken((tToken){.type=tToken_Lessequal});	break;
					};break;
				//case '=':	TkEmittoken((tToken){.type=});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Lessthan});	break;
				};break;
			case '=':switch(buffer=TkGetc()){
				case '=':	TkEmittoken((tToken){.type=tToken_Equals});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Assign});	break;
				};break;
			case '>':switch(buffer=TkGetc()){
				case '>':	switch(buffer=TkGetc()){
					case '=':	TkEmittoken((tToken){.type=tToken_Shiftrightassign});	break;
					default:	TkUngetc(buffer);
							TkEmittoken((tToken){.type=tToken_Shiftright});	break;
					};break;
				case '=':	TkEmittoken((tToken){.type=tToken_Greaterequal});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Greaterthan});	break;
				};break;
			case '?':	TkEmittoken((tToken){.type=tToken_Expressionif});	break;
			case '[':	TkEmittoken((tToken){.type=tToken_Openbrackets});	break;
			case ']':	TkEmittoken((tToken){.type=tToken_Closebrackets});	break;
			case '^':switch(buffer=TkGetc()){
				case '=':	TkEmittoken((tToken){.type=tToken_Bitwisexorassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Bitwisexor});	break;
				};break;
			case '{':	TkEmittoken((tToken){.type=tToken_Opencurlybraces});	break;
			case '|':	switch(buffer=TkGetc()){
				case '|':	TkEmittoken((tToken){.type=tToken_Logicalor});	break;
				case '=':	TkEmittoken((tToken){.type=tToken_Bitwiseorassign});	break;
				default:	TkUngetc(buffer);
						TkEmittoken((tToken){.type=tToken_Bitwiseor});	break;
				};break;
			case '}':	TkEmittoken((tToken){.type=tToken_Closecurlybraces});	break;
			case '~':	TkEmittoken((tToken){.type=tToken_Bitwisenot});		break;
			case ';':	TkEmittoken((tToken){.type=tToken_Semicolon});	break;
			case '\'': {	//Charater
					char* str1;
					str1=mtString_Clone("");
					for(bool continuestring=true;continuestring;){
						switch(buffer2=TkGetc()){
							case '\\':
								switch(buffer2=TkGetc()){
									case 'n':	buffer2='\n';	break;
									case 'r':	buffer2='\r';	break;
									case 't':	buffer2='\t';	break;
									case '\\':	buffer2='\\';	break;
									//case '\"':
									default:
										break;
								};
								mtString_Append(&str1,(char*)(&(char[2]){buffer2,0}));
								break;
							case '\'':
								continuestring=false;
								continue;
							default:
								//Append charater to in-work dynamic string
								mtString_Append(&str1,(char*)(&(char[2]){buffer2,0}));
						};
					};
					TkEmittoken(
						(tToken){
							.type=tToken_Charaterconstant,
							.string = str1
						}
					);
					//free(str1);
				};
				break;
			case '\"':	//String incoming!
				{
					char* str1;
					str1=mtString_Clone("");
					for(bool continuestring=true;continuestring;){
						switch(buffer2=TkGetc()){
							case '\\':
								switch(buffer2=TkGetc()){
									case 'n':	buffer2='\n';	break;
									case 'r':	buffer2='\r';	break;
									case 't':	buffer2='\t';	break;
									//case '\"':
									//case '\\':
									default:
										break;
								};
								mtString_Append(&str1,(char*)(&(char[2]){buffer2,0}));
										
								break;
							case '\"':
								continuestring=false;
								continue;
							default:
								//Append charater to in-work dynamic string
								mtString_Append(&str1,(char*)(&(char[2]){buffer2,0}));
								
						};
					};
					TkEmittoken(
						(tToken){
							.type=tToken_String,
							.string = str1
						}
					);
					//free(str1);
				};
				break;
			case '\t':
				break;
			case '\n':
				TkLinenumber++;
				break;
			case '\r':
				printf("TK: [F] No Windows lineendings!!! \n");
				exit(1);
				break;
			case ' ':
			case '\xFF':
				break;
			default: {	// We apparently got something different from a token
				char* str1;
				bool isakeyword=false;
				{//Create string of identifier
					str1=mtString_Clone((char*)((char[2]){buffer,0}));
					for(bool continuestring=true;continuestring;){
						if(TkIscspecialcharater(buffer2=TkGetc())){
							TkUngetc(buffer2);
							continuestring=false;
							continue;
						}else{
							//Append charater to in-work dynamic string
							mtString_Append(&str1,(char*)((char[2]){buffer2,0}));
						};
					};
				};//str1=__dynamic__ &<The identifier>
				{//Check if identifier is a keyword
					for(int i=0;(KeywordtoTokentype[i].keyword!=0)&&(i>=0);i++){//Search for keyword
						if(mtString_Equals(KeywordtoTokentype[i].keyword,str1)){
							isakeyword=true;
							TkEmittoken((tToken){.type=KeywordtoTokentype[i].tokentype});
							break;
						};
					};
				};
				if(isakeyword){
					//Well, token is already emitted as a keyword, so I guess
					//there is nothing to do here
				}else{
					if(isdigit(str1[0])){
						// A (potential) number. Need to parse it.
						//Negative numbers are handled as unary minus operation applied to positive number and optimizer
						//TODO make it
						
						TkEmittoken(
							(tToken){
								.type=tToken_Integerconstant,
								.constant=TkStringtonumber(str1)
							}
						);
					}else{
						TkEmittoken(
							(tToken){
								.type=tToken_Identifier,
								.string=str1
							}
						);
					};
				};
				//free(str1);
			};
			break;
		};
	};
};
