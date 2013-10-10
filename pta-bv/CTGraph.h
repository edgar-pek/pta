#ifndef CT_GRAPH_H
#define CT_GRAPH_H

#include "global.h"
#include "parser.h"
#include <llvm/ADT/SmallPtrSet.h>
#include "Var.h"
#include "Heap.h"

using llvm::SmallPtrSet;
using llvm::SmallPtrSetIterator;

class CTVertex
{
public:	
	CTVertex( int id_ )
		: in_degree( 0 ), out_degree( 0 )
		, id ( id_ )
		, dfs_visited( false )
	  , dfs_done( false )
		, dfs_time( 0 )
	  , dfs_parent( NULL )
    , scc_low( this )
		, scc_num( -1 )
	{}

	inline int getId( ) { return this->id; }

	inline void setDfsVisited( ) { this->dfs_visited = true; } 
	inline bool isDfsVisited( )  { return this->dfs_visited; }
	inline void setDfsDone( ) { this->dfs_done = true; }
	inline bool isDfsDone( )  { return this->dfs_done; }

	inline void     setDfsTime( unsigned t ) { this->dfs_time = t; }
	inline unsigned getDfsTime( )            { return this->dfs_time; }
	inline void        setDfsParent( CTVertex * v ) { this->dfs_parent = v; }
	inline CTVertex *  getDfsParent( )              { return this->dfs_parent; }
	inline void       setSccLow( CTVertex * v ) { this->scc_low = v; }
	inline CTVertex * getSccLow( )              { return this->scc_low; }
	inline int  getSccLowId( ) { return this->scc_low->id; }
	inline bool isSccRoot( )    { return  (this == this->scc_low); };
	inline void setSccNum( int n ) { this->scc_num = n; }
	inline int  getSccNum( )       { return this->scc_num; }

	inline void printCTVertexId( ostream &os ) { os << this->id; }
	//inline void printCTVertexSccRoot( ostream &os ) { os << this->scc_root; }a
	inline friend ostream	& operator<<( ostream &os, CTVertex * v ) { assert( v ); os << " id: " << v->id << " scc root: " << v->scc_low->id << " scc_num: " << v->scc_num << " dfs time:  " << v->dfs_time; return os; }

	unsigned in_degree, out_degree; 
	void printInfo( );
	// scc adj operations 
	typedef SmallPtrSet< CTVertex *, 8 > SccAdjList;
	inline void insertSccAdjIn( CTVertex * v ) { scc_adj_in.insert( v ); }
	inline bool isSccAdjIn( CTVertex * v ) { return scc_adj_in.count( v ); }
	inline unsigned sccAdjInSize( ) { return scc_adj_in.size( ); }
	inline SccAdjList & getSccAdjList( ) { return scc_adj_in; }
	
	// -------------------------------------------------------------------
private:
	int  id;
	bool dfs_visited;
	bool dfs_done;
	unsigned dfs_time;
	CTVertex * dfs_parent;
	CTVertex * scc_low;
	int scc_num;
	SccAdjList scc_adj_in;
};

enum DfsEdgeType { UNKNOWN, TREE, BACK, FORWARD, CROSS };
class CTEdge
{
public:
	CTEdge( CTVertex * src_, CTVertex * dst_ )
		: src( src_  )
		, dst( dst_  )
		, dfs_type( UNKNOWN )
	{ }


	inline CTVertex * getFrom( ) { return this->src; }
	inline CTVertex * getTo  ( ) { return this->dst; }
	inline CTVertex * getSrc ( ) { return this->src; }
	inline CTVertex * getDst ( ) { return this->dst; }
	inline CTVertex * getLhs ( ) { return this->dst; }
	inline CTVertex * getRhs ( ) { return this->src; }

	inline void printCTEdge( ostream & os ){ os << src->getId( ) << " -> " << dst->getId( ); }
	inline friend ostream & operator<<( ostream & os, CTEdge * e ){ assert(e); e->printCTEdge( os ); return os; }

	DfsEdgeType assignDfsType( );
	
	void printDfsType( );
	void printInfo( );

private:
	CTVertex * src;
	CTVertex * dst;
	DfsEdgeType dfs_type;
};

class CTGraph
{
public: 
	CTGraph( int num_vars ) 
		// statistics
		: duplicates( 0 ), zero_in_degree( 0 ), zero_out_degree( 0 ), max_out_degree( 0 ), max_in_degree( 0 )
		, singular( 0 ), connected( 0 ), cycles( 0 )
		// private members
		, adj    ( vector< AdjList >( num_vars ) )
		, adj_inv( vector< AdjList >( num_vars ) )
		, adj_var( vector< AdjList >( num_vars ) )
	  , vertices( vector< CTVertex * >( num_vars ) )
		, dfs_time( 0 ) 
		, scc_component( -1 )
		, var_ref_size( 0 )
		{ 
			for( int i = 0; i < num_vars; ++ i )
				vertices[ i ] = new CTVertex( i );
		}

	void addAssignConstr( Relation * r );
	void addFieldInsensConstr( Relation & store, Relation & load );
	void addAssignCastConstr( Relation & r );
	void addInsensArrayConstr( Relation & sar, Relation & lar );

	typedef vector< CTEdge * > AdjList;
	void insertEdge( CTVertex * from, CTVertex * to );
	bool edgeExists( CTVertex * from, CTVertex * to );

