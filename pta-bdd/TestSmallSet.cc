#include "TestSmallSet.h" 

void testSmallSet( )
{
	cerr << "testing SmallSet interface :" << endl;
	SmallSet< int, 8 > ss;
	cerr << "inserting 0 : " << ss.insert( 0 ) << endl;
	cerr << "inserting 1 : " << ss.insert( 1 ) << endl;
	cerr << "set size    : " << ss.size( ) << endl;
	cerr << "inserting 3 : " << ss.insert( 3 ) << endl;
	cerr << "inserting 7 : " << ss.insert( 7 ) << endl;
	cerr << "inserting 16: " << ss.insert( 16 ) << endl;
	cerr << "set size    : " << ss.size( ) << endl;
	cerr << "inserting 1 : " << ss.insert( 1 ) << endl;
	cerr << "inserting 3 : " << ss.insert( 3 ) << endl;
	cerr << "query 1 ?   : " << ss.count( 1 ) << endl;
	cerr << "query 3 ?   : " << ss.count( 3 ) << endl;
	cerr << "query 10?   : " << ss.count( 10 ) << endl;
	cerr << "query 16?   : " << ss.count( 16 ) << endl;
	cerr << "query 90?   : " << ss.count( 90 ) << endl;
	cerr << "query 7 ?   : " << ss.count( 7 ) << endl;
}
