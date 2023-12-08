#include "muf.h"

void TestFunc(int a, float b) {}
std::function TestLambda = [](int, float) {};

static_assert(std::is_same_v<muf::func_arg_at_t<0, decltype(TestFunc)>, int>);
static_assert(std::is_same_v<muf::func_arg_at_t<1, decltype(TestFunc)>, float>);
static_assert(std::is_same_v<muf::func_arg_at_t<0, decltype(TestLambda)>, int>);
static_assert(std::is_same_v<muf::func_arg_at_t<1, decltype(TestLambda)>, float>);

muf::func_packer f1(TestLambda);
muf::func_packer f2(&TestFunc);