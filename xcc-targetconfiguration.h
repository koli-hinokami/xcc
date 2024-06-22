// ----------- Target configuration file for xcc ---------------


const tGTargetSizet GTargetStackframeArgumentsstart = 4;
const tGTargetSizet GTargetStackframeLocalsstart = 0; // -2;
// For finetuning stackframes, go into Semanticparser::Allocateargumentstorage,
//                                                   ::Allocatelocalvarstorage,
//                             and some place inside Semanticparser that
//                              mentions `__xcc_variadic`.
const tGTargetSizet GTargetSizeofNearpointer = 2;
const tGTargetSizet GTargetSizeofFarpointer  = 4;

//		Jam-1 stackframe
//	+4	arguments
//	+2	return address
//	FP	prev. fp
//	-2	locals
//	SP

