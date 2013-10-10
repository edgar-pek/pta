#include "DumpDOOP.h"

void DumpDOOP::dumpInputFacts( )
{
	cerr << "Dumping input facts for DOOP ... " << endl;
	dumpAllocRel( );
	dumpAssignRel( );
	dumpStoreRel( );
	dumpLoadRel( );
	dumpDeclType( );
	dumpAllocType( );
	dumpVarType( );
	cerr << "[DONE] \n";
}

void DumpDOOP::dumpAllocRel( )
{
	const char * fname = "AssignHeapAllocation.facts";
	cerr << "\tDumping allocation relation to : " << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getAllocRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int heap_id = tuple->el[ 0 ];
		int var_id  = tuple->el[ 1 ];
		ofs << heap_id << '\t' << var_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

void DumpDOOP::dumpAssignRel( )
{
	const char * fname = "Assign.facts";
	cerr << "\tDumping assign relation to : "  << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getAssignRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int rhs_var_id = tuple->el[ 0 ];
		int lhs_var_id = tuple->el[ 1 ];
		ofs << rhs_var_id << '\t' << lhs_var_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

void DumpDOOP::dumpStoreRel( ) 
{
	const char * fname = "StoreInstanceField.facts";
	cerr << "\tDumping store field relation to : "  << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getStoreRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int rhs_var_id = tuple->el[ 0 ];
		int lhs_var_id = tuple->el[ 1 ];
		int fld_id     = tuple->el[ 2 ];
		ofs << rhs_var_id << '\t' << lhs_var_id << '\t' << fld_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

void DumpDOOP::dumpLoadRel( )
{
	const char * fname = "LoadInstanceField.facts";
	cerr << "\tDumping load field relation to : "  << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getLoadRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int rhs_var_id = tuple->el[ 0 ];
		int fld_id     = tuple->el[ 1 ];
		int lhs_var_id = tuple->el[ 2 ];
		ofs << rhs_var_id << '\t' << fld_id << '\t' << lhs_var_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

void DumpDOOP::dumpDeclType( )
{
	const char * fname = "DeclaredType.facts";
	cerr << "\tDumping declared type relation to : "  << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getDeclTypeRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int dec_type_id = tuple->el[ 0 ];
		int act_type_id = tuple->el[ 1 ];
		ofs << dec_type_id << '\t' << act_type_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

void DumpDOOP::dumpAllocType( )
{
	const char * fname = "AllocType.facts";
	cerr << "\tDumping alloc type relation to : "  << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getAllocTypeRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int heap_id     = tuple->el[ 0 ];
		int act_type_id = tuple->el[ 1 ];
		ofs << heap_id << '\t' << act_type_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

void DumpDOOP::dumpVarType( )
{
	const char * fname = "VarType.facts";
	cerr << "\tDumping var type relation to : "  << fname << " ... ";
	ofstream ofs( fname );
	vector< Tuple * > r = parse_pag->getVarTypeRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * tuple = * it;
		int var_id      = tuple->el[ 0 ];
		int dec_type_id = tuple->el[ 1 ];
		ofs << var_id << '\t' << dec_type_id << endl;
	}
	cerr << "[DONE] \n";
	ofs.close( );
}

