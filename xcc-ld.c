#include "hyperheader.h"
// ----------------- xcc Crossdevelopment suite -- Linker -------------------
// --------- The one that links object files to binary executables ----------

// -- Types --

typedef enum LdArgpOptiontags {
	// This enumeration should have all the options passable
	eLdArgpOptiontags_Architecture = 'a',
	eLdArgpOptiontags_Confdir      = 'c',
	eLdArgpOptiontags_Outputfile   = 'o',
	eLdArgpOptiontags_Nostdlib     = 'f',
	eLdArgpOptiontags_Listing      = 'l',
} eLdArgpOptiontags;

typedef enum eAsmBinarytokensize {
	eAsmBinarytokensize_Lobyte = 1,
	eAsmBinarytokensize_Hibyte,
	eAsmBinarytokensize_Word,
	eAsmBinarytokensize_Label,
} eAsmBinarytokensize;

typedef struct {
	tGTargetSegment segment;
	tGTargetNearpointer offset;
	enum eAsmBinarytokensize size;
	int16_t displacement;
	tList /* <tLdRelocation> */ * relocations;
} tLdDataentry;

enum eAsmRelocationentrykind {
	eAsmRelocationentrykind_Terminator   = 0,
	eAsmRelocationentrykind_Segmentstart = 1,
	eAsmRelocationentrykind_Label        = 2,
};

typedef struct {
	enum eAsmRelocationentrykind kind;
	union {
		int segment;
		char* label;
	};
} tLdRelocation;

typedef enum eLdLinkerscriptentrykind {
	eLdLinkerscriptentrykind_Segment    = 1,
	eLdLinkerscriptentrykind_Align      = 2,
	eLdLinkerscriptentrykind_Pad        = 3,
	eLdLinkerscriptentrykind_Org        = 4,
	eLdLinkerscriptentrykind_Symbol     = 5,
	eLdLinkerscriptentrykind_Loadsymbol = 6,
} eLdLinkerscriptentrykind;
typedef struct {
	// taggedunion : enum eLdLinkerscriptentrykind type {
	// |   .segment : 
	// };
	enum eLdLinkerscriptentrykind type;
	union {
		tGTargetNearpointer offset;
		int segnumber;
		char* symbol;
	};
} tLdLinkerscriptentry;

enum eLdTokenkind {
	eLdTokenkind_Identifier = 1,
	eLdTokenkind_Charater,
	eLdTokenkind_Number,
};

typedef struct {
	enum eLdTokenkind type;
	char* str;
	int   number;
	char  ch;
} tLdToken;
typedef struct {
	char* /* owned */ name;
	tGTargetNearpointer location;
} tLdExternlabel;

// -- Preprocessor constants --

#define qiLdMaxsegments 16
#define qiLdMaxmodules  32

// -- Forward declarations --

error_t LdArgpParser(int key,char* argumentvalue,struct argp_state *state);

// -- Globals --

