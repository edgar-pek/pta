#ifndef ANALYZER_H
#define ANALYZER_H

#include "global.h"
#include "parser.h" 
#include "ParsePAG.h"
#include "CTGraph.h"
#include "Var.h"
#include "Heap.h"
#include <llvm/ADT/SmallVector.h>

using llvm::SmallVector;

class Analyzer
{
public:
	Analyzer( )
		: active_dfs_visited( false )
		, dfs_visited_count( 0 )
		, active_sccroot_ins( false )
		, sccroot_ins_count( 0 )
	{ }

	inline void initVars(  int vars_num  ) { initRefs< Var  >( vars_num,  this->vars  ); }
	inline void initHeaps( int heaps_num ) { initRefs< Heap >( heaps_num, this->heaps ); }
	//inline void initFields( int fields_num ) { initRefs
	inline void initVars(  Reference * var_ref  ) { initRefs< Var  >( var_ref,  this->vars  ); }
	inline void initHeaps( Reference * heap_ref ) { initRefs< Heap >( heap_ref, this->heaps ); }
	inline void setParsePAG( ParsePAG * pp ) { parse_pag = pp; }
	inline void setCTGraph( CTGraph * g )    { ct_graph = g; }

	inline Var  * getVar(  int id ) { return this->getRef( id, this->vars ); }
	inline Heap * getHeap( int id ) { return this->getRef( id, this->heaps ); }
	inline vector< Var * > & getVars( ) { return this->vars; }
	inline vector< Heap * > & getHeaps( ) { return this->heaps; }
	
	void initFromPAG( );
	void setTopOrdVars( );

	bdd getHeaps4DecType( int dec_type_id );
	void createTypeFilter( int dec_type_sz, Relation * declared_types, Relation * alloc_types, Relation * var_types );
	void processAlloc( bool worklist = false );
	bool processFieldSensStores( bool worklist = false );
	bool processFieldSensLoads ( bool worklist = false );
	void mergePtSets( );
	bool propagateAssignsIter( );
	void propagateWorkList( );

	//BitVector queryVarPt( Var * var );
	//BitVector queryVarPtScc( Var * var );
	uint countPtBddNodes( );
	uint countPtSet( );
	uint countPtSetNew( );

	/*
	inline void resetChangedCur( ) { changed_vars_cur.clear( ); }
	inline void resetChangedNew( ) { changed_vars_new.clear( ); }
	inline unsigned countChangedCur( ){ return changed_vars_cur.count( ); }
	inline unsigned countChangedNew( ){ return changed_vars_new.count( ); }
	inline void changedNew2Cur( ) { changed_vars_cur.clear( ); changed_vars_cur |= changed_vars_new; changed_vars_new.clear( ); }
	*/

	void doBaseIterAnalysis( );

	void doWorkListAnalysis( );
	
	void compareWithSpark( const char * fname );
	//void printPtSet( CTGraph * g,  Reference * heap_ref, int var_id );
	void printInfo( );
private:
	inline void initDfsVisited( ) { assert( !active_dfs_visited ); active_dfs_visited = true; dfs_visited.resize( vars.size( ), dfs_visited_count ); dfs_visited_count ++; }
	inline void setDfsVisited( CTVertex * v ) { assert( v->getId( ) < (int) dfs_visited.size( ) );	dfs_visited[ v->getId( ) ] = dfs_visited_count; }
	inline bool isDfsVisited(  CTVertex * v ) { assert( active_dfs_visited ); assert( v->getId( ) < (int) dfs_visited.size( ) ); return dfs_visited[ v->getId( ) ] == dfs_visited_count; }
	inline void doneDfsVisited( ) { assert( active_dfs_visited ); active_dfs_visited = false; }

	//inline void initDfsFinished( ) { assert( !active_dfs_visited ); active_dfs_visited = true; dfs_visited.resize( vars.size( ), dfs_visited_count ); dfs_visited_count ++; }
	//inline void setDfsFinished( CTVertex * v ) { assert( v->getId( ) < (int) dfs_visited.size( ) );	dfs_visited[ v->getId( ) ] = dfs_visited_count; }
	//inline bool isDfsFinished( CTVertex * v ) { assert( active_dfs_visited ); assert( v->getId( ) < (int) dfs_visited.size( ) ); return dfs_visited[ v->getId( ) ] == dfs_visited_count; }
	//inline void doneDfsFinished( ) { assert( active_dfs_visited ); active_dfs_visited = false; }
	
	inline void initSccRootIns( ) { assert( !active_sccroot_ins ); active_sccroot_ins = true; sccroot_ins.resize( vars.size( ), sccroot_ins_count ); sccroot_ins_count ++; }
	inline void setSccRootIns( CTVertex * v ) { assert( v->getId( ) < (int) sccroot_ins.size( ) );	sccroot_ins[ v->getId( ) ] = sccroot_ins_count; }
	inline bool isSccRootIns( CTVertex * v ) { assert( active_sccroot_ins ); assert( v->getId( ) < (int) sccroot_ins.size( ) ); return sccroot_ins[ v->getId( ) ] == sccroot_ins_count; }
	inline void doneSccRootIns( ) { assert( active_sccroot_ins ); active_sccroot_ins = false; }

