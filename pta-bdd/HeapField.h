#ifndef HEAP_FIELD
#define HEAP_FIELD

#include "global.h"
#include "parser.h"
#include "Heap.h"


class HeapField
{
public: 
	HeapField( ) 
		: id ( -1 ), heap_id( -1 ), field_id( -1 ) { }
	HeapField( int id_, int heap_id_, int field_id_ )
		: id( id_ ), heap_id( heap_id_ ), field_id( field_id_ ) { }

	inline int getId( )      { return id;       }
	inline int getHeapId( )  { return heap_id;  }
  inline int getFieldId( ) { return field_id; }

	inline list< Heap * > & getPtSet( ) { return pt_set; }

	inline void insertBackPtSet( list< Heap * > & l ) { pt_set.insert( pt_set.end( ), l.begin( ), l.end( ) ); }
	void printInfo( );


private:
	int id;
	int heap_id;
	int field_id;
	list< Heap * > pt_set;
};

// strict weak ordering for HeapField
struct HeapFieldLessThan
{
	inline bool	operator( )( HeapField * x, HeapField * y )
	{
		return ( (x->getHeapId( ) < y->getHeapId( )) || ( (x->getHeapId( ) == y->getHeapId( )) && (x->getFieldId( ) < y->getFieldId( )) ) );
	}
};

#endif
