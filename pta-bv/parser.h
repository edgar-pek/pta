#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include <llvm/ADT/StringMap.h>

using llvm::StringRef;
using llvm::StringMapEntry;
using llvm::StringMap;

#include <sys/stat.h>

// TODO: figure out how to make this is more clear instead of just naming it static
// ------------------------------------------------------------------------------------------------
static int getDomainBitsNum( unsigned max_val ) {
	int i = 0;
	for ( i = 0; i < 32; i ++ ){
		unsigned msb = max_val & 0x80000000;
		if ( msb > 0 )
			break;
		max_val = max_val << 1;
	}
	return (32 - i);
}

static int numBits( unsigned size ) {
	assert ( size > 0 );
	if ( size == 1 )
		return 1;
	return getDomainBitsNum( size - 1 );
}

/* inline void createLocalStream( ifstream & file, vector< char > & buffer, stringstream & local_stream )
{
	file.seekg(0, std::ios::end);
	std::streampos length = file.tellg( );
	file.seekg(0, std::ios::beg);
	buffer.resize( length );
	file.read( &buffer[0], length );
	local_stream.rdbuf( )->pubsetbuf( &buffer[0], length );
}
*/

// ------------------------------------------------------------------------------------------------

// Handling file names in the facts path
// ----------------------------------------------------------------------------------
struct FileNames
{
	// constructor - argument is the path to the facts
	FileNames( string & path_ ) 
		: path ( path_ )
	{ }

	inline bool fileExists( string & fname ){	struct stat stat_file_info; return ( stat( fname.c_str( ), & stat_file_info ) == -1 ) ? false : true; }
	void setFileNames( vector < string > &  );
	inline string & getFullFileName( const char * relname ){	assert( rel2fname.find( string( relname ) ) != rel2fname.end ( ) );  return rel2fname.find( string( relname ) )->second;	}

	string path;
	map < string, string > rel2fname;
};
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------
// Reference data structure - storing values of the domain
// --------------------------------------------------------------------------------------
class Reference
{
public:	
	// constructor - argument is the name of the file
	Reference ( string & refFName, const char * refName_ ) 
		: refName( string (refName_) )
	{ 
		//this->loadFromFile( refFName );	
	}
	
	void loadFromFile( string & ref_file_name );
	int		insertRef( string & ); 
	void	setLogEncSize( )				{ this->logEncSize = numBits( this->indMap.size( ) ); 	}

	// get
	inline int			getRefMapIndex( string name )	{ Ref2Int::iterator it = this->refMap.find( StringRef( name ) );return  ( it == this->refMap.end( ) ) ? -1 : it->second ;	}
	inline string & getName( int index) { return this->getIndMapString( index); }
	inline string & getIndMapString( int index )	{ return this->indMap[ index ]; }
	inline vector< string > & getId2Name( ) { return this->indMap; }
	inline size_t		getDomainSize( )							{ return refMap.size( ); }
	inline int			getRefMapSize( )							{ return refMap.size( ); }
	inline int			getIndMapSize( )							{ return indMap.size( ); }
	inline unsigned getLogEncSize( )							{ this->setLogEncSize( ); return this->logEncSize; }

	// auxilliary
	void printRefs( ostream & );
	void printInfo( ostream & );

	~Reference ( ) { }

	//typedef std::unordered_map<    string, int > Ref2Int;
	typedef StringMap< int > Ref2Int;
	typedef vector< string > Int2Ref;

protected:
	string		refName;				// name of the reference
	Ref2Int		refMap;					// map : string -> int
	Int2Ref		indMap;					// map : int    -> string
	int				logEncSize;     // size of the domain ( log_2[refMap.size] )
private:
	Reference( const Reference & );             // prevent pass-by-value
	Reference & operator=( const Reference & ); // prevent assignment
};
// ---------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------
// Tuples representation
// ---------------------------------------------------------------------------------------------------------------
struct Tuple 
{
	// constructor 
	Tuple( int arity_ ) 
		:  arity ( arity_ )
		,  el    ( new int[this->arity] )
	{}

