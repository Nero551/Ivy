#include "Core/InnerCore/Engine.hpp"
#include "sketch.hpp"

int main(const int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string_view Argument = argv[i];

        if (Argument == "--debug")
        {
            // Enable debug mode
        }
        //? This is where u can add custom features for command line args
        //? ex: "Nova --renderer vulkan"
    }

    // Sketch::Test();
    // return 0;

    N::C::Engine engine;
    engine.Run();
    return 0;
}
