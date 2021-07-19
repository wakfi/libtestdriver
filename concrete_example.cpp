#include <string>
#include <list>
#include <iostream>
#include "test_driver_decls.h"
#include "boyermoore.h"
#include "naive_string_search.h"

#define TD_ARGS input->pattern, input->text, input->matches
#define TD_RETURN_TO output->success
#define TD_PRE_TIMER input->matches.clear();
#define TD_POST_TIMER output->matched = input->matches.size();
#define TD_INPUT Search
#define TD_OUTPUT Results
#define TD_DATA Metrics
struct Search : TD_TestInput
{
    std::string pattern;
	std::string text;
	std::list<int> matches;
};
struct Results : TD_TestOutput
{
	bool success;
	int matched;
};

TD_EXTEND
struct Metrics : TD_TestMetricsBase
{
	TD_METRICS(Metrics)
	{}

	int match_count = 0;
	int success_count = 0;

	void print_result() const
	{
		using namespace std;
		if(!this->success_count && this->match_count) return;

		cout << "  Search" << " Found....: " << this->success_count << '\n';
		cout << "  Search" << " Matches..: " << this->match_count
			 << " occurances\n";
		cout << " Average" << " Matches..: " << ((1.0 * this->match_count) / this->success_count)
		     << " occurances\n";
	}

	void reset()
	{
		this->success_count = 0;
		this->match_count = 0;
	}
};

#define TD_USE_INPUT
#define TD_USE_OUTPUT

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

TD_HANDLE_OUTPUT
{
	data->match_count += output->matched;
	data->success_count += output->success;
}

bool dummy_search_func(const std::string& a, const std::string& b, std::list<int>& c)
{
	for(unsigned i = 0; i < 10000; ++i);
	return true;
}

int main()
{
	TD_TestDriver td = TD_TestDriver(" Boyer-Moore String Search", boyermoore);
	td.add_test("       Naive String Search", naive_string_search);
	td.add_test("              Search Dummy", dummy_search_func);
	td.run_tests("test_in.txt");
	td.run_tests("rand_10000.txt");
}
