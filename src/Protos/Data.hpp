#pragma once
#include "Diagnostics/Log.hpp"
#include "Reflect/Basic.hpp"
#include "Reflect/ReflectedReference.hpp"
#include "Reflect/Values.hpp"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
namespace Ivy::Datalization
{

template <typename Derived> struct StructuredData
{
  private:
    static Derived LoadJSON(std::string_view path)
    {
        std::ifstream file{std::string{path}};

        if (!file.is_open())
        {
            D::Log::Error("Failed to Open File: ", path);
            return {};
        }

        nlohmann::json json = nlohmann::json::parse(file);
        Derived p;
        Reflect::ReflectedReference rr = Reflect::Inspect(&p, Reflect::GetTypeName<Derived>());

        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it)
        {
            std::cout << it.key() << " : " << it.value() << "\n";
            if (it.value().is_string())
            {
                rr.SetField(it.key(), it.value().get<std::string>());
            }
            else if (it.value().is_number_unsigned())
            {
                rr.SetField(it.key(), it.value().get<unsigned int>());
            }
            else if (it.value().is_number())
            {
                rr.SetField(it.key(), it.value().get<int>());
            }
            else if (it.value().is_boolean())
            {
                rr.SetField(it.key(), it.value().get<bool>());
            }
            else if (it.value().is_number_float())
            {
                rr.SetField(it.key(), it.value().get<float>());
            }
        }
        return p;
    }

    void SaveJSON(std::string_view path)
    {
        nlohmann::json save{};
        Reflect::ReflectedReference rr = Reflect::Inspect(&*this, Reflect::GetTypeName<Derived>());
        std::ofstream file(std::string{path});
        for (Reflect::FieldValue& field : rr.GetFields())
        {
            if (field.Description.Type->TypeId == Reflect::GetTypeId<std::string>())
            {
                save[field.Description.Name] = field.As<std::string>();
            }
            else if (field.Description.Type->TypeId == Reflect::GetTypeId<unsigned int>())
            {
                save[field.Description.Name] = field.As<unsigned int>();
            }
            else if (field.Description.Type->TypeId == Reflect::GetTypeId<int>())
            {
                save[field.Description.Name] = field.As<int>();
            }
            else if (field.Description.Type->TypeId == Reflect::GetTypeId<bool>())
            {
                save[field.Description.Name] = field.As<bool>();
            }
            else if (field.Description.Type->TypeId == Reflect::GetTypeId<float>())
            {
                save[field.Description.Name] = field.As<float>();
            }
            else if (field.Description.Type->TypeId == Reflect::GetTypeId<double>())
            {
                save[field.Description.Name] = field.As<double>();
            }
        }
        file << save.dump(4);
    }
    StructuredData() = default;

  public:
    static Derived Load(std::string_view path)
    {
        std::filesystem::path filePath{path};
        Derived data;
        if (filePath.extension() == ".json")
        {
            data = LoadJSON(path);
        }
        else
        {
            D::Log::Error("File Format Not Supported: ", path);
        }

        return data;
    }

    void Save(std::string_view path)
    {
        std::filesystem::path filePath{path};
        if (filePath.extension() == ".json")
        {
            SaveJSON(path);
        }
        else
        {
            D::Log::Error("File Format Not Supported: ", path);
        }
    }
    friend Derived;
};

} // namespace Ivy::Datalization
