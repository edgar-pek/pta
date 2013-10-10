#ifndef VAR_H
#define VAR_H

#include "global.h"
#include "Heap.h"
#include <llvm/ADT/SparseBitVector.h>

using llvm::SparseBitVector;
using llvm::SparseBitVectorElement;

class Var
{
public:
	Var( int id_ )
		: id( id_ )
		, type_id( -1 )
	{ }

	struct topOrdGreaterThan
	{
		inline bool operator( ) ( Var * x, Var * y ) 
		{ return x->getTopOrd( ) > y->getTopOrd( ); }
	};
	
	//inline bool isInPtSet( Heap * h ) {return binary_search( pt_set.begin( ), pt_set.end( ), h); } // pre: pt_set is sorted
	
	inline bool insertPtSet( int heap_id ){ assert( heap_id >= 0 ); return pt_set.test_and_set( heap_id ); }
	inline void deletePtSet( int heap_id ){ assert( heap_id >= 0 ); pt_set.reset( heap_id ); }
	inline bool insertPtSet( BitVector & rhs_pt_set ) { 
		return pt_set |= ( rhs_pt_set & allowed_heaps ); }
	inline BitVector & getPtSet( ){ return this->pt_set; }
	inline uint getPtSetSize( ){ return pt_set.count( ); }
	//inline void filterPtSet( ) { pt_set &= allowed_heaps; }
	//inline bool insertPtSet( BitVector & rhs_pt_set ) { return pt_set |= ( rhs_pt_set & allowed_heaps ); }
	void filterTypes( vector< Heap * > & heaps );

	inline bool insertPtSetNew( int heap_id ){ assert( heap_id >= 0 ); return pt_set_new.test_and_set( heap_id ); }
	inline bool insertPtSetNew( BitVector & rhs_pt_set ) { 
		return pt_set_new |= ( ( rhs_pt_set - pt_set ) & allowed_heaps ); 
	}
	//inline bool insertPtSetNew( BitVector & rhs_pt_set ) { pt_set_new |= (rhs_pt_set - pt_set ); return pt_set_new &= allowed_heaps; }
	//inline bool insertPtSetNew( BitVector & rhs_pt_set ) { return pt_set_new |= ( ( rhs_pt_set & allowed_heaps ) - pt_set ); } // THIS IS WRONG!
	inline bool isEmptyPtSetNew( ) { return pt_set_new.empty( ); }
	inline BitVector & getPtSetNew( ){ return this->pt_set_new; }
	inline uint getPtSetNewSize( ){ return pt_set_new.count( ); }
	inline bool ptSetContainsNew( ) {return pt_set.contains( pt_set_new ); }
	inline bool flushNewPtSet( ) { bool ret = ( pt_set |= pt_set_new ); pt_set_new.clear( ); return ret; }

	inline void setTopOrd( int ord ) { top_ord = ord;  }
	inline int  getTopOrd( )         { return top_ord; } 
	inline void setTypeId( int type_id_ ) { assert( type_id == -1 ); type_id = type_id_; }
	inline int getId( ) { return id; }
	inline int getTypeId( ) { return type_id; }
	inline bool isTypeSet( ) { return type_id == -1 ? false : true;  }
	inline bool insertAllowedHeaps( BitVector & h ){ return allowed_heaps |= h; }

	void printInfo( );
	void printPointsTo( );
	void printNewPointsTo( );

private:
	int id;
	int type_id;
	int top_ord;
	BitVector allowed_heaps;
	BitVector pt_set;
	BitVector pt_set_new;
	//list< Heap * > pt_set; // list < int > pt_set; pt_set would hold id's of the heap locations
};

#endif
