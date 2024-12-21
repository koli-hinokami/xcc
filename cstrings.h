//Custom library for working with C-Strings - nullterminated byte strings
//The tString type is char* - however it is pretty much not used, cuz
//we all know char* == string, right?
//Most of subroutines are dynamic (in a sense of using heap storage) - I just
//overall like dynamic C.
#include <string.h>
#include "lists.h"
typedef char* tString;

char* mtString_Create(void){return calloc(sizeof(char),1);};
void mtString_Destroy(char* self){free(self);};
int mtString_Length(char* str){
	//I know there is <string.h>::strlen(char*){}.
	//But what about recompiling the CC to some new platform
	//without libc present?
	if(!str)return 0;// -1 maybe?
	char* ptr=str;
	for(ptr=str;(*ptr)!=0;ptr++){;};
	return (int)(ptr-str);
};
int mtString_Fulllength(char* str){
	return mtString_Length(str)+1;
};
char* mtString_Clone(char* src){
	if(src==nullptr)return nullptr;
	int len=mtString_Fulllength(src);
	char* dst = malloc(len);
	assert(dst);
	//if(!dst)return nullptr;
	memcpy(dst,src,len);
	return dst;
};
char* mtString_FindcharLast(char* src, char chr){
	for(char* ptr=src+mtString_Length(src)-1;ptr>=src;ptr--){
		if((*ptr)==chr)return ptr;
	}
	return 0;
}
char* mtString_FindcharFirst(char* src, char ch){
	for(char* ptr=src;ptr[0];ptr++){
		if((*ptr)==ch)return ptr;
	}
	return 0;
}
char* mtString_FindcharFirst_Lambda(char* src, bool(*lambda)(char)){
	for(char* ptr=src;*ptr;ptr++){
		if(0)printf("ul: [T] cstrings.h: mtString_FindcharFirst_Lambda: %i\n",
			lambda(*ptr)
		);
		if(lambda(*ptr))return ptr;
	}
	return 0;
}
char* mtString_FindcharFirst_Clojure(char* src, bool(*lambda)(void* clojureargs,char ch),void* clojureargs){
	for(char* ptr=src;*ptr;ptr++){
		if(lambda(clojureargs,*ptr))return ptr;
	}
	return 0;
}
char* mtString_Join(char* s1,char* s2){
	assert(s1);
	assert(s2);
	//Gives you ownership of dynamic memory
	char* str=malloc(mtString_Length(s1)+mtString_Length(s2)+1);
	assert(str);
	// libc way:
	//strcpy(str,s1);
	//strcat(str,s2);
	// My way:
	memcpy(str,s1,mtString_Length(s1));
	memcpy(str+mtString_Length(s1),s2,mtString_Fulllength(s2));
	return str;
};
void mtString_Append(char** s1,char* s2){
	//s1->: Pointer to variable: takeown's and frees residing string
	//s1<-                       creates new string and gives you ownership of
	//s2:   String:              borrows
	assert(s1);
	assert(*s1);
	assert(s2);
	char* str=mtString_Join(*s1,s2);
	free(*s1);
	*s1=str;
};
void mtString_Appendchar(char** s1,char s2){
	assert(s1);
	assert(*s1);
	char s3[2] = {s2,0};
	char* str=mtString_Join(*s1,s3);
	free(*s1);
	*s1=str;
};
bool mtString_Equals(char* s1,char* s2){
	int i=0;
	for(i=0;(s1[i]!=0)&&(s2[i]!=0);i++){
		if(s1[i]!=s2[i]){
			return false;
		}
	}
	if(s1[i]!=s2[i]){
		return false;
	};
	return true;
}
char* mtString_Last(char* str){ // Doesn't modifies
	if(mtString_Length(str)==0)return nullptr;
	return str+mtString_Length(str)-1;
};
char mtString_Getlast(char* str){
	assert(str);
	assert(mtString_Last(str));
	return mtString_Last(str)[0];
};
char* mtString_Trimlast(char* str){ //Modifies `str`
	if(mtString_Length(str)==0)return 0;
	str[mtString_Length(str)-1]=0;
	return str;
};
char* mtString_Trimfirst(char* str){ //Modifies `str`
	char* src=str+1;
	char* dst=str;
	if(mtString_Length(str)==0)return 0;
	for(;(*dst++=*src++););
	return str;
};
bool mtString_Contains(char* str, char ch){ // Doesn't modifies but excepts to not be modified `str`
	for(int i=0;str[i];i++)if(str[i]==ch)return true;
	return false;
};
bool mtString_Contains_Lambda(char* str, bool(*lambda)(char)){ 
	for(int i=0;str[i];i++)if(lambda(str[i]))return true;
	return false;
};
void mtString_Foreach_Clojure2_Precasttoint(
	char* str, 
	void(*clojure)(int ch, void* args), 
	void* args
){
	for(int i=0;str[i];i++)clojure((int)str[i],args);
};
// ------------ Higher-level routines ---------------
char* mtString_Trimtrailingspace(char* str){ // Modifies `str`
	while(isspace(mtString_Getlast(str)))mtString_Trimlast(str);
	return str;
};
char* mtString_Trimleadingspace(char* str){ // Modifies `str`
	while(isspace(str[0]))mtString_Trimfirst(str);
	return str;
};
tList /* <char*> */ * mtString_Splitat_Modify(char* str, char ch){
	tList* list = mtList_Create();
	mtList_Append(list,str);
	for(char* ptr=str;*ptr;ptr++){
		if(*ptr==ch){
			*ptr=0;
			mtList_Append(list,ptr+1);
		};
	};
	return list;
};
char* mtString_Removecharaters(char* str,char ch){
	if(!*str)
	for(char* ptr=str;*ptr;ptr++){
		if(*ptr==ch){
			mtString_Trimfirst(ptr);
			if(!*ptr)return str;
		};
	};
	return str;
};
char* /* deallocate */ mtString_Format(char* format, ...)
#ifdef __GNU_C__
__attribute__((format(printf,1,2)))
#endif
{
	va_list ap;                                  // Allocate varargs
	// Evaulate size
	va_start(ap,format);
	size_t size = vsnprintf(nullptr,0,format,ap); 
	va_end(ap);
	// Allocate buffer
	char* buffer = malloc(size+1);
	// Print into the buffer
	va_start(ap,format);
	vsnprintf(buffer,size+1,format,ap); 
	va_end(ap);
	// Return
	return buffer;
};
// --------------- Weird stuff -------------------
char* mtString_FromInteger(int self){
	char buffer[512];
	sprintf(buffer,"%i",self);
	return mtString_Clone(buffer);
};
// -- class char --
bool mtChar_IsKind_Libcgatewayclojure(void* /* int(*)(int) */ args, char self){
    return (*(int(*)(int))(args))(self);
};
