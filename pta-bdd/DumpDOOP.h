#ifndef DUMP_DOOP
#define DUMP_DOOP

#include "ParsePAG.h"

class DumpDOOP
{
public:
	DumpDOOP( ParsePAG * parse_pag_ ) 
		: parse_pag( parse_pag_ )
	{ }

	void dumpInputFacts( );
private:
	void dumpAllocRel( );
	void dumpAssignRel( ); 
	void dumpStoreRel( );  
	void dumpLoadRel( );   
	void dumpDeclType( );  
	void dumpAllocType( ); 
	void dumpVarType( );   

	ParsePAG * parse_pag;
};


#endif
