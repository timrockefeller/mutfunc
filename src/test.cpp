#include "mutfunc.hpp"

#include <iostream>
struct rw_context_impl
{
    enum RW_TYPE
    {
        READ,
        WRITE,
    };
    using type_identifier = mutfunc::type_identifier;
    std::unordered_map<type_identifier, RW_TYPE> info;
    void add_write(type_identifier id)
    {
        info[id] = RW_TYPE::WRITE;
    }
    void add_read(type_identifier id)
    {
        info[id] = RW_TYPE::READ;
    }
    void debug()
    {
        for (auto &&[id, rw] : info)
        {
            std::cout << id << " " << (rw == READ ? "read" : "write") << std::endl;
        }
    }
};

void TestRaw(int8_t, float) {}
std::function TestFunc = [](int, double) {};
auto TestLambda = [](const char &, std::size_t &) {};

void _test()
{
    mutfunc::schedule<rw_context_impl> schedule;
    schedule
        .add_system(TestRaw)
        .add_system(TestFunc)
        .add_system(TestLambda);
    schedule.rw.impl.debug();
}

int main()
{
    _test();
    return 0;
}
