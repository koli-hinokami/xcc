// Error reporiting facilities library

#ifdef __CYGWIN__
// Very dirty-hacked backtrace for Cygwin.
// Based on my intimate knowledge of stackframing on x86 from x86-16 to amd64
int backtrace(void** stacktracebuffer,int stacktracesize){
	void** fp;
	__asm__ __volatile__ (
		 "movq %%rbp, %%rax"
		:"=a"(fp)
		:"a"(0)
		:"memory"
	);
	int i;
	for(;fp;i++){
		printf("erf:[M] Backtracing: %p\n", fp);
		fp=*fp;
		stacktracebuffer[i]=fp;
		if(i>=stacktracesize)return i;
	};
	return i;
};
#endif

void ErfDumpstacktrace(){ // Stacktrace
	fprintf(stderr,"erf:[M] Stacktrace:\n");
	int stacktracesize=60; // backtrace(nullptr,0);
	void* stacktracebuffer[stacktracesize];
	stacktracesize = backtrace(stacktracebuffer,stacktracesize);
	char** stacktracesymbols = nullptr; // backtrace_symbols(stacktracebuffer,stacktracesize);
	for(int i=0;i<stacktracesize;i++){
		fprintf(stderr,"erf:[M]  • %i: %p∙%s\n",
			i,
			stacktracebuffer[i],
			(char*)nullptr // stacktracesymbols[i]
		);
	};
};
void ErfError(){
	fprintf(stderr,"erf:[E] Error occured!\n");
	ErfDumpstacktrace();
	fprintf(stderr,"erf:[E] (end)\n");
};
void ErfFatal(){
	fprintf(stderr,"erf:[F] Fatal error occured!\n");
	ErfDumpstacktrace();
	fprintf(stderr,"erf:[F] (end)\n");
};
