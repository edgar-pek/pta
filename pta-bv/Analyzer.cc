#include "Analyzer.h"
#include "Timer.h"
#include <ext/algorithm>

using __gnu_cxx::is_heap;

void Analyzer::initFromPAG( )
{
	initVars(  parse_pag->getMaxVarId( ) + 1 );
	initHeaps( parse_pag->getMaxHeapId( ) + 1);
	createTypeFilter( parse_pag->getMaxDecTypeId( ) + 1, parse_pag->getDeclTypeRel( ), parse_pag->getAllocTypeRel( ),     
			              parse_pag->getVarTypeRel( ) );
}

void Analyzer::setTopOrdVars( )
{
	vector< CTVertex * > vertices = ct_graph->getTopOrdVertices( );
	vector< CTVertex * >::reverse_iterator it;
	int i = 0;
	for ( it = vertices.rbegin( ); it != vertices.rend( ); ++ it )
	{
		CTVertex * v = * it;
		Var * var = vars[ v->getId( ) ];
		var->setTopOrd( i ++ );
	}
	//cerr << "max i " << i << endl;
}

void Analyzer::initVarTypes( Relation * var_types )
{
	vector< Tuple * > r = var_types->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int var_id  = t->el[0]; int dec_type_id = t->el[1];
		dec2var.insert( pair< int, int >( dec_type_id, var_id ) );

		// TODO: clean-up the code for this part
		//Var * var = getVar( var_id );
		//assert( false == var->isTypeSet( ) );
		//var->setTypeId( type_id );
	}
	//cerr << "dec2var size = " << dec2var.size( ) << endl;
}

void Analyzer::initActualTypeRelations( Relation * declared_types, Relation * alloc_types )
{
	vector< Tuple * > r = declared_types->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int dec_type_id = t->el[0]; int act_type_id = t->el[1];
		act2dec.insert( pair< int, int >( act_type_id, dec_type_id ) );
	}
	r = alloc_types->getRelation( );
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int heap_id = t->el[0]; int act_type_id = t->el[1];
		act2heap.insert( pair< int, int >( act_type_id, heap_id ) );
	}
}

void Analyzer::createAssignableTypes( Relation * declared_types, Relation * alloc_types )
{
	initActualTypeRelations( declared_types, alloc_types );
	multimap< int, int >::iterator it;
	pair< multimap<int, int>::iterator, multimap<int, int>::iterator > dec_types; 
	for( it = act2heap.begin( ); it != act2heap.end( ); ++ it )
	{
			dec_types = act2dec.equal_range( it->first );
			multimap< int, int >::iterator it_dec;
			for( it_dec = dec_types.first; it_dec != dec_types.second; ++ it_dec ) 
				dec2heap.insert( pair< int, int > ( it_dec->second, it->second ) );
	}
	//cerr << "dec2heap size = " << dec2heap.size( ) << endl;
}

BitVector Analyzer::getHeaps4DecType( int dec_type_id )
{
	BitVector ret;
	pair< multimap<int, int>::iterator, multimap<int, int>::iterator > heaps_range = dec2heap.equal_range( dec_type_id );
	multimap<int, int>::iterator it;
	for ( it = heaps_range.first; it != heaps_range.second; ++ it )
		ret.set( it->second );
	return ret;
}

void Analyzer::createTypeFilter( int dec_type_sz, Relation * declared_types, Relation * alloc_types, Relation * var_types )
{
	createAssignableTypes( declared_types, alloc_types );
	initVarTypes( var_types );
	pair< multimap<int, int>::iterator, multimap<int, int>::iterator > vars_range;
	multimap< int, int>::iterator var_it;
	BitVector heaps;
	for( int i = 0; i < dec_type_sz; ++ i )
	{
		vars_range = dec2var.equal_range( i );
		heaps = getHeaps4DecType( i );
		//cerr << "dec type : " << i << " vars size : " << dec2var.count( i ) << " heaps size : " << heaps.count( ) << endl;
		for( var_it = vars_range.first; var_it != vars_range.second; ++ var_it )
		{
			Var * var = vars[ var_it->second ];
			var->insertAllowedHeaps( heaps );
		}
	}
}

