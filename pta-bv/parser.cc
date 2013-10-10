#include "parser.h"
#include <ctime>

using namespace std;

// FileNames
// ------------------------------------------------------------------------------------------------
void FileNames::setFileNames( vector < string > & relNames )
{
	vector< string >::iterator it;
	for ( it = relNames.begin( ); it != relNames.end( ); ++ it )
	{
		stringstream ss ( stringstream::in | stringstream::out );
		string relname = * it;
		ss << path <<	relname << ".facts";
		string fname = ss.str( );
		assert( fileExists( fname ) == true );
		this->rel2fname.insert( pair< string, string > ( relname, fname ) );
	}
}
// ------------------------------------------------------------------------------------------------

// Reference
// ------------------------------------------------------------------------------------------------
void Reference::loadFromFile( string & ref_file_name )
{
	ifstream ref_file( ref_file_name.c_str( ) );

	//vector<char> buffer;
	//stringstream local_stream;
	//createLocalStream( ref_file, buffer, local_stream );

	string line;
	//while ( getline ( local_stream, line ) )
	while ( getline ( ref_file, line ) )
		this->insertRef( line );
	ref_file.close( );
}

int Reference::insertRef( string & name  )
{
	int old_map_size = this->refMap.size( );
	this->refMap.GetOrCreateValue( StringRef( name ), refMap.size( ) );
	int mapSize = this->refMap.size( );
	if ( old_map_size < mapSize ) 
	{
		this->indMap.resize( mapSize );
		this->indMap[ mapSize - 1 ] = name;	
	}
	assert( indMap.size( ) == refMap.size( ) );
	return mapSize - 1;
}

void Reference::printRefs( ostream & os ) 
{
	Int2Ref::iterator it;
	for( it = indMap.begin( ); it != indMap.end( ); ++ it )
	{
		os << "index : " << it - indMap.begin( ) << " name : " << *it << endl;
	}
}

void Reference::printInfo( ostream & os )
{
	os << "-----------------------------------------------------\n";
	os << "[INFO] Reference    : " << this->refName << endl;
	os << "[INFO] Domain size  : " << this->getDomainSize( ) << endl;
	os << "[INFO] Encoding size: " << this->getLogEncSize( ) << endl; 
	os << "-----------------------------------------------------\n";
}
// ------------------------------------------------------------------------------------------------
void Relation::initRelation ( string & rel_file_name )
{
	ifstream rel_file( rel_file_name.c_str ( ) );

	//vector<char> buffer;
	//stringstream local_stream;
	//createLocalStream( rel_file, buffer, local_stream );

	string line;
	string  row[ this->arity ];
	Reference * ref;
	int elems[ this->arity ];
	Tuple * tuple;

	istringstream line_stream;
	string val_str;
	//while ( getline( local_stream, line ) ) 
	while ( getline( rel_file, line ) ) 
	{
		line_stream.str( line );
		//cout << "line in relFname :" << line << endl << endl;
		parseLine( line_stream, val_str, this->arity, row );
		line_stream.clear( );
		for ( int i = 0; i < this->arity; i++ )
		{
			ref = refs[i];
			elems[i] = ref->getRefMapIndex( row[i] );
			if ( elems[i] == -1 )
				elems[i] =ref->insertRef( row[i] );
		}
		tuple = new Tuple( this->arity );
		tuple->setElements( elems );
		this->relation.push_back( tuple );
	}
	sortAndUnique( );
	rel_file.close( );
}

void Relation::printInfo( ostream & os) 
{
	os << "-----------------------------------------------------\n";
	os << "[INFO] Relation       : " << this->relName << endl;
	os << "[INFO] Relation size  : " << this->getRelationSize( ) << endl;
	os << "-----------------------------------------------------\n";
}

void Relation::writeRelation( ostream & os, char separator )
{

	vector< Tuple * >::iterator it;
	for ( it = relation.begin( ); it != relation.end( ); ++ it )
	{
		Tuple			* tuple = * it;
		Reference * ref;
		int       i ;
		for (			i = 0; i< this->arity; ++ i ) {
			ref = refs [ i ];
			string elName = ref->getIndMapString( tuple->el [ i ] ) ;
			os << elName << separator;
		}
		for ( i = this->arity; i < this->noCols; ++ i )
			os << "main";
		os << endl;
	}
}

