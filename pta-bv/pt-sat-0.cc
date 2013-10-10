#include "timer.h"
#include "parser.h"
#include <Solver.h>
#include <SimpSolver.h>
#include "var-pt-sat.h"
#include "pt-graph.h"
#include "field.h"
#include "constraint-graph.h"
#include "heap-field-pt-set.h"

using namespace Minisat;

void initVarPt0( Relation & ahp_rel, vector< Var > & heaps, Solver & s, vector< VarPtSat * > & var_pt );
void fixedPoint0( Relation & all_rel, Solver & s, vector< VarPtSat * > & var_pt );
void printFoundConflict( const string );
void printHorizontalLine( ostream & os = cerr, char c = '-', int num = 80 );
void printPointsToRelation( Solver & s, vector< VarPtSat * > & var_pt, vector< Var > & heaps,  Reference * var_ref, Reference * field_ref, Field & field,  Reference * obj_ref );

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
	string ref_relations [] = { "VarRef", "NormalHeapAllocationRef", "AssignHeapAllocation", "AssignLocal"
																/* , "FieldSignatureRef", "StoreInstanceField", "LoadInstanceField"  */ };
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

	// --------------------------------------------------------------------------------------------------------
	//Field field( field_ref, sif_rel, lif_rel ); // I probably don't need this in the new interpretation of the field-sensitive analysis

	//cerr << "Distinct field instances :" << field.getFieldInstanceNum( ) << endl;

	Timer timer_ctg; timer_ctg.start( ); cerr << "Constraint graph analysis ...";
	// FIXME: how to make a clear and elegant constructor?
	// TODO:  support adding vertices incrementally!
	CTGraph ct_graph( var_ref->getDomainSize( ) /*+ field.getFieldInstanceNum( ) */ );
	//CTGraph ct_graph( var_ref->getDomainSize( ) + field_ref->getDomainSize( ) );
	ct_graph.setVarRefSize( var_ref->getDomainSize( ) );
	ct_graph.addAssignConstr( all_rel );
	//ct_graph.setFieldRefSize( field_ref->getDomainSize( ) );
	//ct_graph.addFieldBasedConstr( sif_rel, lif_rel );
	//ct_graph.setFldInsSize( field.getFieldInstanceNum( ) );
	//ct_graph.addFieldConstr( sif_rel, lif_rel, field );
	ct_graph.dfs( );
	ct_graph.printInfo( );
	cerr << " [DONE] \n";
	timer_ctg.printElapsed( );

	//return 0;
	//ct_graph.printTopSort( );
	// --------------------------------------------------------------------------------------------------------
	
	Solver s;
	// s.luby_restart = false; // it seems has no effect 
	// s.restart_inc = 1.5;    // no effect
	// s.phase_saving = 0;     // this one has effect on the number of learned literals
	// s.ccmin_mode = 0;      // ?
	//SimpSolver s; TODO: how to  speed up the computation using the simplifier?
	
	cerr << endl;
	printHorizontalLine( );
	cerr << "SAT-based points-to analysis \n";

	vector< Var > heaps( obj_ref->getDomainSize( ) );
	for ( size_t i = 0; i < heaps.size( ); ++ i ) 
		heaps[ i ] = s.newVar( );
	
	// *** FIELD SENSITIVITY
	//FieldSens field_sens;

	// FIXME: this should be a more clear implementation
	// TODO: think about ways to add points-to variables incrementally?
	//vector< VarPtSat * > var_pt ( var_ref->getDomainSize( ) + field_ref->getDomainSize( ) );
	//vector< VarPtSat * > var_pt ( var_ref->getDomainSize( ) + field.getFieldInstanceNum( )  );
	vector< VarPtSat * > var_pt ( var_ref->getDomainSize( ) );
	for ( size_t i = 0; i < var_pt.size( ); ++ i )
		var_pt[ i ] = new VarPtSat( i );

	//initVarPt0( ahp_rel, heaps, s, var_pt);
	//fixedPoint0( all_rel, s, var_pt );
	
	// initialize var_pt with literals corresponding to the heap variables
	// ---------------------------------------------------------------------------
	cerr << "initializing var points-to and adding initial equivalence constraints ...";
	set< Tuple * > ahp = ahp_rel.getRelation( );
	Tuple * tuple;
	for ( set< Tuple *>::iterator it = ahp.begin( ); it != ahp.end( ); ++ it )
	{
		tuple = * it;
		int obj_id = tuple->el[0];
		int var_id = tuple->el[1];
		CTVertex * v = ct_graph.getCTVertex(var_id );
		CTVertex * scc_low = v->getSccLow( );
		if ( var_pt[ scc_low->getId( ) ]->getLastPtVarIndex( ) == -1 )
			var_pt[ scc_low->getId( ) ]->addToPtVar( s.newVar( ) );
		var_pt[ scc_low->getId( ) ]->addToPtClause( mkLit(heaps[obj_id]) );

		//if ( var_pt[ scc_low->getId( ) ]->addPtConstr( s ) == false ) 
		//	printFoundConflict( "initConstr" );
	}
	// add equivalence constraints based on initial clauses to the solver	
	// TODO: this can be optimized - i.e., done in the previous step
	Timer tPta; tPta.start( );
	for ( size_t i = 0; i < var_pt.size( ); ++ i )
		if ( var_pt[ i ]->getLastPtVarIndex( ) == 0 )
			if ( var_pt[ i ]->addPtConstr( s ) == false ) 
				printFoundConflict( "initConstr" );
	cerr << " [DONE] \n";	tPta.printElapsed( ); 
	
	int iter = 0;
	bool fixed_point = false;
	vector< CTVertex * > sorted_vertices = ct_graph.getTopOrdVertices( );
	vector< CTVertex * >::reverse_iterator it;
	//while( fixed_point == false )
	//{
		fixed_point = true;
		cerr << "iteration # " << iter;
		cerr << " vars : " << s.nVars( ) << " clauses : " << s.nClauses( ) << endl;
		//cerr << "sorted vertices # " << sorted_vertices.size( ) << endl;
		//int cnt = 0;
		for( it = sorted_vertices.rbegin( ); it != sorted_vertices.rend( ); ++ it )
		{
			CTVertex * v = *it;

			//cnt++;
			//double percent = (double) cnt / (double) sorted_vertices.size( );
			//printf( "\r vars: %d clauses: %d  processed: %2.2f %%", s.nVars( ), s.nClauses( ), percent * 100 ); fflush( stdout );

			if ( var_pt[v->getId()]->getLastPtVarIndex( ) < 0 ) 
				continue;
			
			vector< CTEdge * > adj = ct_graph.getAdjList( v->getId( ) );
			//vector< CTEdge * > adj = ct_graph.getVarAdjList( v->getId( ) );
			/*
			for ( unsigned i = 0; i < adj.size( ); ++ i ) {
				CTEdge * e = adj[ i ];
				CTVertex * lhs_v = e->getLhs( );
				CTVertex * rhs_v = e->getRhs( );
			
				if ( lhs_v->getSccLow( ) != rhs_v->getSccLow( ) ) {
					
					if ( var_pt[ rhs_v->getSccLow( )->getId( ) ]->getLastPtVarIndex( ) >= 0 ) {
						//cerr << "generating assignment constraints for " << lhs_v << " := " << rhs_v << endl;
						//TODO: do I need this???? [IMPORTANT] - maybe I can change this to the implication constraints, which could speed it up
						if( var_pt[ lhs_v->getSccLow( )->getId( ) ]->addAssignConstr(s, var_pt[ rhs_v->getSccLow( )->getId( ) ] ) == false )
							printFoundConflict( "addingAssignConstraints" );
						
						// [TODO]: I can use AIG or BDDs here to share the collected info - AIGs are probably cheaper though

						//  [EXPLICT TRACKING OF THE POINTS-TO SET] -- need this for field sensitivity
						//  add the pt_clause of the rhs to the pt_clause of lhs, generate constraints on-demand [lazy]
						//var_pt[lhs_v->getSccLow( )->getId( )]->copyPtClauseFrom( var_pt[rhs_v->getSccLow( )->getId( )] );

						// [ADD CONSTRAINTS - EAGER]
						// add the pt_clause of rhs to the pt_clause of lhs, and add pt_clause constraints for the lhs
						//if ( var_pt[ lhs_v->getSccLow( )->getId( ) ]->addPtConstr( s, var_pt[ rhs_v->getSccLow( )->getId( ) ] ) == false ) 
						//	printFoundConflict( "adding pt_constr" );
						// clear pt_clause after adding constraints [OPTIONAL - to save memory]
						//var_pt[ lhs_v->getSccLow( )->getId( ) ]->clearPtClause( );
					}
				}
			}

			unsigned comp_size;
			if ( v->isSccLow( ) && ( ( comp_size = ct_graph.getSccComponentSize( v->getSccNum( ) ) ) > 1 ) ) {
				//cerr << v << " is the root of the component " << v->getSccNum( ) << " whose size is : " << comp_size	<< endl;
				//cerr << "elements of the component : " << endl;
				vector < CTVertex * > component( it, (it + comp_size) );
				//assert ( v == component[0] );
				for ( unsigned i = 1; i < component.size( ) ; ++ i ) {
					var_pt[component[i]->getId( )]->addToPtVar( var_pt[component[0]->getId( )]->getLastPtVar( ) );

					// [NOTE] this is EAGER, I could do LAZY copy from the scc representative when I need the explicit representation
					// need this for field-sensitivity:
					//var_pt[component[i]->getId( )]->copyPtClauseFrom( var_pt[component[0]->getId( )] );

					// add constraint
					//if ( var_pt[ component[i]->getId( ) ]->addPtConstr( s ) == false ) 
					//	printFoundConflict( "scc components" );				

					//cerr << "generating equivalence constraint for: "<< component[ i ] << " and " << component[i+1] <<  endl;
					//if ( var_pt[ component[i]->getId( ) ]->addSccConstr( s, var_pt[ component[i+1]->getId( ) ] ) == false )
					//	printFoundConflict( "sccConstr" );
				}
			}
		 */
			/*if ( iter >= 1 ) { 	
				if ( v->isSccLow( ) ) {
					//cnt++;
					//double percent = (double) cnt / (double) sorted_vertices.size( );
					//printf( "\r vars: %d clauses: %d  processed: %2.2f %%", s.nVars( ), s.nClauses( ), percent * 100 ); fflush( stdout );
					if ( var_pt[v->getId( )]->checkFixedPointConstr( s ) == false ) {
						fixed_point = false;
						break; // unroll more when first variable with non-fixed point is encountered = EARLY TERMINATION
					} 
				}
			}else 
				fixed_point = false;
		*/
		//}

		//field_sens.initHeapPt( sif_rel, var_pt, s );
		//field_sens.printHeapPt( obj_ref, field_ref );
		//field_sens.updateVarPt( lif_rel, var_pt, s);

		iter ++;
		/*
		if ( iter >= 2 ) {  // FASTER FIXED POINT COMPUTATION FOR iter >= i where i is the number of fixed point iteration
			cerr << "checking for the fixed point " << endl;
			for( size_t i = 0; i < var_pt.size( ); ++ i ) {
				double percent = (double) i / (double) sorted_vertices.size( );
				printf( "\r vars: %d clauses: %d  processed: %2.2f %%", s.nVars( ), s.nClauses( ), percent * 100 ); fflush( stdout );
				CTVertex * v = ct_graph.getCTVertex( i );
				if ( v->isSccLow( ) ) {
					if ( var_pt[i]->checkFixedPointConstr( s ) == false ) {
						fixed_point = false;
						break; // unroll more when first variable with non-fixed point is encountered = EARLY TERMINATION
					} 
				}
			}
		} else 
			fixed_point = false;
		*/
	}
	tPta.printElapsed( );
	cerr << "[ANALYSIS DONE] \n\n";
	
	

	if ( s.nVars( ) < 1000 ) {
		//field_sens.printHeapPt( obj_ref, field_ref );
		//printPointsToRelation( s, var_pt, heaps, var_ref, field_ref, field, obj_ref );
	}

	if ( argc == 3 ) {
		printHorizontalLine( );
		bool is_same = true;
		cerr << "Creating points-to graph for DOOP results \n";
		PTGraph ptGraph( var_ref->getDomainSize( ) );
		cerr << "comparison file : " << argv[2] << endl;
		ptGraph.readDoopRes( argv[2], var_ref, obj_ref );
		cerr << "num of edges in the pt graph : " << ptGraph.getNumEdges( ) << endl;
		cerr << "comparing with doop : " << endl;
		for( size_t i = 0; i < var_ref->getDomainSize( ); ++ i ) 
		{
			if ( var_pt[i]->addCmpConstr( s, ptGraph.getAdjList( i ), heaps ) == false )
				printFoundConflict( "correctness comparison constraints");
			if ( var_pt[i]->checkCmpConstr( s ) == false ){
				cerr << "var : " << var_ref->getName( i ) << " pt info mismatch!\n"; 
				is_same = false;
				break;
			}
		}
		if ( is_same == true ) 
			cerr << "the points-to relation is the same.\n";
		printHorizontalLine( );
	}


	if ( s.nVars( ) < 1000 )
		s.toDimacs( "pt-analysis-encoding.cnf" );

	printHorizontalLine( );
	cerr << "SAT SOLVER STATISTICS: \n";
	cerr << "solves                : " << s.solves << endl;
	cerr << "learned literals      : " << s.learnts_literals << endl;
	s.printStats( );
	printHorizontalLine( );

	return 0;
}

