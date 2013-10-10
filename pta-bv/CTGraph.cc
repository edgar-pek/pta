#include "CTGraph.h"
// ----------------------------------------------------------------------------
// CTVertex
void CTVertex::printInfo( )
{
	cerr << "CTVertex : \n";
	cerr << "\tid: " << id << endl;
	cerr << "\tscc_num: " << scc_num << endl;
	//cerr << "\tdfs visited: " << dfs_visited << endl;
	//cerr << "\tdfs time: " << dfs_time << endl;
	//cerr << "\tdfs done: " << dfs_done << endl;
	//if ( dfs_parent )
	//	cerr << "\tdfs parent :"  << dfs_parent << endl;
  //else 
	//	cerr << "\tno dfs parent" << endl;
}

// ----------------------------------------------------------------------------
// CTEdge
DfsEdgeType CTEdge::assignDfsType( )
{
	if ( dst->getDfsParent( ) == src ) this->dfs_type = TREE;
	if ( dst->isDfsVisited( ) && !dst->isDfsDone( ) ) this->dfs_type = BACK;
	if ( dst->isDfsDone( ) && ( dst->getDfsTime( ) > src->getDfsTime( ) ) ) this->dfs_type = FORWARD;
	if ( dst->isDfsDone( ) && ( dst->getDfsTime( ) < src->getDfsTime( ) ) ) this->dfs_type = CROSS;
	if ( this->dfs_type == UNKNOWN ) {
		cerr << "unclassified edge: \n"; this->printInfo( );
		assert( 0 );
	}
	return this->dfs_type;
}
void CTEdge::printDfsType( )
{
	switch( this->dfs_type ) 
	{
		case UNKNOWN: cerr << "UNKNOWN"; break;
		case TREE   : cerr << "TREE"; break;
		case BACK   : cerr << "BACK"; break;
		case FORWARD: cerr << "FORWARD"; break;
		case CROSS  : cerr << "CROSS"; break;
    default : assert( 0 );
	}
	cerr << endl;
}
void CTEdge::printInfo( )
{
	cerr << "Edge info: " << endl;
	cerr << "src: "; src->printInfo( );
	cerr << "dst: "; dst->printInfo( );
}


// ----------------------------------------------------------------------------
//  CTGraph

bool CTGraph::edgeExists( CTVertex * from, CTVertex * to )
{
	AdjList adj_list = this->getOutEdges( from );
	AdjList::iterator it;
	for( it = adj_list.begin( ); it != adj_list.end( ); ++ it )
	{
		CTVertex * tmp = (*it)->getTo( );
		if  ( tmp->getId( ) == to->getId( ) )
			return true;
	}
	return false;
}

void CTGraph::addAssignConstr( Relation * r )
{
	vector< Tuple * > rel = r->getRelation( );
	vector< Tuple * >::iterator it;
	for( it = rel.begin( ); it != rel.end( ); ++ it ) 
	{
		int src_var_id = (*it)->el[0]; int dst_var_id = (*it)->el[1];
		CTVertex * src = vertices[ src_var_id ];
		CTVertex * dst = vertices[ dst_var_id ];
		// is this correct ?
		//if ( vars[ src_var_id ]->getTypeId( ) == vars[ dst_var_id ]->getTypeId( ) )
			this->insertEdge( src, dst );
	}
}

void CTGraph::addFieldInsensConstr( Relation & store, Relation & load )
{
	vector< Tuple * > rel = store.getRelation( );
	vector< Tuple * >::iterator it;
	for( it = rel.begin( ); it != rel.end( ); ++ it )
	{
		CTVertex * src = vertices[ (*it)->el[0] ];
		CTVertex * dst = vertices[ (*it)->el[1] ];
		this->insertEdge( src, dst );
	}
	rel = load.getRelation( );
	for( it = rel.begin( ); it != rel.end( ); ++ it )
	{
		CTVertex * src = vertices[ (*it)->el[0] ];
		CTVertex * dst = vertices[ (*it)->el[2] ];
		this->insertEdge( src, dst );
	}
}

