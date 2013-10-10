#include "TestStringMap.h"

void testStringMap0( )
{
	cerr << "---------------------" << endl;
	cerr << "testing StringMap 0 " << endl;
	string str0( "test" );
	string str1( "abc" );
	StringRef str_ref0( str0 );
	StringRef str_ref1( str1 );
	StringMap< int > sm;
	// this is the correct usage!
	StringMapEntry< int > & e0 = sm.GetOrCreateValue( str_ref0, 7 );
	// this is an incorrect usage: it must be StringMapEntry< int > & e1
	//StringMapEntry< int > e1 = sm.GetOrCreateValue( StringRef( "abc"), 12 );
	StringMapEntry< int >  & e1 = sm.GetOrCreateValue( StringRef( "abc"), 12 );
	cerr << "string map size : " << sm.size( ) << endl;
	cerr << "sm lookup for " << str_ref0.str( ) << " : " << sm.lookup( str_ref0 ) << endl;
	cerr << "sm lookup for " << str_ref1.str( ) << " : " << sm.lookup( str_ref1 ) << endl;
	
	cerr << "entries : " << endl;
	cerr << "e0 - key : " << e0.getKey( ).str( ) << endl;
	cerr << "e0 - val : " << e0.getValue( ) << endl;
	cerr << "e1 - key : " << e1.getKey( ).str( ) << endl;
	cerr << "e1 - val : " << e1.getValue( ) << endl;
	cerr << "----------------- " << endl;
	cerr << "iterators : " << endl;
	string str_tmp( "abc" );
	StringMap< int >::iterator it = sm.find( StringRef( str_tmp ) );
	if ( it != sm.end( ) )
		cerr << "it->second : " << it->second << endl;
	else
		cerr << str_tmp << " not found in the StringMap! " << endl;
	cerr << "============================================================" << endl;
}
