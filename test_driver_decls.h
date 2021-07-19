#ifndef __TEST_DRIVER_DECLS
#define __TEST_DRIVER_DECLS

#define TD_CAST(t, i) (dynamic_cast<t>(i))
#define TD_PTRCAST(t, ptr_i) (dynamic_cast<t*>(ptr_i))
#define TD_CAST_UNSAFE(t,i) (*((t*)(&i)))
#define TD_PTRCAST_UNSAFE(t,ptr_i) ((t*)(ptr_i))
#define TD_CAST_DERIVED(i) (*(dynamic_cast<void>(i)))
#define TD_PTRCAST_DERIVED(i) (dynamic_cast<void>(i))
#define __TD_NON_TEMPLATE_SUPER(b) (TD_PTRCAST_UNSAFE(b, this))
#define TD_super(b) (TD_PTRCAST_UNSAFE(__TD_SPECIALIZE(b), this))
#define __TD_FUN_SIG_TEMPLATE template<typename R, typename ...Args>
#define __TD_TEMPLATE_ARGS R, Args...
#define __TD_PREPARE_INPUT_DECL bool __prepare(TD_TestInput* _input)
#define __TD_HANDLE_OUTPUT_DECL void __handle(TD_TestFunction<__TD_TEMPLATE_ARGS>* _data, TD_TestOutput* _output)
#define __TD_SPECIALIZE(B) B<__TD_TEMPLATE_ARGS>
#define TD_EXTEND __TD_FUN_SIG_TEMPLATE
#define TD_PREPARE_INPUT __TD_FUN_SIG_TEMPLATE __TD_PREPARE_INPUT_DECL
#define TD_HANDLE_OUTPUT __TD_FUN_SIG_TEMPLATE __TD_HANDLE_OUTPUT_DECL
#define TD_TEST_FUNCTION friend ::TD_TestDriver<__TD_TEMPLATE_ARGS>;
#define TD_FRIEND_PREPARE_INPUT friend bool __TD_SPECIALIZE(__prepare)(TD_TestInput* _input)
#define TD_METRICS(D) using type = __TD_SPECIALIZE(D); D(const std::string& h, R(*f)(Args...)) : TD_TestMetricsBase(h, f)
#define TD_TestDriverBase TD_TestDriver<__TD_TEMPLATE_ARGS>
#define TD_TestMetricsBase TD_TestFunction<__TD_TEMPLATE_ARGS>
#define TD_infile TD_TestDriver<__TD_TEMPLATE_ARGS>::infile
__TD_FUN_SIG_TEMPLATE
class TD_TestDriver;
__TD_FUN_SIG_TEMPLATE
struct TD_TestFunction;
struct TD_TestInput{};
struct TD_TestOutput{};

// Idea to potentially replace input prep/output handle macros
// class TestDataManager
// {
//     public:
//     virtual bool prepare_input(TD_TestInput* in) {return true;};
//     virtual void handle_output(TD_TestOutput* out) {};
// };

#endif // __TEST_DRIVER_DECLS