void CTGraph::addAssignCastConstr( Relation & r )
{
	vector< Tuple * > rel = r.getRelation( );
	vector< Tuple * >::iterator it;
	for( it = rel.begin( ); it != rel.end( ); ++ it ) 
	{
		CTVertex * src = vertices[ (*it)->el[1] ];
		CTVertex * dst = vertices[ (*it)->el[2] ];
		this->insertEdge( src, dst );
	}
}

void CTGraph::addInsensArrayConstr( Relation & store, Relation & load )
{
	vector< Tuple * > rel = store.getRelation( );
	vector< Tuple * >::iterator it;
	for( it = rel.begin( ); it != rel.end( ); ++ it )
	{
		CTVertex * src = vertices[ (*it)->el[0] ];
		CTVertex * dst = vertices[ (*it)->el[1] ];
		this->insertEdge( src, dst );
	}
	rel = load.getRelation( );
	for( it = rel.begin( ); it != rel.end( ); ++ it )
	{
		CTVertex * src = vertices[ (*it)->el[0] ];
		CTVertex * dst = vertices[ (*it)->el[1] ];
		this->insertEdge( src, dst );
	}
}

void CTGraph::printTopSort( )
{
	cerr << "[BEGIN] topological sort \n";
	vector< CTVertex * >::reverse_iterator it;
  for ( it = top_sort.rbegin( ); it != top_sort.rend( ); ++ it )
	{
		CTVertex * v = (*it);
		v->printInfo( );
	}
	
	/*while( !top_sort.empty( ) ){
		CTVertex * v = top_sort.back( );
		v->printInfo( );
		//cerr << v << " ";
		top_sort.pop_back( );
	}*/

	cerr << "\n[END]   topological sort\n";
}

void CTGraph::dfs( )
{
	//cerr << "[BEGIN] dfs\n";
	
	vector< CTVertex * >::iterator it;
	for ( it = vertices.begin( ); it != vertices.end( ); ++ it )
		if ( ! (*it)->isDfsVisited( ) )
			dfsTraverse( *it );

	//cerr << "[END]   dfs \n";
}

void CTGraph::dfsTraverse( CTVertex * v )
{
	//	cerr << "[DEBUG] " << v << endl;
	v->setDfsVisited( );
	v->setDfsTime( this->newDfsTime( ) );

	// check this!!
	this->scc_stack.push_back( v ); // scc computation
	
	AdjList adj_edges = getOutEdges( v );
	AdjList::iterator it;
	for( it = adj_edges.begin( ); it != adj_edges.end( ); ++ it ) {
		CTEdge * e = *it;
		//cerr << "[DEBUG] traversing edge: " << e << " type: ";
		CTVertex * w = e->getDst( );
		if ( ! w->isDfsVisited( ) ) {
			w->setDfsParent( v );
		  this->processEdge( e ); 
			// depends on w's parent being assigned
			//cerr << "[DEBUG] "; e->printDfsType( );
			dfsTraverse( w );
		} else {
			this->processEdge( e );
			//cerr << "[DEBUG] "; e->printDfsType( );
		}
	}
	this->processVertexDfsExit( v );
	v->setDfsDone( );
	//cerr << "exit : " << v << endl;

}

void CTGraph::processEdge( CTEdge * e )
{
	DfsEdgeType edge_type;
	edge_type = e->assignDfsType( );
	CTVertex * src = e->getSrc( );
	CTVertex * dst = e->getDst( );
	if ( edge_type == BACK ) { 
		cycles ++;
		if ( dst->getDfsTime( ) < src->getSccLow( )->getDfsTime( ) )
		{
			src->setSccLow( dst );
		}
	}

	if (edge_type == CROSS ) {
		if ( dst->getSccNum( ) == -1 ) {
			if ( dst->getDfsTime( ) < src->getSccLow( )->getDfsTime( ) ) {
				src->setSccLow( dst );
			}
		}
	}
}

