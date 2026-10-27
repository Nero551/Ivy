#include "Core/Engine.hpp"
#include "Graphics/Context.hpp"
#include "sketch.hpp"
#include <string_view>

int main(const int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string_view argument = argv[i];

        if (argument == "--vulkan")
        {
            //? This is where u can add custom features for command line args
            //? ex: "Ivy --renderer vulkan"
        }
    }

    Sketch::Test();
    return 0;

    Ivy::G::Context::Initialize();
    Ivy::C::Engine engine;
    engine.Run();
    Ivy::G::Context::Terminate();
    return 0;
}