struct argp_option LdArgpOptions[] = {
	{	
		.name = "cfgdir",
		.key = eLdArgpOptiontags_Confdir,
		.arg = "confdir",
		.flags = null,
		.doc = "Directory to search for config files in."
		       "Defaults to /etc/xcc.",
		.group = null,
	},
	{	
		.name = "arch",
		.key = eLdArgpOptiontags_Architecture,
		.arg = "archname",
		.flags = null,
		.doc = "The architecture to compile assembly sources for. "
		       "Retargetablity first!",
		.group = null,
	},
	{	
		.name = "out",
		.key = eLdArgpOptiontags_Outputfile,
		.arg = "outputfile.obj",
		.flags = null,
		.doc = "Output object file",
		.group = null,
	},
	{	
		.name = "nostdlib",
		.key = eLdArgpOptiontags_Nostdlib,
		.arg = nullptr,
		.flags = null,
		.doc = "Don't link standard library",
		.group = null,
	},
	{	
		.name = "listing",
		.key = eLdArgpOptiontags_Listing,
		.arg = "listing.lst",
		.flags = null,
		.doc = "Listing file. Has all the labels in it.",
		.group = null,
	},
	{	// Terminator entry
		.name = nullptr,
		.key = null,
		.arg = nullptr,
		.flags = null,
		.doc = nullptr,
		.group = null,
	}
};
struct argp LdArgpParserstruct = {
	.options = LdArgpOptions,
	.parser = &LdArgpParser,    //error_t (*)(
							     //    int key,   // option's tag
							     //    char *arg, // option's value
							     //    struct argp_state *state
							     //)
	.args_doc = "file.asm"      // Usage
	          "\n-o out.obj file",
	.doc = 
		"Linker"
		"\n\v"
		""
		"\n",
	                             // If non-zero, a string containing extra text
	                             // to be printed before and after the options 
	                             // in a long help message, with the two 
	                             // sections separated by a vertical tab ('\v',
	                             // '\013') character. By convention, the 
	                             // documentation before the options is just a
	                             // short string explaining what the program 
	                             // does. Documentation printed after the 
	                             // options describe behavior in more detail.
	.children = nullptr,         // const struct argp_child *arr children
	.help_filter = nullptr,      // char*(*)(int key,char* text,void* input)
	.argp_domain = nullptr,      // wat
};

FILE* LdTargetfile;
char* LdArchitecturename;
char* LdConfigdir;
FILE* LdListingfile;

tList /* <tLdExternlabel> */ LdExportedsymbols;
tGTargetNearpointer LdCurrentposition,LdCurrentalternativeposition;
tList /* <tLdLinkerscriptentry> */ LdLinkerscript;
tList /* <FILE> */ LdSourcefiles;
bool LdNostandardlibrary;

tGTargetNearpointer LdSegmentstarts[qiLdMaxmodules][qiLdMaxsegments];
 // '- Here module 0 means current module

// -- Auxilirally functions --

int fpeekc(FILE* self){
	int ch = fgetc(self);
	if(ch==EOF){
		if(errno!=0)
			printf("u:  [E] fpeekc(FILE* %p): Error %i•\"%s\" while fetching charater\n",self,errno,strerror(errno));
	};
	return  ungetc(ch,self);
};

bool mtChar_LdIsTokenseparator(char ch){
	return isspace(ch);
};
tLdToken* mtLdToken_Fetch(FILE* stream){
	tLdToken* self = calloc(1,sizeof(tLdToken));
	while(isspace(fpeekc(stream)))fgetc(stream); // Skip whitespace
	if(fpeekc(stream)==EOF) return nullptr;
	if(fpeekc(stream)==';'){ // Comment
		for(int ch = ';';ch!='\n';ch=fgetc(stream)) if(ch==EOF)break;
		return mtLdToken_Fetch(stream);
	};
	if(mtChar_LdIsTokenseparator(fpeekc(stream))){
		self->type = eLdTokenkind_Charater;
		self->ch   = fgetc(stream);
	}else{
		self->type = eLdTokenkind_Identifier;
		char* str = mtString_Create();
		while(!mtChar_LdIsTokenseparator(fpeekc(stream)))
			mtString_Append(&str,(char[2]){fgetc(stream),0});
		self->str = str;
		if(isdigit(*str)){
			self->type=eLdTokenkind_Number;
			char* strtol_tail = nullptr;
			self->number = strtol(str,&strtol_tail,0);
		};
	};
	return self;
};
tGTargetNearpointer LdGetlabelvalue(char* /* takeown */ name){
	tLdExternlabel* self = nullptr;
	for(tListnode /* <tLdExternlabel> */ * i = LdExportedsymbols.first;i;i=i->next){
		if(strcmp(((tLdExternlabel*)i->item)->name,name)==0)
			self=i->item;
	};
	if(!self){
		printf("LD: [E] LdGetlabelvalue: Unresolved symbol \"%s\"\n",name);
		ErfError();
		return 0;
	}else{
		return self->location;
	};
};
void LdCreateexportedlabel(char* /* takeown */ name, tGTargetNearpointer position){
	printf("LD: [D] Exporting label %-25s:%.4x (%i)\n",name,position,position);
	if(LdListingfile){
		fprintf(LdListingfile,"%4x\t%s\n",position,name);
	};
	assert(name);
	tLdExternlabel* self = calloc(1,sizeof(tLdExternlabel));
	self->name = name;
	self->location = position;
	mtList_Prepend(&LdExportedsymbols,self);
};
// -- class tLdRelocation --

