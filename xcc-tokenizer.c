#include "hyperheader.h"

FILE* srcfile;
FILE* dstfile;

tPipe src_putback;

bool endofinstream(void){
	if(mtPipe_Enqueuedbytes(&src_putback)){
		return false;
	}else{
		return feof(srcfile);
	};
};

char fetchchar(void){
	if(mtPipe_Enqueuedbytes(&src_putback)){
		return mtPipe_Readchar(&src_putback);
	}else{
		return fgetc(srcfile);
	};
};

void putcharback(char d){
	mtPipe_Writechar(&src_putback,d);
};

void emitchar(char d){
	fputc(d,dstfile);
};

void emitword(unsigned short d){
	fputc(d&255,dstfile);
	fputc(d>>8,dstfile);
};

void emittoken(tToken token){//does not emits the string
	emitword(token.type);
	emitword(token.reserved1);
	emitword(token.reserved2);
	emitword(token.reserved3);
	if(0)printf("TK: Emitted token %6hu:%s (%4.4x%5.4x%5.4x) \n",
		token.type,
	        TokenidtoName[token.type],
	        token.reserved1,
	        token.reserved2,
	        token.reserved3
	);
};

void emittokenstring(tToken token,char* string){
	emitword(token.type);
	emitword(token.reserved1);
	emitword(token.reserved2);
	emitword(token.reserved3);
	for(char* ptr=string;*ptr!=0;ptr++){
		emitchar(*ptr);
	};
	emitchar(0);
	if(0)printf("TK: Emitted token %6hu:%s (%4.4x%5.4x%5.4x), string:%s\n",
		token.type,
		TokenidtoName[token.type],
	        token.reserved1,
	        token.reserved2,
	        token.reserved3,
		string
	);
};

bool iscspecialcharater(char c){
	if(isalnum(c)){return false;};	//Alphanumeric - usual charater
	if(c=='_')return false;		//Underscore - allowed in tokens
	return true;		//The rest are 'auto-whitespace' chars that begin another token
}


void init(void){
	mtPipe_Clear(&src_putback);
};