void CTGraph::processVertexDfsExit( CTVertex * v )
{
	if ( v->getSccLow( ) == v ) {
		this->sccPopComponent( v );
	}

	if ( v->getDfsParent( ) ) {
		if ( v->getSccLow( )->getDfsTime( ) < v->getDfsParent( )->getSccLow( )->getDfsTime( ) ) {
			v->getDfsParent( )->setSccLow( v->getSccLow( ) );
		}
	}
}

void CTGraph::sccPopComponent( CTVertex * v )
{
	CTVertex * tmp;
	unsigned count = 0;
	this->incSccComponent( );
	v->setSccNum( scc_component );
	while( (tmp = scc_stack.back( )) != v )
	{
		this->pushTopSortStack( tmp ); // push a component vertex on the topological sort stack
		tmp->setSccNum( scc_component );
		//tmp->setSccLow( v );
		scc_stack.pop_back( );
		count ++;
	}
	this->pushTopSortStack( v ); // push the root vertex on the topological sort stack
	scc_stack.pop_back( );
	count ++;
	scc_component_size.push_back( count );
	//cerr << "component # " << scc_component << " size: " << scc_component_size[ scc_component ] << endl;
}

void CTGraph::createSccDag( )
{
	vector< CTVertex * >::reverse_iterator it;
	vector< CTVertex * > component;
	component.reserve( 8 );
	for( it = top_sort.rbegin( ); it != top_sort.rend( ); ++ it )
	{
		CTVertex * v = *it;
		assert ( v->isSccRoot( ) );
		int comp_size = scc_component_size[ v->getSccNum( ) ];
		component.insert( component.begin( ), it, it + comp_size );
		for ( int i = 0; i < comp_size; i ++ ) 
			insertSccEdges( component[ i ] );
		component.clear( );
		advance( it, comp_size - 1 );
	}
}

void CTGraph::printInfo( )
{	
	int num_vertices = this->getNumVertices( );
	int num_edges    = this->getNumEdges( );
	this->collectStats( );
	double density  = double(num_edges) / ((double)(num_vertices));
	cerr << "==============================="   << endl;
	cerr << "constraint graph info :"           << endl;
	cerr << "vertices   : " << num_vertices      << endl;
	cerr << "edges      : " << num_edges         << endl;
	cerr << "|E|/|V|    : " << density          << endl;
	cerr << "duplicates : " << this->duplicates << endl;
	cerr << "singular v : " << this->singular   << endl;
	cerr << "connected v: " << this->connected  << endl;
	cerr << "zero out v : " << this->zero_out_degree << endl;
	cerr << "zero in  v : " << this->zero_in_degree  << endl;
	cerr << "max outdeg : " << this->max_out_degree  << endl;
	cerr << "max indeg  : " << this->max_in_degree   << endl;
	cerr << "cycles     : " << this->cycles          << endl;
	cerr << "components : " << this->scc_component + 1  << endl;
	cerr << "==============================="      << endl;
}

void CTGraph::collectStats( )
{
	vector< CTVertex * >::iterator it;
	for ( it = this->vertices.begin( ); it != this->vertices.end( ); ++ it )
	{
		CTVertex * v = (*it);
		assert( this->getOutDegree( v ) == v->out_degree );
		assert( this->getInDegree( v ) == v->in_degree );
		if ( v->out_degree == 0 )
			this->zero_out_degree ++;
		if ( v->in_degree == 0 )
			this->zero_in_degree ++;
		if ( v->out_degree > this->max_out_degree )
			this->max_out_degree = v->out_degree;
		if ( v->in_degree > this->max_in_degree )
			this->max_in_degree = v->in_degree;
		if ( v->out_degree == 0 && v->in_degree == 0 )
			this->singular ++;
		else 
			this->connected ++;
	}
}