tLdRelocation* mtLdRelocation_CreateSegmentstart(int segment){
	tLdRelocation* self = malloc(sizeof(tLdRelocation));
	self->kind = eAsmRelocationentrykind_Segmentstart;
	self->segment = segment;
	return self;
};
tLdRelocation* mtLdRelocation_CreateLabel(char* label){
	tLdRelocation* self = malloc(sizeof(tLdRelocation));
	self->kind = eAsmRelocationentrykind_Label;
	self->label = label;
	return self;
};

tGTargetNearpointer LdGetrelocationvalue(tLdRelocation* self){
	ErfEnter_String("LdGetrelocationvalue");
	tGTargetNearpointer retval;
	assert(self);
	switch(self->kind){
		case eAsmRelocationentrykind_Segmentstart:
			assert(self->segment<qiLdMaxsegments);
			retval = LdSegmentstarts[0][self->segment];
			break;
		case eAsmRelocationentrykind_Label:
			retval = LdGetlabelvalue(self->label);
			break;
		default:
			printf("LD: [E] LdGetrelocationvalue: Unrecognized relocation type %i",
				(int)self->kind);
			ErfError();
			retval = 0;
			break;
	};
	ErfLeave();
	return retval;
};

// -- class tLdLinkerscriptentry --

tLdLinkerscriptentry* mtLdLinkerscriptentry_Fetchfromfile(FILE* stream){
	assert(false);
};
char* mtLdLinkerscriptentry_ToString(tLdLinkerscriptentry* self){
	return 
			self->type==eLdLinkerscriptentrykind_Segment
		?	mtString_Join(
				"segment ",
				mtString_FromInteger(self->segnumber)
			)
		:	self->type==eLdLinkerscriptentrykind_Org
		?	mtString_Format("org %x (%i)",self->offset,self->offset)
		:	self->type==eLdLinkerscriptentrykind_Symbol
		?	mtString_Format("symbol %s",self->symbol)
		:	self->type==eLdLinkerscriptentrykind_Loadsymbol
		?	mtString_Format("loadsymbol %s",self->symbol)
		:	mtString_Join(
				"(unk type ",
				mtString_Join(
					mtString_FromInteger(self->type),
					")"
				)
			);
		;
};

// -- Reading Linkerscript --

void LdReadlinkerscript(FILE* archdeffile){
	printf("LD: [D] Reading Linkerscript\n");
	for(;;){
		// Check for whitespace and end of file
		if(fpeekc(archdeffile)==EOF)return;
		while(isspace(fpeekc(archdeffile))) fgetc(archdeffile);
		if(fpeekc(archdeffile)==EOF)return;
		// Allocate Linkerscript entry
		tLdLinkerscriptentry* self = calloc(1,sizeof(tLdLinkerscriptentry));
		// Pull token that tells the class of Linkerscript entry
		tLdToken* tok;
		tok = mtLdToken_Fetch(archdeffile);
		if(!tok)break; // Unable to fetch a token == done
		// Check it's type
		assert(tok->type == eLdTokenkind_Identifier);
		if(strcmp(tok->str,"segment")==0){
			// segment <id>
			self->type = eLdLinkerscriptentrykind_Segment;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Number);
			self->segnumber = tok->number;
		}else if(strcmp(tok->str,"align")==0){
			// align val
			self->type = eLdLinkerscriptentrykind_Align;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Number);
			self->offset = tok->number;
		}else if(strcmp(tok->str,"pad")==0){
			// pad position
			self->type = eLdLinkerscriptentrykind_Pad;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Number);
			self->offset = tok->number;
		}else if(strcmp(tok->str,"org")==0){
			// org position
			self->type = eLdLinkerscriptentrykind_Org;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Number);
			self->offset = tok->number;
		}else if(strcmp(tok->str,"symbol")==0){
			// symbol name
			// Isn't mandatory though.
			self->type = eLdLinkerscriptentrykind_Symbol;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Identifier);
			self->symbol = mtString_Clone(tok->str);
		}else if(strcmp(tok->str,"loadsymbol")==0){
			// loadsymbol name
			// Isn't mandatory though.
			self->type = eLdLinkerscriptentrykind_Loadsymbol;
			tok = mtLdToken_Fetch(archdeffile);
			assert(tok->type == eLdTokenkind_Identifier);
			self->symbol = mtString_Clone(tok->str);
		}else{
			fprintf(stderr,"LD: [E] Unrecognized Linkerscript directive \"%s\"\n",tok->str);
			fprintf(stdout,"LD: [E] Unrecognized Linkerscript directive \"%s\"\n",tok->str);
			ErfFatal();
		};
		mtList_Append(&LdLinkerscript,self);
	};
};

