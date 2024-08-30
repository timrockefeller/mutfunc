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
void TestRaw(const int8_t&, float)
{
    std::cout << "invoke TestRaw" << std::endl;
}
std::function TestFunc = [](int, double)
{
    std::cout << "invoke TestFunc" << std::endl;
};
auto TestLambda = [](const char, std::size_t)
{
    std::cout << "invoke TestLambda" << std::endl;
};
struct A_Class
{
    static inline void TestMemberFunc(const int, const double)
    {
        std::cout << "invoke TestMemberFunc" << std::endl;
    }
};

void _test()
{
    mutfunc::schedule<rw_context_impl> schedule;
    schedule
        .add_system(TestRaw)
        .add_system(TestFunc)
        .add_system(TestLambda)
        .add_system(A_Class::TestMemberFunc);

    for (auto &&system : schedule.systems)
    {
        std::cout << system.name << std::endl;
        system.rw.impl.debug();
        std::invoke(system.ft, schedule);
    }
}

int main()
{
    _test();
    return 0;
}