void Analyzer::mergePtSets( )
{
	vector< CTVertex * > vertices = ct_graph->getTopOrdVertices( );
	vector< CTVertex * >::reverse_iterator it;
	//int different_types = 0;
	for( it = vertices.rbegin( ); it != vertices.rend( ); ++ it )
	{
		CTVertex * v = *it;
		assert( v->isSccRoot( ) );
		int comp_size = ct_graph->getSccComponentSize( v->getSccNum( ) );
		if ( comp_size > 1 ) 
		{
			vector< CTVertex * > component( it, (it + comp_size) );
			Var * var_root = getVar( v->getId( ) );
			for( int i = 1; i < comp_size; ++ i )
			{	
				Var * var_comp = getVar( component[ i ]->getId( ) );
				if ( var_comp->getTypeId( ) == var_root->getTypeId( ) )
					var_root->insertPtSet( var_comp->getPtSet( ) );
			}
			advance( it, comp_size-1 );
		}
	}
	//cerr << "different types : " << different_types << endl;
}


void Analyzer::processAlloc( bool worklist )
{
	//cerr << "processing AHP: " << endl;
	vector< Tuple * > r = parse_pag->getAllocRel( )->getRelation( );
	//cerr << "r size = " << r.size( ) << endl;
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = *it;
		int var_id  = t->el[1];
		int heap_id = t->el[0];
		if ( worklist ) 
			pending_vars.set( var_id );

		Var  * var  = getVar( var_id );
		if ( ! worklist )
			var->insertPtSet( heap_id );
		else
			var->insertPtSetNew( heap_id );
	}
	if ( worklist )
		pendingVarsToHeap( );
}
void Analyzer::propagateWorkList( )
{
	vector< CTEdge * > adj_list;
	vector< CTEdge * >::iterator adj_it;
	uint iter = 0;
	while ( ! isEmptyVarsHeap( ) || ! pending_vars.empty( ) )
	{
		if ( isEmptyVarsHeap( ) == true ) 
			pendingVarsToHeap( );

		Var * src_var = popVarsHeap( );
		BitVector new_pt_set = src_var->getPtSetNew( );
		if ( new_pt_set.empty( ) ) continue;

		adj_list = ct_graph->getAdjList( ct_graph->getCTVertex( src_var->getId( ) ) );
		for ( adj_it = adj_list.begin( ); adj_it != adj_list.end( ); ++ adj_it )
		{
			CTEdge * e = * adj_it;
			CTVertex * dst_v = e->getDst( );
			Var * dst_var = vars[ dst_v->getId( ) ];
			//new_pt_set.intersectWithComplement( dst_var->getPtSet( ) );
			assert( src_var->getId( ) != dst_var->getId( ) );
			if ( dst_var->insertPtSetNew( new_pt_set ) )
			{
				pending_vars.set( dst_var->getId( ) );
			}
		}
		
		src_var->flushNewPtSet( );
		iter ++;
	}
	//cerr << "num iterations : " << iter << endl;
	//cerr << "pt set size = " << countPtSet( ) << endl;
}
bool Analyzer::propagateAssignsIter( )
{
	bool changed = false;
	vector< CTVertex * > vertices = ct_graph->getTopOrdVertices( );
	vector< CTVertex * >::reverse_iterator it;
	vector< CTEdge * > adj_list;
	vector< CTEdge * >::iterator adj_it;
	for ( it = vertices.rbegin( );  it != vertices.rend( ); ++ it )
	{
		CTVertex * v = * it;
		adj_list = ct_graph->getAdjList( v->getId( ) );
		for ( adj_it = adj_list.begin( ); adj_it != adj_list.end( ); ++ adj_it )
		{
			CTEdge * e = * adj_it;
			CTVertex * lhs_v = e->getLhs( );
			CTVertex * rhs_v = e->getRhs( );
			Var * lhs_var = vars[ lhs_v->getId( ) ];
			Var * rhs_var = vars[ rhs_v->getId( ) ];
			// insert pt set returns true if pt set of the var has changed; o/w false
			changed |= lhs_var->insertPtSet( rhs_var->getPtSet( ) );
			//int lhs_pt_set_size = lhs_var->getPtSet( ).count( );
		}
	}
	return changed;
}

