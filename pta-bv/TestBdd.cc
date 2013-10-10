#include "TestBdd.h"

void testFdd( )
{
	bdd pta, ptb, ptc;
	enum HEAP_TYPE {HEAP_X, HEAP_Y, HEAP_Z}; 
	HEAP_TYPE x = HEAP_X;
	HEAP_TYPE y = HEAP_Y;
	HEAP_TYPE z = HEAP_Z;

	cerr << "testing Buddy's finite domain interface " << endl;
	bdd_init(1000,1000);

	int heap_domain [] = { 3 }; // want to represent 3 heap locations - the next larget pow 2 is 4
	fdd_extdomain( heap_domain, 1 );

	// init - assign heap allocation
	// X: a = new O( )
	pta = fdd_ithvarpp(0, x);
	// Y: b = new O( )
	ptb = fdd_ithvarpp(0, y);
	// Z: c = new O( )
	ptc = fdd_ithvarpp(0, z);

	// a = b => pt(a) = pt(a) \/ pt(b)
	pta |= ptb;
	// b = a
	ptb |= pta;
	// c = b
	ptc |= ptb;



	char fname0[] = "pta.dot";
	if (bdd_fnprintdot(fname0, pta ) == 0) cout << "Written file : " << fname0 << ".\n";
	char fname1[] = "ptb.dot";
	if (bdd_fnprintdot(fname1, ptb ) == 0) cout << "Written file : " << fname1 << ".\n";
	char fname2[] = "ptc.dot";
	if (bdd_fnprintdot(fname2, ptc ) == 0) cout << "Written file : " << fname2 << ".\n";
	
	bdd_printstat( );
	bdd_done( );
	cerr << "-------------------------------------------------" << endl;

}

void testBdd( )
{
	cerr << "test Buddy BDD interface " << endl;
	bdd x, y, z;

	bdd_init(1000, 1000);
	bdd_setvarnum(5);

	x = bdd_ithvar(0);
	y = bdd_ithvar(1);
	z = x & y;
	
	char fname[] = "test-bdd.dot";

	if (bdd_fnprintdot(fname, z) == 0) cout << "Written file : " << fname << ".";
	cout << bddtable << z << endl;

	bdd_done( );
	cerr << "-------------------------------------------------" << endl;
}