// -- First pass --

void LdFirstpassfile(int currentsegment, FILE* srcfile){
	//printf("LD: [T] LdFirstpassfile: Entered\n");
	int i = 0;
	while(fpeekc(srcfile)!=EOF){
		//printf("LD: [T] LdFirstpassfile: Module %p entry %i\n",srcfile,i++);
		// Get segment
		int segment = fgetc(srcfile);
		assert(segment<qiLdMaxsegments);
		// Fetch size
		int size = fgetc(srcfile);
		// Check for label
		if(size!=eAsmBinarytokensize_Label){
			// Fetch displacement
			int16_t disp  = fgetc(srcfile);
					disp |= fgetc(srcfile)<<8;
			// Fetch relocations
			tList /* <tLdRelocation> */ relocs;
			memset(&relocs,0,sizeof(tList));
			while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
				switch(fgetc(srcfile)){
					case eAsmRelocationentrykind_Terminator:
						// wat
						assert(false);
						break;
					case eAsmRelocationentrykind_Segmentstart:
						// Relative to start of segment
						mtList_Append(
							&relocs,
							mtLdRelocation_CreateSegmentstart(
								fgetc(srcfile)
							)
						);
						break;
					case eAsmRelocationentrykind_Label:
						{
							char* str = mtString_Create();
							while(fpeekc(srcfile)!='\0') 
								mtString_Append(&str,
									(char[2]){fgetc(srcfile),0}
								);
							fgetc(srcfile); // consume the terminator
							mtList_Append(
								&relocs,
								mtLdRelocation_CreateLabel(str)
							);
						};
						break;
					default:
						// Something unrecognized
						assert(false);
						break;
				};
			// Verify that we get terminator
			if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
			// . Check if segment Linkerscript tells us to emit is what we
			// ' got on hands
			if(segment==currentsegment){
				// Emit
				switch(size){
					case eAsmBinarytokensize_Lobyte:
						LdCurrentposition+=1;
						LdCurrentalternativeposition+=1;
						break;
					case eAsmBinarytokensize_Hibyte:
						LdCurrentposition+=1;
						LdCurrentalternativeposition+=1;
						break;
					case eAsmBinarytokensize_Word:
						LdCurrentposition+=2;
						LdCurrentalternativeposition+=2;
						break;
					default:
						printf("LD: [E] LdFirstpassfile: "
						       "Unrecognized token size %i\n",
							   size
						);
						assert(false);
						break;
				};
			};
		}else{
			// Handle an exported label
			char* str = mtString_Create();
			while(fpeekc(srcfile)!='\0') 
				mtString_Append(&str,
					(char[2]){fgetc(srcfile),0}
				);
			fgetc(srcfile); // consume the terminator
			if(segment==currentsegment){
				LdCreateexportedlabel(str,LdCurrentposition);
			};
		};
	};
};