bool Analyzer::processFieldSensStores( bool worklist )
{
	bool changed = false;
	vector< Tuple * > r = parse_pag->getStoreRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int rhs_var_id, lhs_var_id, fld_id;
		// lhs.fld := rhs
		rhs_var_id = t->el[0]; lhs_var_id = t->el[1]; fld_id = t->el[2];
		Var * lhs_var = vars[ lhs_var_id ];
		Var * rhs_var = vars[ rhs_var_id ];
		//BitVector rhs_var_pt_set = rhs_var->getPtSet( );
		BitVector & lhs_var_pt_set = lhs_var->getPtSet( );

		BitVector::iterator heap_it;
		for( heap_it = lhs_var_pt_set.begin( ); heap_it != lhs_var_pt_set.end( ); ++ heap_it )
		{
			//cerr << "heap id : " << * heap_it << endl;
			Heap * heap = heaps[ * heap_it ];
			if ( ! worklist )
				changed |= heap->insertPtSet( fld_id, rhs_var->getPtSet( ) );
			else 
			{
				heap->insertPtSet( fld_id, rhs_var->getPtSet( ) );
				//heap->insertPtSetNew( fld_id, rhs_var->getPtSet( ) );
			}
		}
	}
	return changed;
}

bool Analyzer::processFieldSensLoads( bool worklist )
{
	bool changed = false;
	vector< Tuple * > r = parse_pag->getLoadRel( )->getRelation( );
	vector< Tuple * >::iterator it;
	for ( it = r.begin( ); it != r.end( ); ++ it )
	{
		Tuple * t = * it;
		int rhs_var_id, fld_id, lhs_var_id;
		// lhs := rhs.fld
		rhs_var_id = t->el[0]; fld_id = t->el[1]; lhs_var_id = t->el[2];
		Var * dst_var = vars[ lhs_var_id ];
		Var * src_var = vars[ rhs_var_id ];
		BitVector & src_var_pt_set = src_var->getPtSet( );
		BitVector::iterator heap_it;
		// for each heap s.t. heap in pt(rhs)
		for( heap_it = src_var_pt_set.begin( ); heap_it != src_var_pt_set.end( ); ++ heap_it )
		{
			//cerr << "heap id : " << * heap_it << endl;
			Heap * heap = heaps[ * heap_it ];
			if ( ! worklist ) 
				changed |= dst_var->insertPtSet( heap->getPtSet( fld_id ) );
			else 
			{
				//if( dst_var->insertPtSetNew( heap->getPtSetNew( fld_id ) ) ) 
				if( dst_var->insertPtSetNew( heap->getPtSet( fld_id ) ) ) 
					pending_vars.set( dst_var->getId( ) );
				//changed_heaps.insert( pair< int, int > ( heap->getId( ), fld_id ) );		
			}
		}
	}
	if ( worklist )
		pendingVarsToHeap( );

	//exit( 1 );
	return changed;
}

uint Analyzer::countPtSet( )
{
	uint res = 0;
	vector< Var * >::iterator it;
	for( it = vars.begin( ); it != vars.end( ); ++ it )
	{
		Var * var = * it;
		res += var->getPtSetSize( );
	}
	return res;
}

uint Analyzer::countPtSetNew( )
{
	uint res = 0;
	vector< Var * >::iterator it;
	for( it = vars.begin( ); it != vars.end( ); ++ it )
	{
		Var * var = * it;
		res += var->getPtSetNewSize( );
	}
	return res;
}