void Relation::addRandomTuples( int numTuples )
{
	
	int elems[ this->arity];
	srand( time( 0 ) );
	for ( int i = 0; i < numTuples; ++ i ){
		for ( int j = 0; j < this->arity; ++ j ) {
			Reference * ref = this->refs[ j ];	
			elems[ j ] = rand( ) % ref->getDomainSize( ) ;
		}
		Tuple * tuple = new Tuple( this->arity );
		tuple->setElements( elems );
		//cerr << "[DEBUG] inserting tuple " << tuple << endl;
		this->relation.push_back( tuple );
	}
	sortAndUnique( );
}

// ------------------------------------------------------------------------------------------------
/* adding random tuples to the relation
	ahpRel->addRandomTuples( 3*ahpRel->getRelationSize( ) );
	ofstream ahp_os ( "AssignHeapAllocation.facts", ios::out );
	ahpRel->writeRelation( ahp_os ); ahp_os.close( );

	allRel->addRandomTuples( 3*allRel->getRelationSize( ) );
	ofstream all_os ( "AssignLocal.facts", ios::out );
	allRel->writeRelation( all_os ); all_os.close( );
*/


/*
 * usage examples
int main( int argc, char * argv [] )
{
	if ( argc < 2 ) {
		cerr << "usage: " << argv[0] << " < path-to-facts > " << endl << endl;
		exit( 1 );
	}
	string path = string( argv[ 1 ] );
	FileNames * fileNames = new FileNames( path );
	string refRelations [] = { "VarRef", "NormalHeapAllocationRef", "AssignHeapAllocation", "AssignLocal" };
	//string refRelations [] = { "VarRef", "NormalHeapAllocationRef", "MethodSignatureRef" };
	vector < string > relNames ( refRelations, refRelations + sizeof( refRelations ) / sizeof( string ) );
	fileNames->setFileNames( relNames );
	
	cerr << "reading VarRef ..." << endl;
	Reference * varRef = new Reference( fileNames->getFullFileName( "VarRef" ), "VAR" );
	cerr << "reading NormalHeapAllocationRef ..." << endl;
	Reference * objRef = new Reference( fileNames->getFullFileName( "NormalHeapAllocationRef" ), "HEAP" );
	//Reference * sigRef = new Reference( fileNames->getFullFileName( "MethodSignatureRef" ) );

	vector< Reference * > refs; refs.push_back( objRef ); refs.push_back( varRef );
	cerr << "reading AssignHeapAllocation ... " << endl;
	InputRelation * ahp = new InputRelation( fileNames->getFullFileName("AssignHeapAllocation"), "AHP", refs, 3 );

	refs.clear( );
	refs.push_back( varRef ); refs.push_back( varRef );
	cerr << "reading AssignLocal ... " << endl;
	InputRelation * all = new InputRelation( fileNames->getFullFileName("AssignLocal"), "ALL", refs, 3 );

	cerr << "----- [ DEBUG ] --------" << endl;
	cerr << "varRef size : " << varRef->getRefMapSize( ) << endl;
	//varRef->printRefs( );
	cerr << "objRef size : " << objRef->getRefMapSize( ) << endl;
	//objRef->printRefs( );

	cerr << "relation AssignHeap size : " << ahp->getRelationSize( ) << endl;
	//ahp->printRelation( );
	
	cerr << "relation AssignLocal size : " << all->getRelationSize( ) << endl;
	//all->printRelation( );
	
	// boolean encoding prototyping
	cerr << "var domainSize : " << varRef->getDomainSize( ) << endl;
	cerr << "obj domainSize : " << objRef->getDomainSize( ) << endl;

	ahp->encodeRelation( );
	all->encodeRelation( );

	

	refs.clear( );
	refs.push_back( varRef ); refs.push_back( objRef );
	vector< InputRelation * > rels;
	rels.push_back( ahp ); rels.push_back( all );
	ofstream os ( "points-to.mu" );
	writeMucke( os, refs, rels );
	

	cerr << "[DONE]" << endl;
	return 0;
}
*/
