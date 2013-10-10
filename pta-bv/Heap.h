#ifndef HEAP_H
#define HEAP_H

#include "global.h"
//#include "Var.h"
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallSet.h>

using llvm::DenseMap;
using llvm::SmallSet;

class Heap
{
public:
	Heap( int id_ )
		: id( id_ )
	{ }
	inline int getId( ){ return this->id; }
	void insertDeclaredTypes( BitVector sbv ) { declared_types |= sbv; }
	bool insertPtSet(    int fld_id, BitVector & pt_set );
	bool insertPtSetNew( int fld_id, BitVector & pt_set );
	//void deletePtSet( int fld_id ); todo
	inline BitVector & getPtSet(    int fld_id ) {  return this->heap_pt_set[ fld_id ]; }
	inline BitVector & getPtSetNew( int fld_id ) {  return this->heap_pt_set_new[ fld_id ]; }
	inline bool isPtSetNewEmpty( ) { return heap_pt_set_new.empty( ); }

	inline bool existsPtSet  ( int fld_id ) { return heap_pt_set_new.find( fld_id ) != heap_pt_set_new.end( ); }
	inline void flushNewPtSet( int fld_id );

	bool isDeclaredType( int type_id ) { return declared_types.test( type_id ); }

	void printInfo( );
private:
	int id;
	//map< int, BitVector > heap_pt_set;
	BitVector declared_types;
	DenseMap< int, BitVector > heap_pt_set;
	DenseMap< int, BitVector > heap_pt_set_new; 
};

inline bool Heap::insertPtSet( int fld_id, BitVector & pt_set )
{
	if ( heap_pt_set.find( fld_id ) == heap_pt_set.end( ) ) 
	{
		heap_pt_set.insert( pair< int, BitVector > ( fld_id, pt_set) );
		return true;
	}
	else
		return heap_pt_set[ fld_id ] |= pt_set;
}

inline bool Heap::insertPtSetNew( int fld_id, BitVector & pt_set )
{
	if ( heap_pt_set_new.find( fld_id ) == heap_pt_set_new.end( ) ) 
	{
		heap_pt_set_new.insert( pair< int, BitVector > ( fld_id, pt_set) );
		return true;
	}
	else
	{
		return heap_pt_set_new[ fld_id ] |= ( pt_set /* - heap_pt_set[ fld_id ] */ );
	}
}

inline void Heap::flushNewPtSet( int fld_id  )
{
	if ( heap_pt_set.find( fld_id ) == heap_pt_set.end( ) ) 
		heap_pt_set.insert( pair< int, BitVector > ( fld_id, heap_pt_set_new[ fld_id ] ) );
	else
		heap_pt_set[ fld_id ] |= heap_pt_set_new[ fld_id ];
	
	heap_pt_set_new[ fld_id ].clear( );
}

#endif
