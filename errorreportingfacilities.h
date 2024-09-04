// Error reporiting facilities library

#include "lists.h"
#ifndef __CYGWIN__
#include "execinfo.h"
#endif //  __CYGWIN__

// -------------------- Types --------------------
typedef struct {
	char* /* __needs_free */ string;
} tErfVframe;
// ------------------- Globals -------------------
tList /* <tErfVframe> */ ErfVframes;

// ---------------- Class methods ----------------

tErfVframe* mtErfVframe_Create(){
	return calloc(sizeof(tErfVframe),1);
};
void mtErfVframe_Deallocate(tErfVframe* self){
	free(self);
};
void mtErfVframe_Destroy(tErfVframe* self){
	free(self->string);
	mtErfVframe_Deallocate(self);
};
tErfVframe* mtErfVframe_Create_String(char* /* takeowns */ str){
	tErfVframe* i = mtErfVframe_Create();
	i->string = str;
	return i;
};

// ------------------ Functions ------------------

void ErfDumpvframe(int count, tErfVframe* frame){
	fprintf(stderr,"erf:[M]  • %i: %p•%s\n",
		count,
		frame,
		frame->string
	);
};
void ErfDumpstacktrace(){ // Stacktrace
	// Virtual stackframes - frames wound-up by program
	fprintf(stderr,"erf:[M] Virtual stacktrace:\n");
	int j=0;
	for(tListnode* i=ErfVframes.first;i;i=i->next){
		ErfDumpvframe(j,i->item);
		j++;
	};
	//mtList_Foreach(&ErfVframes, (void(*)(void*))ErfDumpvframe);
	fprintf(stderr,"erf:[M] (end virtualstacktrace)\n");
#ifndef __CYGWIN__
	fprintf(stderr,"erf:[M] Native stacktrace:\n");
	int stacktracesize=60; // backtrace(nullptr,0);
	void* stacktracebuffer[stacktracesize];
	stacktracesize = backtrace(stacktracebuffer,stacktracesize);
	char** stacktracesymbols = backtrace_symbols(stacktracebuffer,stacktracesize);
	for(int i=0;i<stacktracesize;i++){
		fprintf(stderr,"erf:[M]  • %i: %p∙%s\n",
			i,
			stacktracebuffer[i],
			stacktracesymbols?stacktracesymbols[i]:nullptr
		);
	};
	fprintf(stderr,"erf:[M] (end nativestacktrace)\n");
#endif
};
void ErfEnter_String(char* /* borrows */ string){
	mtList_Prepend(&ErfVframes,
		mtErfVframe_Create_String(
			mtString_Clone(string)
		)
	);
};
void ErfLeave(void){
	mtErfVframe_Destroy(mtList_GetFirst(&ErfVframes));
	mtList_Removefirst(&ErfVframes);
};
void ErfWarning(){
	fprintf(stderr,"erf:[W] Warning thrown\n");
	ErfDumpstacktrace();
	fprintf(stderr,"erf:[W] (end errorlog)\n");
};
void ErfError(){
	fprintf(stderr,"erf:[E] Error occured!\n");
	ErfDumpstacktrace();
	fprintf(stderr,"erf:[E] (end errorlog)\n");
	exit(1);
};
void ErfFatal_String(char* /* borrows*/ string){
	fprintf(stderr,"erf:[F] Fatal error occured: %s\n",string);
	ErfDumpstacktrace();
	fprintf(stderr,"erf:[F] (end errorlog)\n");
	exit(1);
};
void ErfFatal(){
	fprintf(stderr,"erf:[F] Fatal error occured!\n");
	ErfDumpstacktrace();
	fprintf(stderr,"erf:[F] (end errorlog)\n");
	exit(1);
};
void* ErfCatchnullptr(void* self){
	if(!self){
		fprintf(stderr,"erf:[F] ErfCatchnullptr: Nullpointer dereference caught!\n");
		ErfFatal();
	};
	return self;
};