void Analyzer::doBaseIterAnalysis( )
{
	processAlloc( );
	cerr << "SCC and topological sorting ...\n";
	ct_graph = new CTGraph( parse_pag->getMaxVarId( ) + 1 );
	ct_graph->setVars( getVars( ) );
	ct_graph->addAssignConstr( parse_pag->getAssignRel( ) );
	ct_graph->dfs( );

	bool changed = true;
	analysis_iter = 0;
	unsigned old_pt_set_sz;
	unsigned new_pt_set_sz = countPtSet( );
	while( changed )
	{
		Timer timer_iter; timer_iter.start( );
		old_pt_set_sz = new_pt_set_sz;
		changed = false;
		cerr << "iteration # " << analysis_iter + 1 << endl;

		cerr << "propagating assignments .. ";
		changed |= propagateAssignsIter( );
		cerr << timer_iter.elapsed( ) << endl;
		
		cerr << "processing field sensitive stores ... ";
		changed |= processFieldSensStores( );
		cerr << timer_iter.elapsed( ) << endl;
		
		cerr << "processing field sensitive loads ... " ;
		changed |= processFieldSensLoads ( );
		cerr << timer_iter.elapsed( ) << endl;
		
		analysis_iter ++;
		new_pt_set_sz = countPtSet( );
		cerr << "== Iteration took : " << timer_iter.elapsed( ) << " s.\n";
		cerr << "== Pt-set size    : " << new_pt_set_sz << endl;
		cerr << "==  delta pt set  : " << new_pt_set_sz - old_pt_set_sz << endl;
		cerr << "-----------------------------------------------------------------------------" << endl;
	}
	cerr << "Analysis done after " << analysis_iter + 1 << " iterations. \n";
	cerr << "Points-to size : " << countPtSet( ) << endl;
	delete ct_graph;
}

/*
void Analyzer::flushNewPtSets( )
{
	vector< Var * >::iterator it;
	for ( it = vars.begin( ); it != vars.end( ); ++ it ) 
		(*it)->flushNewPtSet( );
}
*/

void Analyzer::doWorkListAnalysis( )
{ 
	cerr << "the initial worklist size : " << getVarsHeapSize( ) << endl; // the size of the worklist
	cerr << "SCC and topological sorting ...\n";
	ct_graph = new CTGraph( parse_pag->getMaxVarId( ) + 1 );
	ct_graph->setVars( getVars( ) );
	ct_graph->addAssignConstr( parse_pag->getAssignRel( ) );
	ct_graph->dfs( );
	Timer analysis_timer; analysis_timer.start( );
	setTopOrdVars( );
	processAlloc( true );
	//uint old_pt_set_sz;
	//uint new_pt_set_sz = countPtSet( );
	analysis_iter = 0;
	while( ! isEmptyVarsHeap( ) )
	{
		//cerr << "|vars heap| = " << vars_heap.size( ) << endl;
		//Timer timer_iter; timer_iter.start( );
		//old_pt_set_sz = new_pt_set_sz;
		cerr << "propagating basic stuff ... \n";
		propagateWorkList( ); 
		//cerr << timer_iter.elapsed( ) << endl;
		//cerr << "Points-to size         : " << countPtSet( ) << endl;
		
		cerr << "propagating field sensitive stores ... \n";
		processFieldSensStores( true );
		//cerr << timer_iter.elapsed( ) << endl;

		cerr << "propagating field sensitive loads ... \n";
		processFieldSensLoads( true );
		//cerr << timer_iter.elapsed( ) << endl;

		//cerr << "flushing new pt-sets ... ";
		//flushNewHeapPtSets( );
		//cerr << timer_iter.elapsed( ) << endl;

		//cerr << "== worklist size after " << analysis_iter + 1 << ". iteration " << getVarsHeapSize( ) << endl;
		analysis_iter ++;
		//new_pt_set_sz = countPtSet( );
		//cerr << "== Iteration took : " << timer_iter.elapsed( ) << " s\n";
		//cerr << "== Pt-set size    : " << new_pt_set_sz << endl;
		//cerr << "==  delta pt set  : " << new_pt_set_sz - old_pt_set_sz << endl;
		cerr << "-----------------------------------------------------------------------------" << endl;
	}
	cerr << "Propagation time       : " << analysis_timer.elapsed( ) << " s." << endl;
	cerr << "Propagation iterations : " << analysis_iter << endl;
	Timer pta_timer; pta_timer.start( );
	cerr << "Points-to size         : " << countPtSet( ) << endl;
	cerr << "Points-to set time     : " << pta_timer.elapsed( ) << " s." << endl;
	delete ct_graph;
}

