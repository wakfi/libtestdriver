class Search;
class SearchDriver;
#define INPUT_T Search
#include <string>
#include <list>
#include "test_driver_decls.h"
#include "boyermoore.h"

#define FUN_DECL bool test_func(const std::string&, const std::string&, std::list<int>&)
#define FUN_PTR bool(*)(const std::string&, const std::string&, std::list<int>&)
#define ARGS input.pattern, input.text, matches
#define PRE_TIMER std::list<int> matches;
class Search : TestInput
{
    TEST_DATA_IMPL
	END
    std::string pattern;
	std::string text;
};
#include "TestDriver.h"

// extract input details from file using TD_infile (ifstream)
PREPARE_INPUT_IMPL
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
	input.pattern = std::string(in_str);
	free(in_str);

	TD_infile.getline(in_len, 10, '\0');
	len = atoi(in_len);
	in_str = (char*)malloc(len + 1);
	if(!in_str) return false;
	TD_infile.read(in_str, len);
	if(TD_infile.gcount() != len) goto cleanup;
	TD_infile.ignore();
	*(in_str + len) = '\0';
	input.text = std::string(in_str);
	free(in_str);

	return true;

cleanup:
	free(in_str);
	return false;
}

HANDLE_OUTPUT_IMPL
{}

template<typename R, typename ...Args>
class BMDriver : public TDBase(TestDriver)
{
	public:
	BMDriver() : TDBase(TestDriver)(" Boyer-Moore", boyermoore){};
};
BMDriver() -> BMDriver<bool, const std::string&, const std::string&, std::list<int>&>;

int main(){
	BMDriver d;
	d.run_tests("test_in.txt");
}