void LdFirstpass(tLdLinkerscriptentry* self){
	//printf("LD: [T] LdFirstpass(linkerscriptentry <%s>): Entered\n",
	//	mtLdLinkerscriptentry_ToString(self));
	switch(self->type){
		case eLdLinkerscriptentrykind_Segment:
			// Parse segment and generate exported symbols' position 
			// in that segment
			{
				int i = 1;
				for(tListnode* j = LdSourcefiles.first;j;j=j->next){
					assert(i<=qiLdMaxmodules);
					LdSegmentstarts[i++][self->segnumber]=LdCurrentposition;
					LdFirstpassfile(self->segnumber,j->item);
					rewind(j->item);
				};
			};
			break;
		case eLdLinkerscriptentrykind_Align:
			// Align to address with zeros.
			while(LdCurrentposition % self->offset != 0){
				LdCurrentposition            += 1;
				LdCurrentalternativeposition += 1;
			};
			break;
		case eLdLinkerscriptentrykind_Pad:
			// Pad to address with zeros.
			if(LdCurrentposition>self->offset){
				printf("LD: [T] LdFirstpass: `pad %x (%i)`: Unable to pad: Already over target address\n",self->offset,self->offset);
				ErfError();
			}else{
				unsigned offset = self->offset - LdCurrentposition;
				LdCurrentposition            += offset;
				LdCurrentalternativeposition += offset;
			};
			break;
		case eLdLinkerscriptentrykind_Org:
			// Change current position to argument provided
			// Alternative position stays the same.
			LdCurrentposition=self->offset;
			break;
		case eLdLinkerscriptentrykind_Symbol:
			// Change current position to argument provided
			// Alternative position stays the same.
			LdCreateexportedlabel(mtString_Clone(self->symbol),LdCurrentposition);
			break;
		case eLdLinkerscriptentrykind_Loadsymbol:
			// Change current position to argument provided
			// Alternative position stays the same.
			LdCreateexportedlabel(mtString_Clone(self->symbol),LdCurrentalternativeposition);
			break;
		default:
			printf("LD: [E] LdFirstpass: "
				   "Unrecognized Linkerscriptentry <%s> kind %i\n",
				   mtLdLinkerscriptentry_ToString(self),
				   (int)self->type
			);
			assert(false);
			break;
	};
};

// -- Second pass --

