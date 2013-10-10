#include "Timer.h"
#include "parser.h"
#include "ParsePAG.h"
#include "DumpDOOP.h"
#include "CTGraph.h"
#include "Analyzer.h"
#include "TestBdd.h"

static inline int memReadPeak(void)
{
    char  name[256];
    pid_t pid = getpid();

    sprintf(name, "/proc/%d/status", pid);
    FILE* in = fopen(name, "rb");
    if (in == NULL) return 0;

    // Find the correct line, beginning with "VmPeak:":
    int peak_kb = 0;
    while (!feof(in) && fscanf(in, "VmPeak: %d kB", &peak_kb) != 1)
        while (!feof(in) && fgetc(in) != '\n')
            ;
    fclose(in);

    return peak_kb;
}


int main( int argc, char * argv [] )
{	
	if ( argc < 2 ) {
		cerr << "usage : " << argv[ 0 ] << " < SPARK's PAG-file > \n\n";
		exit( 1 );
	}

	Timer t_glob; 
	t_glob.start( );
	
	ParsePAG parse_pag;
	cerr << "Parsing SPARK's PAG file " << argv[1] << " ..." << endl;
	parse_pag.parsePAGfile( argv[1] );
	t_glob.printElapsed( );
	parse_pag.printInfo( );
	cerr << "\t[DONE] \n";

	// initialize Buddy's BDD manager
	bdd_init(1000000,100000);
	bdd_gbc_hook(NULL);
	bdd_setcacheratio(64);
	bdd_disable_reorder();
	bdd_setmaxincrease(1000000);

	//testBdd( );
	//testFdd( );

	//DumpDOOP dump_doop( & parse_pag );
	//dump_doop.dumpInputFacts( );
	//return 0;
	

	Analyzer analyzer;
	analyzer.setParsePAG( & parse_pag );
	analyzer.initFromPAG( );
	//analyzer.doBaseIterAnalysis( );
	analyzer.doWorkListAnalysis( );
	cerr << "Total time             : " << t_glob.elapsed( ) << " s." << endl;
	cerr << "Peak memory            : " << (double) memReadPeak( )/1024 << " MB." << endl;
	//t_glob.printElapsed( );
	
	/* FIXME: change comparison
	if ( argc > 2 ) 
	{
		cerr << "---------------------------------------------------------------- \n";
		cerr << "Reading SPARK's solution file " << argv[ 2 ] << " ... ";
		analyzer.compareWithSpark( argv[2 ] );
		cerr << "---------------------------------------------------------------- \n";
	}
	*/

	cerr << "Heap domain size is: " << fdd_domainsize(0) << endl;
	bdd_printstat( );
	bdd_done( );
	return 0;
}

//#include "SmtEncoding.h"
	/* string fname = "test-enc.smt";
	ofstream os( fname.c_str( ) );
	SmtEncoding se( os );
	se.setVarRef( var_ref );
	se.setObjRef( obj_ref );
	se.setAhpRel( & ahp_rel );
	se.setAllRel( & all_rel );
	se.writeEncoding( );
	cerr << "smt encoding written in " << fname << endl;
	se.printInfo( );
	os.close( );
	*/

/* OLD INPUT PARSING
	// --------------------------------------------------------------------------------------------------------
	string path = string( argv[ 1 ] );
	cout << "----------------------------------------------------------- \n";
	cout << "Reading input facts...                                      \n";
	FileNames * fileNames = new FileNames( path );
	string ref_relations [] = { "VarRef", "NormalHeapAllocationRef", "AssignHeapAllocation", "AssignLocal",
		                          "FieldSignatureRef", "StoreInstanceField", "LoadInstanceField", 
															"ClassType", "AssignCast", "StoreArrayIndex", "LoadArrayIndex"}; 
	vector < string > relNames ( ref_relations, ref_relations + sizeof( ref_relations ) / sizeof( string ) );
	fileNames->setFileNames( relNames );
	
	// references
	//cerr << "\treading VarRef ..." << endl;
	Reference * var_ref = new Reference( fileNames->getFullFileName( "VarRef" ), "VAR" );
	//t_glob.printElapsed( );
	//cerr << "\treading NormalHeapAllocationRef ... \n";
	Reference * obj_ref = new Reference( fileNames->getFullFileName( "NormalHeapAllocationRef" ), "HEAP" );
	//t_glob.printElapsed( );
	
	//cerr << "\treading FieldSignatureRef ... \n";
	Reference * field_ref = new Reference( fileNames->getFullFileName( "FieldSignatureRef" ), "FIELD" );
	//type_ref->loadFromFile( fileNames->getFullFileName( "InterfaceType" ) );
	// relations
	vector< Reference * > refs; refs.push_back( obj_ref ); refs.push_back( var_ref );
	cerr << "\treading AssignHeapAllocation ... \n";
	Relation ahp_rel ( fileNames->getFullFileName("AssignHeapAllocation"), "AHP", refs, 3 );
	//t_glob.printElapsed( );
	refs.clear( ); refs.push_back( var_ref ); refs.push_back( var_ref );
	cerr << "\treading AssignLocal ... \n";
	Relation all_rel ( fileNames->getFullFileName("AssignLocal"), "ALL", refs, 3 );
	
	refs.clear( ); refs.push_back( var_ref ); refs.push_back( var_ref ); refs.push_back( field_ref );
	cerr << "\treading StoreInstanceField ... \n";
	Relation sif_rel ( fileNames->getFullFileName("StoreInstanceField"), "SIF", refs, 4 );
	refs.clear( ); refs.push_back( var_ref ); refs.push_back( field_ref ); refs.push_back( var_ref );
	cerr << "\treading LoadInstanceField ... \n";
	Relation lif_rel ( fileNames->getFullFileName("LoadInstanceField"), "LIF", refs, 4 );
	// --------------------------------------------------------------------------------------------------------
	
*/
 
