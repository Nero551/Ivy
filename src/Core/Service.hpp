#pragma once
#include "Utilities/CheckedPtr.hpp"
#include "Utilities/DataStructures/TypedVector.hpp"
#include "Utilities/Log.hpp"

namespace N::C
{
struct Service;
template <typename T>
concept ServiceType = std::derived_from<T, Service>;

/**
 * @brief Base class for globally accessible services.
 * Services aren't owned by the Engine, primarily to avoid cyclic dependencies.
 * services do get destroyed ,created & updated in the engine loop though.
 */
struct Service
{
    virtual ~Service() = default;
    /**
     * @brief Retrieves a registered service by type.
     * @tparam T Type of the service to retrieve.
     * @return Reference to the registered service.
     */
    template <ServiceType T> static T& Get()
    {
        auto service = Services.Find<T>();
        if (service == Services.end())
        {
            U::Log::Fatal(std::format("Service Not Found: {}", typeid(T).name()));
        }
        return static_cast<T&>(**service);
    }

    /**
     * @brief Returns all currently registered services.
     * @return A vector of pointers to the registered services.
     */
    static const U::TypedVector<std::unique_ptr<Service>>& GetAll()
    {
        return Services;
    }

  protected:
    friend struct Engine;
    virtual void Start() {}
    virtual void BeginFrame(double dt) {}
    virtual void FixedUpdate(double fdt) {}
    virtual void Update(double dt) {}
    virtual void Render() {}
    virtual void EndFrame() {}
    virtual void Stop() {}

  private:
    /**
    * @brief Registers a new service of the specified type.
    * If a service of the same type is already registered, the existing
    * service is returned instead.
    * @tparam T Type of the service to register.
    * @return Reference to the registered service.
    */
    template <ServiceType T> static T& Add()
    {
        if (Services.Contains<T>())
        {
            U::Log::Error(std::format(" Service {} Already Added", typeid(T).name()));
            return static_cast<T&>(*Services.At<T>());
        }

        auto service = std::make_unique<T>();
        Services.Emplace<T>(std::move(service));

        return Get<T>();
    }

    /** @brief Destroys all services */
    static void TerminateServices()
    {
        Services.Clear();
    }

    static U::TypedVector<std::unique_ptr<Service>> Services;
};
inline U::TypedVector<std::unique_ptr<Service>> Service::Services;
} // namespace N::C
