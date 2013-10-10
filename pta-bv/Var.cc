#include "Var.h"

void Var::printInfo( )
{
	cerr << "VAR - INFO " << endl;
}
void Var::printPointsTo( )
{
	cerr << "var id " << this->id << endl;
	cerr << "|pt set| = " << pt_set.count( ) << endl;
	cerr << "pt(" << this->id << ") = ";
	BitVector::iterator it;
	for ( it = pt_set.begin( ) ; it != pt_set.end( ) ; ++ it )
		cerr << *it << " ";
	cerr << endl;
}
void Var::printNewPointsTo( )
{
	cerr << "var id " << this->id << endl;
	cerr << "|pt set new| = " << pt_set_new.count( ) << endl;
	cerr << "pt_new(" << this->id << ") = ";
	BitVector::iterator it;
	for ( it = pt_set_new.begin( ) ; it != pt_set_new.end( ) ; ++ it )
		cerr << *it << " ";
	cerr << endl;
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
