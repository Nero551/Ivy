#include "ECS/.hpp"
#include "ECS/Component.hpp"
#include "ECS/ECS.hpp"
#include "tracy/Tracy.hpp"

namespace Bencher
{
using namespace Ivy;
struct A : ECS::Component
{
    int Number = 10;
};
struct B : ECS::Component
{
    int Number = 10;
};
struct D : ECS::Component
{
    int Number = 10;
};

struct G : ECS::Component
{
    int Number = 10;
};
void BenchECS()
{
    C::EntityComponentSystem ecs;
    for (int i = 0; i < 10000000; ++i)
    {
        auto& e = ecs.CreateEntity<C::>();
        if (i % 2 == 0)
        {
            ecs.Query.SetArchetype<A, B, D>(e);
        }
        else
        {
            ecs.Query.SetArchetype<D, B, G>(e);
        }
        ZoneScopedN("Creation");
    }

    double elapsed = 0;
    while (true)
    {
        if (elapsed >= 10)
        {
            ZoneScopedN("Iteration");
            elapsed = 0;
            ecs.Query.ForEach<D>([](unsigned int id, D& d) { d.Number += id; });
        }
        elapsed += 0.016;
    }
}
} // namespace Bencher
