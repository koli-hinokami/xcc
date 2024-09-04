//1-----10--------20--------30--------40--------50--------60--------70-------80>

//                   ...........................
//                   :Koli's C macroses library:
//                   '''''''''''''''''''''''''''
//	Namespace:     Mf
//	 Subnamespace: MfMm

#define MfCreateinstance(T) calloc(sizeof(T),1)
#define MfCreateinstance_Uninitialized(T) malloc(sizeof(T))
#define MfCreateinstance_Zeroed(T) calloc(sizeof(T),1)


