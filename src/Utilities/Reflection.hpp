#pragma once

#include <string_view>

namespace N::Reflection
{

//TODO- understanding macros is the key to making reflection.

using TypeId = unsigned long;

template <typename T> consteval std::string_view GetTypeName()
{
    constexpr std::string_view Function = __PRETTY_FUNCTION__;
    constexpr std::string_view Prefix = "T = ";
    constexpr std::size_t Start = Function.find(Prefix) + Prefix.size();
    constexpr std::size_t End = Function.find(']', Start);

    return Function.substr(Start, End - Start);
}

consteval unsigned long Hash(const std::string_view typeName)
{
    unsigned long hashValue = 0xcbf29ce484222325ULL;
    for (char c : typeName)
    {
        constexpr unsigned long prime = 0x100000001b3ULL;
        hashValue ^= static_cast<unsigned long>(c);
        hashValue *= prime;
    }
    return hashValue;
}

template <typename T> consteval TypeId GetTypeId()
{
    return Hash(GetTypeName<T>());
}

struct TypeInfo
{
    std::string_view Name{};
    unsigned int Size = 0;
    unsigned int Align = 0;
    TypeId Id = 0;

    friend std::ostream& operator<<(std::ostream& os, const TypeInfo& typeInfo)
    {
        os << "TypeInfo {\n"
           << "    Name:  " << typeInfo.Name << '\n'
           << "    Size:  " << typeInfo.Size << '\n'
           << "    Align: " << typeInfo.Align << '\n'
           << "    Id:    " << std::hex << typeInfo.Id << std::dec << '\n'
           << '}';

        return os;
    }
};

template <typename T> consteval TypeInfo GetTypeInfo()
{
    return {.Name = GetTypeName<T>(), .Size = sizeof(T), .Align = alignof(T), .Id = GetTypeId<T>()};
}

class TypeRegistry
{
  public:
    static void Register(const TypeInfo& Type)
    {
        Types.emplace(Type.Id, Type);
    }

    static const TypeInfo* Get(const TypeId Id)
    {
        const auto It = Types.find(Id);

        if (It == Types.end())
        {
            return nullptr;
        }

        return &It->second;
    }

  private:
    inline static std::unordered_map<TypeId, TypeInfo> Types{};
};

struct AttributeInfo
{
    std::string_view Name{};
    unsigned int Offset = 0;
    TypeId TypeID = 0;

    friend std::ostream& operator<<(std::ostream& os, const AttributeInfo& attributeInfo)
    {
        os << "AttributeInfo {\n"
           << "    Name:   " << attributeInfo.Name << '\n'
           << "    Offset: " << attributeInfo.Offset << '\n'
           << "    Type:   " << std::hex << attributeInfo.TypeID << std::dec << '\n'
           << '}';

        return os;
    }

    const TypeInfo& GetType() const
    {
        return *TypeRegistry::Get(TypeID);
    }
};

template <typename T> AttributeInfo RegisterAttribute(const std::string_view name, const unsigned int offset)
{
    TypeRegistry::Register(GetTypeInfo<T>());
    return {.Name = name, .Offset = offset, .TypeID = GetTypeId<T>()};
}

#define REGISTER_ATTRIBUTE(Struct, Member)                                                                   \
    N::Reflection::RegisterAttribute<decltype(Struct::Member)>(#Member, offsetof(Struct, Member))

} // namespace N::Reflection