void Analyzer::compareWithSpark( const char * fname )
{
	parse_pag->parseSolutionFile( fname );
	vector< Var * >::iterator it;
	BitVector diff;
	cerr << "comparing analysis with spark's pt solution ... ";
	//unsigned cnt = 0;
	for( it = vars.begin( ); it != vars.end( ); ++ it )
	{
		//cnt ++;
		Var * var = * it;
		diff = var->getPtSet( ) - parse_pag->getSparkPtSetSol( var->getId( ) );
		if ( diff.count( ) != 0 ) 
		{
			cerr << "pt set for var " << var->getId( ) << " mismatch! \n";
			cerr << "difference set size : " << diff.count( ) << endl;
			exit( 1 );
		}
		//double percent = (double) cnt / (double) vars.size( );
		//printf( "\r processed : %2.2f %%", percent * 100 ); fflush( stdout );
	}
	cerr << " [OK] points-to sets are the same.\n";
}

BitVector Analyzer::queryVarPtScc( Var * var )
{
	BitVector r;
	vector< CTVertex * > dfs_stack;
	//deque< CTVertex * > bfs_queue;
	//dfs_stack.reserve( 100000 );
	CTVertex * v = ct_graph->getCTVertex( var->getId( ) );
	v = ct_graph->getSccRoot( v );
	//cerr << "v = " << v << endl;
	dfs_stack.push_back( v );
	//bfs_queue.push_front( v );
	initDfsVisited( );
	initSccRootIns( );
	//while( ! bfs_queue.empty( ) )
	while( ! dfs_stack.empty( ) )
	{
		CTVertex * s = dfs_stack.back( );
		assert( s->isSccRoot( ) );
		//CTVertex * s = bfs_queue.front( );
		//assert( ! isDfsVisited( s ) );
		dfs_stack.pop_back( );
		//bfs_queue.pop_front( );
		setDfsVisited( s );
		//cerr << "s = " << s << endl;

		//CTVertex * scc_root = g->getSccRoot( s );
		if ( false == isSccRootIns( s ) )
		{
			Var * var_root = vars[ s->getId( ) ];
			//cerr << "var_root size = " << var_root->getPtSet().size() << endl;
			//cerr << "r size = " << r.size() << endl;
			r |= var_root->getPtSet( );
			setSccRootIns( s );
		}

		//setDfsVisited( scc_root );

		CTVertex::SccAdjList adj_list = s->getSccAdjList( );
		for( CTVertex::SccAdjList::iterator it = adj_list.begin( ); it != adj_list.end( ); ++ it )
		{
			CTVertex * t = *it;
			assert( t->isSccRoot( ) );
			//cerr << "edge = " << (*it) << endl;
			//cerr << "t = " << t << endl;
			if ( ( ! isDfsVisited( t ) )  )
				dfs_stack.push_back( t );
				//bfs_queue.push_front( t );
		}
	}
	doneDfsVisited( );
	doneSccRootIns( );
	return r;

}

// TODO: I can speed-up querying if I work on DAG instead of a graph
// - precomputed SCC DAG then do the query
BitVector Analyzer::queryVarPt( Var * var )
{
	BitVector r;
	vector< CTVertex * > dfs_stack;
	//deque< CTVertex * > bfs_queue;
	//dfs_stack.reserve( 100000 );
	CTVertex * v = ct_graph->getCTVertex( var->getId( ) );
	//cerr << "v = " << v << endl;
	dfs_stack.push_back( v );
	//bfs_queue.push_front( v );
	initDfsVisited( );
	initSccRootIns( );
	//while( ! bfs_queue.empty( ) )
	while( ! dfs_stack.empty( ) )
	{
		CTVertex * s = dfs_stack.back( );
		//CTVertex * s = bfs_queue.front( );
		//assert( ! isDfsVisited( s ) );
		dfs_stack.pop_back( );
		//bfs_queue.pop_front( );
		setDfsVisited( s );
		//cerr << "s = " << s << endl;

		CTVertex * scc_root = ct_graph->getSccRoot( s );
		if ( false == isSccRootIns( scc_root ) )
		{
			assert( scc_root->isSccRoot( ) );
			Var * var_root = vars[ scc_root->getId( ) ];
			//cerr << "var_root size = " << var_root->getPtSet().size() << endl;
			//cerr << "r size = " << r.size() << endl;
			r |= var_root->getPtSet( );
			setSccRootIns( scc_root );
		}

		//setDfsVisited( scc_root );

		CTGraph::AdjList adj_list = ct_graph->getInEdges( s );
		CTGraph::AdjList::iterator it;
		for( it = adj_list.begin( ); it != adj_list.end( ); ++ it )
		{
			CTVertex * t = (*it)->getFrom( );
			//cerr << "edge = " << (*it) << endl;
			//cerr << "t = " << t << endl;
			if ( ( ! isDfsVisited( t ) )  )
				dfs_stack.push_back( t );
				//bfs_queue.push_front( t );
		}
	}
	doneDfsVisited( );
	doneSccRootIns( );
	return r;
}


