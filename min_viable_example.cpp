#include "tcf.h"
#include "TestDriver.h"

using namespace tcf;

int main()
{
	TD_TestDriver td(" Time-consuming Function 1", tcf1); // A function for testing is required during construction (to capture signature)
	td.add_test(" Time-consuming Function 2", tcf2); // Optional, same signature as tcf1
	td.add_test(" Time-consuming Function 3", tcf3); // Optional, same signature as tcf1
	td.add_test(" Time-consuming Function 4", tcf4); // Optional, same signature as tcf1
	td.add_test(" Time-consuming Function 5", tcf5); // Optional, same signature as tcf1

    // add_test is chainable
	td.add_test(" Time-consuming Function 6", tcf6)  // Optional, same signature as tcf1
	  .add_test(" Time-consuming Function 7", tcf7)  // Optional, same signature as tcf1
	  .add_test(" Time-consuming Function 8", tcf8); // Optional, same signature as tcf1

    // All functions added must have the same signature as the initial function. If different signatures
    // are needed for tests, additional TD_TestDriver instances are required.

    // Run all tests, no input file
	td.run_tests();
}
