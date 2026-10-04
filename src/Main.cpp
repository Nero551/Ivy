#include "Ivy.hpp"
#include "sketch.hpp"

int main(const int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        std::string_view argument = argv[i];

        if (argument == "--debug")
        {
            // Enable debug mode
        }
        //? This is where u can add custom features for command line args
        //? ex: "Ivy --renderer vulkan"
    }

    Sketch::Test();
    return 0;

    Ivy::C::Engine engine;
    engine.Run();
    return 0;
}
