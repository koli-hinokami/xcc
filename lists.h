/* // Koli's List library for pure C - actively using heap
 *
 */
#ifndef qvmtList_MfIncluded
#define qvmtList_MfIncluded
typedef struct tListnode {
	void* item;
	struct tListnode* next;
} tListnode;
typedef struct tList {
	tListnode* first;
	tListnode* last;
} tList, *ptList;


typedef struct {
	void* first;
	void* next;
	void* previous;
} tDoublylinkedlistnode;
typedef struct {
	tDoublylinkedlistnode* first;
	tDoublylinkedlistnode* last;
} tDoublylinkedlist;

tListnode* mtListnode_Cons(void* item,tListnode* next){
	tListnode* i = malloc(sizeof(mtListnode_Cons));
	i->item=item;
	i->next=next;
	return i;
}
tList* mtList_Create(){
	return calloc(1,sizeof(tList));
};
bool mtList_Isempty(ptList self){
	if(self->last == nullptr){ // List empty
		return true;
	}else{
		return false;
	};
};
void* mtList_GetFirstitem(ptList self){
	return self->first->item;
};
void mtList_Removefirst(ptList self){
	if(self->last == nullptr){ // List empty
		return;
	}else if(self->first == self->last){ // List contains one item
		self->last = nullptr;
		self->first = nullptr;
	}else{ // List normal - two or more items
		tListnode* i = self->first->next;
		free(self->first);
		self->first = i;
	};
}
void mtList_Prepend(ptList self, void* item){
	if(self->last == nullptr){ // List empty
		self->first = mtListnode_Cons(item,nullptr);
		self->last = self->first;
	}else if(self->first == self->last){ // List contains one item
		self->first = mtListnode_Cons(item,self->last);
	}else { // List normal - two or more items
		self->first = mtListnode_Cons(item,self->first);
	};
};
tList* mtList_Append(ptList self, void* item){
	if(self==nullptr){
		// who is the idiot who tries to append an item to nowhere
		printf("ul: [E] lists.h: mtList_Append: who is the idiot who tries to append an item to nowhere \n");
		return self;
	}else if(self->last == nullptr){ // List empty
		self->last = mtListnode_Cons(item,nullptr);
		self->first = self->last;
		return self;
	}else if(self->first == self->last){ // List contains one item
		self->last = mtListnode_Cons(item,nullptr);
		self->first->next = self->last;
		return self;
	}else { // List normal - two or more items
		self->last->next=/* nullptr -> */ mtListnode_Cons(item,nullptr); 
		// now list contains one more item and `last` points to next-to-last
		self->last=self->last->next;
		// now list is normal
		return self;
	};
	return self;
}
unsigned mtList_Count(ptList self){
	unsigned i=0;
	if(self==nullptr){
		return 0; // -1 maybe?
	}else if(self->last == nullptr){ // List empty
		return 0;
	}else if(self->first == self->last){ // List contains one item
		return 1;
	}else { // List normal - two or more items
		for(tListnode* ptr=self->first;ptr;ptr=ptr->next){
			i++;
		};
		return i;
	};
};
/* MmHeapobject */ tList* mtList_Transform(ptList self, void* (*lambda)(void*)){
	if(!self){
		printf("ul: [W] lists.h: mtList_Transform: self==nullptr \n");
		return nullptr;
	}else if(self->last == nullptr){ // List empty
		return mtList_Create();
	}else if(self->first == self->last){ // List contains one item
		return mtList_Append(mtList_Create(),lambda(self->first->item));	
		//lambda(self->first->item);
	}else { // List normal - two or more items
		tList* list = mtList_Create();
		for(tListnode* ptr=self->first;ptr!=nullptr;ptr=ptr->next){
			mtList_Append(list,lambda(ptr->item));
		};
		return list;
	};
};
void mtList_Foreach(ptList self, void(*lambda)(void*)){
	if(!self){
		printf("ul: [W] lists.h: mtList_Foreach: self==nullptr \n");
	}else if(self->last == nullptr){ // List empty
		return;
	}else if(self->first == self->last){ // List contains one item
		lambda(self->first->item);
	}else { // List normal - two or more items
		for(tListnode* ptr=self->first;ptr!=nullptr;ptr=ptr->next){
			lambda(ptr->item);
		};
	};
};
void mtList_Foreach_Lvalue(ptList self, void(*lambda)(void**)){
	if(!self){
		printf("ul: [W] lists.h: mtList_Foreach: self==nullptr \n");
	}else if(self->last == nullptr){ // List empty
		return;
	}else if(self->first == self->last){ // List contains one item
		lambda(&(self->first->item));
	}else { // List normal - two or more items
		for(tListnode* ptr=self->first;ptr!=nullptr;ptr=ptr->next){
			lambda(&(ptr->item));
		};
	};
};
void mtList_Destroy(ptList self){ // Doesn't destroys items in list!! 
                                  // Only list itself!!
	tListnode* ptr=self->first;
	tListnode* j;
	while(ptr){
		j=ptr->next; // `free(ptr);ptr=ptr->next;` except no use after free
		free(ptr);
		ptr=j;
	};
};
void* mtList_Find_Lambda(ptList self, bool(*lambda)(void* item)){
	if(self->last == nullptr){ // List empty
		return nullptr;
	}else if(self->first == self->last){ // List contains one item
		if(lambda(self->first->item)){
			return self->first->item;
		}else{
			return nullptr;
		};
	}else { // List normal - two or more items
		for(tListnode* ptr=self->first;ptr!=nullptr;ptr=ptr->next){
			if(lambda(ptr->item))return ptr->item;
		};
		return nullptr;
	};
};
//void* mtList_Find_Capturelesslambda(ptList self, struct{bool(*func)(void* args,void* item);void*args;}*lambda){
//	if(self->last == nullptr){ // List empty
//		return nullptr;
//	}else if(self->first == self->last){ // List contains one item
//		if(lambda->func(lambda->args,self->first->item)){
//			return self->first->item;
//		}else{
//			return nullptr;
//		};
//	}else { // List normal - two or more items
//		for(tListnode* ptr=self->first;ptr!=nullptr;ptr=ptr->next){
//			if(lambda->func(lambda->args,ptr->item))return ptr->item;
//		};
//		return nullptr;
//	};
//};

// --------------------- tListnode family of functions -----------------------

void mtListnode_Foreach(tListnode* self, void(*lambda)(void*)){
	for(tListnode* i=self;i!=nullptr;i=i->next)lambda(i->item);
};
tListnode* mtListnode_Findlast(tListnode* self){
	for(tListnode* i=self;i!=nullptr;i=i->next)if(i->next==nullptr)return i;
};
tListnode* mtListnode_Merge(tListnode* self, tListnode* arg){
	mtListnode_Findlast(self)->next=arg;
	return self;
};

//template<T> struct ttListnode {void* item;ttListnode<T> next;};
//template<T> struct ttList {ttListnode<T> first;ttListnode<T> last;};

#endif // qvmtList_MfIncluded