void LdSecondpassfile(int currentsegment, FILE* srcfile, FILE* dstfile){
	//printf("LD: [T] LdSecondpassfile(int currentsegment %i, FILE* src %p, FILE* dst %p): Entered\n",currentsegment,srcfile,dstfile);
	int i = 0;
	ErfEnter_String("LdSecondpassfile");
	while(fpeekc(srcfile)!=EOF){
#ifdef qvGTrace
		printf("LD: [T] LdSecondpassfile: Entry %i\n",i++);
#endif
		// Get segment
		ErfUpdate_String("LdSecondpassfile: Fetching segment");
		int segment = fgetc(srcfile);
		assert(segment<qiLdMaxsegments);
		// Fetch size
		ErfUpdate_String("LdSecondpassfile: Fetching size");
		int size = fgetc(srcfile);
		// Check for label
		if(size!=eAsmBinarytokensize_Label){
			ErfUpdate_String("LdSecondpassfile: Fetching displacement");
			// Fetch displacement
			int16_t disp  = fgetc(srcfile);
					disp |= fgetc(srcfile)<<8;
			// Fetch relocations
			ErfUpdate_String("LdSecondpassfile: Fetching relocations");
			tList /* <tLdRelocation> */ relocs;
			memset(&relocs,0,sizeof(tList));
			while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
				switch(fgetc(srcfile)){
					case eAsmRelocationentrykind_Terminator:
						// wat
						assert(false);
						break;
					case eAsmRelocationentrykind_Segmentstart:
						// Relative to start of segment
						mtList_Append(
							&relocs,
							mtLdRelocation_CreateSegmentstart(
								fgetc(srcfile)
							)
						);
						break;
					case eAsmRelocationentrykind_Label:
						{
							char* str = mtString_Create();
							while(fpeekc(srcfile)!='\0') 
								mtString_Append(&str,
									(char[2]){fgetc(srcfile),0}
								);
							fgetc(srcfile); // consume the terminator
							mtList_Append(
								&relocs,
								mtLdRelocation_CreateLabel(str)
							);
						};
						break;
					default:
						// Something unrecognized
						assert(false);
						break;
				};
			// Verify that we get terminator
			if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
			ErfUpdate_String("LdSecondpassfile: Applying relocations");
			// Pre-emit logging
#ifdef qvGTrace
			printf("LD: [T] LdSecondpassfile: Entry %-4i %02X:%04X (%-5i) %s %i(",
				i,segment,LdCurrentposition,LdCurrentposition,
				 size==eAsmBinarytokensize_Lobyte?"lobyte "
				:size==eAsmBinarytokensize_Hibyte?"hibyte "
				:size==eAsmBinarytokensize_Word  ?"word   "
				:"?",
				(int)disp
			);
			for(tListnode* i=relocs.first; i; i=i->next)
				printf("%i,",((tLdRelocation*)i->item)->kind);
			printf(")\n");
#endif
			// Apply relocations
			for(tListnode* i=relocs.first; i; i=i->next)
				disp += LdGetrelocationvalue(i->item);
			// . Check if segment Linkerscript tells us to emit is what we
			// ' got on hands
			ErfUpdate_String("LdSecondpassfile: Conditional emitting ");
			if(segment==currentsegment){
				ErfUpdate_String("LdSecondpassfile: Emitting ");
				// Emit
				switch(size){
					case eAsmBinarytokensize_Lobyte:
						fputc((uint8_t)disp,dstfile);
						LdCurrentposition+=1;
						LdCurrentalternativeposition+=1;
						break;
					case eAsmBinarytokensize_Hibyte:
						fputc((uint8_t)(disp>>8),dstfile);
						LdCurrentposition+=1;
						LdCurrentalternativeposition+=1;
						break;
					case eAsmBinarytokensize_Word:
						fputc((uint8_t)(disp),dstfile);
						fputc((uint8_t)(disp>>8),dstfile);
						LdCurrentposition+=2;
						LdCurrentalternativeposition+=2;
						break;
					default:
						printf("LD: [E] LdSecondpassfile: "
						       "Unrecognized token size %i\n",
							   size
						);
						assert(false);
						break;
				};
			};
		}else{
			ErfUpdate_String("LdSecondpassfile: Exported label ");
			// Handle an exported label
			while(fgetc(srcfile)!=0); // Consume and ignore the string
		};
	};
	ErfLeave();
};

void LdSecondpass(tLdLinkerscriptentry* self){
	//printf("LD: [T] LdSecondpass(linkerscriptentry <%s>): Entered\n",
	//	mtLdLinkerscriptentry_ToString(self));
	switch(self->type){
		case eLdLinkerscriptentrykind_Segment:
			// Emit a segment while applying relocations
			{
				int i = 1;
				for(tListnode* j = LdSourcefiles.first;j;j=j->next){
					assert(i<=qiLdMaxmodules);
					memcpy(
						LdSegmentstarts[0],
						LdSegmentstarts[i++],
						sizeof(*LdSegmentstarts)
					);
					LdSecondpassfile(self->segnumber,j->item, LdTargetfile);
					rewind(j->item);
				};
			};
			break;
		case eLdLinkerscriptentrykind_Align:
			// Align to address with zeros.
			while(LdCurrentposition % self->offset){
				fputc(0,LdTargetfile);
				LdCurrentposition            += 1;
				LdCurrentalternativeposition += 1;
			};
			break;
		case eLdLinkerscriptentrykind_Pad:
			// Pad to address with zeros.
			if(LdCurrentposition>self->offset){
				printf("LD: [T] LdSecondpass: `pad %x (%i)`: Unable to pad: Already over target address\n",self->offset,self->offset);
				ErfError();
			}else{
				unsigned offset = self->offset - LdCurrentposition;
				for(unsigned i=offset;--i;)
					fputc(0,LdTargetfile);
				LdCurrentposition            += offset;
				LdCurrentalternativeposition += offset;
			};
			break;
		case eLdLinkerscriptentrykind_Org:
			// Change current position to argument provided
			// Alternative position stays the same.
			LdCurrentposition=self->offset;
			break;
		case eLdLinkerscriptentrykind_Symbol:
		case eLdLinkerscriptentrykind_Loadsymbol:
			// Both Linkerscriptentries create a symbol, which isn't done 
			// in second pass.
			break;
		default:
			printf("LD: [E] LdSecondpass: "
				   "Unrecognized Linkerscriptentry <%s> kind %i\n",
				   mtLdLinkerscriptentry_ToString(self),
				   (int)self->type
			);
			assert(false);
			break;
	};
};

