#ifndef PARSE_PAG
#define PARSE_PAG

#include "global.h"
#include "parser.h"

class ParsePAG
{
public:
	ParsePAG( ) 
		: max_var_id( 0 ), max_heap_id( 0 ), max_fld_id( 0 ), max_dec_type_id( 0 ), max_act_type_id( 0 )
		, alloc_rel( new Relation ( 2 ) ), assign_rel( new Relation( 2 ) ), store_rel( new Relation( 3 ) ), load_rel( new Relation( 3 ) )
		, decl_type( new Relation( 2 ) ), alloc_type( new Relation( 2 ) ), var_type( new Relation( 2 ) ) 
	{ }
	void parsePAGfile( const char * fname );

	inline int getMaxVarId( )     { return max_var_id;  }
	inline int getMaxHeapId( )    { return max_heap_id; }
	inline int getMaxFldId( )     { return max_fld_id;  }
	inline int getMaxDecTypeId( ) { return max_dec_type_id; }
	inline int getMaxActTypeId( ) { return max_act_type_id; }

	inline Relation * getAllocRel( )  { return alloc_rel;  }
	inline Relation * getAssignRel( ) { return assign_rel; }
	inline Relation * getStoreRel( )  { return store_rel;  }
	inline Relation * getLoadRel( )   { return load_rel;   }
	inline Relation * getDeclTypeRel( )  { return decl_type;  }
	inline Relation * getAllocTypeRel( ) { return alloc_type; }
	inline Relation * getVarTypeRel( )   { return var_type;   }
	
	inline void insertPtSets( int var_id, int heap_id ) { assert( (unsigned) var_id < pt_sets.size( ) ); pt_sets[ var_id ].set( heap_id ); }
	void parseSolutionFile( const char * fname );

	vector< BitVector > & getSparkPtSetsSol( ) { return this->pt_sets; }
	BitVector & getSparkPtSetSol( int var_id ) { assert( (unsigned) var_id < pt_sets.size( )  ); return pt_sets[ var_id ];}

	void printInfo( );

	~ ParsePAG( )
	{ 
		delete alloc_rel; delete assign_rel; delete store_rel; delete load_rel; 
		delete decl_type; delete alloc_type; delete var_type; 
	}

private:
	enum READ_STATE { START, ALLOC_DATA, ASSIGN_DATA, STORE_DATA, LOAD_DATA, DECL_TYPE, ALLOC_TYPE, ASSIGN_TYPE, SOLUTION };
	void processInputLine( string & input_line, READ_STATE read_state );
	void processAllocData(  istringstream & is );
	void processAssignData( istringstream & is );
	void processStoreData(  istringstream & is );
	void processLoadData(   istringstream & is );
	void processDeclType(   istringstream & is );
	void processAllocType(  istringstream & is );
	void processVarType(    istringstream & is );
	void processSolution(   istringstream & is );

	int max_var_id;
	int max_heap_id;
	int max_fld_id;
	int max_dec_type_id;
	int max_act_type_id;

	Relation * alloc_rel;
	Relation * assign_rel;
	Relation * store_rel;
	Relation * load_rel;
	Relation * decl_type;
	Relation * alloc_type;
	Relation * var_type;

	//map< int, BitVector > pt_sets;
	vector< BitVector > pt_sets;
};

#endif