	inline unsigned  getOutDegree( CTVertex * v ) { assert( v != NULL && (unsigned) v->getId( ) <    adj.size( ) ); return adj[v->getId()].size( ); }
	inline unsigned  getInDegree ( CTVertex * v ) { assert( v != NULL && (unsigned) v->getId( ) < adj_inv.size( ) ); return adj_inv[v->getId( )].size( ); }
	inline AdjList & getOutEdges(  CTVertex * v ) { assert( v != NULL && (unsigned) v->getId( ) <    adj.size( ) ); return adj[v->getId( )]; }
	inline AdjList & getInEdges (  CTVertex * v ) { assert( v != NULL && (unsigned) v->getId( ) < adj_inv.size( ) ); return adj_inv[v->getId( )]; }
	
	inline int getNumVertices( ) { return adj.size( );   };
	inline CTVertex * getCTVertex( int id ) { assert( (unsigned) id < vertices.size() ); return vertices[ id ]; }

	inline int getNumEdges   ( ) { return edges.size( ); }
	
	void printTopSort( );
	inline void pushTopSortStack( CTVertex * v ){ this->top_sort.push_back( v ); }
	// dfs traversal
	void dfs( ); 
	void dfsTraverse( CTVertex * v ); 
	inline unsigned newDfsTime( ) { return ++ this->dfs_time ; }
	void processEdge( CTEdge * e );

	void processVertexDfsExit( CTVertex * v ); 
	void sccPopComponent( CTVertex * v );
	void incSccComponent( ) { this->scc_component ++; } 
	int  getSccComponent( ) { return scc_component; }

	inline AdjList & getAdjList( int i ) { assert( (unsigned) i < this->adj.size( ) ); return adj[i];  }
	inline AdjList & getAdjList( CTVertex * v ) { assert( (unsigned) v->getId( ) < this->adj.size( ) ); return adj[v->getId( )];  }
	inline AdjList & getInvAdjList( int i ) { assert( (unsigned) i < this->adj_inv.size( ) ); return adj_inv[i];  }
	inline AdjList & getVarAdjList( int i ) { assert( (unsigned) i < this->adj_var.size( ) ); return adj_var[i];  }

	inline vector< CTVertex * > & getTopOrdVertices( ){ return this->top_sort; }

	inline vector< unsigned >   & getSccComponentSizeVector( ) { return this->scc_component_size; }
	inline unsigned getSccComponentSize( int i ) { assert( (unsigned) i < scc_component_size.size( ) ); return scc_component_size[ i ]; }
	inline unsigned getSccComponentSize( CTVertex * v ) { assert( v ); return scc_component_size[ v->getSccNum( ) ]; } 
	CTVertex * getSccRoot( CTVertex * v ); // inlined below
	// scc_dag manipulation
	void createSccDag( );
	void insertSccEdges( CTVertex * v ); // inlined below

	void printInfo( );

	//statistics: read only member variables
	unsigned duplicates, zero_in_degree, zero_out_degree, max_out_degree, max_in_degree, singular, connected; 
	unsigned cycles;
	void collectStats( );

	inline void setVarRefSize( int n ) { this->var_ref_size = n; }
	inline void setVars( vector< Var * > & vars_ ) { this->vars = vars_; }

private:
	vector< AdjList >    adj;
	vector< AdjList >    adj_inv;
	vector< AdjList >    adj_var;
	vector< CTEdge * >   edges;
	vector< CTVertex * > vertices;
	unsigned dfs_time;
	vector< CTVertex * > top_sort;         // stack for the topological sorting
	vector< CTVertex * > scc_stack;        // stack for the scc
	int scc_component;                     // the scc component number
	vector< unsigned > scc_component_size; // tracking the number of vertices in an scc
	unsigned var_ref_size;
	vector< Var * > vars;
};

// =============================================================================================
//                                        inlined methods 
//==============================================================================================

inline void CTGraph::insertEdge( CTVertex * from, CTVertex * to ) 
{ 
	if ( !edgeExists( from, to) ) 
	{ 
		CTEdge * e = new CTEdge( from, to ); 
		from->out_degree++; to->in_degree++; 
		this->adj[ from->getId( ) ].push_back( e ); 
		this->adj_inv[ to->getId( ) ].push_back( e ); 
		this->edges.push_back( e ); } 
	else 
	{
		this->duplicates++;
	} 
}

inline CTVertex * CTGraph::getSccRoot( CTVertex * v ) 
{ 
	Var * var = vars[ v->getId( ) ];
	CTVertex * scc_root = v->getSccLow( ); 
	Var * scc_root_var = vars[ scc_root->getId( ) ];
	while( ! scc_root->isSccRoot( ) ) 
		scc_root = scc_root->getSccLow( );
	if( var->getTypeId( ) == scc_root_var->getTypeId( ) )
		return scc_root;
	else 
		return v;
}

inline void CTGraph::insertSccEdges( CTVertex * v )
{
	AdjList adj_list = getInEdges( v );
	AdjList::iterator it;
	for( it = adj_list.begin( ); it != adj_list.end( ); ++ it )
		getSccRoot( v )->insertSccAdjIn( getSccRoot( (*it)->getFrom( ) ) );
}


#endif
