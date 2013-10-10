#include "Heap.h"

vector< uint > decoded_heap_elements;

// FROM: revue-src/solve-bdd/hcd.cc
void decode_heap_domain( char * vset, int size )
{
	vector < uint > dc; // vector of "don't cares"
	uint i, j, x, k = 0;

#define  TEST(a,b) (a & (1 << b))
#define   SET(a,b) (a |= (1 << b))
#define CLEAR(a,b) (a & ~(1 << b))
	
	for(i = 0; i < (uint) fdd_varnum(0); i++)
	{
		if      (vset[i] < 0) { dc.push_back(i); }
		else if (vset[i] > 0) { SET(k,i);        }
	}

	x = dc.size( ); // size of don't cares
	if (x == 0) { decoded_heap_elements.push_back(k); return; }

	// enumerate all don't cares
	for(i = 0; i < (uint)(1 << x); i++) 
	{
		for (j = 0; j < x ; j ++)
		{
			if  (TEST(i,j)) { SET(k,dc[j]);    }
			else            { CLEAR(k, dc[j]); }
		}
		decoded_heap_elements.push_back(k);
	}
}
void Heap::printInfo( )
{
	cerr << "HEAP -INFO " << endl;
}
