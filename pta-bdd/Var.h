#ifndef _VAR_H
#define _VAR_H

#include "global.h"
#include "Heap.h"

class Var
{
public:
	Var( int id_ )
		: id( id_ )
		, type_id( -1 )
		, allowed_heaps( bddfalsepp )
		, pt_set( bddfalsepp )
		, pt_set_new( bddfalsepp )
	{ }

	struct topOrdGreaterThan
	{
		inline bool operator( ) ( Var * x, Var * y ) 
		{ return x->getTopOrd( ) > y->getTopOrd( ); }
	};
	
	//inline bool isInPtSet( Heap * h ) {return binary_search( pt_set.begin( ), pt_set.end( ), h); } // pre: pt_set is sorted
	
	bool insertPtSet( int heap_id );
	inline void deletePtSet( int heap_id ){ assert( heap_id >= 0 ); pt_set = bddfalsepp; }
	bool insertPtSet( bdd & rhs_pt_set );

	inline bdd & getPtSet( ){ return this->pt_set; }
	// FIXME decode bdd domain!!!
	inline uint getPtSetSize( ){ 
		uint sz; bdd_allsat(pt_set, decode_heap_domain); sz = decoded_heap_elements.size( ); decoded_heap_elements.clear( );  return sz; }
	inline uint getPtSetBddNodes( ) { return bdd_nodecount( pt_set ); }
	//inline void filterPtSet( ) { pt_set &= allowed_heaps; }
	//inline bool insertPtSet( BitVector & rhs_pt_set ) { return pt_set |= ( rhs_pt_set & allowed_heaps ); }
	void filterTypes( vector< Heap * > & heaps );

	bool insertPtSetNew( int heap_id );
	inline bool insertPtSetNew( bdd & rhs_pt_set );
	//inline bool insertPtSetNew( BitVector & rhs_pt_set ) { pt_set_new |= (rhs_pt_set - pt_set ); return pt_set_new &= allowed_heaps; }
	//inline bool insertPtSetNew( BitVector & rhs_pt_set ) { return pt_set_new |= ( ( rhs_pt_set & allowed_heaps ) - pt_set ); } // THIS IS WRONG!
	inline bool isEmptyPtSetNew( ) { return pt_set_new == bddfalsepp; }
	inline bdd & getPtSetNew( ){ return this->pt_set_new; }
	inline uint getPtSetNewSize( ){ 
		uint sz; bdd_allsat(pt_set_new, decode_heap_domain); sz = decoded_heap_elements.size( ); decoded_heap_elements.clear( );  return sz;  }
	inline bool ptSetContainsNew( ) {return ( ( pt_set_new & pt_set ) == pt_set_new); }
	inline void flushNewPtSet( ) { pt_set |= pt_set_new; pt_set_new = bddfalsepp; }

	inline void setTopOrd( int ord ) { top_ord = ord;  }
	inline int  getTopOrd( )         { return top_ord; } 
	inline void setTypeId( int type_id_ ) { assert( type_id == -1 ); type_id = type_id_; }
	inline int getId( ) { return id; }
	inline int getTypeId( ) { return type_id; }
	inline bool isTypeSet( ) { return type_id == -1 ? false : true;  }
	inline void insertAllowedHeaps( bdd & h ){ allowed_heaps |= h; }

	void printInfo( );
	void printPointsTo( );
	void printNewPointsTo( );

private:
	int id;
	int type_id;
	int top_ord;
	bdd allowed_heaps;
	bdd pt_set;
	bdd pt_set_new;
};

inline bool Var::insertPtSet( int heap_id )
{ 
	assert( heap_id >= 0 ); 
	bdd delta = ( fdd_ithvarpp( 0, heap_id ) /*& allowed_heaps */ ) - pt_set;
	if ( delta == bddfalsepp ) return false;
	pt_set |= delta;
	return true;
}

inline bool Var::insertPtSet( bdd & rhs_pt_set ) 
{ 
	bdd delta = ( rhs_pt_set & allowed_heaps ) - pt_set; 
	if (delta == bddfalsepp) return false; 
	pt_set |= delta; 
	return true;
}

inline bool Var::insertPtSetNew( int heap_id )
{ 
	assert( heap_id >= 0 ); 
	bdd delta = ( fdd_ithvarpp( 0, heap_id ) & allowed_heaps ) - pt_set_new;
	if ( delta == bddfalsepp ) return false;
	pt_set_new |= delta;
	return true;
}

inline bool Var::insertPtSetNew( bdd & rhs_pt_set ) 
{ 
	bdd delta = ( rhs_pt_set & allowed_heaps) - pt_set - pt_set_new; 
	if (delta == bddfalsepp) return false; 
	pt_set_new |= delta; 
	return true;
}

#endif
