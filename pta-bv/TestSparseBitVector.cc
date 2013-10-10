#include "TestSparseBitVector.h"

void testSparseBitVector0( )
{
	cerr << "testSparseBitVector0 " << endl;
	SparseBitVector< > bve;
	cerr << " set 0 : " << bve.test_and_set( 0 ) << endl;
	cerr << " set 2 : " << bve.test_and_set( 2 ) << endl;
	cerr << " set 9 : " << bve.test_and_set( 9 ) << endl;
	cerr << " set 10000 : " << bve.test_and_set( 10000 ) << endl;
	cerr << " set 55000 : " << bve.test_and_set( 55000 ) << endl;
	cerr << " count : " << bve.count( ) << endl;
	cerr << " hash  : " << bve.getHashValue( ) << endl;
	//llvm::dump( bve, llvm::errs( ) );
	cerr << "-------------------------------" << endl;
}
