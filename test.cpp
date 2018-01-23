#include "amortizer.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

// Convert integer to binary string in reverse order (LSB left)
string dec2binrev(int a)
{
    string binary;
    int mask = 1;
    for(int i = 0; i < 31; i++)
    {
        if((mask&a) >= 1)
            binary += '1';
        else
            binary += '0';
        mask<<=1;
    }
    return binary;
}

// Determine number of bits changing with increment
double get_ci_binary_counter( int from ){
	string sfrom = dec2binrev( from );
	sfrom += '0';
	string sto = dec2binrev( from + 1 );
	int count = 0;
	for( unsigned int i = 0 ; i < sto.length() ; i++ )
		if( sfrom[i] != sto[i] )
			count++;
	return count;
}

double get_ci_dynamic_array( int i ){
	if( ((i - 1) & (i - 2)) == 0 && i )
		return i;
	return 1;
}

double get_ci( int i ){
	// return get_ci_binary_counter(i); // Amortizable to 2
	return get_ci_dynamic_array(i); // Amortizable to 3
	// return i*log2(i);  // Not amortizable
}

void print_times( vector<double>& times ){
	for( double time : times ){
		printf("%2.2f\t", time);
		for( int i = 0 ; i < time ; i++ )
			cout << "=";
		cout << endl;
	}
}

int main( int argc , char** argv )
{
	vector<double>	times;
	vector<int>		x;
	for( int i = 1 ; i < 37 ; i++ ){
		times.push_back( get_ci(i) );
		x.push_back(i);
	}
	vector<double> new_times;
	
	cout << "Starting with:" << endl;
	print_times( times );

	
	amortizer()
		.fn_values( times )
		.apply( std::back_inserter(new_times) );
	
	
	cout << "Ending with:" << endl;
	print_times( new_times );
	
	return 0;
}