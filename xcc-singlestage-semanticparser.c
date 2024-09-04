// Semantic(al) parser

tSpNode* mtSpNode_Create(void){
	return calloc(sizeof(tSpNode),1);
};
tSpNode* mtSpNode_Clone(tSpNode* self){
	return memcpy(malloc(sizeof(tSpNode)),self,sizeof(tSpNode));
};

tSpNode* SpParse(tLxNode* self){ // Semantic parser primary driver
	if(!self){
		printf("SP: [E] SpParse: Nullpointer\n");
		return nullptr;
	}else{
		switch(self->type){
			case tLexem_Declarationlist:
				// Program root, pretty much
				return mtSpNode_Clone(
					&(tSpNode){
						.type=tSplexem_Declarationlist,
						.left=SpParse(self->left),
						.right=SpParse(self->right)
					}
				);
			default:
				printf("SP: [E] SpParse: unrecognized node %i:%s\n",self->type,TokenidtoName[self->type]);
					return nullptr;
		};
	};
};