	void setElements( int * el_ ){ for ( int i = 0; i < this->arity; ++ i ) el[ i ] = el_[i] ;	}

	void  printTuple( ostream & );
	inline friend ostream & operator<<( ostream & os, Tuple * tuple ){ assert( tuple ); tuple->printTuple( os ); return os; }

	inline bool operator == ( const Tuple & x ) const
	{
		assert ( x.arity == arity );
		for ( int i = 0; i < arity; i++ ) 
			if ( x.el[ i ] == el[ i ] ) continue;
			else return false;
		return true;
	}

	// destructor
	~Tuple ( ){ delete [] el; } 

	int arity;	// arity of the relation
	int * el;		// elements of the relation
};
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// relation representation
// ------------------------------------------------------------------------------------------------
class Relation 
{
public:
	Relation( string								 & relFName, 
						const char						 * relName_, 
						vector < Reference * > & refs_, 
						int										 noCols_ 
					) 
		: arity(   refs_.size( ) )
		, noCols(  noCols_ )
		, refs(    refs_ )
		, relName( string( relName_ ) )
	{
		initRelation( relFName );
	}
	// copy constructor
	Relation( Relation & r )
		: arity(  r.refs.size( ) )
		, noCols( r.noCols )
	{ 
		this->setRefs( r.refs );
		this->setRelation( r.relation );
		this->setRelName( r.relName );
	}
	Relation ( int arity_ ) 
		: arity( arity_ )
	{ }

	inline void addTuple( Tuple * t ) { assert( t->arity == this->arity );  this->relation.push_back( t ); }
	inline void setRefs(     vector< Reference * > & r ) { this->refs = r; }
	inline void setRelation( vector< Tuple * >        & r ) { this->relation = r; }
	inline void setRelName(  string                & s ) { this->relName = s; }
	// get methods
	inline int              getRelationSize( ){ return this->relation.size( ); }
	inline vector< Tuple * > & getRelation( ) { return this->relation; };

	// auxilliary
	void printRelation( ) { vector< Tuple * > :: iterator it;	for ( it = relation.begin( ); it != relation.end( ); ++ it )	cout << * it << endl;	}
	void printInfo		( ostream & );
	void writeRelation( ostream &, char separator = '\t' );
	void addRandomTuples( int );
	
	void clearRelation( ) { relation.clear( ); }
	~Relation( ) {	}

protected:
	int										 arity;		// arity of the relation
	int										 noCols;	// the number of columns in the file
	vector < Reference * > refs;		// domain of the relation: refs[0] x refs[1] x refs[2] x ... refs[n] 
	vector < Tuple * >		 relation;	// tuples in the relation
	string								 relName;		// name of the relation
private:
	Relation & operator=( const Relation & ); // prevent assignment

	void initRelation( string & );
	void sortAndUnique( ); // inlined below
	void parseLine( istringstream & line_stream, string & val_str, int arity, string * row ); // inlined below
};
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
//                                          INLINED METHODS
// ------------------------------------------------------------------------------------------------
inline void Relation::sortAndUnique( )
{
	sort( relation.begin( ), relation.end( ) ); 
	vector< Tuple * >::iterator it = unique( relation.begin( ), relation.end( ) );
	relation.resize( it - relation.begin( ) );
}

// Tuple
inline void Tuple::printTuple ( ostream & os ) {
	os << "( ";
	for (int i = 0; i < this->arity; ++ i ) {
		os << el[ i ];
		if ( i == ( this->arity - 1 ) ) os << " )";
		else os << ", ";
	}
}

// InputRelation
inline void Relation::parseLine( istringstream & line_stream, string & val_str, int arity, string * row ) 
{
	for ( int i = 0; i < this->noCols; i ++ )
	{
		getline( line_stream, val_str, '\t' );
		if ( i < arity )
			row[i] =  val_str ;
	}
}


#endif
