#pragma once

#include <iostream>
#include <source_location>
#include <string_view>
#include <type_traits>

namespace Ivy::U
{

struct Inspector
{
  private:
    template <typename T> struct PInspect
    {
        static void Print(const T& Value)
        {
            if constexpr (requires { std::cerr << Value; })
            {
                std::cerr << Value;
            }
            else
            {
                std::cerr << "<uninspectable>";
            }
        }
    };

    template <typename T> static constexpr std::string_view TypeName()
    {
        std::string_view Function = __PRETTY_FUNCTION__;
        constexpr std::string_view Prefix = "T = ";
        const size_t Start = Function.find(Prefix) + Prefix.size();
        const size_t End = Function.rfind(']');

        return Function.substr(Start, End - Start);
    }

  public:
    template <typename T> struct TInspect;

    template <typename T>
    static void Inspect(
        const T& Value, const std::source_location& Location = std::source_location::current())
    {
        using Type = std::remove_cvref_t<T>;

        std::cerr << "  Type:    " << TypeName<Type>() << '\n'
                  << "  Address: " << static_cast<const void*>(&Value) << '\n'
                  << "  Size:    " << sizeof(Type) << " bytes\n"
                  << "  Align:   " << alignof(Type) << " bytes\n"
                  << "  Trivial: " << std::boolalpha << std::is_trivially_copyable_v<Type> << '\n'
                  << "  At:      " << Location.file_name() << ':' << Location.line() << '\n'
                  << "  Value:   ";

        PInspect<Type>::Print(Value);
        std::cerr << "\n\n";
    }

    Inspector() = delete;
    ~Inspector() = delete;
    Inspector& operator=(Inspector&) = delete;
};
} // namespace Ivy::U
