cmake_minimum_required(VERSION 4.4.3)

project(Ivy)

add_executable(Ivy
        src/Main.cpp
        src/TracyMemory.cpp
        src/Utilities/STB.cpp
)


target_precompile_headers(Ivy PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp>"
)

set_target_properties(Ivy PROPERTIES UNITY_BUILD ON)
set_property(TARGET Ivy PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set(TRACY_ENABLE ON CACHE BOOL "Enable Tracy" FORCE)

add_subdirectory(src/Core)
add_subdirectory(src/Modules)
add_subdirectory(src/World)
add_subdirectory(src/Physics)
add_subdirectory(src/Graphics)
add_subdirectory(External/tracy)

include(scripts/GenerateUmbrellas.cmake)
generate_umbrella_header(Core Core)
generate_umbrella_header(World World)
generate_umbrella_header(Modules/Renderer Header)
generate_umbrella_header(Modules/Input Header)
generate_umbrella_header(Math Math)
generate_umbrella_header(Physics Physics)
generate_umbrella_header(Graphics Graphics)
generate_umbrella_header(Utilities Utilities)
generate_engine_umbrella(Ivy)

target_include_directories(Ivy PRIVATE
        src
)

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glslang REQUIRED)
find_package(magic_enum REQUIRED)

target_link_libraries(Ivy
        PRIVATE
        glfw
        assimp::assimp
        glad::glad
        nlohmann_json::nlohmann_json
        glslang::glslang
        glslang::SPIRV
        glslang::glslang-default-resource-limits
        Tracy::TracyClient
)

target_compile_options(Ivy PRIVATE
        #        -Wall
        #        -Wextra
        #        -Wpedantic
        #        -g -fno-omit-frame-pointer -rdynamic
)