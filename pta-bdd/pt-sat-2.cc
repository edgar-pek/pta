#include "Timer.h"
#include "parser.h"
#include <Solver.h>
#include <SimpSolver.h>
#include "VptSat.h"
#include "ObjSat.h"
#include "Analyzer.h"
Solver * Analyzer::s = NULL;

using namespace Minisat;

void printHorizontalLine( ostream & os = cerr, char c = '-', int num = 80 );
//void printPointsToRelation( Solver & s, vector< VarPtSat * > & var_pt, vector< Var > & heaps,  Reference * var_ref, Reference * field_ref, Field & field,  Reference * obj_ref );

// pt-sat-2.cc
int main( int argc, char * argv [ ] )
{
	// TODO: parsing command line argumens will be done in a separate compilation unit
	if ( argc < 2 ) {
		cerr << "usage : " << argv[ 0 ] << " < path-to-facts > \n\n";
		exit( 1 );
	}

	Timer tGlob; tGlob.start( );
	// --------------------------------------------------------------------------------------------------------
	string path = string( argv[ 1 ] );
	cout << "----------------------------------------------------------- \n";
	cout << "Reading input facts...                                      \n";
	FileNames * fileNames = new FileNames( path );
	string ref_relations [] = { "VarRef", "NormalHeapAllocationRef", "AssignHeapAllocation", "AssignLocal" };
//		                          "FieldSignatureRef", /*"ClassType", "InterfaceType", */ "StoreInstanceField", "LoadInstanceField" };
	vector < string > relNames ( ref_relations, ref_relations + sizeof( ref_relations ) / sizeof( string ) );
	fileNames->setFileNames( relNames );

	// references
	cerr << "\treading VarRef ..." << endl;
	Reference * var_ref = new Reference( fileNames->getFullFileName( "VarRef" ), "VAR" );
	cerr << "\treading NormalHeapAllocationRef ... \n";
	Reference * obj_ref = new Reference( fileNames->getFullFileName( "NormalHeapAllocationRef" ), "HEAP" );

	//cerr << "\treading FieldSignatureRef ... \n";
	//Reference * field_ref = new Reference( fileNames->getFullFileName( "FieldSignatureRef" ), "FIELD" );
	//cerr << "\treading ClassType ...\n";
	//Reference * type_ref = new Reference( fileNames->getFullFileName( "ClassType" ) , "TYPE" );
	//cerr << "\treading InterfaceType ... \n";
	//type_ref->loadFromFile( fileNames->getFullFileName( "InterfaceType" ) );
	// relations
	vector< Reference * > refs; refs.push_back( obj_ref ); refs.push_back( var_ref );
	cerr << "\treading AssignHeapAllocation ... \n";
	Relation ahp_rel ( fileNames->getFullFileName("AssignHeapAllocation"), "AHP", refs, 3 );
	refs.clear( ); refs.push_back( var_ref ); refs.push_back( var_ref );
	cerr << "\treading AssignLocal ... \n";
	Relation all_rel ( fileNames->getFullFileName("AssignLocal"), "ALL", refs, 3 );

	//refs.clear( ); refs.push_back( var_ref ); refs.push_back( var_ref ); refs.push_back( field_ref );
	//cerr << "\treading StoreInstanceField ... \n";
	//Relation sif_rel ( fileNames->getFullFileName("StoreInstanceField"), "SIF", refs, 4 );
	//refs.clear( ); refs.push_back( var_ref ); refs.push_back( field_ref ); refs.push_back( var_ref );
	//cerr << "\treading LoadInstanceField ... \n";
	//Relation lif_rel ( fileNames->getFullFileName("LoadInstanceField"), "LIF", refs, 4 );
	// --------------------------------------------------------------------------------------------------------
	cerr << "\t[DONE] \n";
	tGlob.printElapsed( );
	cerr << endl;

	// instance of the MiniSAT solver
	Solver s;
	Analyzer::setSATSolver( & s );
	
	VptSat vpt_sat( s, var_ref ); 
	ObjSat obj_sat( s, obj_ref );
	//vpt_sat.printInfo( );
	//obj_sat.printInfo( );
	Analyzer analyzer( & vpt_sat, & obj_sat );
	analyzer.printInfo( );

	if ( s.nVars( ) < 1000 )
		s.toDimacs( "pt-analysis-encoding.cnf" );

	printHorizontalLine( );
	Analyzer::printSolverStats( );
	printHorizontalLine( );

	return 0;
}

void printHorizontalLine( ostream & os, char c, int num )
{
	for ( int i = 0; i < num; ++ i )
		os << c;
	os << endl;
}

// TODO: refactor this
/*
void printPointsToRelation( Solver & s, vector< VarPtSat * > & var_pt, vector< Var > & heaps,  Reference * var_ref, Reference * field_ref, Field & field, Reference * obj_ref )
{
	cerr << "POINTS-TO RELATION : \n";
	for( size_t i = 0; i < var_pt.size( ); ++ i )
	{
		if ( i < var_ref->getDomainSize( ) )
			cerr << "var " << var_ref->getName( i ) << " -> ";
		else
		{
			//cerr << "field " << field_ref->getName( i - var_ref->getDomainSize( ) ) << " -> ";
			FieldInstance * f = field.getFieldInstance( i - var_ref->getDomainSize( ) );
			cerr << "var.fld " << var_ref->getName( f->var_id ) << "." << field_ref->getName( f->fld_id ) << " -> ";
		}
		for( size_t j = 0; j < heaps.size( ); ++ j ) 
		{
			if ( var_pt[i]->checkPointsTo( s, heaps[j] ) == true )
				cerr << obj_ref->getName( j ) << " ";
		}
		cerr << endl;
	}
}
*/