// -- Reading an object file --

//void LdReadfile(FILE* srcfile){ // Remnant of old architecture
//	// Read the file itself
//	int i = 0;
//	while(fpeekc(srcfile)!=EOF){
//		printf("LD: [T] Entry %i\n",i++);
//		// Get segment
//		int segment = fgetc(srcfile);
//		assert(segment<qiLdMaxsegments);
//		// Fetch size
//		int size = fgetc(srcfile);
//		// Check for label
//		if(size!=eAsmBinarytokensize_Label){
//			// Fetch displacement
//			int16_t disp  = fgetc(srcfile);
//					disp |= fgetc(srcfile)<<8;
//			// Fetch relocations
//			tList /* <tLdRelocation> */ relocs;
//			memset(&relocs,0,sizeof(tList));
//			while(fpeekc(srcfile)!=eAsmRelocationentrykind_Terminator)
//				switch(fgetc(srcfile)){
//					case eAsmRelocationentrykind_Terminator:
//						// wat
//						assert(false);
//						break;
//					case eAsmRelocationentrykind_Segmentstart:
//						// Relative to start of segment
//						mtList_Append(
//							&relocs,
//							mtLdRelocation_CreateSegmentstart(
//								fgetc(srcfile)
//							)
//						);
//						break;
//					default:
//						// Something unrecognized
//						assert(false);
//						break;
//				};
//			// Verify that we get terminator
//			if(fgetc(srcfile)!=eAsmRelocationentrykind_Terminator)assert(false);
//			// Create entry
//			tLdDataentry entry;
//			// Fill up the entry
//			memset(&entry,0,sizeof(tLdDataentry));
//			// | Set position
//			entry.offset = LdPositions[segment];
//			// | Advance position
//			LdPositions[segment]+=
//				 size==eAsmBinarytokensize_Lobyte?1
//				:size==eAsmBinarytokensize_Hibyte?1
//				:size==eAsmBinarytokensize_Word  ?2
//				:(assert(false),0);
//			// | Set size
//			entry.size = size;
//			// | Set displacement
//			entry.displacement = disp;
//			// | Set relocations
//			entry.relocations = mtList_Shallowclone(&relocs);
//			// Register in internal base
//			mtList_Append(
//				&(LdSegments[segment]),
//				memcpy(malloc(sizeof(tLdDataentry)),&entry,sizeof(tLdDataentry))
//			);
//		}else{
//			// Handle an exported label
//			assert(false);
//		};
//	};
//};

void LdCompileoutputbinary(void){
	fprintf(stdout,"LD: [D] Generating symbols \n");
	assert(false);
	fprintf(stdout,"LD: [D] Emitting binary\n");
	assert(false);
};

// -- Functions --

