/*
struct tLxFetcher {
	tListnode<tToken> fetchto,fetchfrom;
};
	Fetcher is needed because things like {
		int, short;
		unsigned, signed char;
		struct T, enum T;
		unsigned long long;
		T<unsigned>
		tDictionary<unsigned long long,tDictionary<int,char*> >;
	} exist. Okay last one was a tiniest bit too much, but just in case.
*/
// ---------------- Fetcher ----------------
typedef struct {
	tListnode* fetchfrom;
	tListnode* fetchto;
	// `fetchto` points to point of stopping. Look into self.eof for more details.
} tLxFetcher;
tLxFetcher* mtLxFetcher_Create(tListnode* startpoint, tListnode* endpoint){
	tLxFetcher* ptr = malloc(sizeof(tLxFetcher));
	ptr->fetchfrom=startpoint;
	ptr->fetchto=endpoint;
	return ptr;
};
tLxFetcher* mtLxFetcher_Clone(tLxFetcher* self){
	return memcpy(malloc(sizeof(tLxFetcher)),self,sizeof(tLxFetcher));
};
bool mtLxFetcher_Eof(tLxFetcher* self){
	if(self->fetchfrom==self->fetchto){
		return true;
	}else{
		return false;
	}
};
tToken* mtLxFetcher_Peek(tLxFetcher* self){
	return self->fetchfrom->item;
}
tToken* mtLxFetcher_Peeklast(tLxFetcher* self){
	tListnode* i;
	for(
		i=self->fetchfrom;
		i->next!=self->fetchto;
		i=i->next
	);
	return i->item;
}
tLxFetcher* mtLxFetcher_Trimfirst(tLxFetcher* self){ // Recreates!
	return mtLxFetcher_Clone(
		&(tLxFetcher){
			.fetchfrom=self->fetchfrom->next,
			.fetchto=self->fetchto
		}
	);
};
tLxFetcher* mtLxFetcher_Trimlast(tLxFetcher* self){ // Recreates!
	tListnode* i;
	if(self->fetchfrom->next==self->fetchto){
		return mtLxFetcher_Clone(
			&(tLxFetcher){
				.fetchfrom=nullptr,
				.fetchto=nullptr
			}
		);
	};
	for(
		i=self->fetchfrom;
		i->next!=self->fetchto;
		i=i->next
	);
	return mtLxFetcher_Clone(
		&(tLxFetcher){
			.fetchfrom=self->fetchfrom,
			.fetchto=i
		}
	);
};
tToken* mtLxFetcher_Fetch(tLxFetcher* self){
	tToken* i = mtLxFetcher_Peek(self);
	if(mtLxFetcher_Eof(self)){
		//eof!
		return nullptr;
	}else{
		self->fetchfrom=self->fetchfrom->next;
	};
	return i;
};
bool mtLxFetcher_Advance(tLxFetcher* self){
	if(mtLxFetcher_Eof(self)){
		return false; // eof
	}else if(self->fetchfrom==nullptr){
		fprintf(stderr,"LX: [E] Advancing fetcher not at EOF with fetchfrom being nullptr \n");
		return false;
	}else{
		self->fetchfrom=self->fetchfrom->next;
		return true;
	};
};
tLxFetcher* mtLxFetcher_Fetchuntil_Variadic(tLxFetcher* fetcher, int tokenidcount,/*variadic tTokentype tokenid[tokenidcount]*/...){
	bool found = false;
	tLxFetcher* targetfetcher = malloc(sizeof(tLxFetcher));
	targetfetcher->fetchfrom=fetcher->fetchfrom;
	tListnode/*<tToken>*/* tempptr;
	for(
		tempptr = targetfetcher->fetchfrom;
		tempptr!=null;
		tempptr=tempptr->next
	){
		va_list args;
		va_start(args,tokenidcount);
		for(int index=0;index<tokenidcount;index++){
			tTokentype arg = va_arg(args,/*tTokentype*/int);
			if(arg==((tToken*)tempptr->item)->type){
				found = true;
			};
			if(found)break;
		};
		va_end(args);
		if(found){
			//found->((tToken*)tempptr->item)->type==va_arg(args,tTokentype)
			fetcher->fetchfrom=tempptr;
			targetfetcher->fetchto=tempptr;
			return targetfetcher;
		};
	};
	printf("DBG:[W] Unimplemented function mtLxFetcher_Fetchuntil_Variadic \n");
};
void mtLxFetcher_Print(tLxFetcher* self){
	printf("DBG:[T] . Fetcher %p print request \n",self);
	int j=0; 
	for(tListnode* i=self->fetchfrom;i!=self->fetchto;i=i->next){
		tToken* t=i->item;
		if(i->next==self->fetchto){
			printf("DBG:[T] ' Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		}else{
			printf("DBG:[T] | Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		};
		j++;
	};
};
void mtLxFetcher_Print_Limited(tLxFetcher* self){
	printf("DBG:[T] . Fetcher %p print request \n",self);
	int j=0; 
	for(tListnode* i=self->fetchfrom;(i!=self->fetchto)&&(j<10);i=i->next){
		tToken* t=i->item;
		if((i->next==self->fetchto)||(j>=10)){
			printf("DBG:[T] ' Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		}else{
			printf("DBG:[T] | Line %3i Token %2i: %3i:%s\n",t->linenumber,j,t->type,TokenidtoName[t->type]);
		};
		j++;
	};
};
tLxFetcher* mtLxFetcher_FetchuntilParenthesized(tLxFetcher* fetcher, tTokentype token){
#ifdef qvGTrace
	printf("LX: [T] mtLxFetcher_FetchuntilParenthesized: entered \n");
#endif
#ifdef qvGTraceexpressions
	printf("LX: [T] mtLxFetcher_FetchuntilParenthesized: printing fetcher \n");
	mtLxFetcher_Print_Limited(fetcher);
#endif
	bool found = false;
	tLxFetcher* targetfetcher = malloc(sizeof(tLxFetcher));
	targetfetcher->fetchfrom=fetcher->fetchfrom;
	tListnode /* <tToken> */ * tempptr = nullptr;
	int parenthesation = 0;
	for(
		tempptr = targetfetcher->fetchfrom;
		tempptr!=null;
		tempptr=tempptr->next
	){
		//printf("123: %i:%s ",((tToken*)tempptr->item)->type,token); 
		if(parenthesation<=0){
			if(((tToken*)tempptr->item)->type==token){
				found = true;
			};
		};
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
		if(found){
			//found->((tToken*)tempptr->item)->type==va_arg(args,tTokentype)
			fetcher->fetchfrom=tempptr;
			targetfetcher->fetchto=tempptr;
			return targetfetcher;
		};
		if(0)if(parenthesation<0){
			printf("LX: [W] mtLxFetcher_FetchuntilParenthesized: Broken parenthesation! \n");
			//break;
		};
	};
	printf("LX: [W] mtLxFetcher_FetchuntilParenthesized: Target token not found\n");
	return nullptr;
};

