#pragma once
#include <functional>
#include <type_traits>
#include <string>
namespace mutfunc
{

#pragma region type_list

    // type list
    template <typename... Ts>
    struct type_list;

    template <>
    struct type_list<>
    {
    };

    template <typename _Head, typename... _Tails>
    struct type_list<_Head, _Tails...>
    {
        using head = _Head;
        using tails = type_list<_Tails...>;
    };

    // Length

    template <typename _TList>
    struct length;

    template <typename... _Types>
    struct length<type_list<_Types...>>
    {
        static constexpr std::size_t value = sizeof...(_Types);
    };

    template <typename _TList>
    inline constexpr std::size_t length_v = length<_TList>::value;

    // index

    template <typename _TList, std::size_t index>
    struct type_at;

    template <typename _Head, typename... _Tails>
    struct type_at<type_list<_Head, _Tails...>, 0>
    {
        using type = _Head;
    };

    template <typename _Head, typename... _Tails, std::size_t index>
    struct type_at<type_list<_Head, _Tails...>, index>
    {
        static_assert(index < sizeof...(_Tails) + 1, "index out of range");
        using type = typename type_at<type_list<_Tails...>, index - 1>::type;
    };

    template <typename _TList, std::size_t index>
    using type_at_t = typename type_at<_TList, index>::type;

    // skip

    template <typename _TList, std::size_t count>
    struct skip;

    template <typename... _Types>
    struct skip<type_list<_Types...>, 0>
    {
        using result_type = type_list<_Types...>;
    };

    template <typename _Head, typename... _Tails>
    struct skip<type_list<_Head, _Tails...>, 0>
    {
        using result_type = type_list<_Head, _Tails...>;
    };

    template <typename _Head, typename... _Tails, std::size_t count>
    struct skip<type_list<_Head, _Tails...>, count>
    {
        using result_type = typename skip<type_list<_Tails...>, count - 1>::result_type;
    };

    template <typename _TList, std::size_t count>
    using skip_t = typename skip<_TList, count>::result_type;

    // append

    template <typename _TList1, typename _TList2>
    struct append;

    template <typename... _TList, typename T>
    struct append<type_list<_TList...>, T>
    {
        using result_type = type_list<_TList..., T>;
    };

    template <typename T, typename... _TList>
    struct append<T, type_list<_TList...>>
    {
        using result_type = type_list<T, _TList...>;
    };

    template <typename... TList1, typename... TList2>
    struct append<type_list<TList1...>, type_list<TList2...>>
    {
        using result_type = type_list<TList1..., TList2...>;
    };

    template <typename TList1, typename TList2>
    using append_t = typename append<TList1, TList2>::result_type;

#pragma endregion type_list

#pragma region func_parse

    template <typename Ret, typename... Args>
    struct func_parse
    {
        using return_type = Ret;
        using argument_types = type_list<Args...>;
    };

    template <typename Ret, typename... Args>
    func_parse<Ret, Args...> constexpr parse_func(Ret (*)(Args...)) noexcept
    {
        return func_parse<Ret, Args...>{};
    }

    template <typename F, typename Ret, typename... Args>
    func_parse<Ret, Args...> constexpr parse_func(Ret (F::*)(Args...)) noexcept
    {
        return func_parse<Ret, Args...>{};
    }
    template <typename F, typename Ret, typename... Args>
    func_parse<Ret, Args...> constexpr parse_func(Ret (F::*)(Args...) const) noexcept
    {
        return func_parse<Ret, Args...>{};
    }

    // Handle std::function
    template <typename Ret, typename... Args>
    func_parse<Ret, Args...> constexpr parse_func(const std::function<Ret(Args...)> &)
    {
        return func_parse<Ret, Args...>{};
    }

    // Handle lambda
    template <typename F>
    auto constexpr parse_func(F) -> decltype(parse_func(&std::remove_reference_t<F>::operator()))
    {
        return parse_func(&std::remove_reference_t<F>::operator());
    }

#pragma endregion func_parse

#pragma region rw_context

#pragma region typeid

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#ifndef USE_RTTI
#define USE_RTTI 1
#endif

    using type_identifier = const char *;
    template <typename T>
    type_identifier constexpr identify_type() noexcept
    {
#if USE_RTTI
        return typeid(std::decay_t<T>).name();
#else
        return __PRETTY_FUNCTION__;
#endif
    }

    template <typename T>
    type_identifier constexpr identify_type(T) noexcept
    {
        return type_identifier<T>();
    }

#pragma endregion typeid

#pragma region rw_context

    template <typename Impl>
    struct rw_context
    {
        void constexpr add_write(type_identifier id)
        {
            impl.add_write(id);
        }
        template <typename T>
        void constexpr add_write()
        {
            add_write(identify_type<T>());
        }
        void constexpr add_read(type_identifier id)
        {
            impl.add_read(id);
        }
        template <typename T>
        void constexpr add_read()
        {
            add_read(identify_type<T>());
        }
        Impl impl;
    };

    template <typename TList, typename Impl>
    void parse_rw(rw_context<Impl> &context)
    {
        if constexpr (length_v<TList> == 0)
        {
            return;
        }
        else
        {
            using CurType = type_at_t<TList, 0>;
            using CurDecay = std::remove_reference_t<CurType>;
            if constexpr (std::is_const_v<CurDecay> || std::is_same_v<CurType, CurDecay>)
            {
                context.template add_read<CurDecay>();
            }
            else
            {

                context.template add_write<CurDecay>();
            }
            parse_rw<skip_t<TList, 1>>(context);
        }
    }

#pragma endregion rw_context

#pragma region type_erased_storage



#pragma endregion type_erased_storage

#pragma region data_context
    template <typename Impl>
    struct data_context
    {
        Impl impl;
    };

#pragma endregion data_context

#pragma region registry

    template <typename RwImpl>
    struct function_proxy;

    template <typename RwImpl>
    struct schedule
    {
        /**
         * @param f will be moved into system's scope, don't use it after calling this function.
         */
        template <typename F>
        schedule &add_system(F f)
        {
            systems.push_back(function_proxy<RwImpl>{f});
            return *this;
        }
        std::vector<function_proxy<RwImpl>> systems;
    };

    template <typename... Args, typename... Impls>
    auto gen_arg(type_list<Args...>, schedule<Impls...> &s)
    {
        return std::tuple<Args...>{};
    }

    template <typename F, typename... Impls>
    bool apply_system(F f, schedule<Impls...> &s)
    {
        using rt = typename decltype(parse_func(f))::argument_types;
        auto args = gen_arg(rt{}, s);
        std::apply(f, args);
        return false;
    }

    template <typename RwImpl>
    struct function_proxy
    {
        function_proxy() = delete;

        template <typename F>
        function_proxy(F f)
        {
            name = __PRETTY_FUNCTION__;
            init(std::forward<F>(f));
        }

        template <typename F>
        void init(F f)
        {
            using rt = typename decltype(parse_func(f))::argument_types;
            parse_rw<rt>(rw);
            ft = [f = std::move(f)](schedule<RwImpl> &s)
            {
                // prepare arguments here
                apply_system(f, s);
            };
        }

        // debug name
        std::string name;

        std::function<void(schedule<RwImpl> &)> ft;

        rw_context<RwImpl> rw;
    };

#pragma endregion registry

} // namespace muf
