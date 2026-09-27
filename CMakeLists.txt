cmake_minimum_required(VERSION 4.3.1)

project(Nova)

add_executable(Nova
        src/Main.cpp
        src/TracyMemory.cpp
)


target_precompile_headers(Nova PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp>"
)

set_target_properties(Nova PROPERTIES UNITY_BUILD ON)
set_property(TARGET Nova PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
set(TRACY_ENABLE ON CACHE BOOL "Enable Tracy" FORCE)

add_subdirectory(src/Core)
add_subdirectory(src/Utilities)
add_subdirectory(src/Modules)
add_subdirectory(src/World)
add_subdirectory(src/Math)
add_subdirectory(src/Physics)
add_subdirectory(External/tracy)

target_include_directories(Nova PRIVATE
        src
)

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glslang REQUIRED)
find_package(magic_enum REQUIRED)

target_link_libraries(Nova
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

target_compile_options(Nova PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -g -fno-omit-frame-pointer -rdynamic
)