#pragma once
#include <functional>
#include <type_traits>
namespace muf
{

    // type list

    template <typename... Ts>
    struct type_list
    {
        using type = type_list;
        static constexpr auto size = sizeof...(Ts);
    };

    template <std::size_t, typename>
    struct type_list_at;

    template <std::size_t Index, typename Type, typename... Other>
    struct type_list_at<Index, type_list<Type, Other...>>
        : type_list_at<Index - 1, type_list<Other...>>
    {
    };

    template <typename Type, typename... Other>
    struct type_list_at<0u, type_list<Type, Other...>>
    {
        using type = Type;
    };

    template <std::size_t Index, typename List>
    using type_list_at_t = typename type_list_at<Index, List>::type;

    template <typename... Type, typename... Other>
    constexpr type_list<Type..., Other...> operator+(type_list<Type...>,
                                                     type_list<Other...>)
    {
        return {};
    }

    // func

    template <std::size_t, typename>
    struct func_arg_at;

    template <std::size_t Idx, typename Ret, typename... Args>
    struct func_arg_at<Idx, Ret(Args...)> : type_list_at<Idx, type_list<Args...>>
    {
    };

    template <std::size_t Idx, typename Ret, typename... Args>
    struct func_arg_at<Idx, std::function<Ret(Args...)>>
        : type_list_at<Idx, type_list<Args...>>
    {
        // todo: extract lambda without std::function
    };

    template <typename Ret, typename... Args>
    struct func_packer
    {
        std::function<Ret(Args...)> fun;

        func_packer() noexcept {}
        func_packer(std::nullptr_t) noexcept {}
        func_packer(std::function<Ret(Args...)> _fun) : fun{_fun} {}
        func_packer(Ret (*mf)(Args...)) : fun{mf} {}

        template <std::size_t Idx>
        using arg_at = func_arg_at<Idx, decltype(fun)>;

        Ret invoke(Args... args)
        {
            return fun(args...);
        }
    };

    template <std::size_t Idx, typename Func>
    using func_arg_at_t = typename func_arg_at<Idx, Func>::type;

} // namespace muf
