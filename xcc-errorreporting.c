// ----------------------- Error reporting library ------------------------- //

// Namespace:      Erf

typedef struct { 
	char* name;
} tErfStacktraceentry;
tList /* <tErfStacktraceentry> */ * ErfStacktrace;

void ErfEnterframe(char* name){
};
void ErfDumpcore(){
	printf("erf: [M] . Dumping core \n");
};
