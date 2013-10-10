#ifndef HEAP_H
#define HEAP_H

#include "global.h"
//#include "Var.h"
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallSet.h>

using llvm::DenseMap;
using llvm::SmallSet;

//TODO: to save space, this could be a sparse bitvector, this could also be cached
extern vector< uint > decoded_heap_elements;
extern void decode_heap_domain( char * vset, int size );

class Heap
{
public:
	Heap( int id_ )
		: id( id_ )
	{ }
	inline int getId( ){ return this->id; }
	bool insertPtSet(    int fld_id, bdd & pt_set );
	bool insertPtSetNew( int fld_id, bdd & pt_set );
	//void deletePtSet( int fld_id ); todo
	inline bdd & getPtSet(    int fld_id ) {  return this->heap_pt_set[ fld_id ]; }
	inline bdd & getPtSetNew( int fld_id ) {  return this->heap_pt_set_new[ fld_id ]; }
	inline bool isPtSetNewEmpty( ) { return heap_pt_set_new.empty( ); }

	inline bool existsPtSet  ( int fld_id ) { return heap_pt_set_new.find( fld_id ) != heap_pt_set_new.end( ); }
	inline void flushNewPtSet( int fld_id );

	void printInfo( );
private:
	int id;
	//map< int, bdd > heap_pt_set;
	DenseMap< int, bdd > heap_pt_set;
	DenseMap< int, bdd > heap_pt_set_new; 
};

inline bool Heap::insertPtSet( int fld_id, bdd & pt_set )
{
	if ( heap_pt_set.find( fld_id ) == heap_pt_set.end( ) ) 
	{
		heap_pt_set.insert( pair< int, bdd > ( fld_id, pt_set) );
		return true;
	}
	else
	{ 
		bdd delta = pt_set - heap_pt_set[ fld_id ];
		if ( delta == bddfalsepp )
			return false;
		heap_pt_set[ fld_id ] |= delta;
		return true;
		//return heap_pt_set[ fld_id ] |= pt_set; 
	}
}

inline bool Heap::insertPtSetNew( int fld_id, bdd & pt_set )
{
	if ( heap_pt_set_new.find( fld_id ) == heap_pt_set_new.end( ) ) 
	{
		heap_pt_set_new.insert( pair< int, bdd > ( fld_id, pt_set) );
		return true;
	}
	else
	{
		bdd delta = pt_set - heap_pt_set_new[ fld_id ];
		if ( delta == bddfalsepp )
			return false;
		heap_pt_set_new[ fld_id ] |= delta;
		return true;
		//return heap_pt_set_new[ fld_id ] |= ( pt_set /* - heap_pt_set[ fld_id ] */ );
	}
}

inline void Heap::flushNewPtSet( int fld_id  )
{
	if ( heap_pt_set.find( fld_id ) == heap_pt_set.end( ) ) 
		heap_pt_set.insert( pair< int, bdd > ( fld_id, heap_pt_set_new[ fld_id ] ) );
	else
		heap_pt_set[ fld_id ] |= heap_pt_set_new[ fld_id ];
	
	heap_pt_set_new[ fld_id ] = bddfalsepp;
}

#endif