error_t LdArgpParser(int optiontag,char* optionvalue,struct argp_state *state){
	switch(optiontag){
		case eLdArgpOptiontags_Confdir:
			if(LdConfigdir){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdConfigdir=mtString_Clone(optionvalue);
			};
			break;
		case eLdArgpOptiontags_Architecture: {
			LdArchitecturename=mtString_Clone(optionvalue);
			// Open Linkerscript
			FILE* archdeffile = fopen(
				mtString_Join(
					LdConfigdir?:"/etc/xcc/",
					mtString_Join(
						optionvalue,
						"/archdef.lds"
					)
				),
				"r"
			);
			if(!archdeffile){
				printf(
					"LD: [F] Unable to open linkerscript for arch \"%s\": Error %i·%s\n",
					optionvalue,
					errno,
					strerror(errno)
				);
				ErfFatal();
			};
			// Read Linkerscript
			LdReadlinkerscript(archdeffile);
		};	break;
		case eLdArgpOptiontags_Outputfile:
			if(LdTargetfile){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdTargetfile = fopen(optionvalue,"wb");
				if(!LdTargetfile){
					printf("LD: [E] Unable to open output file \"%s\": %i•%s\n",
						optionvalue,errno,strerror(errno));
					ErfError();
					return 0;
				};
			};
			break;
		case eLdArgpOptiontags_Nostdlib:
			LdNostandardlibrary = true;
			return 0;
			break;
		case eLdArgpOptiontags_Listing:
			if(LdListingfile){
				return ARGP_ERR_UNKNOWN;
			}else{
				LdListingfile = fopen(optionvalue,"w");
				if(!LdTargetfile){
					printf("LD: [E] Unable to create listing file \"%s\": %i•%s\n",
						optionvalue,errno,strerror(errno));
					ErfError();
					return 0;
				};
			};
			break;
		case ARGP_KEY_ARG: {
			FILE* srcfile = fopen(optionvalue,"rb");
			if(!srcfile){
				printf("LD: [E] Unable to open source file \"%s\": %i•%s\n",
					optionvalue,errno,strerror(errno));
				ErfError();
				return 0;
			};
			mtList_Append(&LdSourcefiles,srcfile);
		};	break;
		case ARGP_KEY_END: // Create and emit output binary
			if(!LdNostandardlibrary){
				printf("LD: [D] Opening standard library\n");
				// Verify we know what crt0 and libc of existing to use
				assert(LdArchitecturename);
				// Load libc
				char* fname = 
					mtString_Join(
						"/etc/xcc/",
						mtString_Join(
							LdArchitecturename,
							"/libc.obj"
						)
					);
				FILE* file = fopen(fname,"rb");
				if(!file){
					printf("LD: [E] Unable to open libc file \"%s\": %i•%s\n",
						fname,errno,strerror(errno));
					ErfError();
				}else{
					mtList_Append(&LdSourcefiles,file);
				};
				// Load crt0
				fname = mtString_Join(
					"/etc/xcc/",
					mtString_Join(
						LdArchitecturename,
						"/crt0.obj"
					)
				);
				file = fopen(fname,"rb");
				if(!file){
					printf("LD: [E] Unable to open crt0 file \"%s\": %i•%s\n",
						fname,errno,strerror(errno));
					ErfError();
				}else{
					mtList_Append(&LdSourcefiles,file);
				};
			};
			// First pass on linker script entries - get addresses
			for(tListnode* i = LdLinkerscript.first;i;i=i->next)
				LdFirstpass(i->item);
			// Interpass
			LdCurrentposition = 0;
			for(tListnode* i = LdSourcefiles.first;i;i=i->next)
				rewind(i->item);
			// Second pass on linker script entries - emit while relocating
			for(tListnode* i = LdLinkerscript.first;i;i=i->next)
				LdSecondpass(i->item);
			// Finalize
			if(LdListingfile) // Vim modeline
				fprintf(LdListingfile,"\n;\tvim:tw=78:ts=8:noet:\n");
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
};
FILE* LdFileerrorfilter(FILE* self){
	if(self)return self;
	printf("LD: [E] LdFileerrorfilter: Error %i·%s\n",errno,strerror(errno));
	return nullptr;
};

// -- Main loop --

// -- Launcher aka init code --

void LnFailedassertionhandler(int signum){
	fprintf(stderr,"Ln: [F] Failed assertion catched! \n");
	ErfFatal();
};
void LnNullpointerhandler(int signum){
	fprintf(stderr,"Ln: [F] Segfault catched! \n");
	ErfFatal();
};
int main(int argc, char** argv){
	signal(SIGSEGV,LnNullpointerhandler);
	signal(SIGABRT,LnFailedassertionhandler);
	// Parse arguments
	argp_parse(
		&LdArgpParserstruct,
		argc,
		argv,
		0,
		0,
		0
	);
	// Today main loop is in LdArgpParser
	return 0;
};
