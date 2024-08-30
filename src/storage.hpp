#pragma once
#include "mutfunc.hpp"
#include <memory>

#pragma region type_erased_storage

struct basic_storage
{
};

template <typename T>
struct storage : basic_storage
{
    using StorageType = std::decay_t<T>;
    StorageType data;
};

struct data_registry
{
    template <typename T>
    T &get()
    {
        return std::static_pointer_cast<storage<T>>(assure<T>())->data;
    }

    template <typename T>
    const T &get() const
    {
        return std::static_pointer_cast<storage<T>>(assure<T>())->data;
    }

    template <typename T>
    std::shared_ptr<basic_storage> assure()
    {
        using StorageType = std::decay_t<T>;
        auto type_id = mutfunc::identify_type<StorageType>();
        if (pool.find(type_id) == pool.end())
        {
            pool[type_id] = std::make_shared<storage<StorageType>>();
        }
        return pool.at(type_id);
    }
    std::unordered_map<mutfunc::type_identifier, std::shared_ptr<basic_storage>> pool;
};

#pragma endregion type_erased_storage