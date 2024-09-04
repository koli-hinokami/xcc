typedef struct tPipe {
	char data[64];
	unsigned int writeptr,readptr;
} tPipe, *ptPipe;

void mtPipe_Clear(tPipe* pipe){
	for(int i=0;i<64;i++){pipe->data[i]=0;};
	pipe->writeptr=0;
	pipe->readptr=0;
	
};

size_t mtPipe_Enqueuedbytes(tPipe* pipe){
	return (pipe->writeptr-pipe->readptr)%64;
}

void mtPipe_Writechar(tPipe* pipe,unsigned char data){
	if((pipe->writeptr-pipe->readptr)%64==63){
		fprintf(stderr,"[W]:Pipe %p written while full! Ignoring write\n",pipe);
		return;
	};
	pipe->data[pipe->writeptr]=data;
	pipe->writeptr=(pipe->writeptr+1)%64;
};

char mtPipe_Readchar(tPipe* pipe){
	if((pipe->writeptr-pipe->readptr)%64==0){
		fprintf(stderr,"[W]:Pipe %p read while empty! Returning \'\\0\'==0\n",pipe);
		return 0;
	};
	char tmp=pipe->data[pipe->readptr];
	pipe->readptr=(pipe->readptr+1)%64;
	return tmp;
};

short mtPipe_Readword(tPipe* pipe){
	unsigned int tlo=mtPipe_Readchar(pipe);
	unsigned int thi=mtPipe_Readchar(pipe);
	return tlo+(thi>>8);
};

void mtPipe_Writeword(tPipe* pipe,unsigned short data){
	mtPipe_Writechar(pipe,data&255);
	mtPipe_Writechar(pipe,data>>8);
};