int main(int argc, char* argv[]){
	init();
	printf("TK: XCC Tokenizer: \"%s\"->\"%s\"\n",argv[1],argv[2]);
	srcfile = fopen(argv[1],"r");
	dstfile = fopen(argv[2],"w");
	char buffer,buffer2;
	while(!endofinstream()){
		switch(buffer=fetchchar()){
			case '!':switch(buffer=fetchchar()){
				case '=':	emittoken((tToken){.type=tToken_Notequal});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Logicalnot});	break;
				};break;
			case '%':switch(buffer=fetchchar()){
				case '=':	emittoken((tToken){.type=tToken_Moduloassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Modulo});	break;
				};break;
			case '&':switch(buffer=fetchchar()){
				case '&':	emittoken((tToken){.type=tToken_Logicaland});	break;
				case '=':	emittoken((tToken){.type=tToken_Bitwiseandassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Bitwiseand});	break;
				};break;
			case '(':	emittoken((tToken){.type=tToken_Openparentheses});	break;
			case ')':	emittoken((tToken){.type=tToken_Closeparentheses});	break;
			case '*':switch(buffer=fetchchar()){
				case '=':	emittoken((tToken){.type=tToken_Multiplyassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Multiply});	break;
				};break;
			case '+':switch(buffer=fetchchar()){
				case '+':	emittoken((tToken){.type=tToken_Increment});	break;
				case '=':	emittoken((tToken){.type=tToken_Addassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Plus});	break;
				};break;
			case ',':	emittoken((tToken){.type=tToken_Comma});	break;
			case '-':switch(buffer=fetchchar()){
				case '-':	emittoken((tToken){.type=tToken_Decrement});	break;
				case '>':	emittoken((tToken){.type=tToken_Memberbypointer});	break;
				case '=':	emittoken((tToken){.type=tToken_Substractassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Minus});	break;
				};break;
			case '.':	emittoken((tToken){.type=tToken_Dot});	break;
			case '/':switch(buffer=fetchchar()){
				case '=':	emittoken((tToken){.type=tToken_Divideassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Divide});	break;
				};break;
			case ':':switch(buffer=fetchchar()){
				case ':':	emittoken((tToken){.type=tToken_Namespace});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Colon});	break;
				};break;
			case '<':switch(buffer=fetchchar()){
				case '<':	switch(buffer=fetchchar()){
					case '=':	emittoken((tToken){.type=tToken_Shiftleftassign});	break;
					default:	putcharback(buffer);
							emittoken((tToken){.type=tToken_Shiftleft});	break;
					};break;
				case '=':	switch(buffer=fetchchar()){
					case '>':	emittoken((tToken){.type=tToken_Threewaycomparison});	break;
					default:	putcharback(buffer);
							emittoken((tToken){.type=tToken_Lessequal});	break;
					};break;
				//case '=':	emittoken((tToken){.type=});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Lessthan});	break;
				};break;
			case '=':switch(buffer=fetchchar()){
				case '=':	emittoken((tToken){.type=tToken_Equals});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Assign});	break;
				};break;
			case '>':switch(buffer=fetchchar()){
				case '>':	switch(buffer=fetchchar()){
					case '=':	emittoken((tToken){.type=tToken_Shiftrightassign});	break;
					default:	putcharback(buffer);
							emittoken((tToken){.type=tToken_Shiftright});	break;
					};break;
				case '=':	emittoken((tToken){.type=tToken_Greaterequal});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Greaterthan});	break;
				};break;
			case '?':	emittoken((tToken){.type=tToken_Expressionif});	break;
			case '[':	emittoken((tToken){.type=tToken_Openbrackets});	break;
			case ']':	emittoken((tToken){.type=tToken_Closebrackets});	break;
			case '^':switch(buffer=fetchchar()){
				case '=':	emittoken((tToken){.type=tToken_Bitwisexorassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Bitwisexor});	break;
				};break;
			case '{':	emittoken((tToken){.type=tToken_Opencurlybraces});	break;
			case '|':	switch(buffer=fetchchar()){
				case '|':	emittoken((tToken){.type=tToken_Logicalor});	break;
				case '=':	emittoken((tToken){.type=tToken_Bitwiseorassign});	break;
				default:	putcharback(buffer);
						emittoken((tToken){.type=tToken_Bitwiseor});	break;
				};break;
			case '}':	emittoken((tToken){.type=tToken_Closecurlybraces});	break;
			case '~':	emittoken((tToken){.type=tToken_Bitwisenot});		break;
			case ';':	emittoken((tToken){.type=tToken_Semicolon});	break;
			case '\'':	
				{
					tString str1,str2;//cuz `char* str1,str2` defines str2 as `char`,not `char*`
					str1=mtString_Clone("");
					for(bool continuestring=true;continuestring;){
						switch(buffer2=fetchchar()){
							case '\\':
								switch(buffer2=fetchchar()){
									case 'n':	buffer2='\n';	break;
									case 'r':	buffer2='\r';	break;
									case 't':	buffer2='\t';	break;
									//case '\"':
									//case '\\':
									default:
										break;
								};
								str2=mtString_Join(str1,(char*)(&(char[2]){buffer2,0}));
								free(str1);
								str1=str2;
										
								break;
							case '\'':
								continuestring=false;
								continue;
							default:
								//Append charater to in-work dynamic string
								mtString_Append(&str1,(char*)(&(char[2]){buffer2,0}));
						};
					};
					emittokenstring((tToken){.type=tToken_Charaterconstant},str1);
					free(str1);
				};
				break;
			case '\"':
				{
					tString str1,str2;//cuz `char* str1,str2` defines str2 as `char`,not `char*`
					str1=mtString_Clone("");
					for(bool continuestring=true;continuestring;){
						switch(buffer2=fetchchar()){
							case '\\':
								switch(buffer2=fetchchar()){
									case 'n':	buffer2='\n';	break;
									case 'r':	buffer2='\r';	break;
									case 't':	buffer2='\t';	break;
									//case '\"':
									//case '\\':
									default:
										break;
								};
								str2=mtString_Join(str1,(char*)(&(char[2]){buffer2,0}));
								free(str1);
								str1=str2;
										
								break;
							case '\"':
								continuestring=false;
								continue;
							default:
								//Append charater to in-work dynamic string
								mtString_Append(&str1,(char*)(&(char[2]){buffer2,0}));
								
						};
					};
					emittokenstring((tToken){.type=tToken_String},str1);
					free(str1);
				};
				break;
			case '\t':
			case '\n':
			case '\r':
			case ' ':
			case '\xFF':
				break;
			default:
				{
					tString str1,str2;
					bool isakeyword=false;
					{//Create string of identifier
						str1=mtString_Clone((char*)(&(char[2]){buffer,0}));
						for(bool continuestring=true;continuestring;){
							if(iscspecialcharater(buffer2=fetchchar())){
								putcharback(buffer2);
								continuestring=false;
								continue;
							}else{
								//Append charater to in-work dynamic string
								str2=mtString_Join(str1,(char*)(&(char[2]){buffer2,0}));
								free(str1);
								str1=str2;
							};
						};
					};//str1=__dynamic__ &<The identifier>
					{//Check if identifier is a keyword
						for(int i=0;(KeywordtoTokentype[i].keyword!=0)&&(i>=0);i++){//Search for keyword
							if(mtString_Equals(KeywordtoTokentype[i].keyword,str1)){
								isakeyword=true;
								emittoken((tToken){.type=KeywordtoTokentype[i].tokentype});
								break;
							};
						};
					};
					if(isakeyword){
						//Well, token is already emitted as a keyword, so I guess
						//there is nothing to do here
					}else{
						emittokenstring((tToken){.type=tToken_Identifier},str1);
					};
					free(str1);
				};
				break;
		};
	};
	fclose(srcfile);
	fclose(dstfile);
};