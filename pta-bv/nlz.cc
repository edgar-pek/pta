#include <iostream>
using namespace std;

unsigned num_bits( unsigned x )
{
	unsigned n;
	if ( x == 0 ) return( 0 );
	n = 1;
	if (( x >> 16 ) == 0 ) { n = n + 16; x = x << 16; }
	if (( x >> 24 ) == 0 ) { n = n +  8; x = x <<  8; } 
	if (( x >> 28 ) == 0 ) { n = n +  4; x = x <<  4; }
	if (( x >> 30 ) == 0 ) { n = n +  2; x = x <<  2; }
	n = n - ( x >> 31 );
	return 32 - n;
}

int main( )
{
	unsigned num;
	cout << "Enter num: ";
	cin >> num;
	cout << "number of bits for " << num <<  " is : " << num_bits(num) << endl;
	cout << "[END]" << endl;

}


