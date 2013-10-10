#include "ParsePAG.h"
#include <cctype>

void ParsePAG::parsePAGfile( const char * fname )
{
	ifstream pag_file( fname );
	READ_STATE read_state = START;
	string input_line;
	while( getline( pag_file, input_line ) )
	{
		if ( 0 == input_line.compare( "Allocations:" ) )
			read_state = ALLOC_DATA;
		else if ( 0 == input_line.compare( "Assignments:" ) )
			read_state = ASSIGN_DATA;
		else if ( 0 == input_line.compare( "Stores:") )
			read_state = STORE_DATA;
		else if ( 0 == input_line.compare( "Loads:" ) )
			read_state = LOAD_DATA;
		else if ( 0 == input_line.compare( "Declared Types:" ) )
			read_state = DECL_TYPE;
		else if ( 0 == input_line.compare( "Allocation Types:" ) )
			read_state = ALLOC_TYPE;
		else if ( 0 == input_line.compare( "Variable Types:" ) )
			read_state = ASSIGN_TYPE;
		else if ( input_line[0] == '#' )
			; // SKIP
		else if ( isdigit( input_line[0] ) )
			processInputLine( input_line, read_state );
		else 
			cerr << "unknown data: " << input_line << " in the input file. \n";
			
	}
	pag_file.close( );
}

void ParsePAG::processInputLine( string & input_line, READ_STATE read_state )
{
	istringstream is;
	switch( read_state )
	{
		case ALLOC_DATA :
			is.str( input_line );
			processAllocData( is );
			break;
		case ASSIGN_DATA :
			is.str( input_line );
			processAssignData( is );
			break;
		case STORE_DATA :
			is.str( input_line );
			processStoreData( is );
			break;
		case LOAD_DATA :
			is.str( input_line );
			processLoadData( is );
			break;
		case DECL_TYPE : 
			is.str( input_line );
			processDeclType( is );
			break;
		case ALLOC_TYPE :
			is.str( input_line );
			processAllocType( is );
			break;
		case ASSIGN_TYPE :
			is.str( input_line );
			processVarType( is );
			break;
		case SOLUTION :
			is.str( input_line );
			processSolution( is );
			break;
		default:
			cerr << "reading data " << input_line <<  " in illegal state : " << read_state << endl;
			break;
	}
}

void ParsePAG::processAllocData( istringstream & is )
{
	int heap_id, var_id;
	is >> heap_id >> var_id;
	assert ( is.eof( ) );

	int elems[2];
	elems[0] = heap_id; elems[1] = var_id;
	if ( heap_id > max_heap_id ) max_heap_id = heap_id;
	if ( var_id  > max_var_id  ) max_var_id  = var_id;

	Tuple * tuple = new Tuple( 2 ); tuple->setElements( elems );
	alloc_rel->addTuple( tuple );
}

void ParsePAG::processAssignData( istringstream & is )
{
	int rhs_var_id, lhs_var_id;
	is >> rhs_var_id >> lhs_var_id;
	assert( is.eof( ) );

	int elems[2];
	elems[0] = rhs_var_id; elems[1] = lhs_var_id;
	if ( rhs_var_id > max_var_id ) max_var_id = rhs_var_id;
	if ( lhs_var_id > max_var_id ) max_var_id = lhs_var_id;
	
	Tuple * tuple = new Tuple( 2 ); tuple->setElements( elems );
	assign_rel->addTuple( tuple );
}

void ParsePAG::processStoreData( istringstream & is )
{
	int rhs_var_id, lhs_var_id, fld_id;
	is >> rhs_var_id >> lhs_var_id >> fld_id;
	assert( is.eof( ) );

	int elems[3];
	elems[0] = rhs_var_id; elems[1] = lhs_var_id; elems[2] = fld_id;
	if ( rhs_var_id > max_var_id ) max_var_id = rhs_var_id;
	if ( lhs_var_id > max_var_id ) max_var_id = lhs_var_id;
	if ( fld_id > max_fld_id ) max_fld_id = fld_id;
	
	Tuple * tuple = new Tuple( 3 ); tuple->setElements( elems );
	store_rel->addTuple( tuple );
}

