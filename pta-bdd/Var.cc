#include "Var.h"

void Var::printInfo( )
{
	cerr << "VAR - INFO " << endl;
}

void Var::printPointsTo( )
{
	cerr << "var id " << this->id << endl;
	bdd_allsat( pt_set, decode_heap_domain );
	cerr << "|pt set| = " << decoded_heap_elements.size( ) << endl;
	cerr << "pt(" << this->id << ") = ";
	for ( uint i = 0 ; i < decoded_heap_elements.size( ) ; i ++ )
		cerr << decoded_heap_elements[ i ] << " ";
	cerr << endl;
	decoded_heap_elements.clear( );
}
void Var::printNewPointsTo( )
{
	cerr << "var id " << this->id << endl;
	bdd_allsat( pt_set_new, decode_heap_domain );
	cerr << "|pt set new| = " << decoded_heap_elements.size( ) << endl;
	cerr << "pt_new(" << this->id << ") = ";
	for ( uint i = 0 ; i < decoded_heap_elements.size( ) ; i ++ )
		cerr << decoded_heap_elements[ i ] << " ";
	cerr << endl;
	decoded_heap_elements.clear( );
}
/* FIXME!!!
void Var::filterTypes( vector< Heap * > & heaps )
{
	unsigned deleted = 0;
	SparseBitVector< >::iterator it;
	for( it = pt_set.begin( ); it != pt_set.end( ); ++ it )
	{
		Heap * heap = heaps[ * it ];
		if ( ! heap->isDeclaredType( type_id ) ) 
		{
			deletePtSet( heap->getId( ) );
			deleted ++;
		}
		cerr << "non-matching types: " << deleted << endl;
	}
}
*/
