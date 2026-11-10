#include "Descriptors.hpp"
#include <ostream>

namespace Ivy::Meta
{
std::ostream& operator<<(std::ostream& os, const TypeDescriptor& type)
{
    os << "{ " << type.Name << ", " << type.Size << ", " << type.Alignment << ", Fields: [ ";

    for (const auto& field : type.Fields)
    {
        os << field << ", ";
    }

    os << " ] }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const FieldDescriptor& field)
{
    os << "{ " << field.Name << ", " << field.Offset << ", " << field.IsConst << ", " << field.IsPointer
       << ", " << field.IsReference << ", " << field.IsRValue;

    if (field.Type != nullptr)
    {
        os << *field.Type;
    }
    else
    {
        os << "nullptr";
    }

    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ParameterDescriptor& parameter)
{
    os << "{ " << parameter.Type << ", " << parameter.IsConst << ", " << parameter.IsPointer << ", "
       << parameter.IsReference << ", " << parameter.IsRValue;

    if (parameter.Type != nullptr)
    {
        os << *parameter.Type;
    }
    else
    {
        os << "nullptr";
    }

    os << " }";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ConstructorDescriptor& constructor)
{
    os << "Constructor(";

    for (auto& parameter : constructor.Parameters)
    {
        os << parameter;
    }

    os << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const MethodDescriptor& method)
{
    os << method.Return << method.Name << "(";

    for (auto& parameter : method.Parameters)
    {
        os << parameter << ", ";
    }

    os << ")";
    return os;
}
} // namespace Ivy::Meta
