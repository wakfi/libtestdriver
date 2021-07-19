//----------------------------------------------------------------------
// AUTHOR: W. Gray
//
// DESCRIPTION: Abstract test harness
// ----------------------------------------------------------------------

#ifndef __TEST_DRIVER_H
#define __TEST_DRIVER_H
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <list>
#include "test_driver_decls.h"


#ifdef TD_USE_INPUT
TD_PREPARE_INPUT;
#define __TD_PREPARE_INPUT __prepare<__TD_TEMPLATE_ARGS>
#endif
#ifdef TD_USE_OUTPUT
TD_HANDLE_OUTPUT;
#define __TD_HANDLE_OUTPUT __handle
#endif

#ifndef TD_PRE_TIMER
#define TD_PRE_TIMER
#endif // TD_PRE_TIMER
#ifdef TD_RETURN_TO
#define __TD_RETURN_TARGET TD_RETURN_TO =
#else
#define __TD_RETURN_TARGET
#endif // TD_RETURN_TO
#ifndef TD_ARGS
#define TD_ARGS
#endif // TD_ARGS
#ifndef TD_POST_TIMER
#define TD_POST_TIMER
#endif // TD_POST_TIMER

#ifdef TD_INPUT
#define input TD_PTRCAST_UNSAFE(TD_INPUT, _input)
#else
#define TD_INPUT TD_TestInput
#define input _input
#endif
#ifdef TD_OUTPUT
#define output TD_PTRCAST_UNSAFE(TD_OUTPUT, _output)
#else
#define TD_OUTPUT TD_TestOutput
#define output _output
#endif
#ifdef TD_DATA
#define data TD_PTRCAST_UNSAFE(__TD_SPECIALIZE(TD_DATA), _data)
#else
#define TD_DATA TD_TestFunction
#define data _data
#endif


template<typename R, typename ...Args>
struct TD_TestFunction
{
	TD_TEST_FUNCTION
	using type = __TD_SPECIALIZE(TD_TestFunction);

	TD_TestFunction(const std::string& header, R(*f)(Args...))
	: header('\n' + header + '\n' + std::string(header.length(), '=') + '\n')
	, f(f)
    {}

    R operator()(Args... args)
	{
        return this->f(args...);
    }

	virtual void reset()
	{}

	virtual void print_result() const
	{}

	// Internal use. Undocumented
	void _reset()
	{
		this->reset_base_data();
		this->reset();
	}

	// Internal use. Undocumented
	void _print_result() const
	{
		this->print_base_result();
		this->print_result();
		std::cout << std::endl;
	}
private:
	void reset_base_data()
	{
		total_search = 0;
		search_times = 0;
	}

	void print_base_result() const
	{
		using namespace std;
		if(!this->total_search && this->search_times) return;

		cout << this->header << '\n';
		cout << "  Test" << " Calls......: " << this->total_search << '\n';
		cout << "  Test" << " Time.......: " << this->search_times
			<< " microseconds\n";
		cout << " Average" << " Time.....: " << ((1.0 * this->search_times) / this->total_search)
			<< " microseconds\n";
	}

	// test result header
	std::string header;
	// test results for printing
	int total_search = 0, search_times = 0;
    // Pointer-to-function under test
	R(*f)(Args...);
};
template<typename R, typename ...Args>
TD_TestFunction(const std::string&, R(*f)(Args...)) -> TD_TestFunction<R, Args...>;

template<typename R, typename ...Args>
class TD_TestDriver
{
public:
	using type = __TD_SPECIALIZE(TD_TestDriver);

	TD_TestDriver(const std::string& header, R(*test_func)(Args...));
	~TD_TestDriver();
	void run_tests();
	#ifdef __TD_PREPARE_INPUT
	void run_tests(const std::string& filename);
	#endif
	void print_results() const;
	TD_TestDriver& add_test(const std::string& header, R(*test_func)(Args...));

	#ifdef __TD_PREPARE_INPUT
	TD_FRIEND_PREPARE_INPUT;
	#endif
private:
	// reset testing metadata
	void reset();
	// run each test on the TD_TestInput provided as an argument
	void run(const TD_TestInput* _input);
	#ifdef __TD_PREPARE_INPUT
	// test file
	static inline std::ifstream infile;
	#endif
	// print helper function
	void print_one_result(int total, int times) const;
	// helper functions to get timing results
	int timed(const TD_TestInput* _input, TD_TestOutput* _output, TD_TestFunction<R, Args...>*);
	// function or system being tested (as a function)
	std::list<TD_TestFunction<R, Args...>*> test_funcs;
};

// Deduction guide
template<typename R, typename ...Args>
TD_TestDriver(const std::string& name, R(*test_func)(Args...)) -> TD_TestDriver<R, Args...>;

template<typename R, typename ...Args>
TD_TestDriver<R, Args...>::TD_TestDriver(const std::string& header, R(*test_func)(Args...))
{
	add_test(header, test_func);
}

template<typename R, typename ...Args>
TD_TestDriver<R, Args...>& TD_TestDriver<R, Args...>::add_test(const std::string& header, R(*test_func)(Args...))
{
	test_funcs.push_back(new TD_DATA(header, test_func));
	return *this;
}

template<typename R, typename ...Args>
TD_TestDriver<R, Args...>::~TD_TestDriver()
{
	for(auto ptr : test_funcs) delete ptr;
}

template<typename R, typename ...Args>
void TD_TestDriver<R, Args...>::print_results() const
{
	for(auto test_func : test_funcs)
	{
		test_func->_print_result();
	}
}

template<typename R, typename ...Args>
void TD_TestDriver<R, Args...>::run(const TD_TestInput* _input)
{
	for(auto test_func : test_funcs)
	{
		TD_OUTPUT _output;
		test_func->search_times += this->timed(_input, &_output, test_func);
		++(test_func->total_search);
		#ifdef __TD_HANDLE_OUTPUT
		__TD_HANDLE_OUTPUT(test_func, &_output);
		#endif
	}
}

template<typename R, typename ...Args>
void TD_TestDriver<R, Args...>::run_tests()
{
	this->reset();

	#ifdef __TD_PREPARE_INPUT
	// read & run tests
	while(infile)
	{
	#endif
		TD_INPUT _input;
		#ifdef __TD_PREPARE_INPUT
		if(!__TD_PREPARE_INPUT(&_input)) break;
		#endif
		run(&_input);
	#ifdef __TD_PREPARE_INPUT
	}
	#endif

	print_results();
}

#ifdef __TD_PREPARE_INPUT
template<typename R, typename ...Args>
void TD_TestDriver<R, Args...>::run_tests(const std::string& filename)
{
	// open the file
	infile.open(filename);
	this->run_tests();
	infile.close();
}
#endif

template<typename R, typename ...Args>
int TD_TestDriver<R, Args...>::timed(const TD_TestInput* _input, TD_TestOutput* _output, TD_TestFunction<R, Args...>* test_func)
{
	using namespace std::chrono;
	TD_PRE_TIMER
	auto start = high_resolution_clock::now();
	__TD_RETURN_TARGET (*test_func)(TD_ARGS);
	auto end = high_resolution_clock::now();
	auto time = duration_cast<microseconds>(end - start);
	TD_POST_TIMER
	return time.count();
}

template<typename R, typename ...Args>
void TD_TestDriver<R, Args...>::reset()
{
	for(auto test_func : test_funcs)
	{
		test_func->_reset();
	}
}

#endif
