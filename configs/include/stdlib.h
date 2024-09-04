char __MmMiniheap[8192];
int  __MmMiniheap_Current;
void* malloc(size_t size){
	void* area = __MmMiniheap+__MmMiniheap_Current;
	__MmMiniheap_Current+=size;
	return area;
};
void free(void* area){
	; //Ignore
};