	template < class T > void initRefs( int sz, vector< T * > & v );
	template < class T > void initRefs( Reference * ref, vector< T * > & v );
	template < class T > 
		inline T * getRef( int id, vector< T * > & v ) { assert( (unsigned) id < v.size( ) ); return v[ id ]; }

	void initVarTypes( Relation * var_types );
	void createAssignableTypes( Relation * declared_types, Relation * alloc_types );
	void initActualTypeRelations( Relation * declared_types, Relation * alloc_types );
	
	// worklist operations
	//void worklistInsert( int var_id );
	//void worklistNewInsert( int var_id );
	//void worklistRemove(   int var_id );
	//bool worklistContains( int var_id );
	//bool worklistEmpty( );
	//unsigned worklistSize( );
	void flushNewHeapPtSets( );
	
	// heap operations
	void pendingVarsToHeap( );    // inlined below
	void  pushVarsHeap( Var * v); // inlined below
	Var *  popVarsHeap( );        // inlined below
	bool isEmptyVarsHeap( ) { return vars_heap.empty( ); }
	uint getVarsHeapSize( )    { return vars_heap.size( );  } 

	// for DFS search (query)
	bool active_dfs_visited;
	vector< int > dfs_visited;
	int dfs_visited_count;
	//bool active_dfs_finished;
	//vector< int > dfs_finished;
	//int dfs_finished_count;
	bool active_sccroot_ins;
	vector< int > sccroot_ins;
	int sccroot_ins_count;
	
	vector< Var *  > vars;
	vector< Heap * > heaps;
	multimap< int, int > dec2heap;
	multimap< int, int > dec2var;
	multimap< int, int > act2dec;
	multimap< int, int > act2heap;
	ParsePAG * parse_pag;
	CTGraph  * ct_graph;

	uint analysis_iter;
	//BitVector changed_vars_cur;
	//BitVector changed_vars_new;
	//BitVector changed_vars_old;
	vector< Var * > vars_heap;
	BitVector pending_vars;
	//SmallVector< int, 1000 > changed_vars_new;
	set< pair< int, int > > changed_heaps;
};
/*
inline void Analyzer::worklistInsert( int var_id )
{
	changed_vars_cur.set( var_id );
}

inline void Analyzer::worklistNewInsert( int var_id )
{
	changed_vars_new.set( var_id );
}

inline void Analyzer::worklistRemove( int var_id )
{
	changed_vars_cur.reset( var_id );
	//changed_vars_old.set( var_id );
}

inline bool Analyzer::worklistContains( int var_id )
{
	return changed_vars_cur.test( var_id );
}

inline bool Analyzer::worklistEmpty( )
{
	return changed_vars_cur.empty( );
}

inline unsigned Analyzer::worklistSize( ) 
{
	return changed_vars_cur.count( );
}
*/ 
inline void Analyzer::flushNewHeapPtSets( )
{
	set< pair< int, int > >::iterator it;
	for( it = changed_heaps.begin( ); it != changed_heaps.end( ); ++ it )
	{
		pair< int, int > p = * it;
		int heap_id = p.first;
		int fld_id = p.second;
		heaps[ heap_id ]->flushNewPtSet( fld_id );
	}
	changed_heaps.clear( );
}
inline void Analyzer::pendingVarsToHeap( ) 
{
	BitVector::iterator it;
	for( it = pending_vars.begin( ); it != pending_vars.end( ); ++ it )
		vars_heap.push_back( vars[ * it ] );
	make_heap( vars_heap.begin( ), vars_heap.end( ), Var::topOrdGreaterThan( ) );
	pending_vars.clear( );
}


inline void Analyzer::pushVarsHeap( Var * v) 
{ 
	vars_heap.push_back( v ); 
	push_heap( vars_heap.begin( ), vars_heap.end( ), Var::topOrdGreaterThan( ) ); 
}
inline Var * Analyzer:: popVarsHeap( )        
{ 
	Var * v = vars_heap.front( ); 
	pop_heap( vars_heap.begin( ), vars_heap.end( ), Var::topOrdGreaterThan( ) ); 
	vars_heap.pop_back( ); 
	return v; 
}

template< class T >
inline void Analyzer::initRefs( int sz, vector< T * > & v )
{
	v.resize( sz );
	for ( int i = 0; i < sz; ++ i )
		v[ i ] = new T( i );
}

template < class T  >
inline void Analyzer::initRefs( Reference * ref, vector< T * > & v )
{
	int size = ref->getDomainSize( );
	v.resize( size );
	for( int i = 0; i < size; ++ i )
		v[ i ] = new T( i );
}

#endif
