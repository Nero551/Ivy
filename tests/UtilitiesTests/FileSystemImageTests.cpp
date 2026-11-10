#include "Utilities/FileSystem.hpp"
#include "Graphics/Image.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <vector>

namespace Ivy::UnitTests
{

// ==================== FileSystem ====================
TEST(FileSystem, WriteThenRead)
{
    auto path = std::filesystem::temp_directory_path() / "ivy_test_file.txt";

    U::FileSystem::WriteFile(path.string(), "hello world");
    auto content = U::FileSystem::ReadFile(path.string());
    EXPECT_EQ(content, "hello world");

    std::filesystem::remove(path);
}

TEST(FileSystem, ReadMissingFileReturnsEmpty)
{
    auto path = std::filesystem::temp_directory_path() / "ivy_nonexistent_file_xyz.txt";
    EXPECT_EQ(U::FileSystem::ReadFile(path.string()), "");
}

TEST(FileSystem, OverwriteFile)
{
    auto path = std::filesystem::temp_directory_path() / "ivy_test_overwrite.txt";

    U::FileSystem::WriteFile(path.string(), "first");
    U::FileSystem::WriteFile(path.string(), "second");
    EXPECT_EQ(U::FileSystem::ReadFile(path.string()), "second");

    std::filesystem::remove(path);
}

// ==================== Image ====================
TEST(Image, DefaultConstructor)
{
    G::Image img;
    EXPECT_EQ(img.Width, 0);
    EXPECT_EQ(img.Height, 0);
    EXPECT_TRUE(img.Pixels.empty());
}

TEST(Image, ConstructFromDimensions)
{
    std::vector<unsigned char> pixels = {255, 0, 0, 0, 255, 0, 0, 0, 255}; // 3 pixels RGB
    G::Image img(3, 1, G::Image::ColorChannels::RGB, pixels);
    EXPECT_EQ(img.Width, 3);
    EXPECT_EQ(img.Height, 1);
    EXPECT_EQ(img.Channels, G::Image::ColorChannels::RGB);
    EXPECT_EQ(img.Pixels.size(), 9u);
    EXPECT_EQ(img.Pixels, pixels);
}

TEST(Image, FlipVertically)
{
    // 2x2 RGB image: row0 = [red, green], row1 = [blue, yellow]
    std::vector<unsigned char> pixels = {
        255, 0, 0, 0, 255, 0,  // row 0
        0, 0, 255, 255, 255, 0 // row 1
    };
    G::Image img(2, 2, G::Image::ColorChannels::RGB, pixels);
    img.FlipVertically();

    // row0 should now be [blue, yellow]
    EXPECT_EQ(img.Pixels[0], 0);
    EXPECT_EQ(img.Pixels[1], 0);
    EXPECT_EQ(img.Pixels[2], 255);
    EXPECT_EQ(img.Pixels[3], 255);
    EXPECT_EQ(img.Pixels[4], 255);
    EXPECT_EQ(img.Pixels[5], 0);

    // row1 should now be [red, green]
    EXPECT_EQ(img.Pixels[6], 255);
    EXPECT_EQ(img.Pixels[7], 0);
    EXPECT_EQ(img.Pixels[8], 0);
    EXPECT_EQ(img.Pixels[9], 0);
    EXPECT_EQ(img.Pixels[10], 255);
    EXPECT_EQ(img.Pixels[11], 0);
}

TEST(Image, FlipVerticallyOddHeightMiddleRowUnchanged)
{
    // 3x1 RGB image: only one row, nothing to flip
    std::vector<unsigned char> pixels = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    G::Image img(3, 1, G::Image::ColorChannels::RGB, pixels);
    img.FlipVertically();
    EXPECT_EQ(img.Pixels, pixels);
}

} // namespace Ivy::UnitTests