void initVarPt0( Relation & ahp_rel, vector< Var > & heaps, Solver & s, vector< VarPtSat * > & var_pt )
{
	// initialize var_pt with literals corresponding to the heap variables
	// ---------------------------------------------------------------------------
	set< Tuple * > ahp = ahp_rel.getRelation( );
	Tuple * tuple;
	for ( set< Tuple *>::iterator it = ahp.begin( ); it != ahp.end( ); ++ it )
	{
		tuple = * it;
		int obj_id = tuple->el[0];
		int var_id = tuple->el[1];
		if ( var_pt[ var_id ]->getLastPtVarIndex( ) == -1 )
			var_pt[ var_id ]->addToPtVar( s.newVar( ) );
		var_pt[ var_id ]->addToPtClause( mkLit(heaps[obj_id]) );
	}
	// add equivalence constraints based on initial clauses to the solver	
	// TODO: this can be optimized - i.e., done in the previous step
	for ( size_t i = 0; i < var_pt.size( ); ++ i )
		if ( var_pt[ i ]->getLastPtVarIndex( ) == 0 )
			if ( var_pt[ i ]->addPtConstr( s ) == false ) 
				printFoundConflict( "initConstr" );
}

void fixedPoint0( Relation & all_rel, Solver & s, vector< VarPtSat * > & var_pt )
{
	// ---------------------------------------------------------------------------
	// TODO: optimize this part, don't have to go through all the iterations
	// - adjacency list representation of the assign relation would help in doing smarter iteration
	// - iterate only through the list of variables for which lhs has changed
	// - put lhs in the priority queue sorted by topological order
	// TODO:  maintain a delta points-to set
	set< Tuple * >::iterator it;
	Tuple * tuple;
	set < Tuple * > all = all_rel.getRelation( );
	int iter = 0;
	bool fixed_point = false;
	Timer tPta; tPta.start( );
	while( fixed_point == false )
	{
		fixed_point = true;
		cerr << "iteration # " << iter;
		cerr << " vars : " << s.nVars( ) << " clauses : " << s.nClauses( ) << endl;
		// iterating through assignments: TODO: optimize this
		for ( it = all.begin( ); it != all.end( ); ++ it ) 
		{
			tuple = * it;
			int rhsVarId = tuple->el[0];
			int lhsVarId = tuple->el[1];
			//cerr << lhsVarId << " := " << rhsVarId << endl;
			if ( var_pt[rhsVarId]->getLastPtVarIndex( ) >= 0 )
			{
				if ( var_pt[lhsVarId]->addAssignConstr( s, var_pt[rhsVarId] ) == false )
					printFoundConflict( "addingConstraints" );
			}
		}
		tPta.printElapsed( );
		// check for a fixed point per variable
		// TODO: optimize this, this doesn't have to be done separately from the assignment iterations
		if ( iter >= 5 ) {  // FASTER FIXED POINT COMPUTATION FOR iter >= i where i is the numbe of fixed point iteration
			cerr << "checking for the fixed point " << endl;
			for( size_t i = 0; i < var_pt.size( ); ++ i ) {
				if ( var_pt[i]->checkFixedPointConstr( s ) == false ) {
					fixed_point = false;
					//break; // unroll more when first variable with non-fixed point is encountered = EARLY TERMINATION
				} 
			}
		} else 
			fixed_point = false;

		assert( s.okay( ) );
	
		iter ++;
	}
	cerr << "FIXED POINT REACHED. \n";
	tPta.printElapsed( );
}

void printFoundConflict( const string s )
{
	cerr << "conflict found while adding constraints @ " << s << endl;
	cerr << "SAT solver is in inconsistent state. \n";
	cerr << "Exiting...\n";
}

void printHorizontalLine( ostream & os, char c, int num )
{
	for ( int i = 0; i < num; ++ i )
		os << c;
	os << endl;
}

// TODO: refactor this
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
