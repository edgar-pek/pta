#include "SmtEncoding.h"

void SmtEncoding::printInfo( )
{
	cerr << "smt encoding - print info " << endl;
	var_ref->printInfo( cerr );
	obj_ref->printInfo( cerr );
	all_rel->printInfo( cerr );
	ahp_rel->printInfo( cerr );
}

void SmtEncoding::writeHeader( )
{
	os << "(declare-fun a (Int Int) Bool) \n";
	os << "(declare-fun pt(Int Int Int)  Int) \n";
}

void SmtEncoding::writeRule( )
{
	os << endl;
	os << "; rule pt(v2,h) and a(v1,v2) => pt(v1,h)\n";
	os << "(assert (forall ((v1 Int) (v2 Int) (h Int)) (or (= (pt v2 h 0) 0) (not (a v1 v2)) (= (pt v1 h 0) 1) ))) \n";
}
void SmtEncoding::writeFooter( )
{
	os << endl;
	os << "(check-sat)\n";
	os << "(get-model)\n";
}

void SmtEncoding::processAHP( )
{
	vector< Tuple * > r = ahp_rel->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int var_id  = t->el[1];
		int heap_id = t->el[0];
		//cerr << var_ref->getName( var_id ) << " := " << obj_ref->getName( heap_id ) << endl;
		os << "(assert (= (pt " << var_id + 1 << " " << heap_id + 1 << " 0) 1)) \n";
	}
}

void SmtEncoding::processALL( )
{
	vector< Tuple * > r = all_rel->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int src_id = t->el[0];
		int dst_id = t->el[1];
		//cerr << var_ref->getName( dst_id ) << " := " << var_ref->getName( src_id ) << endl;
		os << "(assert (= (a " << dst_id + 1 << " " << src_id + 1 << ") true)) \n";
	}
}
void SmtEncoding::writeEncoding( )
{
	writeHeader( );
	processAHP( );
	processALL( );
	writeRule( );
	writeFooter( );
}