void Analyzer::printInfo( )
{
	cerr << "Analyzer - info \n";
	cerr << "vars size  = " << vars.size( ) << endl;
	cerr << "heaps size = " << heaps.size( ) << endl;
}
/*
void Analyzer::propagateWorkList( )
{
	vector< CTVertex * > vertices = ct_graph->getTopOrdVertices( );
	vector< CTVertex * >::reverse_iterator it;
	vector< CTEdge * > adj_list;
	vector< CTEdge * >::iterator adj_it;
	uint iter = 0;
	//cerr << endl;
	while ( ! worklistEmpty( ) )
	{
		//cerr << "-----------------------------------------------------------------------" << endl;
		//cerr << "inner iteration # " << iter + 1;
		//cerr << " worklist size    : " << worklistSize( ) << endl;
		//uint pt_sz_b = countPtSet( ); 
		//uint pt_new_sz_b = countPtSetNew( );
		for ( it = vertices.rbegin( );  it != vertices.rend( ); ++ it )
		{
			CTVertex * v = * it;
			if ( ! worklistContains( v->getId( ) ) ) // is in the worklist?
				continue;
			
			worklistRemove( v->getId( ) ); // remove from the worklist
			adj_list = ct_graph->getAdjList( v->getId( ) );
			Var * rhs_var = vars[ v->getId( ) ];
			for ( adj_it = adj_list.begin( ); adj_it != adj_list.end( ); ++ adj_it )
			{
				CTEdge * e = * adj_it;
				CTVertex * lhs_v = e->getLhs( );
				//CTVertex * rhs_v = e->getRhs( );
				Var * lhs_var = vars[ lhs_v->getId( ) ];
				//assert( v->getId( ) == rhs_v->getId( ) );
				if ( lhs_var->insertPtSet( rhs_var->getPtSet( ) ) )
					worklistInsert( lhs_var->getId( ) ); // insert in the worklist
				//if ( ! lhs_var->ptSetContainsNew( ) )
				//	worklistInsert( lhs_var->getId( ) ); // insert in the worklist
			}
			//rhs_var->flushNewPtSet( );
		}
		//cerr << "new worklist size                    : " << changed_vars_new.count( ) << endl;
		//cerr << "flushing new pt sets ... " << endl;
			
		iter ++;
		//uint pt_sz_e = countPtSet( );
		//uint pt_new_sz_e = countPtSetNew( );
		//cerr << "pt set new size before iteration # " << iter << " : " << pt_new_sz_b << endl; 
		//cerr << "pt set new size after iteration  # " << iter << " : " << pt_new_sz_e << endl; 
		//cerr << "delta pt set new size                : " << pt_new_sz_e - pt_new_sz_b << endl;
		//cerr << "pt set size before iteration # " << iter << " : " << pt_sz_b << endl; 
		//cerr << "pt set size after iteration  # " << iter << " : " << pt_sz_e << endl; 
		//cerr << "delta pt set size                : " << pt_sz_e - pt_sz_b << endl;
	}
	//cerr << "pt set size = " << countPtSet( ) << endl;
}
*/
