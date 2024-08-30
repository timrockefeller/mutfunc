#include "mutfunc.hpp"
#include "storage.hpp"
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

struct arg_context_impl
{
    data_registry data;
    template <typename T>
    struct data_provider
    {
        static T get(data_registry &data)
        {
            return data.get<T>();
        }
    };

    template <typename T>
    struct data_provider<T*>
    {
        static T *get(data_registry &data)
        {
            return &(data.get<T>());
        }
    };

    template <typename T>
    T parse()
    {
        return data_provider<T>::get(data);
    }
};

void TestRaw(int &i, float &f)
{
    std::cout << i << " " << f << std::endl;
    std::cout << "invoke TestRaw" << std::endl;
    i = 1;
    f = 1;
    std::cout << i << " " << f << std::endl;
}
std::function TestFunc = [](int i, const float f)
{
    std::cout << i << " " << f << std::endl;
    std::cout << "invoke TestFunc" << std::endl;
    i = 2;
    // f = 2;
    std::cout << i << " " << f << std::endl;
};
auto TestLambda = [](int *i, const float &f)
{
    std::cout << *i << " " << f << std::endl;
    std::cout << "invoke TestLambda" << std::endl;
    *i = 3;
    // f = 3;
    std::cout << *i << " " << f << std::endl;
};
struct A_Class
{
    static inline void TestMemberFunc(int &i, float f)
    {
        std::cout << i << " " << f << std::endl;
        i = 4;
        f = 4;
        std::cout << "invoke TestMemberFunc" << std::endl;
        std::cout << i << " " << f << std::endl;
    }
};

void _test_system()
{
    mutfunc::schedule<rw_context_impl, arg_context_impl> schedule;
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
    _test_system();
    return 0;
}