void ParsePAG::processLoadData( istringstream & is )
{
	int rhs_var_id, fld_id, lhs_var_id;
	is >> rhs_var_id >> fld_id >> lhs_var_id;
	assert( is.eof( ) );

	int elems[3];
	elems[0] = rhs_var_id; elems[1] = fld_id; elems[2] = lhs_var_id;
	if ( rhs_var_id > max_var_id ) max_var_id = rhs_var_id;
	if ( lhs_var_id > max_var_id ) max_var_id = lhs_var_id;
	if ( fld_id > max_fld_id ) max_fld_id = fld_id;

	Tuple * tuple = new Tuple( 3 ); tuple->setElements( elems );
	load_rel->addTuple( tuple );

}

void ParsePAG::processDeclType( istringstream & is )
{
	int dec_type_id, act_type_id;
	is >> dec_type_id >> act_type_id;
	assert( is.eof( ) );

	int elems[2];
	elems[0] = dec_type_id; elems[1] = act_type_id;
	if ( dec_type_id > max_dec_type_id ) max_dec_type_id = dec_type_id;
	if ( act_type_id > max_act_type_id ) max_act_type_id = act_type_id;

	Tuple * tuple = new Tuple( 2 ); tuple->setElements( elems );
	decl_type->addTuple( tuple );
}

void ParsePAG::processAllocType( istringstream & is )
{
	int heap_id, act_type_id;
	is >> heap_id >> act_type_id;
	assert( is.eof( ) );

	int elems[2];
	elems[0] = heap_id; elems[1] = act_type_id;
	if ( heap_id > max_heap_id ) max_heap_id = heap_id;
	if ( act_type_id > max_act_type_id ) max_act_type_id = act_type_id;

	Tuple * tuple = new Tuple( 2 ); tuple->setElements( elems );
	alloc_type->addTuple( tuple );
}

void ParsePAG::processVarType( istringstream & is )
{
	int var_id, dec_type_id;
	is >> var_id >> dec_type_id;
	assert( is.eof( ) );

	int elems[2];	elems[0] = var_id; elems[1] = dec_type_id;
	if ( var_id > max_var_id ) max_var_id = var_id;
	if ( dec_type_id > max_dec_type_id ) max_dec_type_id = dec_type_id;

	Tuple * tuple = new Tuple( 2 ); tuple->setElements( elems );
	var_type->addTuple( tuple );
}

void ParsePAG::processSolution( istringstream & is )
{
	int var_id, heap_id;
	is >> var_id >> heap_id;
	assert( is.eof( ) );
	insertPtSets( var_id, heap_id );
}

void ParsePAG::parseSolutionFile( const char * fname )
{
	pt_sets.resize( max_var_id + 1 );
	//cerr << "reading solution file : " << fname << " ";
	ifstream sol_file( fname );
	READ_STATE read_state = START;
	string input_line;
	while( getline( sol_file, input_line ) )
	{
		if ( 0 == input_line.compare( "Solution:" ) )
			read_state = SOLUTION;
		else if ( input_line[0] == '#' )
			;
		else if ( isdigit( input_line[0] ) )
			processInputLine( input_line, read_state );
		else 
		{
			cerr << "error while reading solution file : " << fname << "!" <<endl;
			exit( 1 );
		}
	}	
	sol_file.close( );
	cerr << " [DONE]\n";
	//cerr << "Solution size: " << pt_sets.size( ) << endl;
}


void ParsePAG::printInfo( )
{
	cerr << "ParsePAG info: " << endl;
	cerr << "------------------------------------------------" << endl;
	cerr << "max var id      : " << max_var_id << endl;
	cerr << "max heap id     : " << max_heap_id << endl;
	cerr << "max field id    : " << max_fld_id << endl;
	cerr << "max dec type id : " << max_dec_type_id << endl;
	cerr << "max act type id : " << max_act_type_id << endl;
	cerr << "alloc rel size  : " << alloc_rel->getRelationSize( ) << endl;
	cerr << "assign rel size : " << assign_rel->getRelationSize( ) << endl;
	cerr << "store rel size  : " << store_rel->getRelationSize( ) << endl;
	cerr << "load rel size   : " << load_rel->getRelationSize( ) << endl;
	cerr << "decl type size  : " << decl_type->getRelationSize( ) << endl;
	cerr << "alloc type size : " << alloc_type->getRelationSize( ) << endl;
	cerr << "var type size   : " << var_type->getRelationSize( ) << endl; 
	cerr << "-------------------------------------------------" << endl;
}
