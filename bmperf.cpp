//----------------------------------------------------------------------
// NAME: Walker Gray
// FILE: bmperf.cpp
// DATE: Summer 2021
// DESC: Driver program for Boyer-Moore string search
//----------------------------------------------------------------------

#include <string>
#include <iostream>
// Not reccomended to #include TestDriver here, can cause it to be improperly defined.
// Instead, create a header file to handle the inlcude(s) and any configuration needed
#include "search_tests_example.h"
#include "boyermoore.h"
#include "naive_string_search.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		std::cerr << "usage: " << argv[0] << " [filename]" << std::endl;
		return 1;
	}
	// Use default file if no input file given
	string file(argc==2 ? argv[1] : "test_in.txt");

    TD_TestDriver td = TD_TestDriver(" Boyer-Moore String Search", boyermoore);
    td.add_test("       Naive String Search", naive_string_search);
	td.run_tests(file);
}
