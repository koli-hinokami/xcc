// ----------- Symbol list generator ------------

void SgParse(tGNamespace* namespace, tLxNode* ast){
	if(ast==nullptr)return;
	if(namespace==nullptr)return;
	switch(ast->type){
		case tLexem_Declarationlist:
			// Program root, pretty much
			SgParse(namespace,ast->left);
			if(ast->right)SgParse(namespace,ast->right);
			break;
		case tLexem_Variabledeclaration:
			
			break;
		default:
			fprintf(stderr,"SG: [E] SgParse: Unrecognized node type \n");
			break;
	};
};


