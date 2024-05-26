/* // Koli's Dictionary library for pure C - heap-oriented
 *
 */

#ifndef qvListlibraryIncluded
#define qvListlibraryIncluded
typedef intptr_t tDictionaryentry_Key;
typedef intptr_t tDictionaryentry_Value;
typedef struct {
	tDictionaryentry_Key   key;
	tDictionaryentry_Value value;
} tDictionaryentry;
typedef tDictionaryentry tArraydictionary;
typedef struct tListdictionarynode /* : tListnode */{
	tDictionaryentry* item;
	struct tListdictionary* next;
} tListdictionarynode;
typedef struct tListdictionary /* : tList */ {
	tListdictionarynode* first;
	tListdictionarynode* last;
} tListdictionary;


// ----------------- tListdictionary ---------------------
bool mtListdictionary_Checkmatching(
	tDictionaryentry_Key key,
	tDictionaryentry* self
){
	if(self->key==key){
		return true;
	};
	return false;
};
tDictionaryentry_Value mtListdictionary_Getitem(
	tListdictionary* self,
	tDictionaryentry_Key key
){
	return (
		(tDictionaryentry*)
		mtList_Find_Clojure(
			(tList*)self,
			(bool(*)(void*, void*))mtListdictionary_Checkmatching,
			(void*)key
		)
	)->value;
};
bool mtListdictionary_Checkmatching_Clojure(
	struct { 
		tDictionaryentry_Key key;
		bool(*clojure)(
			void* args, 
			tDictionaryentry_Key key, 
			tDictionaryentry_Key target
		);
		void* clojureargs;
	} * args,
	tDictionaryentry* self
){
	if(
		args->clojure(args->clojureargs,args->key,self->key)
	){
		return true;
	};
	return false;
};
tDictionaryentry_Value mtListdictionary_Getitem_Clojure(
	tListdictionary* self,
	tDictionaryentry_Key key,
	bool(*func)(void* clojureargs,tDictionaryentry_Key key,tDictionaryentry_Key target),
	void* clojureargs
){
	return (
		(tDictionaryentry*)
		mtList_Find_Clojure(
			(tList*)self,
			(bool(*)(void*,void*))mtListdictionary_Checkmatching_Clojure,
			&(
				struct {
					tDictionaryentry_Key key;
					bool(*clojure)(
						void* args, 
						tDictionaryentry_Key key, 
						tDictionaryentry_Key target
					);
					void* clojureargs;
				}
			){
				.key=key,
				.clojure=func,
				.clojureargs=clojureargs,
			}
		)
	)->value;
};
// ----------------- tArraydictionary --------------------
tDictionaryentry_Value mtArraydictionary_Getitem(
	tArraydictionary* self, 
	tDictionaryentry_Key item
){
	printf("ul: [F] dictionary.h: Unimplemented function mtArraydictionary_Getitem\n");
	fprintf(stderr,"ul: [F] dictionary.h: Unimplemented function mtArraydictionary_Getitem\n");
	exit(1);
};

#endif // qvListlibraryIncluded
