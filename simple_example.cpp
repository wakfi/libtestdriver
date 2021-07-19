#include <string>
#include <list>
#include "test_driver_decls.h"
#include "boyermoore.h"
#include "naive_string_search.h"

#define TD_USE_INPUT
#define TD_INPUT Search
#define TD_ARGS input->pattern, input->text, input->matches
struct Search : TD_TestInput
{
    std::string pattern;
	std::string text;
	std::list<int> matches;
};
#include "TestDriver.h"

// extract input details from file using TD_infile (ifstream)
TD_PREPARE_INPUT
{
	if(!TD_infile) return false;

	unsigned int len;
	char in_len[10] = {};

	TD_infile.getline(in_len, 10, '\0');
	len = atoi(in_len);
	if(!len) return false;
	if(!TD_infile) return false;
	char* in_str = (char*)malloc(len + 1);
	if(!in_str) return false;
	TD_infile.read(in_str, len);
	if(TD_infile.gcount() != len) goto cleanup;
	if(TD_infile.peek()) goto cleanup;
	TD_infile.ignore();
	if(!TD_infile) goto cleanup;
	*(in_str + len) = '\0';
	input->pattern = std::string(in_str);
	free(in_str);

	TD_infile.getline(in_len, 10, '\0');
	len = atoi(in_len);
	in_str = (char*)malloc(len + 1);
	if(!in_str) return false;
	TD_infile.read(in_str, len);
	if(TD_infile.gcount() != len) goto cleanup;
	TD_infile.ignore();
	*(in_str + len) = '\0';
	input->text = std::string(in_str);
	free(in_str);

	return true;

cleanup:
	free(in_str);
	return false;
}

int main()
{
	TD_TestDriver td(" Boyer-Moore String Search", boyermoore);
	td.add_test("       Naive String Search", naive_string_search);
	td.run_tests("test_in.txt");
}
