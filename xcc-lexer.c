#include "hyperheader.h"

FILE* srcfile;
FILE* dstfile;

tPipe src_putback;
int argc;
char **argv;
bool terminatelexer;

void init(void){
	mtPipe_Clear(&src_putback);
	printf("LX: XCC Lexer: \"%s\"->\"%s\"\n",argv[1],argv[2]);
	srcfile = fopen(argv[1],"r");
	dstfile = fopen(argv[2],"w");
};

void putcharback(char d){
	mtPipe_Writechar(&src_putback,d);
};

void putwordback(short d){
	mtPipe_Writeword(&src_putback,d);
};

void puttokenback(ptToken token){
	putwordback(token->type);
	putwordback(token->reserved1);
	putwordback(token->reserved2);
	putwordback(token->reserved3);
	if(token->type&0x0100){
		//Token have a string
		for(char* str=(char*)(&token->string);*str!=0;str++){
			putcharback(*str);
		};
		putcharback(0);
	};
	
};

void finalize(void){
	fclose(srcfile);
	fclose(dstfile);
	exit(0);
};

bool endofinstream(void){
	if(mtPipe_Enqueuedbytes(&src_putback)){
		return false;
	}else{
		char tmp=fgetc(srcfile);
		bool end=feof(srcfile);
		putcharback(tmp);
		return end;
	};
};

unsigned char fetchchar(void){
	if(mtPipe_Enqueuedbytes(&src_putback)){
		return mtPipe_Readchar(&src_putback);
	}else{
		//int tmp2= endofinstream();
		//char tmp = fgetc(srcfile);
		//printf("DBG:%i \n",endofinstream());
		//return tmp;
		return fgetc(srcfile);
	};
};

short fetchword(void){
	unsigned int tlo=fetchchar();
	unsigned int thi=fetchchar();
	return (tlo)+(thi<<8);
	
};

void dumptoken_stderr(ptToken token){
	if(token==0){
		fprintf(stderr,"LX: [E]: Fatal! Attempt to dump null token!");
		finalize();		
	};
	if(token->type&0x0100){
		fprintf(stderr,"%6hu:%s (%4.4x%5.4x%5.4x), string:%s",
			token->type,
			TokenidtoName[token->type],
			token->reserved1,
			token->reserved2,
			token->reserved3,
			token->string
		);
	}else{
		fprintf(stderr,"%6hu:%s (%4.4x%5.4x%5.4x)",
			token->type,
			TokenidtoName[token->type],
			token->reserved1,
			token->reserved2,
			token->reserved3
		);
	};
};

ptToken fetchtoken(void){
	//!Gives you malloc'd memory
	
	ptToken token = malloc(sizeof(tToken));//8
	char* str="";		//String belonging to token
	char c;
	token->type	 = fetchword();
	token->reserved1 = fetchword();
	token->reserved2 = fetchword();
	token->reserved3 = fetchword();
	if(token->type&0x0100){
		//Token have a string
		for(str=mtString_Clone(""),c=fetchchar();c!=0;c=fetchchar()){
			mtString_Append(&str,(char*)(&(char[2]){c,0}));
			if(endofinstream()){
				fprintf(stderr,"LX: [E]: Unhandled end of inpit stream\n");
				exit(1);
			}
		};
		ptToken tmp=malloc(mtString_Fulllength(str)+sizeof(tToken));
		memcpy(tmp,token,sizeof(tToken));
		memcpy(tmp->string,str,mtString_Fulllength(str));
		free(str);
		free(token);
		return tmp;
	};
	return token;
};

void emitchar(char d){
	fputc(d,dstfile);
};

void emitword(unsigned short d){
	fputc(d&255,dstfile);
	fputc(d>>8,dstfile);
};

typedef enum tLexerState {
	LxState_Pendingtermination = 0,
	LxState_Scrolling = 1,
	LxState_Expressionparsing = 2,
} tLexerState;

struct{
	ptSubroutine init;
	ptSubroutine finalize;
	ptSubroutine tick;
}	StatesDescription[25] = {
	{},
};

void StScrolling(){
	//Default state of lexer going through the sources searching for stuff
	ptToken token;
	while(!endofinstream()){
		token = fetchtoken();
		switch(token->type){
			default:
				fprintf(stderr,"LX: [W]: Unknown token `");
				dumptoken_stderr(token);
				fprintf(stderr,"`, ignoring\n");	
		};
		free(token);
	};
};

void StParsestatement(){
	//Parse statement
	ptToken token = 0;
	
	switch((token = fetchtoken())->type){
		case tToken_Opencurlybraces://Compound statement
			{
				ptToken token2;
				bool terminateloop=false;
				while(!terminateloop){
					token2=fetchtoken();
					switch(token2->type){
						case tToken_Closecurlybraces:
							terminateloop=true;
							break;
						default:
							//We have fetched a token of statement - put it back and go parse the statement
							puttokenback(token2);
							StParsestatement();
					};
					free(token2);
				};
				
			};
			break;
		default:
			fprintf(stderr,"LX: [W]: Unknown token ");
			dumptoken_stderr(token);
			fprintf(stderr,"\n");
	};
	free(token);	
}

int main(int ac, char* av[]){
	argc=ac;
	argv=av;
	init();
	tLexerState lexerstate;
	char buffer,buffer2;
	//StScrolling();
	/*
	while(lexerstate!=LxState_Pendingtermination)
	{
		switch(lexerstate){
			
		}
		terminatelexer|=endofinstream();
	
	};
	*/
	finalize();
};